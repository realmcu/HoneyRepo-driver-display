#include "rtl_idu_int.h"

uint32_t hal_idu_get_dma_ctl_low_int(GDMA_ChannelTypeDef *dma)
{
    return dma->GDMA_CTLx_L;
}

bool hal_idu_get_dma_busy_state(uint8_t channel_num)
{
    return GDMA_GetChannelStatus(channel_num);
}

void hal_idu_wait_dma_idle(GDMA_ChannelTypeDef *dma)
{
    while (!GDMA_GetSuspendChannelStatus(dma));
}

void hal_idu_rx_handshake_init(GDMA_InitTypeDef *init_struct)
{
    init_struct->GDMA_Secure_En = ENABLE;
    init_struct->GDMA_DestHandshake = GDMA_Handshake_IDU_RX;
}

void hal_idu_tx_handshake_init(GDMA_InitTypeDef *init_struct)
{
    init_struct->GDMA_Secure_En = ENABLE;
    init_struct->GDMA_SourceHandshake = GDMA_Handshake_IDU_TX;
}

void hal_idu_hw_handshake_init(IDU_InitTypeDef *init_struct)
{
    return;
}

void hal_idu_fill_hw_hs_reg_int(IDU_CTL1_TypeDef *reg, IDU_InitTypeDef *IDU_init_struct)
{
    return;
}


IDU_ERROR hal_idu_decode_direct_int(uint8_t *file, IDU_decode_range *range,
                                      IDU_DMA_config *dma_cfg, GDMA_LLIDef *RX_LLI)
{
    return IDU_ERROR_INVALID_PARAM;
}
