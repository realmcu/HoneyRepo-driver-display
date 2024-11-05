#include "rtl_gdma.h"
#include "rtl_rcc.h"
#include "rtl_nvic.h"
#include "rtl_imdc.h"

void hal_imdc_channel_init_int(uint8_t *high_speed_channel, uint8_t *low_speed_channel);

GDMA_ChannelTypeDef *hal_imdc_get_dma_channel_int(uint8_t channel_num);
