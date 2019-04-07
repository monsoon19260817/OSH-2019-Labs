.section .init
.global _start
_start:


ldr r0,  =0x3F200000  @ GPIO 控制器内存地址
ldr r10, =0x3F003000  @ 时钟所在地址
ldr r11, =0x1E8480    @ 2000000

loop:

@ 设置 ACT LED 为输出模式
mov r1, #1
lsl r1, #27
str r1, [r0, #8]
@ 亮
lsl r1, #2
str r1, [r0, #28]

ldr r3, [r10, #4]   
add r3, r3, r11
loop1:  @ 延迟 2s
ldr r4, [r10, #4]
cmp r3, r4
bne loop1

@ 暗
mov r1, #0
str r1, [r0, #8]

ldr r3, [r10, #4]
add r3, r3, r11
loop2:  @ 延迟 2s
ldr r4, [r10, #4]
cmp r3, r4
bne loop2

b loop
