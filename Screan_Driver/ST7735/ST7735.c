#include "gpio.h"
#include "spi.h"
#include "ST7735.h"
#include "ST7735_Data.h"
#include <string.h>
#include <math.h>
#include "IT_Freq.h"
#include "stdio.h"
#include <stdarg.h>
#include "usart.h"
ST7735 st7735;
uint8_t  Data_E0[]={0x04,0x22,0x07,0x0A,0x2E,0x30,0x25,0x2A,0x28,0x26,0x2E,0x3A,0x00,0x01,0x03,0x13};//初始化配置数组
uint8_t  Data_E1[]={0x04,0x16,0x06,0x0D,0x2D,0x26,0x23,0x27,0x27,0x25,0x2D,0x3B,0x00,0x01,0x04,0x13};//初始化配置数组

/*基础驱动函数，移植时需适配以下函数*/
/*#################################################################################*/
void ST7735_Start_SPI(void)//开启SPI通讯
{
    HAL_GPIO_WritePin(ST7735_CS_GPIOX,ST7735_CS,GPIO_PIN_RESET);//拉低ST7735_CS
}
void ST7735_Close_SPI(void)//停止SPI通讯
{
    HAL_GPIO_WritePin(ST7735_CS_GPIOX,ST7735_CS,GPIO_PIN_SET);//拉高ST7735_CS
}
void ST7735_Data_Falg(void)//置发送数据信号
{
	HAL_GPIO_WritePin(ST7735_DC_GPIOX,ST7735_DC,GPIO_PIN_SET);//拉高ST7735_DC
}
void ST7735_Command_Falg(void)//置发送命令信号
{
	HAL_GPIO_WritePin(ST7735_DC_GPIOX,ST7735_DC,GPIO_PIN_RESET);//拉低ST7735_DC
}
void ST7735_Reset(void)//复位屏幕
{
    HAL_GPIO_WritePin(ST7735_RES_GPIOX,ST7735_RES,GPIO_PIN_RESET);//拉低RES
    HAL_Delay(1);
    HAL_GPIO_WritePin(ST7735_RES_GPIOX,ST7735_RES,GPIO_PIN_SET);//拉高RES
    HAL_Delay(120);
}
void ST7735_Write_Data(uint8_t Data)//向ST7735写8位数据
{
    ST7735_Start_SPI();
	ST7735_Data_Falg();
    HAL_SPI_Transmit(ST7735_SPI,&Data,sizeof(Data),10);//SPI发送数据
    ST7735_Close_SPI();
}

void ST7735_Write_Command(uint8_t Command)//向ST7735写命令
{
    ST7735_Start_SPI();
	ST7735_Command_Falg();
    HAL_SPI_Transmit(ST7735_SPI,&Command,sizeof(Command),10);//SPI发送数据
}
void ST7735_Write_Data_DMA(uint8_t Cmd,uint8_t *Data)//以DMA模式向ST7735写数据
{
    ST7735_Write_Command(Cmd);
    ST7735_Start_SPI();
	ST7735_Data_Falg();
    st7735.ST7735_Mode=Mode_Single_Data;
    HAL_SPI_Transmit_DMA(ST7735_SPI,Data,sizeof(Data));//SPI的DMA模式发送数据
}
/*#################################################################################*/

void ST7735_Write_Data_U16(uint16_t Data)//向ST7735写16位数据
{
    ST7735_Write_Data(Data>>8);
    ST7735_Write_Data(Data&0xFF); 
}
void ST7735_Unsleep(void)//退出休眠
{
    ST7735_Write_Command(Cmd_Unsleep);
    HAL_Delay(120);
}
void ST7735_Sleep(void)//进入休眠
{
    ST7735_Write_Command(Cmd_Sleep);
}

void ST7735_Set_RGB(uint8_t Data_RGB)//设置RGB模式
{
    ST7735_Write_Command(Cmd_RGB);
    ST7735_Write_Data(Data_RGB);
}

