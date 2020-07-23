#ifndef PROTOCOL_H
#define PROTOCOL_H

class MasterSet
{
public:
    enum preview
    {
        PREVIEW_ENABLE = 0x0000000C,
        SYS_INFO       = 0x00000013,

        SAMPLE_LEN     = 0x00000008,
        PREVIEW_RATIO  = 0x00000009,
        FIRST_POS      = 0x00000029,
        FIRST_LEN      = 0x0000002a,
        SECOND_POS     = 0x0000002e,
        SECOND_LEN     = 0x00000030,
        SUM_THRE       = 0x0000002b,
        MAX_THRE       = 0x0000002c,
        COMPRESS_LEN   = 0x00200001,
        COMPRESS_RATIO = 0x00200002,
    };
    enum update
    {
        ERASE_ADDR = 0x00100000,
        READ_ADDR  = 0x00100001,
        WRITE_ADDR = 0x00100002,
        WRITE_DATA = 0x00100003,
        WRITE_RUN  = 0x00100004,
    };
};

class SlaveUp
{
public:
    enum recv_preview
    {
        SYS_INFO     = 0x80000005,
        COMMAND_CNT  = 0x80000001,
        PREVIEW_DATA = 0x80000006
    };
    enum recv_update
    {
        FLASH_DATA = 0x80100001
    };
};

#endif
