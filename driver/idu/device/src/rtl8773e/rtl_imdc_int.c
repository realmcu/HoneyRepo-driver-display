#include "rtl_imdc.h"
#include "rtl_imdc_int.h"
#include "hal_imdc_int.h"
#include "stdio.h"

uint32_t hal_imdc_get_dma_ctl_low_int(GDMA_ChannelTypeDef *dma)
{
    return dma->CTL_LOW;
}

bool hal_imdc_get_dma_busy_state(uint8_t channel_num)
{
    return (GDMA0->ChEnReg & (0x01 << channel_num));
}

void hal_imdc_wait_dma_idle(GDMA_ChannelTypeDef *dma)
{
    while (!(dma->CFG_LOW & BIT0));
}

void hal_imdc_rx_handshake_init(GDMA_InitTypeDef *init_struct)
{
    init_struct->GDMA_DestHandshake = GDMA_Handshake_RTZIP_RX;
}

void hal_imdc_tx_handshake_init(GDMA_InitTypeDef *init_struct)
{
    init_struct->GDMA_SourceHandshake = GDMA_Handshake_RTZIP_TX;
}

void hal_imdc_hw_handshake_init(IMDC_InitTypeDef *init_struct)
{
    init_struct->hw_handshake = IMDC_HW_HANDSHAKE_DMA;
}

void hal_imdc_fill_hw_hs_reg_int(IMDC_CTL1_TypeDef *reg, IMDC_InitTypeDef *IMDC_init_struct)
{
    reg->b.hw_handshake_mux = IMDC_init_struct->hw_handshake;
}