void ST7735_Set_Scan_Dir(uint8_t Scan_Dir)//设置屏幕扫描模式
{
    ST7735_Write_Command(Cmd_Scan_Dir);
    ST7735_Write_Data(Scan_Dir);
}
void ST7735_Set_View_Window(uint16_t X_Start,uint16_t Y_Start,uint16_t X_End,uint16_t Y_End)//设置屏幕矩形范围
{
	X_Start+=1;
	X_End+=1;
	Y_Start+=26;
	Y_End+=26;
    ST7735_Write_Command(Cmd_Set_List);
    ST7735_Write_Data((X_Start>>8));
    ST7735_Write_Data(X_Start&0xFF);
    ST7735_Write_Data((X_End>>8));
    ST7735_Write_Data(X_End&0xFF);

    ST7735_Write_Command(Cmd_Set_Row);
    ST7735_Write_Data((Y_Start>>8));
    ST7735_Write_Data(Y_Start&0xFF);
    ST7735_Write_Data((Y_End>>8));
    ST7735_Write_Data(Y_End&0xFF);

    ST7735_Write_Command(Cmd_Write_Mem);
}

void ST7735_Clear_View(uint16_t RGB_Color)//清屏
{
    ST7735_Fill_Color(0,0,159,79,RGB_Color);
}
void ST7735_Fill_Color(uint8_t X_Start,uint8_t Y_Start,uint8_t X_End,uint8_t Y_End,uint16_t RGB_Color)//填充矩形色块
{
	st7735.X_Len=X_End-X_Start+1;
    st7735.Y_All_Flag=Y_End-Y_Start+1;
    for (uint16_t i=0;i<st7735.X_Len;i++)
    {
        st7735.X_Buffer[2*i]=RGB_Color>>8;
        st7735.X_Buffer[2*i+1]=RGB_Color&0xFF;
    }
    ST7735_Set_View_Window(X_Start,Y_Start,X_End,Y_End);
    ST7735_Start_SPI();
    HAL_GPIO_WritePin(ST7735_DC_GPIOX,ST7735_DC,GPIO_PIN_SET);
    st7735.ST7735_Mode=Mode_Fill_Color;
    HAL_SPI_Transmit_DMA(ST7735_SPI,st7735.X_Buffer,2*st7735.X_Len);
}
void ST7735_Fill_Color_LVGL(uint8_t X_Start,uint8_t Y_Start,uint8_t X_End,uint8_t Y_End,uint8_t *RGB_Color)//为LVGL提供的填充色块函数
{
	uint16_t width=X_End-X_Start+1;
	uint16_t height = Y_End - Y_Start + 1;
	uint32_t total_pixels = width * height;

    ST7735_Set_View_Window(X_Start,Y_Start,X_End,Y_End);
    ST7735_Start_SPI();
    HAL_GPIO_WritePin(ST7735_DC_GPIOX,ST7735_DC,GPIO_PIN_SET);
    st7735.ST7735_Mode=Mode_Single_Data;
    HAL_SPI_Transmit_DMA(ST7735_SPI,RGB_Color,2*total_pixels);
}

void ST7735_ShowImage(uint8_t X_Start,uint8_t Y_Start,uint8_t Width, uint8_t Height,const uint8_t* Image_Data,uint16_t Font_Color,uint16_t Back_Color)//发送图像
{
	uint16_t Color_Index=0;
    for(uint8_t i=0;i<Height*Width/8;i++)
    {
        for(uint8_t j=0;j<8;j++)
        {
            if((Image_Data[i]&(0x01<<j)))
            {
                st7735.Chinese_Buffer[2*Color_Index]=Font_Color>>8;
                st7735.Chinese_Buffer[2*Color_Index+1]=Font_Color&0xFF;
                Color_Index++;
            }
            else
            {
                st7735.Chinese_Buffer[2*Color_Index]=Back_Color>>8;
                st7735.Chinese_Buffer[2*Color_Index+1]=Back_Color&0xFF;
               Color_Index++;
            }
        }
    }
	Color_Index=0;
    ST7735_Set_View_Window(X_Start,Y_Start,X_Start+Width-1,Y_Start+Height-1);
    ST7735_Start_SPI();
    HAL_GPIO_WritePin(ST7735_DC_GPIOX,ST7735_DC,GPIO_PIN_SET);
    st7735.ST7735_Mode=Mode_Single_Data;
    HAL_SPI_Transmit_DMA(ST7735_SPI,st7735.Chinese_Buffer,Width*Height*2);
}

