#include "rtl_imdc_int.h"

uint32_t hal_imdc_get_dma_ctl_low_int(GDMA_ChannelTypeDef *dma)
{
    return dma->GDMA_CTLx_L;
}

bool hal_imdc_get_dma_busy_state(uint8_t channel_num)
{
    return GDMA_GetChannelStatus(channel_num);
}

void hal_imdc_wait_dma_idle(GDMA_ChannelTypeDef *dma)
{
    while (!GDMA_GetSuspendChannelStatus(dma));
}

void hal_imdc_rx_handshake_init(GDMA_InitTypeDef *init_struct)
{
    init_struct->GDMA_Secure_En = ENABLE;
    init_struct->GDMA_DestHandshake = GDMA_Handshake_IMDC_RX;
}

void hal_imdc_tx_handshake_init(GDMA_InitTypeDef *init_struct)
{
    init_struct->GDMA_Secure_En = ENABLE;
    init_struct->GDMA_SourceHandshake = GDMA_Handshake_IMDC_TX;
}

void hal_imdc_hw_handshake_init(IMDC_InitTypeDef *init_struct)
{
    return;
}

void hal_imdc_fill_hw_hs_reg_int(IMDC_CTL1_TypeDef *reg, IMDC_InitTypeDef *IMDC_init_struct)
{
    return;
}


IMDC_ERROR hal_imdc_decode_direct_int(uint8_t *file, IMDC_decode_range *range,
                                      IMDC_DMA_config *dma_cfg, GDMA_LLIDef *RX_LLI)
{
    return IMDC_ERROR_INVALID_PARAM;
}
