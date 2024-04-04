#!/usr/bin/python3

# arf

# Copyright (C) 2024 NiceBlueChai.

from bcc import BPF
from time import sleep
import resource
import subprocess
import os
import sys
import argparse


def run_command_get_pid(command):
    p = subprocess.Popen(command.split())
    return p.pid

examples = """
Exmaples:

./af.py -p $(pidof a.out)

./af.py -c "./a.out"

./af.py -O "curl" -A "curl_easy_init" -F "curl_easy_cleanup" -c "./a.out" 

"""

description = """
追踪资源分配和释放函数
"""

parser = argparse.ArgumentParser(description= description,
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=examples)

parser.add_argument("-p", "--pid", type=int, default=-1,
        help="the PID to trace; if not specified, trace kernel allocs")
parser.add_argument("-v", "--verbose", default=False, action="store_true",
        help="show allocation address as well as call stacks")
parser.add_argument("-c", "--command", 
        help="execte and trace the specified command")
parser.add_argument("-O", "--obj", type=str, default="sqlite3",
        help="attach to allocations function in the specified object; default is sqlite3")
parser.add_argument("-S", "--size", default=False, action="store_true",
        help="show allocation size from aclloc-func param1.; default is False.")
# 使用alloc-func-addrress或者指定alloc-func函数名， 参数二选一
parser.add_argument("-A", "--alloc-func", type=str, default="sqlite3_mprintf",
        help="attach to the specified allocation resource function; default is sqlite3_mprintf.")
parser.add_argument("--alloc-func-addr", type=str, default=None,
        help="alloc-func-address; default is None.")
parser.add_argument("-F", "--free-func", type=str, default="sqlite3_free",
        help="attach to the specified free resource function; default is sqlite3_free.")
parser.add_argument("--free-func-addr", type=str, default=None,
        help="free-func-address; default is None.")
parser.add_argument("-R", "--reset-func", type=str, default=None,
        help="attach to the specified reset resource function; example curl_easy_reset; default is None.")
parser.add_argument("--reset-func-addr", type=str, default=None,
        help="reset-func-address; default is None.")
args=parser.parse_args()

pid = args.pid
command = args.command
obj = args.obj
verbose = args.verbose
alloc_func = args.alloc_func
free_func = args.free_func
reset_func = args.reset_func



# eBPF C 代码
bpf_text = """
#include <uapi/linux/ptrace.h>
#include <linux/sched.h>
#include <uapi/linux/bpf.h>

struct alloc_info_t {
    int pid;
    u64 addr;
    u64 size;
    char comm[16];
    int stack_id;
    int reset_cnt;
};

BPF_HASH(allocs, u64, struct alloc_info_t);
BPF_HASH(sizes, u64, u64);
BPF_STACK_TRACE(stack_traces, 10240);

int trace_alloc_entry(struct pt_regs *ctx, int size) {
    u64 pid_tgid = bpf_get_current_pid_tgid();
    u64 size64 = size;
    sizes.update(&pid_tgid, &size64);

    return 0;
}

static inline int trace_alloc_exit2(struct pt_regs *ctx, void *ptr) {
    struct alloc_info_t info = {};
    info.addr = (u64) ptr;
    u64 pid_tgid = bpf_get_current_pid_tgid();

    info.pid = bpf_get_current_pid_tgid() >> 32;
    sizes.delete(&pid_tgid);
    bpf_get_current_comm(&info.comm, sizeof(info.comm));
    info.stack_id = stack_traces.get_stackid(ctx, BPF_F_USER_STACK);
    bpf_trace_printk("%s Allocated at 0x%x\\n",info.comm, info.addr);
    allocs.update(&info.addr, &info);
    return 0;
}

int trace_alloc_exit(struct pt_regs *ctx) {
        return trace_alloc_exit2(ctx, PT_REGS_RC(ctx));
}

int trace_free_entry(struct pt_regs *ctx, void *ptr) {
    u64 addr = (u64) ptr;
    allocs.delete(&addr);
    bpf_trace_printk("Freed bytes at 0x%x\\n", addr);
    return 0;
}

int trace_reset_entry(struct pt_regs *ctx, void *ptr) {
        u64 addr = (u64) ptr;
        struct alloc_info_t *alloc = allocs.lookup(&addr);
        if (!alloc) {
               return 0;
        }
        alloc->reset_cnt += 1;
        bpf_trace_printk("Reset ptr at %p cnt %d\\n", ptr, alloc->reset_cnt);
        return 0;
}

"""

if command is not None:
        print("Executing '%s' and tracing the result process." % command)
        pid = run_command_get_pid(command)


b = BPF(text=bpf_text)

if args.size:
        if args.alloc_func_addr:
                print("tracing alloc resource entry with alloc_func_addr 0x%X" % int(args.alloc_func_addr, 16))
                b.attach_uprobe(name=obj, addr=int(args.alloc_func_addr, 16), fn_name="trace_alloc_entry")
        else:
                print("tracing all resource entry with alloc_func %s" % alloc_func)
                b.attach_uprobe(name=obj, sym=alloc_func, fn_name="trace_alloc_entry")

if args.alloc_func_addr:
        print("tracing alloc resource exit with alloc_func_addr 0x%X" % int(args.alloc_func_addr, 16))
        b.attach_uretprobe(name=obj, addr=int(args.alloc_func_addr, 16), fn_name="trace_alloc_entry")
else:
        print("tracing alloc resource exit with alloc_func %s" % alloc_func)
        b.attach_uretprobe(name=obj, sym=alloc_func, fn_name="trace_alloc_exit")

if args.free_func_addr:
        print("tracing free resource exit with free_func_addr 0x%X" % int(args.free_func_addr, 16))
        b.attach_uprobe(name=obj, addr=int(args.free_func_addr, 16), fn_name="trace_free_entry")
else:
        print("tracing free resource exit with free_func %s" % free_func)
        b.attach_uprobe(name=obj, sym=free_func, fn_name="trace_free_entry")

if args.reset_func_addr:
        print("tracing reset resource exit with reset_func_addr 0x%X" % int(args.reset_func_addr, 16))
        b.attach_uprobe(name=obj, addr=int(args.reset_func_addr, 16), fn_name="trace_reset_entry")
elif args.reset_func:
        print("tracing reset resource exit with reset_func %s" % reset_func)
        b.attach_uprobe(name=obj, sym=reset_func, fn_name="trace_reset_entry")

print("Tracing ... Hit Ctrl-C to end.")

# 每 5 秒打印一次统计结果
while True:
    try:
        # 输出当前未释放的内存块及其调用堆栈信息
        for k, v in b["allocs"].items():
            stack_traces = b["stack_traces"]
            user_stack = list(stack_traces.walk(v.stack_id))
            if v.reset_cnt > 0:
                print("ptr:  0x%X: (reset_cnt: %d)" % (k.value, v.reset_cnt))
            else:
                print("ptr:  0x%X:" % k.value)
            print("%s (pid: %d) stack:" % ( v.comm.decode('utf-8', 'replace'), v.pid))
            for addr in user_stack:
                print("%-16s 0x%x    %s" % ("-", addr, b.sym(addr, v.pid, show_module=True, show_offset=True).decode('utf-8', 'replace')))
        sleep(5)

    except KeyboardInterrupt:
        break

b.cleanup()