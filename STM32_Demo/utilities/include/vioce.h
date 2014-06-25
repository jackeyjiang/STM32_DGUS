#ifndef _VOICE_H
#define _VOICE_H

#define VOICE_1    0x00   //请选择份数
#define VOICE_2    0x01   //请选择付款方式
#define VOICE_3    0x02   //请投币
#define VOICE_4    0x03   //请刷卡
#define VOICE_5    0x04   //请将IC卡贴紧刷卡区域
#define VOICE_6    0x05   //卡内余额不足，请充值
#define VOICE_7    0x06   //请选择是否打印小票
#define VOICE_8    0x07   //出餐中请等待
#define VOICE_9    0x08   //请取餐
#define VOICE_10   0x09   //请先取走出餐口的餐盒
#define VOICE_11   0x0A   //出餐出错请联系管理员
#define VOICE_12   0X0B   //欢迎下次光临
#define VOICE_13   0X0C   //请选择份数


void PlayMusic(char Data);
void InitVoice(void);
#endif
