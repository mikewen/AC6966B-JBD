#include "user_fun_cfg.h"

#ifdef CONFIG_BOARD_AC6965E
USER_DEV_CHECK user_mic_check ={
    //设置
    .dev_name="mic check",
    .multiplex_sd = 0,//sd cmd复用
    .enable = 1,
    .port = IO_PORTB_01,            //检测IO
    .up = 1,                        // 检测IO上拉使能
    .down = 0,                      // 检测IO下拉使能
    .ad_channel = NO_CONFIG_PORT,   // 检测IO是否使用AD检测
    .ad_vol = 950,                    // AD检测时的阀值

    .scan_time = 50,
    .msg = USER_MSG_SYS_SPK_STATUS, // 发送的消息

    //状态
    .step = 0,
    .stu  = 0,//1
    .online = false,
};
#endif
#ifdef CONFIG_BOARD_AC6966B
USER_DEV_CHECK user_mic_check ={
    //设置
    .dev_name="mic check",
    .multiplex_sd = 1,//sd cmd复用
    .enable = 1,
    .port = IO_PORTC_04,            //检测IO
    .up = 1,                        // 检测IO上拉使能
    .down = 0,                      // 检测IO下拉使能
    .ad_channel = AD_CH_PC4,   // 检测IO是否使用AD检测
    .ad_vol = 950,                    // AD检测时的阀值

    .scan_time = 50,
    .msg =USER_MSG_SYS_SPK_STATUS, // 发送的消息

    //状态
    .step = 0,
    .stu  = 0,//1
    .online = false,
};
#endif