#include "rtl_idu_int.h"

void rtl_idu_channel_init_int(uint8_t *high_speed_channel, uint8_t *low_speed_channel)
{
    return;
}

GDMA_ChannelTypeDef *rtl_idu_get_dma_channel_int(uint8_t channel_num)
{
    return GDMA_GetGDMAChannelx(channel_num);
}

uint32_t rtl_idu_get_dma_ctl_low_int(GDMA_ChannelTypeDef *dma)
{
    return dma->GDMA_CTLx_L;
}

bool rtl_idu_get_dma_busy_state(uint8_t channel_num)
{
    return GDMA_GetChannelStatus(channel_num);
}

void rtl_idu_wait_dma_idle(GDMA_ChannelTypeDef *dma)
{
    while (!GDMA_GetSuspendChannelStatus(dma));
}

void rtl_idu_rx_handshake_init(GDMA_InitTypeDef *init_struct)
{
    init_struct->GDMA_Secure_En = ENABLE;
    init_struct->GDMA_DestHandshake = GDMA_Handshake_IDU_RX;
}

void rtl_idu_tx_handshake_init(GDMA_InitTypeDef *init_struct)
{
    init_struct->GDMA_Secure_En = ENABLE;
    init_struct->GDMA_SourceHandshake = GDMA_Handshake_IDU_TX;
}

void rtl_idu_hw_handshake_init(IDU_InitTypeDef *init_struct)
{
    return;
}

void rtl_idu_fill_hw_hs_reg_int(IDU_CTL1_TypeDef *reg, IDU_InitTypeDef *IDU_init_struct)
{
    return;
}


IDU_ERROR rtl_idu_decode_direct_int(uint8_t *file, IDU_decode_range *range,
                                      IDU_DMA_config *dma_cfg, GDMA_LLIDef *RX_LLI)
{
    return IDU_ERROR_INVALID_PARAM;
}
