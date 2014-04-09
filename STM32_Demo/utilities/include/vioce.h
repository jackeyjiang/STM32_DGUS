#ifndef _VOICE_H
#define _VOICE_H

#define VOICE_1    0   //请选择份数，并付款
#define VOICE_2    1   //出餐中请等待
#define VOICE_3    2   //请取餐
#define VOICE_4    3   //欢迎下次光临
#define VOICE_5    4   //请先取走出餐口的餐盒
#define VOICE_6    5   //卡内余额不足，请充值
#define VOICE_7    6   //请投纸币
#define VOICE_8    7   //清按确认货返回


void PlayMusic( unsigned char );
void InitVoice(void);
#endif
