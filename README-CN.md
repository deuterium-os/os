[ 中文 | [English](README.md) ]
# Deuterium OS
Deuterium OS 是一个从头编写的通用操作系统
## 构建
### 先决条件
要构建 Deuterium OS，你需要一套 GCC 交叉编译工具链。预编译的工具链可以在[此处](https://wiki.osdev.org/GCC_Cross_Compiler#Prebuilt_Toolchains)找到。如果你想从源码编译工具链，你需要阅读此[文档](https://wiki.osdev.org/GCC_Cross_Compiler).。
你需要一些工具，它们在下表列出:
Name|Source Code|Debian(apt)|RedHat(dnf)|Gentoo(emerge)|
----|-----------|-----------|-----------|--------------|
[NASM][1]|[NASM][2]|nasm|nasm|dev-lang/nasm
[QEMU][3]|[QEMU][4]|qemu-system|qemu-system-*|app-emulation/qemu
mkbootimg|[mkbootimg][5]|N/A|N/A|N/A

[1]: https://nasm.us/ "Netwide Assembler 官网"
[2]: https://github.com/netwide-assembler/nasm.git
[3]: https://www.qemu.org/ "QEMU 官网"
[4]: https://gitlab.com/qemu-project/qemu.git
[5]: https://gitlab.com/bztsrc/bootboot/-/tree/master/mkbootimg "mkbootimg的源代码。它是 BOOTBOOT 源代码 GitLab 仓库的一部分。"
### 构建
运行 `make` 以构建操作系统。随后将会生成一个名为 `deuterium-os.img` 的原始磁盘镜像。你可以将它直接写入硬盘或U盘以启动操作系统。运行 `make run` 以在QEMU上启动操作系统。
如果你使用VSCode进行开发，你也可以运行 VSCode 中的 `Launch with GDB` 配置以构建操作系统并在QEMU上运行。
