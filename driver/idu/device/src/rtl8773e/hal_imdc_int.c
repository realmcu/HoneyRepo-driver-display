#include "hal_imdc_int.h"
#include "dma_channel.h"

void hal_imdc_channel_init_int(uint8_t *high_speed_channel, uint8_t *low_speed_channel)
{
    *high_speed_channel = 0xA5;
    *low_speed_channel = 0xA5;
    if (!GDMA_channel_request(high_speed_channel, NULL, true))
    {
        assert_param(*high_speed_channel != 0xA5);
        return;
    }
    if (!GDMA_channel_request(low_speed_channel, NULL, false))
    {
        assert_param(*low_speed_channel != 0xA5);
        return;
    }
    return;
}

GDMA_ChannelTypeDef *hal_imdc_get_dma_channel_int(uint8_t channel_num)
{
    return DMA_CH_BASE(channel_num);
}
