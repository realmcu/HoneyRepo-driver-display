#include "stdint.h"
#include "stdbool.h"
typedef struct
{
    uint32_t length;
    uint32_t src_stride;
    uint32_t dst_stride;
    uint32_t height;
} hal_imdc_dma_info;

typedef struct
{
    uint32_t start_line;
    uint32_t end_line;
    uint32_t start_column;
    uint32_t end_column;
    uint32_t raw_data_address;
    uint32_t length;
    uint32_t dst_stride;
} hal_imdc_decompress_info;

void hal_dma_copy(hal_imdc_dma_info *info, uint8_t *src, uint8_t *dst);
bool hal_imdc_decompress(hal_imdc_decompress_info *info, uint8_t *dst);
bool hal_imdc_decompress_rect(hal_imdc_decompress_info *info, uint8_t *dst);
void hal_dma_channel_init(uint8_t *high_speed_channel, uint8_t *low_speed_channel);
