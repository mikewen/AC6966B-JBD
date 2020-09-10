#include "user_fun_cfg.h"
#if USER_VBAT_CHECK_EN
USER_POWER_INFO user_power_io={
    .pro = IO_PORTC_02,
    .ch  = AD_CH_PC2,
    .vol = 0,
};
#endif
// void user_bt_noconect_power_off(void *p_time){
//     static u32 user_power_off_time = -1;

//     if(p_time){
        
//     }

//     // if(-1 == user_power_off_time){
//     //     return;
//     // }

//     if(user_power_off_time)
// }
void user_eq_mode_sw(void){
    #if USER_EQ_FILE_ADD_EQ_TABLE
        static int user_eq_mode = 0;
        user_eq_mode++;
        if(user_eq_mode>EQ_MODE_CUSTOM){
            user_eq_mode = 0;
        }
        printf(">>>> eq mode %d\n",user_eq_mode);
        
        #if USER_EQ_LIVE_UPDATE
        int tp_bass = (eq_tab_custom[USER_EQ_BASS_INDEX].gain)>>20;
        int tp_terble = (eq_tab_custom[USER_EQ_TERBLE_INDEX].gain)>>20;
        #endif

        if(user_eq_mode<EQ_MODE_CUSTOM){
            memcpy(eq_tab_custom,eq_type_tab[user_eq_mode],sizeof(EQ_CFG_SEG)*SECTION_MAX);
        }else{
            memcpy(eq_tab_custom,user_eq_tab_custom,sizeof(EQ_CFG_SEG)*SECTION_MAX);
        }

        #if USER_EQ_LIVE_UPDATE
        eq_tab_custom[USER_EQ_BASS_INDEX].gain = tp_bass<<20;
        eq_tab_custom[USER_EQ_TERBLE_INDEX].gain = tp_terble<<20;
        #endif

        eq_mode_set(EQ_MODE_CUSTOM);
#if TCFG_UI_ENABLE
    ui_set_tmp_menu(MENU_SET_EQ, 1000, user_eq_mode, NULL);
#endif        
    #else
    eq_mode_sw();
    #endif
}

void user_eq_bass_terble_set(int bass,int terble){
    #if USER_EQ_LIVE_UPDATE
    eq_tab_custom[USER_EQ_BASS_INDEX].gain = bass<<20;
    eq_tab_custom[USER_EQ_TERBLE_INDEX].gain = terble<<20;
    
    eq_mode_set(EQ_MODE_CUSTOM);
    #endif
    return;
}

void user_bass_terble_updata(u32 bass_ad,u32 terble_ad){
    static int bass_old =0,terble_old = 0;

    s32 bass  = bass_ad,terble = terble_ad;

    //无效值
    if((512-20) < bass_ad || (512+20) > terble_ad){
        return;
    }

    if((USER_EQ_BASS_AD_MIN-10) > bass_ad){
        bass_ad = USER_EQ_BASS_AD_MIN;
    }

    if((USER_EQ_TERBLE_AD_MIN-10) > terble){
        terble = USER_EQ_TERBLE_AD_MIN;
    }
    

    bass = ((bass-USER_EQ_BASS_AD_MIN)*(USER_EQ_BASS_GAIN_MAX-USER_EQ_BASS_GAIN_MIN))/(USER_EQ_BASS_AD_MAX-USER_EQ_BASS_AD_MIN);
    terble = ((terble-USER_EQ_TERBLE_AD_MIN)*(USER_EQ_TERBLE_GAIN_MAX-USER_EQ_TERBLE_GAIN_MIN))/(USER_EQ_TERBLE_AD_MAX-USER_EQ_TERBLE_AD_MIN);
    bass += USER_EQ_BASS_GAIN_MIN;
    terble += USER_EQ_TERBLE_GAIN_MIN;
        
    if(bass_old!=bass || terble_old!=terble){
        if(bass_old!=bass){
            bass_old = bass;
        }
        if(terble_old!=terble){
            terble_old = terble;
        }

        printf(">>>>> eq bass %d terble %d\n",bass_old,terble_old);
        user_eq_bass_terble_set(bass_old,terble_old);
    }
}

