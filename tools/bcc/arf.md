## arf.py (alloc reset free)


> 该文档由通义灵码编写

该Python脚本实现了一个基于BPF（Berkeley Packet Filter）技术的资源分配与释放跟踪工具，用于在Linux环境下监控特定进程或对象库的内存分配、释放及可能存在的重置操作。以下是其详细功能说明：

1. 命令行参数解析

通过argparse模块创建命令行接口，提供以下参数选项：

-p / --pid: 指定要追踪的进程ID。默认值为-1，表示不指定，此时将追踪内核层的资源分配。
-v / --verbose: 启用详细模式，显示分配地址和调用栈。
-c / --command: 执行并追踪指定命令启动的新进程。
-O / --obj: 要追踪的对象库，默认为"sqlite3"。此参数用于确定要附加探针的目标动态链接库或可执行文件。
-S / --size: 显示分配大小。若启用，将在跟踪时收集每个分配的大小。
-A / --alloc-func, -F / --free-func: 分别指定资源分配和释放函数名称，默认分别为"sqlite3_mprintf"和"sqlite3_free"。
--alloc-func-addr, --free-func-addr: 分别指定资源分配和释放函数的地址。二者与对应的函数名参数互斥，仅需指定其中一个。
-R / --reset-func: 指定资源重置函数名。
--reset-func-addr: 指定资源重置函数的地址。与--reset-func互斥，仅需指定其中一个。
2. 准备BPF程序

定义了包含如下主要部分的eBPF C源码字符串bpf_text：

结构体alloc_info_t：存储资源分配信息，包括进程ID、分配地址、大小、进程名和调用栈ID。
定义两个哈希表（BPF_MAP_TYPE_HASH）：allocs用于存储分配信息；sizes用于临时存储分配大小（当--size启用时）。
定义一个堆栈跟踪表（BPF_MAP_TYPE_STACK_TRACE）：stack_traces用于存储调用栈信息。
编写四个BPF辅助函数：

trace_alloc_entry()：当进入资源分配函数时被触发，记录分配大小至sizes表。
trace_alloc_exit2()：当退出资源分配函数时被触发，捕获返回值（即分配地址），填充alloc_info_t结构体，删除sizes表中对应项，获取当前进程名和调用栈ID，然后将分配信息写入allocs表并打印日志。
trace_free_entry()：当进入资源释放函数时被触发，从allocs表中删除已释放资源的记录并打印日志。
trace_reset_entry()：当进入资源重置函数时被触发，打印重置操作的日志。
3. 运行流程

解析命令行参数，根据--command参数执行并获取新进程PID（若有指定）。
创建BPF对象b，加载上述eBPF C源码。
根据用户提供的参数，以指定的函数名或地址为目标，分别向目标对象库的入口和出口处附加探针（uprobe/uretprobe）。探针触发相应的BPF辅助函数以监控资源分配、释放及重置事件。
开始循环监听，每5秒打印一次当前未释放的资源信息及其调用栈。按Ctrl+C中断循环。
清理BPF对象及相关资源。
综上所述，该Python脚本利用BPF技术构建了一款强大的资源分配与释放跟踪工具，支持灵活配置追踪目标（进程、对象库、函数名/地址），实时监控资源分配、释放及重置情况，有助于开发者分析内存泄漏、资源管理问题以及优化程序性能