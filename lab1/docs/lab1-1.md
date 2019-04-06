# 树莓派启动过程和原理

## 1. 启动流程


## 

与传统 X86 设备不同，出于降低成本的考虑，树莓派没有使用 BIOS 和 MBR 等通用的引导机制，其启动过程可分为以下四个阶段：

First stage bootloader

树莓派上电后，SoC 中的 bootloader 首先被执行，其作用是挂载 SD 卡上的 FAT32 分区，从而加载下一阶段的 bootloader。这部分程序被固化在 SoC 的 ROM 中，用户无法修改。

Second stage bootloader (bootcode.bin)

这个阶段的 bootloader 会从 SD 卡上检索 GPU 固件，将固件写入 GPU，随后启动 GPU。

GPU firmware (start.elf)

本阶段中，GPU 启动后会检索附加配置文件（config.txt、fixup.dat），根据其内容设置 CPU 运行参数及内存分配情况，随后将用户代码加载至内存，启动 CPU。

User code (kernel8.img)

通常情况下，CPU 启动后便开始执行 kernel8.img 中的指令，初始化操作系统内核，在某些情况下，也可以被 U-BOOT 代替，由 U-BOOT 来加载内核。在树莓派 1 代中，User code 部分被保存在 kernel.img 文件中，2 代中，该文件更名为 kernel7.img，3 代中，该文件更名为 kernel8.img，本项目的全部工作，都在该文件上完成。

综上所述，树莓派上电后的初始化工作大多是在 GPU 中完成的（GPU 内的一颗 RSIC 核心用于完成这些操作），这部分内容不在我们操作系统移植工作的范畴内，故不在此赘述。

目前树莓派的 GPU bootloader 仍未开源，只以二进制形式发布，本项目 bottom 路径下提供了从 raspberrypi/firmware 官方仓库中提取了 bootcode.bin 和 start.elf，直接复制到 SD 卡根目录下即可。

更多关于树莓派启动的资料，可参见 elinux.org 的介绍。