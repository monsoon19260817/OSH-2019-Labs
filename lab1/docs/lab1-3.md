# 利用汇编实现 LED 闪烁

## 问题探究

### 在这一部分实验中 /dev/sdc1 中除了 kernel7.img （即 led.img）以外的文件哪些是重要的？他们的作用是什么？

bootcode.bin 和 start.elf 是重要的。bootcode.bin 负责检索并载入 GPU 固件 start.elf, start.elf 检索并载入 kernel7.img(led.img).

### 在这一部分实验中 /dev/sdc2 是否被用到？为什么？

未用到。led.img 是运行在裸机上的程序，没有使用操作系统的其他程序、数据等。

### 生成 led.img 的过程中用到了 as, ld, objcopy 这三个工具，他们分别有什么作用，我们平时编译程序会用到其中的哪些？

- `as` 汇编器。把 ARM 汇编码转化成目标文件(object file).

- `ld` 链接器。解析目标文件中未定义的符号引用（如 led.s 中的 `_start`），将其替换成地址，并和库文件连接成一个可执行可链接文件(elf).

- `objcopy` 去除 elf 文件中的非机器码数据，生成可被裸机直接执行的 raw file (img).

普通编译流程中用到 as 和 ld.