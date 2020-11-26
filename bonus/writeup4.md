With any machine that you have root access, you can (once you're root):
- Mount the .iso
`mount -o loop BornToSecHackMe-v1.1.iso /mnt/Boot2Root`
Then go to any directory (go to /tmp for exemple), then :
`unsquashfs -f -d vm_content /mnt/Boot2Root/casper/filesystem.squashfs`
And now, you're root and you have access to everything inside the .iso.