void ST7735_ShowChar(uint8_t X, uint8_t Y, char Char,uint16_t Font_Color,uint16_t Back_Color)
{
    ST7735_ShowImage(X,Y,8,16,OLED_F8x16[Char-' '],Font_Color,Back_Color);
}

void ST7735_ShowString(int16_t X, int16_t Y, char *String,uint16_t Font_Color,uint16_t Back_Color)
{
	uint16_t i = 0;
	char SingleChar[5];
	uint8_t CharLength = 0;
	uint16_t XOffset = 0;
	uint16_t pIndex;
	
	while (String[i] != '\0')	//遍历字符串
	{
		
#ifdef OLED_CHARSET_UTF8						//定义字符集为UTF8
		/*此段代码的目的是，提取UTF8字符串中的一个字符，转存到SingleChar子字符串中*/
		/*判断UTF8编码第一个字节的标志位*/
		if ((String[i] & 0x80) == 0x00)			//第一个字节为0xxxxxxx
		{
			CharLength = 1;						//字符为1字节
			SingleChar[0] = String[i ++];		//将第一个字节写入SingleChar第0个位置，随后i指向下一个字节
			SingleChar[1] = '\0';				//为SingleChar添加字符串结束标志位
		}
		else if ((String[i] & 0xE0) == 0xC0)	//第一个字节为110xxxxx
		{
			CharLength = 2;						//字符为2字节
			SingleChar[0] = String[i ++];		//将第一个字节写入SingleChar第0个位置，随后i指向下一个字节
			if (String[i] == '\0') {break;}		//意外情况，跳出循环，结束显示
			SingleChar[1] = String[i ++];		//将第二个字节写入SingleChar第1个位置，随后i指向下一个字节
			SingleChar[2] = '\0';				//为SingleChar添加字符串结束标志位
		}
		else if ((String[i] & 0xF0) == 0xE0)	//第一个字节为1110xxxx
		{
			CharLength = 3;						//字符为3字节
			SingleChar[0] = String[i ++];
			if (String[i] == '\0') {break;}
			SingleChar[1] = String[i ++];
			if (String[i] == '\0') {break;}
			SingleChar[2] = String[i ++];
			SingleChar[3] = '\0';
		}
		else if ((String[i] & 0xF8) == 0xF0)	//第一个字节为11110xxx
		{
			CharLength = 4;						//字符为4字节
			SingleChar[0] = String[i ++];
			if (String[i] == '\0') {break;}
			SingleChar[1] = String[i ++];
			if (String[i] == '\0') {break;}
			SingleChar[2] = String[i ++];
			if (String[i] == '\0') {break;}
			SingleChar[3] = String[i ++];
			SingleChar[4] = '\0';
		}
		else
		{
			i ++;			//意外情况，i指向下一个字节，忽略此字节，继续判断下一个字节
			continue;
		}
#endif
		
#ifdef OLED_CHARSET_GB2312						//定义字符集为GB2312
		/*此段代码的目的是，提取GB2312字符串中的一个字符，转存到SingleChar子字符串中*/
		/*判断GB2312字节的最高位标志位*/
		if ((String[i] & 0x80) == 0x00)			//最高位为0
		{
			CharLength = 1;						//字符为1字节
			SingleChar[0] = String[i ++];		//将第一个字节写入SingleChar第0个位置，随后i指向下一个字节
			SingleChar[1] = '\0';				//为SingleChar添加字符串结束标志位
		}
		else									//最高位为1
		{
			CharLength = 2;						//字符为2字节
			SingleChar[0] = String[i ++];		//将第一个字节写入SingleChar第0个位置，随后i指向下一个字节
			if (String[i] == '\0') {break;}		//意外情况，跳出循环，结束显示
			SingleChar[1] = String[i ++];		//将第二个字节写入SingleChar第1个位置，随后i指向下一个字节
			SingleChar[2] = '\0';				//为SingleChar添加字符串结束标志位
		}
#endif
		
		/*显示上述代码提取到的SingleChar*/
		if (CharLength == 1)	//如果是单字节字符
		{
			/*使用OLED_ShowChar显示此字符*/
			ST7735_ShowChar(X + XOffset, Y, SingleChar[0],Font_Color,Back_Color);
			XOffset += 8;
		}
		else					//否则，即多字节字符
		{
			/*遍历整个字模库，从字模库中寻找此字符的数据*/
			/*如果找到最后一个字符（定义为空字符串），则表示字符未在字模库定义，停止寻找*/
			for (pIndex = 0; strcmp(OLED_CF16x16[pIndex].Index, "") != 0; pIndex ++)
			{
				/*找到匹配的字符*/
				if (strcmp(OLED_CF16x16[pIndex].Index, SingleChar) == 0)
				{
					break;		//跳出循环，此时pIndex的值为指定字符的索引
				}
			}
				ST7735_ShowImage(X + XOffset, Y, 16, 16, OLED_CF16x16[pIndex].Data,Font_Color,Back_Color);
				XOffset += 16;
			}
			
		}
	}
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//结果默认为1
	while (Y --)			//累乘Y次
	{
		Result *= X;		//每次把X累乘到结果上
	}
	return Result;
}

