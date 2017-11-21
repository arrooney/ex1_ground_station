INCLUDE strings.fth
INCLUDE gom.fth

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

: mock.start
    S" /dev/ttyUSB2" DFGM-MOCK.INIT
;

: mock.start1
    S" /dev/ttyUSB1" DFGM-MOCK.INIT
;

: mock.read
    S" 001R0001.bin" DROP 24 103588 DFGM-MOCK.READ-RING
;

: mock.val
    DFGM-MOCK.VAL
;

: mnlp.clean
    S" ftp rm /sd/M360206H.bin" GOM
    S" ftp rm /sd/M360206E.bin" GOM
    S" ftp rm /sd/M360206S.bin" GOM
    
    S" ftp rm /sd/M360207H.bin" GOM
    S" ftp rm /sd/M360207E.bin" GOM
    S" ftp rm /sd/M360207S.bin" GOM
    
    S" ftp rm /sd/M360208H.bin" GOM
    S" ftp rm /sd/M360208E.bin" GOM
    S" ftp rm /sd/M360208S.bin" GOM
    
    S" ftp rm /sd/M360209H.bin" GOM
    S" ftp rm /sd/M360209E.bin" GOM
    S" ftp rm /sd/M360209S.bin" GOM

    S" ftp rm /sd/M360210H.bin" GOM
    S" ftp rm /sd/M360210E.bin" GOM
    S" ftp rm /sd/M360210S.bin" GOM

    S" ftp rm /sd/M360211H.bin" GOM
    S" ftp rm /sd/M360211E.bin" GOM
    S" ftp rm /sd/M360211S.bin" GOM

    S" ftp rm /sd/M360212H.bin" GOM
    S" ftp rm /sd/M360212E.bin" GOM
    S" ftp rm /sd/M360212S.bin" GOM

    S" ftp rm /sd/M360213H.bin" GOM
    S" ftp rm /sd/M360213E.bin" GOM
    S" ftp rm /sd/M360213S.bin" GOM

    S" ftp rm /sd/M360214H.bin" GOM
    S" ftp rm /sd/M360214E.bin" GOM
    S" ftp rm /sd/M360214S.bin" GOM

    S" ftp rm /sd/M360215H.bin" GOM
    S" ftp rm /sd/M360215E.bin" GOM
    S" ftp rm /sd/M360215S.bin" GOM

    S" ftp rm /sd/M360216H.bin" GOM
    S" ftp rm /sd/M360216E.bin" GOM
    S" ftp rm /sd/M360216S.bin" GOM

    S" ftp rm /boot/M360206H.bin" GOM
    S" ftp rm /boot/M360206E.bin" GOM
    S" ftp rm /boot/M360206S.bin" GOM
    
    S" ftp rm /boot/M360207H.bin" GOM
    S" ftp rm /boot/M360207E.bin" GOM
    S" ftp rm /boot/M360207S.bin" GOM
    
    S" ftp rm /boot/M360208H.bin" GOM
    S" ftp rm /boot/M360208E.bin" GOM
    S" ftp rm /boot/M360208S.bin" GOM
    
    S" ftp rm /boot/M360209H.bin" GOM
    S" ftp rm /boot/M360209E.bin" GOM
    S" ftp rm /boot/M360209S.bin" GOM

    S" ftp rm /boot/M360210H.bin" GOM
    S" ftp rm /boot/M360210E.bin" GOM
    S" ftp rm /boot/M360210S.bin" GOM

    S" ftp rm /boot/M360211H.bin" GOM
    S" ftp rm /boot/M360211E.bin" GOM
    S" ftp rm /boot/M360211S.bin" GOM

    S" ftp rm /boot/M360212H.bin" GOM
    S" ftp rm /boot/M360212E.bin" GOM
    S" ftp rm /boot/M360212S.bin" GOM

    S" ftp rm /boot/M360213H.bin" GOM
    S" ftp rm /boot/M360213E.bin" GOM
    S" ftp rm /boot/M360213S.bin" GOM

    S" ftp rm /boot/M360214H.bin" GOM
    S" ftp rm /boot/M360214E.bin" GOM
    S" ftp rm /boot/M360214S.bin" GOM

    S" ftp rm /boot/M360215H.bin" GOM
    S" ftp rm /boot/M360215E.bin" GOM
    S" ftp rm /boot/M360215S.bin" GOM

    S" ftp rm /boot/M360216H.bin" GOM
    S" ftp rm /boot/M360216E.bin" GOM
    S" ftp rm /boot/M360216S.bin" GOM
;