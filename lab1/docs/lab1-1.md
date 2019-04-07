# 树莓派启动过程和原理

## 启动流程

树莓派启动过程可分为以下几个阶段：

1. 上电后，ARM CPU 核心处于关闭状态，GPU 核心处于开启状态。

2. GPU 加载固化在 SoC ROM 中的 bootloader 并执行。此过程会检索并加载 SD 卡 FAT32分区 中的 bootcode.bin.

3. bootcode.bin 被执行，检索并加载 GPU 固件 start.elf.

4. GPU 读取 kernel7.img 并载入处理器分配的共享内存中， 同时读取的还有相应的配置文件 config.txt, cmdline.txt.

5. CPU 执行 kernel 代码，载入 init 程序，完成其他启动工作。

## 与主流计算机的不同之处

主流计算机启动过程为:

BIOS/UEFI -> bootloader（Linux on x86 中常见的为 GRUB） -> kernel -> init/systemd

整个过程都是 CPU 负责运行。

而树莓派没有使用流行的 BIOS/UEFI 体系，并且使用 GPU 运行 kernel 加载之前的程序。此做法更类似于嵌入式设备。

## 所用的文件系统

所用到的文件系统有

- `FAT32` 存放启动所用到的 bootcode.bin, start.elf, cmdline.txt, config.txt 等，以及内核文件 kernel7.img.

- `EXT4` 存放操作系统运行时作用的程序、数据和其他文件。