void user_mic_vol_updata(u32 vol){
    static int mic_old =0;
    //ad 512 无效
    if((512+20) > vol || !user_get_mic_status()){
        return;
    }
    
    s32 mic_vol = ((vol-USER_EQ_MIC_AD_MIN)*(USER_EQ_MIC_GAIN_MAX-USER_EQ_MIC_GAIN_MIN))/(USER_EQ_MIC_AD_MAX-USER_EQ_MIC_AD_MIN);
    mic_vol += USER_EQ_MIC_GAIN_MIN;
        
    if(mic_old!=mic_vol ){
        mic_old = mic_vol;
        printf(">>>>> mic_old %d vol %d\n",mic_old,vol);
        audio_mic_set_gain(mic_old);
    }
}

void user_mic_reverb_updata(u32 vol){
    #if (defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE)
    static int rev_old =0;
    //ad 512 无效
    if((512-20) < vol || !user_get_mic_status()){
        return;
    }
    
    s32 rev_vol = ((vol-USER_EQ_REV_AD_MIN)*(USER_EQ_REV_GAIN_MAX-USER_EQ_REV_GAIN_MIN))/(USER_EQ_REV_AD_MAX-USER_EQ_REV_AD_MIN);
    rev_vol += USER_EQ_REV_GAIN_MIN;

    if((USER_EQ_REV_GAIN_MAX-USER_EQ_REV_GAIN_MIN)>=50){
        rev_vol /=10;
        rev_vol *=10;
    }

    if(rev_old!=rev_vol ){
        rev_old = rev_vol;
        printf(">>>>> rev_old %d vol %d\n",rev_old,vol);
        // mic_effect_set_echo_decay(rev_old);
        mic_effect_set_echo_delay(rev_old);
    }
    #endif
}

void user_4ad_fun_features(u32 *vol){
    // printf(">>>> [%03d  %03d %03d  %03d][%03dV  %03dV %03dV  %03dV]\n",vol[0],vol[1],vol[2],vol[3],
    // (320*vol[0])/1024,
    // (320*vol[1])/1024,
    // (320*vol[2])/1024,
    // (320*vol[3])/1024);
    user_bass_terble_updata(vol[USER_EQ_BASS_BIT],vol[USER_EQ_TERBLE_BIT]);
    user_mic_vol_updata(vol[USER_MIC_VOL_BIT]);
    user_mic_reverb_updata(vol[USER_REVER_BOL_BIT]);    
    
}

static u16 auto_time_id = 0;
void user_music_play_finle_number(void *priv){
    int *filenum = (int *)priv;
    auto_time_id = 0;
    printf(">>> kkkk number %d %d\n",*filenum);
    if(*filenum){
        music_player_play_by_number(music_player_get_dev_cur(),*filenum);
        // music_play_msg_post(3, MPLY_MSG_FILE_BY_DEV_FILENUM, (int)music_play_get_cur_dev(), *filenum);
    }
    *filenum = 0;
    printf(">>>> 000 %d\n",*filenum);
}
void user_music_set_file_number(int number){
    #if USER_IR_PLAY_FILE_NUMBER
    static int filenum = 0;
    s32 tp = filenum*10+number;

    printf(">>>>>music_play_get_file_total_file %d\n",music_player_get_file_total());

    if(auto_time_id){
        sys_timeout_del(auto_time_id);
    }

    if((tp>music_player_get_file_total()) || !tp || tp/10000){
        filenum = 0;
        #if TCFG_UI_ENABLE
        ui_set_tmp_menu(MENU_RECODE_ERR, 3000, 0, NULL);
        #endif        
        return;
    }else{
        filenum = tp;
    }

    printf(">>>>> filenum %d\n",filenum);


    #if TCFG_UI_ENABLE
    ui_set_tmp_menu(MENU_FILENUM, 4000, filenum, NULL);
    #endif

    if(filenum>1000){
        user_music_play_finle_number(&filenum);
        filenum = 0;
    }else{
        auto_time_id = sys_timeout_add(&filenum,user_music_play_finle_number,3000);
    }
    #endif
}

