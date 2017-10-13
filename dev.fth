: reboot S" reboot 1" GOM ;

: mkfs.sd
    S" ftp backend 1" GOM
    S" ftp mkfs 0" GOM
    s" ftp backend 2" GOM
;

: mkfs.boot
    S" ftp backend 2" GOM
    S" ftp mkfs 0" GOM
;

: ls.sd
    S" ftp ls /sd/" GOM
;

: ls.boot
    S" ftp ls /boot/" GOM
;

: cwod
    S" ftp rm /sd/001W0019.bin" GOM
    S" ftp rm /sd/002W0020.bin" GOM
    S" ftp rm /sd/003W0003.bin" GOM
    S" ftp rm /sd/001W0022.bin" GOM
    S" ftp rm /sd/002W0017.bin" GOM
    S" ftp rm /sd/000W0018.bin" GOM
    S" ftp rm /sd/000W0021.bin" GOM
;