void ST7735_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length,uint16_t Font_Color, uint16_t Back_Color)
{
	uint8_t i;
	for (i = 0; i < Length; i++)		//遍历数字的每一位							
	{
		/*调用OLED_ShowChar函数，依次显示每个数字*/
		/*Number / OLED_Pow(10, Length - i - 1) % 10 可以十进制提取数字的每一位*/
		/*+ '0' 可将数字转换为字符格式*/
		ST7735_ShowChar(X + i * 8, Y, Number / OLED_Pow(10, Length - i - 1) % 10 + '0',Font_Color,Back_Color);
	}
}
void ST7735_ShowSignedNum(int16_t X, int16_t Y, int32_t Number, uint8_t Length,uint16_t Font_Color, uint16_t Back_Color)
{
	uint8_t i;
	uint32_t Number1;
	
	if (Number >= 0)						//数字大于等于0
	{
		ST7735_ShowChar(X, Y, '+', Font_Color,Back_Color);	//显示+号
		Number1 = Number;					//Number1直接等于Number
	}
	else									//数字小于0
	{
		ST7735_ShowChar(X, Y, '-', Font_Color,Back_Color);	//显示-号
		Number1 = -Number;					//Number1等于Number取负
	}
	
	for (i = 0; i < Length; i++)			//遍历数字的每一位								
	{
		/*调用OLED_ShowChar函数，依次显示每个数字*/
		/*Number1 / OLED_Pow(10, Length - i - 1) % 10 可以十进制提取数字的每一位*/
		/*+ '0' 可将数字转换为字符格式*/
		ST7735_ShowChar(X + (i + 1) * 8, Y, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0',Font_Color,Back_Color);
	}
}
void ST7735_ShowHexNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length,uint16_t Font_Color, uint16_t Back_Color)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)		//遍历数字的每一位
	{
		/*以十六进制提取数字的每一位*/
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		
		if (SingleNumber < 10)			//单个数字小于10
		{
			/*调用OLED_ShowChar函数，显示此数字*/
			/*+ '0' 可将数字转换为字符格式*/
		    ST7735_ShowChar(X + i * 8, Y, SingleNumber + '0',Font_Color,Back_Color);
		}
		else							//单个数字大于10
		{
			/*调用OLED_ShowChar函数，显示此数字*/
			/*+ 'A' 可将数字转换为从A开始的十六进制字符*/
			ST7735_ShowChar(X + i * 8, Y, SingleNumber - 10 + 'A',Font_Color,Back_Color);
		}
	}
}
void ST7735_ShowBinNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length,uint16_t Font_Color, uint16_t Back_Color)
{
	uint8_t i;
	for (i = 0; i < Length; i++)		//遍历数字的每一位	
	{
		/*调用OLED_ShowChar函数，依次显示每个数字*/
		/*Number / OLED_Pow(2, Length - i - 1) % 2 可以二进制提取数字的每一位*/
		/*+ '0' 可将数字转换为字符格式*/
		ST7735_ShowChar(X + i * 8, Y, Number / OLED_Pow(2, Length - i - 1) % 2 + '0',Font_Color,Back_Color);
	}
}
void ST7735_ShowFloatNum(int16_t X, int16_t Y, double Number, uint8_t IntLength, uint8_t FraLength,uint16_t Font_Color, uint16_t Back_Color)
{
	uint32_t PowNum, IntNum, FraNum;
	
	if (Number >= 0)						//数字大于等于0
	{
		ST7735_ShowChar(X, Y, '+', Font_Color,Back_Color);	//显示+号
	}
	else									//数字小于0
	{
		ST7735_ShowChar(X, Y, '-', Font_Color,Back_Color);	//显示-号
		Number = -Number;					//Number取负
	}
	
	/*提取整数部分和小数部分*/
	IntNum = Number;						//直接赋值给整型变量，提取整数
	Number -= IntNum;						//将Number的整数减掉，防止之后将小数乘到整数时因数过大造成错误
	PowNum = OLED_Pow(10, FraLength);		//根据指定小数的位数，确定乘数
	FraNum = round(Number * PowNum);		//将小数乘到整数，同时四舍五入，避免显示误差
	IntNum += FraNum / PowNum;				//若四舍五入造成了进位，则需要再加给整数
	
	/*显示整数部分*/
	ST7735_ShowNum(X + 8, Y, IntNum, IntLength,  Font_Color,Back_Color);
	
	/*显示小数点*/
	ST7735_ShowChar(X + (IntLength + 1) * 8, Y, '.',  Font_Color,Back_Color);
	
	/*显示小数部分*/
	ST7735_ShowNum(X + (IntLength + 2) * 8, Y, FraNum, FraLength,  Font_Color,Back_Color);
}

