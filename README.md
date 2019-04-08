# try_os
It was my first time to write an amateur operating system.

This project provides an amateur operating system written by x86 assembly and C language. It can be run on a virtual machine, such as VMware. You will find a .img file, named "a.img". It's a floppy disk image.
If you want to run this image on your Vmware virtual machine, you can follow the steps:

Step1:
Create a virtual machine on VMware. 
Client OS: others
Version: others

Step2:
Create a virtual device: Floppy driver
Choose the a.img (maybe, you should check the file name in the directory. It may contian ".flp" and you should delete that)

Step3:
Just run the virtual machine, and you will see four windows in the virtual machine.

This amateur OS can handle the interrupts from keyboad and mouse. You can drag the window with your mouse and the window you selected will be highlighted. Every window is processing by a individual task (try_os is a multi-task system). In the buttom of the virtual machine, it will print the memory detection information (memory size and free memory size). If you change the memory size from the VMware, try_os can detect it, and the memory information will change when you restart the virtual machine.
