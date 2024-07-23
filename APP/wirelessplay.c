#include "wirelessplay.h"
#include "spi.h"
#include "paint.h"
#include "lcd.h"
#include "touch.h"
#include "key.h"
#include "UWB.h" // 包含UWB头文件以访问UWB坐标变量
#include "LoRa.h"

u8 *const wireless_cbtn_caption_tbl[GUI_LANGUAGE_NUM] =
    {
        "重画",
        "重畫",
        "CLEAR",
};

u8 *const wireless_remind_msg_tbl[GUI_LANGUAGE_NUM] =
    {
        "未检测到LoRa模块,请检查!",
        "未檢測到LoRa模塊,請檢查!",
        "No LoRa module, Please Check...",
};

u8 *const wireless_mode_tbl[GUI_LANGUAGE_NUM][2] =
    {
        {
            "发送模式",
            "接收模式",
        },
        {
            "發送模式",
            "接收模式",
        },
        {
            "TX Mode",
            "RX Mode",
        },
};

void Display_Coordinates(uint16_t x, uint16_t y)
{
    char coord_str[20];
    sprintf(coord_str, "X: %d, Y: %d", x, y);
    // 清除中央区域，以显示新的坐标
    LCD_Fill(0, 260, 240, 299, WHITE); // 具体的清除区域和颜色根据你的LCD库调整
    // 在屏幕中央显示坐标字符串
    LCD_ShowString(75, 270, 200, 16, 16, (u8 *)coord_str); // 具体的显示函数根据你的LCD库调整
}

