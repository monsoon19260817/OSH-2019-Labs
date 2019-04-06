# 编译内核

仅说明修改的部分

## Gernel setup

- `Kernel compression mode`  顾名思义。有 Gzip, LZMA, XZ, LZO, LZ4 等选项，默认选择 Gzip, **改为更快的 LZ4**。

- `Support for paging of anonymous memory (swap)`  swap 分区支持。默认选中，**由于此次实验不需要使用 swap, 关闭此选项。**

- `uselib syscall` uselib 系统调用，属于被淘汰技术，默认关闭，不修改。 -->

- `IRQ subsystem` 中的 `Expose irq internals in debugfs` 在 debugfs 中显示中断请求的内部信息，用于调式，**改为关闭**。

- `Timers subsystem` 时钟子系统。

  - `Old Idle dynticks config` 兼容性选项，**改为关闭**。

  - `High Resolution Timer Support` 高精度定时器(hrtimer), 用于取代传统 timer wheel (基于 jiffies 定时器)的时钟子系统。可以降低与内核其他模块的耦合性，还可以提供比1毫秒更高的精度，可以更好的支持音视频等对时间精度要求较高的应用。此处无需此功能，**改为关闭**。

- `CPU/Task time and stats accounting` 用于进程追踪和统计。

  - `BSD Process Accounting` BSD 进程记账支持。用户空间程序可以要求内核将进程的统计信息写入一个指定的文件，主要包括进程的创建时间/创建者/内存占用等信息。**改为关闭**。

  - `Export task/process statistics through netlink` 通过 netlink 接口向用户空间导出进程的统计信息。无需此功能，**改为关闭**。

- `Kernel .config support` 将编译时的配置文件写入内核，无需此功能，**改为关闭**。

- `Control Group support` cgroup 支持，本次实验无需使用，**关闭全部子选项**。

- `Namespace support` 顾名思义，无需使用，**全部关闭**。

- `Automatic process group scheduling` 每个TTY动态地创建任务分组(cgroup), 这样就可以降低高负载情况下的桌面延迟。**改为关闭**。

- `Support initial ramdisk/ramfs compressed using ...` **仅保留 LZ4, 其余全部关闭**

- `Enable madvise/fadvise syscalls` 许应用程序预先提示内核，它将如何使用特定的内存与文件。**关闭**。

- `Embedded system` 嵌入式系统支持，**关闭**。

- `Kernel Performance Events And Counters` 性能相关的事件和计数器支持，一般用于剖析程序，**关闭**。

- `Enable VM event counters for /proc/vmstat` /proc/vmstat 中包含了从内核导出的虚拟内存的各种统计信息。开启此项后可以显示较详细的信息（包含各种事件计数器），关闭此项则仅仅显示内存页计数。主要用于调试和统计。**关闭**。

- `Enable SLUB debugging support` 用于调式，**关闭**。

- `Disable heap randomization` 安全特性，**改为开启**。

- `Profiling support` 性能剖析，**关闭**。

- `Kprobes` 调试工具，**关闭**。

## Enable loadable module support

- `Source checksum for all modules` 为模块添加"srcversion" 字段，以帮助模块维护者准确的知道编译此模块所需要的源文件，从而可以校验源文件的变动。**关闭**。

## Enable the block layer

块设备支持

- `Support for large (2TB+) block devices and files` 顾名思义，默认开启，**改为关闭**。

- `Block layer debugging information in debugfs` 用于调式，**关闭**。

- `I/O Scheduler` I/O 调度算法。**仅保留对 flash 设备最好的 NOOP 算法，其余全部关闭**。

## System Type

此选项中都是涉及 CPU 硬件平台支持的选项，为保证安全不做修改。

## Bus support

总线支持，无需修改。

## Kernel Features

- `Symmetric Multi-Processing` 支持多处理器或者多核系统。树莓派使用多核处理器，**此选项以及连带的子选项应全部开启**。

## Boot options

保持默认。

## CPU Power Management

- `CPU Frequency scaling` CPU 频率调整，此实验中无需使用，**关闭全部子选项**。

## Floating point emulation

浮点模拟，至少要选择一个。**只保留 VFP-format floating point maths**.

## Userspace binary formats

- `Write ELF core dumps with partial segments` 用于调试，**改为关闭**。

- `Kernel support for MISC binaries` 允许插入二进制封装层到内核中，直接运行 Java, .NET(Mono-based), Python, Emacs-Lisp 等语言编译的二进制程序时需要它, DOSEMU也需要它。**此实验无需使用，关闭**。

- `Enable core dump support` 用于调试，**关闭**。

## Power management options

`Suspend to RAM and standby` 不需要 suspend 功能，**改为关闭**。

其余选项都默认关闭，保持不变。

## Networking support

本实验无需网络，**改为关闭**。

## Devices Driver

**关闭`Multiple devices driver support (RAID and LVM)`, `Remote Controller support`, `Multimedia support`, `Graphics support`, `Sound card support` 等不需要的功能。**

## Firmware Drivers

保持默认。

## File systems

- **关闭所有不需要的文件系统的支持**，如 NTFS, Reiserfs, F2FS 等，仅保留 ext4, vfat, fuse 等.

- `Quota support` 磁盘配额支持，**关闭**。

## Kernel hacking

kernel 开发和调试相关，**全部关闭**

## Security options

安全增强，**全部关闭**。

**关闭全部**

## Cryptographic API

内核加密API支持，这里的加密算法被广泛的应用于驱动程序通信协议等机制中。为避免不必要的问题，全部保持默认。

## Library routines

库子程序。可能有内核功能依赖。为避免不必要的问题，全部保持默认。

## Virtualization

虚拟化，保持关闭。