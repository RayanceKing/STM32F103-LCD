#ifndef __WIRELESSPLAY_H
#define __WIRELESSPLAY_H

#include "LoRa.h"
#include "lcd.h"
#include "touch.h"
#include "spi.h"	
#include "paint.h"	

extern u8 *const wireless_cbtn_caption_tbl[GUI_LANGUAGE_NUM];
extern u8 *const wireless_remind_msg_tbl[GUI_LANGUAGE_NUM];
extern u8 *const wireless_mode_tbl[GUI_LANGUAGE_NUM][2];

u8 wireless_play(void);

#endif /* __WIRELESSPLAY_H */
