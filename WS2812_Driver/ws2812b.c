#include "gpio.h"
#include "ws2812b.h"

void WS2812B_Set_Color(uint32_t RGB)
{
    uint32_t GRB=0;
    GRB|=(RGB&0xFF00)<<8;
    GRB|=(RGB&0xFF0000)>>8;
    GRB|=RGB&0xFF;
    for(uint8_t i=0;i<24;i++)
    {
        if(GRB&(0x800000>>i))
        {
           WS2812B_Height();
           WS2812B_Delay_Long_ns();
           WS2812B_Low();
           WS2812B_Delay_Short_ns(); 
        }
        else
        {
            WS2812B_Height();
            WS2812B_Delay_Short_ns(); 
            WS2812B_Low();
            WS2812B_Delay_Long_ns();
        }
    }
}
