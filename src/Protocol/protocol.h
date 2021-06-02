#ifndef PROTOCOL_H
#define PROTOCOL_H

/*
帧头+指令序号+指令+包序号+有效数据长度+数据+校验和

帧头：AA 55 5A A5 AA 55 5A A5，共8字节
指令序号：4字节，软件启动后从0开始，每次发送后+1
指令：4字节
包序号：4字节，从0开始，不分包的情况下始终为0，数据长度超过每帧长度时，分帧使用
有效数据长度：4字节，本帧后续数据的有效长度，最大值为256
数据：256字节，用不完填充为0
校验和：4字节，从指令序号段开始做校验和之前所有4字节为单位的累加和
 */

/*
 * 声明协议里的一些变量
 */

class MasterSet
{
public:
    enum preview
    {
        PREVIEW_ENABLE = 0x0000000C,
        SYS_INFO       = 0x00000013,

        SAMPLE_LEN    = 0x00000008,
        PREVIEW_RATIO = 0x00000009,
        // 陆地雷达参数
        // 当初没有设计好，不然可以合并
        ALGO_A_POS        = 0x00000022,
        ALGO_A_LEN        = 0x00000023,
        ALGO_B_POS        = 0x00000025,
        ALGO_B_SUM_THRE   = 0x00000026,
        ALGO_B_VALUE_THRE = 0x00000027,

        // 海洋雷达参数
        FIRST_POS         = 0x00000029,
        FIRST_LEN         = 0x0000002a,
        SECOND_POS        = 0x0000002e,
        SECOND_LEN        = 0x00000030,
        SUM_THRE          = 0x0000002b,
        VALUE_THRE        = 0x0000002c,
        COMPRESS_LEN      = 0x00200001,
        COMPRESS_RATIO    = 0x00200002,
        PMT_CONTROL_TIME  = 0x00000032,
        SAMPLE_DELAY_TIME = 0x00000033,
    };
    enum laser
    {
        LASER_PENETRATE = 0x00000001,
        LASER_FREQ      = 0x00000002,
        LASER_ENABLE    = 0x00000028,  // 好像
    };
    enum motor
    {
        MOTOR_PENETRATE = 0x00000003,
    };
    enum update
    {
        ERASE_ADDR = 0x00100000,
        READ_ADDR  = 0x00100001,
        WRITE_ADDR = 0x00100002,
        WRITE_DATA = 0x00100003,
        WRITE_RUN  = 0x00100004,
    };

    enum saveWave
    {
        // 按顺序设置以下命令
        SET_STORE_FILE_NAME = 0x0000000A,  // 硬盘Unit号(4Bytes)，文件名(252Bytes)
        SET_WRITE_DATA_UNIT = 0x00000019,
        STORE_FILE_STATUS   = 0x0000000B,  //使能存储：数据00 00 00 01 停止存储：数据 00 00 00 00

        READ_SSD_UNIT = 0x00000018,
    };

    enum misc
    {
        DA_SET_VALUE    = 0x00000004,
        AD_GET_VALUE    = 0x00000005,
        CAMERA_FREQ_SET = 0x0000000D,
        TRG_MODE        = 0x00700001,
    };

    enum _trg_mode
    {
        INSIDE_TRG  = 0x00,
        OUTSIDE_TRG = 0x01,
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
        FLASH_DATA = 0x80100001,
    };

    enum gps
    {
        GPS_PENETRATE = 0x80000002,
    };
    enum laser
    {
        LASER_PENETRATE = 0x80000003,
    };
    enum motor
    {
        MOTOR_PENETRATE = 0x8000000C,
    };
    enum attitude
    {
        ATTITUDE_PENETRATE = 0x80600001,
    };
    enum saveWave
    {
        RESPONSE_SSD_UNIT = 0x8000000A,  // 硬盘数据
    };

    enum misc
    {
        DA_RETURN_DATA = 0x80000015,
        AD_RETURN_DATA = 0x80000016,
    };
};

#endif
