#ifndef _VOICE_H
#define _VOICE_H

#define VOICE_1    0   //请选择份数
#define VOICE_2    1   //请选择付款方式
#define VOICE_3    2   //请投币
#define VOICE_4    3   //请刷卡
#define VOICE_5    4   //请将IC卡贴紧刷卡区域
#define VOICE_6    5   //卡内余额不足，请充值
#define VOICE_7    6   //请选择是否打印小票
#define VOICE_8    7   //出餐中请等待
#define VOICE_9    8   //请取餐
#define VOICE_10   9   //请先取走出餐口的餐盒
#define VOICE_11   10  //出餐出错请联系管理员 20没有定义
#define VOICE_12   11  //欢迎下次光临


void PlayMusic( unsigned char Data);
void InitVoice(void);
#endif
