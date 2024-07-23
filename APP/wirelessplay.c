#include "wirelessplay.h"
#include "spi.h"
#include "paint.h"
#include "lcd.h"
#include "touch.h"
#include "key.h"
#include "UWB.h" // ����UWBͷ�ļ��Է���UWB�������
#include "LoRa.h"

u8 *const wireless_cbtn_caption_tbl[GUI_LANGUAGE_NUM] =
    {
        "�ػ�",
        "�خ�",
        "CLEAR",
};

u8 *const wireless_remind_msg_tbl[GUI_LANGUAGE_NUM] =
    {
        "δ��⵽LoRaģ��,����!",
        "δ�z�y��LoRaģ�K,Ո�z��!",
        "No LoRa module, Please Check...",
};

u8 *const wireless_mode_tbl[GUI_LANGUAGE_NUM][2] =
    {
        {
            "����ģʽ",
            "����ģʽ",
        },
        {
            "�l��ģʽ",
            "����ģʽ",
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
    // ���������������ʾ�µ�����
    LCD_Fill(0, 260, 240, 299, WHITE); // ���������������ɫ�������LCD�����
    // ����Ļ������ʾ�����ַ���
    LCD_ShowString(75, 270, 200, 16, 16, (u8 *)coord_str); // �������ʾ�����������LCD�����
}

u8 wireless_play(void)
{
    LCD_Init(); // ��ʼ��LCD��ʾ
    TP_Init();  // ��ʼ��������
    KEY_Init();
    LoRa_ConfigTypeDef loraConfig;

    // ����LoRaģ�����
    loraConfig.frequency = 433000000; // ����LoRaƵ��Ϊ433MHz����������ʵ�����ã�
    loraConfig.spreadingFactor = 7;   // ������Ƶ����
    loraConfig.bandwidth = 125;       // ���ô���
    loraConfig.codingRate = 1;        // ���ñ�����

    LoRa_Init(&loraConfig); // ��ʼ��LoRaģ��
    UWB_Init();             // ��ʼ��UWBģ��

    _btn_obj *cbtn = 0;     // �����ť�ؼ�
    _btn_obj *back_btn = 0; // ���ذ�ť�ؼ�
    u8 res;
    u8 mode = 0;   // 0,����ģʽ;1,����ģʽ
    u8 tmp_buf[6]; // buf[0~3]:����ֵ;buf[4]:0,������ͼ;1,����;2,�˳�.
    u16 x = 0, y = 0;
    u16 lastx = 0XFFFF, lasty = 0XFFFF;
    u8 rval = 0;

    // ��ѡ��ģʽ
    res = app_items_sel((lcddev.width - 180) / 2, (lcddev.height - 152) / 2, 180, 72 + 40 * 2, (u8 **)wireless_mode_tbl[gui_phy.language], 2, (u8 *)&mode, 0X90, (u8 *)APP_MODESEL_CAPTION_TBL[gui_phy.language]); // 2��ѡ��
    if (res == 0)                                                                                                                                                                                                  // ȷ�ϼ�������,ͬʱ������������
    {
        gui_fill_rectangle(0, 0, lcddev.width, lcddev.height, LGRAY);                                   // ��䱳��ɫ
        gui_fill_rectangle(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight, LGRAY); // ��䱳��ɫ

        if (mode == 0)
            LoRa_SetTxMode(); // ����Ϊ����ģʽ
        else
            LoRa_SetRxMode(); // ����Ϊ����ģʽ

        app_filebrower((u8 *)wireless_mode_tbl[gui_phy.language][mode], 0X07); // ��ʾ����

        cbtn = btn_creat(0, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbheight + 8, gui_phy.tbheight - 1, 0, 0x03); // �������ְ�ť
        if (cbtn == NULL)
            rval = 1; // û���㹻�ڴ湻����
        else
        {
            cbtn->caption = (u8 *)wireless_cbtn_caption_tbl[gui_phy.language]; // �ػ�
            cbtn->font = gui_phy.tbfsize;                                      // �����µ������С
            cbtn->bcfdcolor = WHITE;                                           // ����ʱ����ɫ
            cbtn->bcfucolor = WHITE;                                           // �ɿ�ʱ����ɫ
            if (mode == 0)
                btn_draw(cbtn); // ����ģʽ����Ҫ�ػ���ť
        }

        // �������ذ�ť����������Ļ���½�
        back_btn = btn_creat(lcddev.width - 2 * gui_phy.tbheight - 8, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbheight + 8, gui_phy.tbheight - 1, 0, 0x03);
        if (back_btn == NULL)
            rval = 1; // û���㹻�ڴ湻����
        else
        {
            back_btn->caption = (u8 *)GUI_BACK_CAPTION_TBL[gui_phy.language]; // ����
            back_btn->font = gui_phy.tbfsize;                                 // �����µ������С
            back_btn->bcfdcolor = WHITE;                                      // ����ʱ����ɫ
            back_btn->bcfucolor = WHITE;                                      // �ɿ�ʱ����ɫ
            btn_draw(back_btn);                                               // ���Ʒ��ذ�ť
        }

        tmp_buf[4] = tp_dev.touchtype & 0X00; // �õ����������� (test)
        // tmp_buf[4]:
        // b3~0:0,�����κδ���
        //      1,����
        //      2,����
        //      3,�˳�
        // b4~6:����
        // b7:0,������;1,������

        while (rval == 0)
        {
            u8 key_val = KEY_Scan(1); // ����ɨ��
            if (key_val == KEY0_PRES) // ��⵽KEY0����
            {
                return 0; // ����
            }
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); // �õ�������ֵ
            tmp_buf[4] &= 0X00;                     // ���ԭ��������
            res = btn_check(cbtn, &in_obj);         // ����ػ���ť
            if (res)                                // �ػ���ť����Ч����
            {
                if (((cbtn->sta & 0X80) == 0)) // ��ť״̬�ı���
                {
                    //                    tmp_buf[0] = uwb_x >> 8;
                    //                    tmp_buf[1] = uwb_x & 0xFF;
                    //                    tmp_buf[2] = uwb_y >> 8;
                    //                    tmp_buf[3] = uwb_y & 0xFF;
                    tmp_buf[4] |= 0X02; // ����2,����
                    //LoRa_SendPacket(1, uwb_x, uwb_y); // ����UWB��ȡ������
                }
            }
            res = btn_check(back_btn, &in_obj); // ��鷵�ذ�ť
            if (res)                            // ���ذ�ť����Ч����
            {
                if (((back_btn->sta & 0X80) == 0)) // ��ť״̬�ı���
                {
//                    // ��ʾUWB����
//                    void UWB_GetCoordinates();
////                    tmp_buf[0] = uwb_x >> 8;
////                    tmp_buf[1] = uwb_x & 0xFF;
////                    tmp_buf[2] = uwb_y >> 8;
////                    tmp_buf[3] = uwb_y & 0xFF;
//                    Display_Coordinates(uwb_x, uwb_y);
//                    LoRa_SendPacket(1, uwb_x, uwb_y); // ����UWB��ȡ������
					// ��LoRaģ�������������
					uint16_t length = 0;
					if (LoRa_ReceivePacket(tmp_buf, &length)) // ���յ�����
					{
						u16 lora_x = tmp_buf[0] << 8 | tmp_buf[1];
						u16 lora_y = tmp_buf[2] << 8 | tmp_buf[3];

						// ��ʾLoRa���յ�������
						Display_Coordinates(lora_x, lora_y);

						// ����LoRa���յ�������
						LoRa_SendPacket(1, lora_x, lora_y);
					}
                }
            }
            if (tp_dev.sta & TP_PRES_DOWN) // ������������
            {
                if (tp_dev.y[0] < (lcddev.height - gui_phy.tbheight) && tp_dev.y[0] > (gui_phy.tbheight + 1)) // �ڻ�ͼ������
                {
                    x = tp_dev.x[0];
                    y = tp_dev.y[0];
                    tmp_buf[0] = tp_dev.x[0] >> 8;
                    tmp_buf[1] = tp_dev.x[0] & 0xFF;
                    tmp_buf[2] = tp_dev.y[0] >> 8;
                    tmp_buf[3] = tp_dev.y[0] & 0xFF;
                    tmp_buf[4] |= 0X01; // ����Ϊ1,������ͼ

                    // ��ʾ����
                    Display_Coordinates(x * 1.34, (y - 20) * 1.34);

                    // LoRa_SendPacket(1, tp_dev.x[0], tp_dev.y[0]); // ��������
                    LoRa_SendPacket(1, x * 1.34, (y - 20) * 1.34); // ��������
                                                                   // LoRa_SendPacket(1,1,1);
                }
            }

            if (mode == 1) // ����ģʽ
            {
                uint16_t length = 0;
                if (LoRa_ReceivePacket(tmp_buf, &length)) // ���յ�����
                {
                    x = tmp_buf[0] << 8 | tmp_buf[1];
                    y = tmp_buf[2] << 8 | tmp_buf[3];
                }
            }

            // �������ݰ�����
            if (tmp_buf[4] & 0X7F) // ��鹦��
            {
                switch (tmp_buf[4] & 0X7F)
                {
                case 1:                  // ����
                    if (lastx == 0XFFFF) // ��һ�λ���
                    {
                        lastx = x;
                        lasty = y;
                    }
                    gui_draw_bline(lastx, lasty, x, y, 2, RED); // ����
                    lastx = x;
                    lasty = y;
                    break;
                case 2:                                                                                                 // ����
                    gui_fill_rectangle(0, gui_phy.tbheight, lcddev.width, lcddev.height - 2 * gui_phy.tbheight, LGRAY); // ��䱳��ɫ
                    break;
                case 3:       // �˳�
                    rval = 1; // ����ѭ��
                    break;
                }
            }
            else
                lastx = 0XFFFF;

            delay_ms(100); // ������ʱ
        }

        if (cbtn != NULL)
            btn_delete(cbtn); // ɾ����ť
        if (back_btn != NULL)
            btn_delete(back_btn); // ɾ�����ذ�ť
    }

    return 0;
}