void ST7735_Init(void)
{
    ST7735_Reset();
    ST7735_Unsleep();

    ST7735_Write_Command(0xB1);
    ST7735_Write_Data(0x05);
    ST7735_Write_Data(0x3C);
    ST7735_Write_Data(0x3C);

    ST7735_Write_Command(0xB2);
    ST7735_Write_Data(0x05);
    ST7735_Write_Data(0x3C);
    ST7735_Write_Data(0x3C);

    ST7735_Write_Command(0xB3);
    ST7735_Write_Data(0x05);
    ST7735_Write_Data(0x3C);
    ST7735_Write_Data(0x3C);
    ST7735_Write_Data(0x05);
    ST7735_Write_Data(0x3C);
    ST7735_Write_Data(0x3C);

    ST7735_Write_Command(0xC1);
    ST7735_Write_Data(0xC0);
    ST7735_Write_Data(0x00);

    ST7735_Write_Command(0xC2);
    ST7735_Write_Data(0x0D);

    ST7735_Write_Command(0xC3);
    ST7735_Write_Data(0x8D);
    ST7735_Write_Data(0x00);

    ST7735_Write_Command(0xC4);
    ST7735_Write_Data(0x8D);
    ST7735_Write_Data(0xEE);

    ST7735_Write_Command(0xC5);
    ST7735_Write_Data(0x1A);
   
    ST7735_Write_Data_DMA(0xE0,Data_E0);
    ST7735_Write_Data_DMA(0xE1,Data_E1);

    ST7735_Set_Scan_Dir(Data_Unnormal);
    ST7735_Write_Command(0x21);
    ST7735_Set_RGB(Data_RGB565);
    ST7735_Write_Command(Cmd_Start_View); 
	ST7735_Clear_View(0x0000);

	#if(ST7735_Use_Tim==1)
	Set_TIM_IT_Freq(APB1,ST7735_Tim,5);
	HAL_TIM_Base_Start_IT(ST7735_Tim);
	#endif
	

}
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		HAL_UART_Transmit(&huart4,(uint8_t*)(String+i),1,100);
	}
}

void Serial_Printf(char *format, ...)
{
	char String[100];				//定义字符数组
	va_list arg;					//定义可变参数列表数据类型的变量arg
	va_start(arg, format);			//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);					//结束变量arg
	Serial_SendString(String);		//串口发送字符数组（字符串）
}