u8 wireless_play(void)
{
    LCD_Init(); // 初始化LCD显示
    TP_Init();  // 初始化触摸屏
    KEY_Init();
    LoRa_ConfigTypeDef loraConfig;

    // 配置LoRa模块参数
    loraConfig.frequency = 433000000; // 设置LoRa频率为433MHz（根据您的实际设置）
    loraConfig.spreadingFactor = 7;   // 设置扩频因子
    loraConfig.bandwidth = 125;       // 设置带宽
    loraConfig.codingRate = 1;        // 设置编码率

    LoRa_Init(&loraConfig); // 初始化LoRa模块
    UWB_Init();             // 初始化UWB模块

    _btn_obj *cbtn = 0;     // 清除按钮控件
    _btn_obj *back_btn = 0; // 返回按钮控件
    u8 res;
    u8 mode = 0;   // 0,发送模式;1,接收模式
    u8 tmp_buf[6]; // buf[0~3]:坐标值;buf[4]:0,正常画图;1,清屏;2,退出.
    u16 x = 0, y = 0;
    u16 lastx = 0XFFFF, lasty = 0XFFFF;
    u8 rval = 0;

    // 先选择模式
    res = app_items_sel((lcddev.width - 180) / 2, (lcddev.height - 152) / 2, 180, 72 + 40 * 2, (u8 **)wireless_mode_tbl[gui_phy.language], 2, (u8 *)&mode, 0X90, (u8 *)APP_MODESEL_CAPTION_TBL[gui_phy.language]); // 2个选择
    if (res == 0)                                                                                                                                                                                                  // 确认键按下了,同时按键正常创建
    {
        gui_fill_rectangle(0, 0, lcddev.width, lcddev.height, LGRAY);                                   // 填充背景色
        gui_fill_rectangle(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight, LGRAY); // 填充背景色

        if (mode == 0)
            LoRa_SetTxMode(); // 设置为发送模式
        else
            LoRa_SetRxMode(); // 设置为接收模式

        app_filebrower((u8 *)wireless_mode_tbl[gui_phy.language][mode], 0X07); // 显示标题

        cbtn = btn_creat(0, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbheight + 8, gui_phy.tbheight - 1, 0, 0x03); // 创建文字按钮
        if (cbtn == NULL)
            rval = 1; // 没有足够内存够分配
        else
        {
            cbtn->caption = (u8 *)wireless_cbtn_caption_tbl[gui_phy.language]; // 重画
            cbtn->font = gui_phy.tbfsize;                                      // 设置新的字体大小
            cbtn->bcfdcolor = WHITE;                                           // 按下时的颜色
            cbtn->bcfucolor = WHITE;                                           // 松开时的颜色
            if (mode == 0)
                btn_draw(cbtn); // 发送模式，需要重画按钮
        }

        // 创建返回按钮，放置在屏幕右下角
        back_btn = btn_creat(lcddev.width - 2 * gui_phy.tbheight - 8, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbheight + 8, gui_phy.tbheight - 1, 0, 0x03);
        if (back_btn == NULL)
            rval = 1; // 没有足够内存够分配
        else
        {
            back_btn->caption = (u8 *)GUI_BACK_CAPTION_TBL[gui_phy.language]; // 返回
            back_btn->font = gui_phy.tbfsize;                                 // 设置新的字体大小
            back_btn->bcfdcolor = WHITE;                                      // 按下时的颜色
            back_btn->bcfucolor = WHITE;                                      // 松开时的颜色
            btn_draw(back_btn);                                               // 绘制返回按钮
        }

        tmp_buf[4] = tp_dev.touchtype & 0X00; // 得到触摸屏类型 (test)
        // tmp_buf[4]:
        // b3~0:0,不做任何处理
        //      1,画点
        //      2,清屏
        //      3,退出
        // b4~6:保留
        // b7:0,电阻屏;1,电容屏

        while (rval == 0)
        {
            u8 key_val = KEY_Scan(1); // 按键扫描
            if (key_val == KEY0_PRES) // 检测到KEY0按下
            {
                return 0; // 返回
            }
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); // 得到按键键值
            tmp_buf[4] &= 0X00;                     // 清除原来的设置
            res = btn_check(cbtn, &in_obj);         // 检查重画按钮
            if (res)                                // 重画按钮有有效操作
            {
                if (((cbtn->sta & 0X80) == 0)) // 按钮状态改变了
                {
                    //                    tmp_buf[0] = uwb_x >> 8;
                    //                    tmp_buf[1] = uwb_x & 0xFF;
                    //                    tmp_buf[2] = uwb_y >> 8;
                    //                    tmp_buf[3] = uwb_y & 0xFF;
                    tmp_buf[4] |= 0X02; // 功能2,清屏
                    //LoRa_SendPacket(1, uwb_x, uwb_y); // 发送UWB获取的坐标
                }
            }
            res = btn_check(back_btn, &in_obj); // 检查返回按钮
            if (res)                            // 返回按钮有有效操作
            {
                if (((back_btn->sta & 0X80) == 0)) // 按钮状态改变了
                {
//                    // 显示UWB坐标
//                    void UWB_GetCoordinates();
////                    tmp_buf[0] = uwb_x >> 8;
////                    tmp_buf[1] = uwb_x & 0xFF;
////                    tmp_buf[2] = uwb_y >> 8;
////                    tmp_buf[3] = uwb_y & 0xFF;
//                    Display_Coordinates(uwb_x, uwb_y);
//                    LoRa_SendPacket(1, uwb_x, uwb_y); // 发送UWB获取的坐标
					// 从LoRa模块接收坐标数据
					uint16_t length = 0;
					if (LoRa_ReceivePacket(tmp_buf, &length)) // 接收到数据
					{
						u16 lora_x = tmp_buf[0] << 8 | tmp_buf[1];
						u16 lora_y = tmp_buf[2] << 8 | tmp_buf[3];

						// 显示LoRa接收到的坐标
						Display_Coordinates(lora_x, lora_y);

						// 发送LoRa接收到的坐标
						LoRa_SendPacket(1, lora_x, lora_y);
					}
                }
            }
            if (tp_dev.sta & TP_PRES_DOWN) // 触摸屏被按下
            {
                if (tp_dev.y[0] < (lcddev.height - gui_phy.tbheight) && tp_dev.y[0] > (gui_phy.tbheight + 1)) // 在画图区域内
                {
                    x = tp_dev.x[0];
                    y = tp_dev.y[0];
                    tmp_buf[0] = tp_dev.x[0] >> 8;
                    tmp_buf[1] = tp_dev.x[0] & 0xFF;
                    tmp_buf[2] = tp_dev.y[0] >> 8;
                    tmp_buf[3] = tp_dev.y[0] & 0xFF;
                    tmp_buf[4] |= 0X01; // 功能为1,正常画图

                    // 显示坐标
                    Display_Coordinates(x * 1.34, (y - 20) * 1.34);

                    // LoRa_SendPacket(1, tp_dev.x[0], tp_dev.y[0]); // 发送数据
                    LoRa_SendPacket(1, x * 1.34, (y - 20) * 1.34); // 发送数据
                                                                   // LoRa_SendPacket(1,1,1);
                }
            }

            if (mode == 1) // 接收模式
            {
                uint16_t length = 0;
                if (LoRa_ReceivePacket(tmp_buf, &length)) // 接收到数据
                {
                    x = tmp_buf[0] << 8 | tmp_buf[1];
                    y = tmp_buf[2] << 8 | tmp_buf[3];
                }
            }

            // 处理数据包功能
            if (tmp_buf[4] & 0X7F) // 检查功能
            {
                switch (tmp_buf[4] & 0X7F)
                {
                case 1:                  // 画点
                    if (lastx == 0XFFFF) // 第一次画点
                    {
                        lastx = x;
                        lasty = y;
                    }
                    gui_draw_bline(lastx, lasty, x, y, 2, RED); // 画线
                    lastx = x;
                    lasty = y;
                    break;
                case 2:                                                                                                 // 清屏
                    gui_fill_rectangle(0, gui_phy.tbheight, lcddev.width, lcddev.height - 2 * gui_phy.tbheight, LGRAY); // 填充背景色
                    break;
                case 3:       // 退出
                    rval = 1; // 结束循环
                    break;
                }
            }
            else
                lastx = 0XFFFF;

            delay_ms(100); // 空闲延时
        }

        if (cbtn != NULL)
            btn_delete(cbtn); // 删除按钮
        if (back_btn != NULL)
            btn_delete(back_btn); // 删除返回按钮
    }

    return 0;
}
