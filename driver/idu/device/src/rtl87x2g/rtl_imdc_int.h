#include "rtl_gdma.h"
#include "rtl_rcc.h"
#include "rtl_nvic.h"
#include "rtl_imdc.h"
#include "utils/rtl_utils.h"

uint32_t hal_imdc_get_dma_ctl_low_int(GDMA_ChannelTypeDef *dma);

bool hal_imdc_get_dma_busy_state(uint8_t channel_num);

void hal_imdc_wait_dma_idle(GDMA_ChannelTypeDef *dma);

void hal_imdc_rx_handshake_init(GDMA_InitTypeDef *init_struct);

void hal_imdc_tx_handshake_init(GDMA_InitTypeDef *init_struct);

void hal_imdc_hw_handshake_init(IMDC_InitTypeDef *init_struct);

void hal_imdc_fill_hw_hs_reg_int(IMDC_CTL1_TypeDef *reg, IMDC_InitTypeDef *IMDC_init_struct);

IMDC_ERROR hal_imdc_decode_direct_int(uint8_t *file, IMDC_decode_range *range,
                                      IMDC_DMA_config *dma_cfg, GDMA_LLIDef *RX_LLI);
