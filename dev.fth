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
    S" ftp rm /boot/M360206H.bin" GOM
    S" ftp rm /boot/M360206S.bin" GOM
    S" ftp rm /boot/M360206E.bin" GOM
    S" ftp rm /boot/M360207H.bin" GOM
    S" ftp rm /boot/M360207S.bin" GOM
    S" ftp rm /boot/M360207E.bin" GOM
    S" ftp rm /boot/M360208H.bin" GOM
    S" ftp rm /boot/M360208S.bin" GOM
    S" ftp rm /boot/M360208E.bin" GOM
    S" ftp rm /boot/M360209H.bin" GOM
    S" ftp rm /boot/M360209S.bin" GOM
    S" ftp rm /boot/M360209E.bin" GOM
;
