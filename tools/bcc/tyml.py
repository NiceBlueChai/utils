#!/usr/bin/python3
# 由通义灵码编写
from bcc import BPF
from time import sleep

# eBPF C 代码
bpf_text = """
#include <uapi/linux/ptrace.h>

struct alloc_info_t {
    int pid;
    u64 addr;
    u64 size;
    char comm[16];
    int stack_id;
};

BPF_HASH(allocs, u64, struct alloc_info_t);
BPF_HASH(sizes, u64, u64);
BPF_STACK_TRACE(stack_traces, 10240);

int trace_malloc_entry(struct pt_regs *ctx, int size) {
    u64 pid_tgid = bpf_get_current_pid_tgid();
    u64 size64 = size;
    sizes.update(&pid_tgid, &size64);

    return 0;
}
int trace_alloc_exit(struct pt_regs *ctx, void *ptr) {
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

int trace_free_entry(struct pt_regs *ctx, void *ptr) {
    u64 addr = (u64) ptr;
    allocs.delete(&addr);
    struct alloc_info_t * info = allocs.lookup(&addr);
    if (info == 0) {
        return 0;
    }
    bpf_trace_printk("%s Freed at 0x%x\\n",info->comm, addr);
    return 0;
}

"""

b = BPF(text=bpf_text)

# 注册探针
# b.attach_uprobe(name="sqlite3", sym="sqlite3_malloc", fn_name="trace_malloc_entry")
# b.attach_uretprobe(name="sqlite3", sym="sqlite3_malloc", fn_name="trace_alloc_exit")
# b.attach_uretprobe(name="sqlite3", sym="sqlite3_malloc64", fn_name="trace_alloc_exit")
b.attach_uretprobe(name="sqlite3", sym="sqlite3_mprintf", fn_name="trace_alloc_exit")
b.attach_uprobe(name="sqlite3", sym="sqlite3_free", fn_name="trace_free_entry")

print("Tracing SQLite3 memory allocations... Ctrl-C to quit.")

# 每 5 秒打印一次统计结果
while True:
    try:
        # 输出当前未释放的内存块及其调用堆栈信息
        for k, v in b["allocs"].items():
            stack_traces = b["stack_traces"]
            user_stack = list(stack_traces.walk(v.stack_id))
            print("ptr:  0x%X:" % k.value)
            print("%s (pid: %d) stack:" % ( v.comm.decode('utf-8', 'replace'), v.pid))
            for addr in user_stack:
                print("%-16s 0x%x    %s" % ("-", addr, b.sym(addr, v.pid, show_module=True, show_offset=True).decode('utf-8', 'replace')))
        sleep(5)

    except KeyboardInterrupt:
        break

b.cleanup()
