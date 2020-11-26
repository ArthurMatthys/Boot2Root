# Actions to become root

1. Start the VM and get the `boot:` prompt.

    After starting the VM press the `SHIFT` key immediately to get the `boot:` prompt.

2. Start isolinux menu.

    With this prompt you can access all files in the `/cdrom` directory, in this directory you can call kernel images or a file to start the `isolinux` boot loader menu. To open this menu you have to enter this path `/isolinux/vesamenu.c32` in the `boot:` prompt.

3. Start a shell with root privileges.

    When the menu is open press the `Tab` key and add this  `init=/bin/bash` config option to  the line and press enter.

4. You are Root.

    ```bash
    root@BornToSecHackMe:~# whoami 
    root
    root@BornToSecHackMe:~# id
    uid=0(root) gid=0(root) groups=0(root)
    ```