void user_vbat_check_init(void){
    #if (defined(USER_VBAT_CHECK_EN) && USER_VBAT_CHECK_EN)
    adc_add_sample_ch(user_power_io.ch);          //注意：初始化AD_KEY之前，先初始化ADC

    gpio_set_die(user_power_io.pro, 0);
    gpio_set_direction(user_power_io.pro, 1);
    gpio_set_pull_down(user_power_io.pro, 0);
    gpio_set_pull_up(user_power_io.pro, 0);
    #endif
    return;
}

u16 user_fun_get_vbat(void){
    u16 tp = 0;
    #if (defined(USER_VBAT_CHECK_EN) && USER_VBAT_CHECK_EN)
    u32 vddio_m = 0;
    user_power_io.vol = adc_get_value(user_power_io.ch);
    vddio_m = (TCFG_LOWPOWER_VDDIOM_LEVEL<=3)?(220+TCFG_LOWPOWER_VDDIOM_LEVEL*20):(300+(TCFG_LOWPOWER_VDDIOM_LEVEL-VDDIOM_VOL_30V)*20);

    // printf("user vbat >>>> %d %dV\n",user_power_io.vol,(vddio_m*2*user_power_io.vol)/0x3ffL);
    user_power_io.vol = (vddio_m*2*user_power_io.vol)/0x3ffL;
    tp = user_power_io.vol;
    return tp;
    #endif
    return tp;
}


void user_fm_vol_set(bool cmd){
#if (defined(USER_FM_MODE_SYS_VOL) && USER_FM_MODE_SYS_VOL)

    static u8 user_fm_sys_vol = 0xff;
    u8 tp_vol = 0;

    if(cmd){//进fm 保存音量
        tp_vol = user_fm_sys_vol = app_audio_get_volume(APP_AUDIO_STATE_MUSIC);
        while (tp_vol > USER_FM_MODE_SYS_VOL){
            app_audio_set_volume(APP_AUDIO_STATE_MUSIC, USER_FM_MODE_SYS_VOL, 1);
            tp_vol = app_audio_get_volume(APP_AUDIO_STATE_MUSIC);
        }
    }else{//退出FM
        while(app_audio_get_volume(APP_AUDIO_STATE_MUSIC)!=user_fm_sys_vol){
            app_audio_set_volume(APP_AUDIO_STATE_MUSIC, user_fm_sys_vol, 1);
        }
    }

#endif
}

//sd pg power
void user_sd_power(u8 cmd){
    #if(defined(USER_SD_POWER_SWITCH_EN) && USER_SD_POWER_SWITCH_EN)
    //绑定引脚高阻态
    #if (USER_SD_POWER_IO != NO_CONFIG_PORT)
    gpio_set_die(USER_SD_POWER_IO, 0);
    gpio_set_direction(USER_SD_POWER_IO, 1);
    gpio_set_pull_down(USER_SD_POWER_IO, 0);
    gpio_set_pull_up(USER_SD_POWER_IO, 0);
    #endif

    sdpg_config(cmd?1:0);
    #endif
}

//关机
void user_power_off(void){
    user_led_io_fun(USER_IO_LED,LED_POWER_OFF);
    pa_ex_fun.strl(PA_POWER_OFF);
    user_sd_power(0);
}

//开机 io口初始化
void user_fun_io_init(void){
    pa_ex_fun.pa_io_init();
    user_sd_power(1);
    user_vbat_check_init();
}

//开机 功能初始化
void user_fun_init(void){
    pa_ex_fun.pa_fun_init();

    user_4ad_check_init(user_4ad_fun_features);

    user_led_io_fun(USER_IO_LED,LED_IO_INIT);
    user_led_io_fun(USER_IO_LED,LED_POWER_ON);

    // #if (USER_IC_MODE == USER_IC_6966B)
    // ex_dev_detect_init(& user_mic_check);
    // #endif
    ex_dev_detect_init(& user_mic_check);


}
