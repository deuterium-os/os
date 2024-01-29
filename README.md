# Deuterium OS
Deuterium OS is a general purpose operating system written from scratch. 
## Building
### Prerequisites
To build Deuterium OS you'll need a GCC cross-compiler toolchain. Prebuilt toolchain can be found at [Here](https://wiki.osdev.org/GCC_Cross_Compiler#Prebuilt_Toolchains). If you want to build the toolchain from sources, you'll need to read this [Document](https://wiki.osdev.org/GCC_Cross_Compiler).
You'll need some tools too. They are listed below:
Name|Source Code|Debian(apt)
----|-----------|-----------
[NASM][1]|[NASM][2]|nasm
[QEMU][3]|[QEMU][4]|qemu-system
mkbootimg|[mkbootimg][5]|N/A

[1]: https://nasm.us/ "The offical website of Netwide Assembler"
[2]: https://github.com/netwide-assembler/nasm.git
[3]: https://www.qemu.org/ "The offical website of QEMU"
[4]: https://gitlab.com/qemu-project/qemu.git
[5]: https://gitlab.com/bztsrc/bootboot/-/tree/master/mkbootimg "The source code of mkbootimg. It's a part of The GitLab Repository of BOOTBOOT."
### Build
Run `make` to build the OS. Then a raw disk image named `deuterium-os.img` will be generated. You can apply it directly to a hard drive or USB stick to boot. Run `make run` to boot the OS on QEMU.  
You also can run the `Launch with GDB` configuration in Visual Studio Code to build and boot the OS on QEMU simply.