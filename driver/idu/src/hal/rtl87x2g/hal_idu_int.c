#include "hal_idu_int.h"

void hal_idu_channel_init_int(uint8_t *high_speed_channel, uint8_t *low_speed_channel)
{
    return;
}

GDMA_ChannelTypeDef *hal_idu_get_dma_channel_int(uint8_t channel_num)
{
    return GDMA_GetGDMAChannelx(channel_num);
}