IMDC_ERROR hal_imdc_decode_direct_int(uint8_t *file, IMDC_decode_range *range,
                                      IMDC_DMA_config *dma_cfg, GDMA_LLIDef *RX_LLI)
{
    uint32_t decompress_start_line;
    uint32_t decompress_end_line;
    uint32_t decompress_start_column;
    uint32_t decompress_end_column;
    RCC_PeriphClockCmd(APBPeriph_IMDC, APBPeriph_IMDC_CLOCK, ENABLE);
    if (file == NULL)
    {
        return IMDC_ERROR_NULL_INPUT;
    }
    if (dma_cfg == NULL)
    {
        return IMDC_ERROR_INVALID_PARAM;
    }
    uint32_t compressed_data_start_address = (uint32_t)file;
    IMDC_file_header *header = (IMDC_file_header *)compressed_data_start_address;
    if (range != NULL)
    {
        if ((range->start_line >= header->raw_pic_height) || (range->start_column >= header->raw_pic_width))
        {
            return IMDC_ERROR_START_EXCEED_BOUNDARY;
        }
        if ((range->start_line > range->end_line) || (range->start_column > range->end_column))
        {
            return IMDC_ERROR_START_LARGER_THAN_END;
        }
        if ((range->end_line >= header->raw_pic_height) || (range->end_column >= header->raw_pic_width))
        {
            return IMDC_ERROR_END_EXCEED_BOUNDARY;
        }
        decompress_start_line = range->start_line;
        decompress_end_line = range->end_line;
        decompress_start_column = range->start_column;
        decompress_end_column = range->end_column;
    }
    else
    {
        decompress_start_line = 0;
        decompress_end_line = header->raw_pic_height - 1;
        decompress_start_column = 0;
        decompress_end_column = header->raw_pic_width - 1;
    }
    if ((!IS_IMDC_ALGORITHM(header->algorithm_type.algorithm)) ||
        (!IS_IMDC_PIXEL_BYTES(header->algorithm_type.pixel_bytes)))
    {
        return IMDC_ERROR_INVALID_PARAM;
    }

    uint32_t start_line_address = IMDC_Get_Line_Start_Address(compressed_data_start_address,
                                                              decompress_start_line);
    uint32_t compressed_data_size = IMDC_Get_Line_Start_Address(compressed_data_start_address,
                                                                decompress_end_line + 1) - start_line_address;

    IMDC_InitTypeDef IMDC_struct_init;
    IMDC_struct_init.algorithm_type            = (IMDC_ALGORITHM)header->algorithm_type.algorithm;
    IMDC_struct_init.head_throw_away_byte_num  = THROW_AWAY_0BYTE;
    IMDC_struct_init.pic_pixel_size            = (IMDC_PIXEL_SIZE)header->algorithm_type.pixel_bytes;
    IMDC_struct_init.pic_decompress_height     = (decompress_end_line - decompress_start_line + 1);
    IMDC_struct_init.pic_raw_width             = header->raw_pic_width;
    IMDC_struct_init.tx_column_start           = decompress_start_column;
    IMDC_struct_init.tx_column_end             = decompress_end_column ;
    IMDC_struct_init.compressed_data_size      = compressed_data_size;
    IMDC_struct_init.pic_length2_size          = (IMDC_RLE_RUNLENGTH_SIZE)
                                                 header->algorithm_type.feature_2;
    IMDC_struct_init.pic_length1_size          = (IMDC_RLE_RUNLENGTH_SIZE)
                                                 header->algorithm_type.feature_1;
    IMDC_struct_init.yuv_blur_bit              = (IMDC_YUV_BLUR_BIT)header->algorithm_type.feature_2;
    IMDC_struct_init.yuv_sample_type           = (IMDC_YUV_SAMPLE_TYPE)
                                                 header->algorithm_type.feature_1;
    IMDC_struct_init.rx_fifo_dma_enable        = (uint32_t)ENABLE;
    IMDC_struct_init.tx_fifo_dma_enable        = (uint32_t)ENABLE;
    IMDC_struct_init.rx_fifo_dma_threshold     = IMDC_RX_FIFO_DEPTH / 2;
    IMDC_struct_init.tx_fifo_dma_threshold     = IMDC_TX_FIFO_DEPTH / 2;
    IMDC_struct_init.rx_fifo_int_threshold     = dma_cfg->RX_FIFO_INT_threshold;
    IMDC_struct_init.tx_fifo_int_threshold     = dma_cfg->TX_FIFO_INT_threshold;
    IMDC_struct_init.hw_handshake              = IMDC_HW_HANDSHAKE_PPE;
    IMDC_Init(&IMDC_struct_init);


    uint32_t DMA_compressed_data_size_word = ((compressed_data_size % 4) ?
                                              (compressed_data_size / 4 + 1) : (compressed_data_size / 4));
    /* Configure DMA */
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    GDMA_InitTypeDef RX_GDMA_InitStruct;
    GDMA_ChannelTypeDef *RX_DMA = hal_imdc_get_dma_channel_int(dma_cfg->RX_DMA_channel_num);
    GDMA_ChannelTypeDef *TX_DMA = hal_imdc_get_dma_channel_int(dma_cfg->TX_DMA_channel_num);
    uint32_t rx_block_num = 0;
    if (DMA_compressed_data_size_word >= 65535)
    {
        rx_block_num = DMA_compressed_data_size_word / 65535;
        if (DMA_compressed_data_size_word % 65535)
        {
            rx_block_num = rx_block_num + 1;
        }
    }
    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&RX_GDMA_InitStruct);
    RX_GDMA_InitStruct.GDMA_ChannelNum          = dma_cfg->RX_DMA_channel_num;
    RX_GDMA_InitStruct.GDMA_BufferSize          = DMA_compressed_data_size_word;
    RX_GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    RX_GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    RX_GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    RX_GDMA_InitStruct.GDMA_SourceMsize         =
        GDMA_Msize_16;                         // 8 msize for source msize
    RX_GDMA_InitStruct.GDMA_DestinationMsize    =
        GDMA_Msize_8;                         // 8 msize for destiantion msize
    RX_GDMA_InitStruct.GDMA_DestinationDataSize =
        GDMA_DataSize_Word;                   // 32 bit width for destination transaction
    RX_GDMA_InitStruct.GDMA_SourceDataSize      =
        GDMA_DataSize_Word;                   // 32 bit width for source transaction
    RX_GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)start_line_address;
    RX_GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)(&IMDC->RX_FIFO);
    hal_imdc_rx_handshake_init(&RX_GDMA_InitStruct);
    if (rx_block_num)
    {
        RX_GDMA_InitStruct.GDMA_BufferSize = 65535;
        RX_GDMA_InitStruct.GDMA_Multi_Block_Mode = LLI_TRANSFER;
        RX_GDMA_InitStruct.GDMA_Multi_Block_En = 1;
        RX_GDMA_InitStruct.GDMA_Multi_Block_Struct = (uint32_t)RX_LLI;
    }
    GDMA_Init(RX_DMA, &RX_GDMA_InitStruct);
    if (rx_block_num)
    {
        for (int i = 0; i < rx_block_num; i++)
        {
            if (i == rx_block_num - 1)
            {
                RX_LLI[i].SAR = RX_GDMA_InitStruct.GDMA_SourceAddr + 65535 * 4 * i;
                RX_LLI[i].DAR = (uint32_t)(&IMDC->RX_FIFO);
                RX_LLI[i].LLP = 0;
                /* configure low 32 bit of CTL register */
                RX_LLI[i].CTL_LOW = (BIT(0)
                                     | (RX_GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                     | (GDMA_DataSize_Byte << 4)
                                     | (RX_GDMA_InitStruct.GDMA_DestinationInc << 7)
                                     | (RX_GDMA_InitStruct.GDMA_SourceInc << 9)
                                     | (RX_GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                     | (RX_GDMA_InitStruct.GDMA_SourceMsize << 14)
                                     | (RX_GDMA_InitStruct.GDMA_DIR << 20));
                /* configure high 32 bit of CTL register */
                uint32_t block_size = compressed_data_size - 65535 * 4 * i;
                RX_LLI[i].CTL_HIGH = block_size;
            }
            else
            {
                RX_LLI[i].SAR = RX_GDMA_InitStruct.GDMA_SourceAddr + 65535 * 4 * i;
                RX_LLI[i].DAR = (uint32_t)(&IMDC->RX_FIFO);
                RX_LLI[i].LLP = (uint32_t)&RX_LLI[i + 1];
                /* configure low 32 bit of CTL register */
                RX_LLI[i].CTL_LOW = hal_imdc_get_dma_ctl_low_int(RX_DMA);
                /* configure high 32 bit of CTL register */
                RX_LLI[i].CTL_HIGH = 65535;
            }
        }
    }

    GDMA_Cmd(dma_cfg->RX_DMA_channel_num, ENABLE);

    IMDC_ClearINTPendingBit(IMDC_DECOMPRESS_FINISH_INT);
    IMDC_INTConfig(IMDC_DECOMPRESS_FINISH_INT, ENABLE);
    IMDC_MaskINTConfig(IMDC_DECOMPRESS_FINISH_INT, DISABLE);

    IMDC_ClearINTPendingBit(IMDC_DECOMPRESS_ERROR_INT);
    IMDC_INTConfig(IMDC_DECOMPRESS_ERROR_INT, ENABLE);
    IMDC_MaskINTConfig(IMDC_DECOMPRESS_ERROR_INT, DISABLE);
    return IMDC_SUCCESS;
}
