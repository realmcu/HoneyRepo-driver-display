/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* \file     rtl_imdc.c
* \brief    This file provides all the LCDC HANDLER firmware functions.
* \details
* \author   astor zhang
* \date     2023-10-17
* \version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "stdio.h"
#include "rtl_imdc.h"
#include "rtl_imdc_int.h"
#include "hal_imdc_int.h"

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
GDMA_LLIDef RX_GDMA_LLIStruct[20];
GDMA_LLIDef TX_GDMA_LLIStruct[20];

void IMDC_TxFifoClear(void)
{
    IMDC_CTL1_TypeDef imdc_reg_0x04 = {.d32 = IMDC->IMDC_CTL1};
    imdc_reg_0x04.b.tx_fifo_clear = 1;
    IMDC->IMDC_CTL1 = imdc_reg_0x04.d32;
}

void IMDC_RxFifoClear(void)
{
    IMDC_CTL1_TypeDef imdc_reg_0x04 = {.d32 = IMDC->IMDC_CTL1};
    imdc_reg_0x04.b.rx_fifo_clear = 1;
    IMDC->IMDC_CTL1 = imdc_reg_0x04.d32;
}

ITStatus IMDC_GetINTStatus(uint32_t IMDC_INT)
{
    ITStatus bit_status = RESET;

    /* Check the parameters */
    //assert_param(IS_IMDC_INT(IMDC_INT));

    if (IMDC->INT_STATUS & IMDC_INT)
    {
        bit_status = SET;
    }

    return  bit_status;
}

void IMDC_INTConfig(uint32_t IMDC_INT, FunctionalState NewState)
{
    /* Check the parameters */
    //assert_param(IS_IMDC_INT(IMDC_INT_MSK));
    //assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState == ENABLE)
    {
        IMDC->INT_ENABLE |= IMDC_INT;
    }
    else
    {
        IMDC->INT_ENABLE &= ~IMDC_INT;
    }
}

void IMDC_MaskINTConfig(uint32_t IMDC_INT_MSK, FunctionalState NewState)
{
    /* Check the parameters */
    //assert_param(IS_IMDC_INT(IMDC_INT_MSK));
    //assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState == ENABLE)
    {
        IMDC->INT_MASK |= IMDC_INT_MSK;
    }
    else
    {
        IMDC->INT_MASK &= ~IMDC_INT_MSK;
    }
}

void IMDC_ClearINTPendingBit(uint32_t IMDC_INT)
{
    //assert_param(IS_IMDC_INT(IMDC_INT));
    IMDC->INT_CLEAR |= IMDC_INT;
}

void IMDC_Cmd(FunctionalState state)
{
    assert_param(IS_FUNCTIONAL_STATE(state));
    IMDC_CTL0_TypeDef imdc_reg_0x00 = {.d32 = IMDC->IMDC_CTL0};
    if (state)
    {
        imdc_reg_0x00.b.imdc_enable = 1;
    }
    else
    {
        imdc_reg_0x00.b.imdc_enable = 0;
    }
    IMDC->IMDC_CTL0 = imdc_reg_0x00.d32;
}

void IMDC_Run(FunctionalState state)
{
    assert_param(IS_FUNCTIONAL_STATE(state));
    IMDC_CTL0_TypeDef imdc_reg_0x00 = {.d32 = IMDC->IMDC_CTL0};
    if (state)
    {
        imdc_reg_0x00.b.imdc_reset_decompress_start = 1;
    }
    else
    {
        imdc_reg_0x00.b.imdc_reset_decompress_start = 0;
    }
    IMDC->IMDC_CTL0 = imdc_reg_0x00.d32;
}

ITStatus IMDC_GetInternalBufferState(void)
{
    ITStatus rv = RESET;
    IMDC_CTL1_TypeDef imdc_reg_0x04 = {.d32 = IMDC->IMDC_CTL1};
    if (imdc_reg_0x04.b.imdc_internal_buffer_empty)
    {
        rv = SET;
    }
    return rv;
}

uint32_t IMDC_GetDecompressLine(void)
{
    uint32_t reg_value = IMDC->VERTICAL_COUNTER;
    return reg_value;
}

uint32_t IMDC_GetHorizontalByteNum(void)
{
    uint32_t reg_value = IMDC->HORIZONTAL_COUNTER;
    return reg_value;
}

uint32_t IMDC_GetCompressedSize(void)
{
    uint32_t reg_value = IMDC->COMPRESSED_DATA_SIZE;
    return reg_value;
}

uint32_t IMDC_Get_Line_Start_Address(uint32_t compressed_start_address,
                                     uint32_t line_number)
{
    // line number start from0 and ends to (raw_pic_height-1)
    // return the absolute address of line

    IMDC_file_header *rle_file_header = ((IMDC_file_header *)(compressed_start_address));
    if (line_number > rle_file_header->raw_pic_height)
    {
//        DBG_DIRECT("[error]line number won't be equals to or bigger than pic height");
        return 0;
    }
    else
    {
        uint32_t *line_address = (uint32_t *)(compressed_start_address + 12 + line_number * 4);
        return (line_address[0] + compressed_start_address);
    }
}


void IMDC_Init(IMDC_InitTypeDef *IMDC_init_struct)
{
    IMDC_InitTypeDef *IMDC_struct_init = IMDC_init_struct;
    IMDC_CTL0_TypeDef imdc_reg_0x00 = {.d32 = IMDC->IMDC_CTL0};
    imdc_reg_0x00.b.imdc_algorithm = IMDC_struct_init->algorithm_type;
    IMDC->IMDC_CTL0 = imdc_reg_0x00.d32;

    IMDC_CTL1_TypeDef imdc_reg_0x04 = {.d32 = IMDC->IMDC_CTL1};
    imdc_reg_0x04.b.head_throw_away_byte_num = IMDC_struct_init->head_throw_away_byte_num;
    imdc_reg_0x04.b.pic_pixel_size = IMDC_struct_init->pic_pixel_size;
    hal_imdc_fill_hw_hs_reg_int(&imdc_reg_0x04, IMDC_struct_init);
    IMDC->IMDC_CTL1 = imdc_reg_0x04.d32;

    IMDC->PIC_RAW_WIDTH = IMDC_struct_init->pic_raw_width;
    IMDC->TX_COLUMN_START = IMDC_struct_init->tx_column_start;
    IMDC->TX_COLUMN_END = IMDC_struct_init->tx_column_end;
    IMDC->DECOMPRESS_OUTPUT_PIXEL = (
                                        IMDC_struct_init->pic_decompress_height\
                                        * (IMDC_struct_init->tx_column_end - IMDC_struct_init->tx_column_start + 1)
                                    );
    IMDC->PIC_DECOMPRESS_TOTAL_PIXEL = IMDC_struct_init->pic_raw_width *
                                       IMDC_struct_init->pic_decompress_height;

    RLE_FASTLZ_CTL_TypeDef imdc_reg_0x1c = {.d32 = IMDC->RLE_FASTLZ_CTL};
    imdc_reg_0x1c.b.pic_length1_size = IMDC_struct_init->pic_length1_size;
    imdc_reg_0x1c.b.pic_length2_size = IMDC_struct_init->pic_length2_size;
    IMDC->RLE_FASTLZ_CTL = imdc_reg_0x1c.d32;

    YUV_SBF_CTL_TypeDef imdc_reg_0x20 = {.d32 = IMDC->YUV_SBF_CTL};
    imdc_reg_0x20.b.yuv_blur_bit = IMDC_struct_init->yuv_blur_bit;
    imdc_reg_0x20.b.yuv_sample_type = IMDC_struct_init->yuv_sample_type;
    IMDC->YUV_SBF_CTL = imdc_reg_0x20.d32;

    IMDC->COMPRESSED_DATA_SIZE = IMDC_struct_init->compressed_data_size;

    RX_FIFO_DMA_THRESHOLD_TypeDef imdc_reg_0x48 = {.d32 = IMDC->RX_FIFO_DMA_THRESHOLD};
    imdc_reg_0x48.b.rx_fifo_dma_threshold = IMDC_struct_init->rx_fifo_dma_threshold;
    imdc_reg_0x48.b.rx_dma_enable = IMDC_struct_init->rx_fifo_dma_enable;
    IMDC->RX_FIFO_DMA_THRESHOLD = imdc_reg_0x48.d32;

    TX_FIFO_DMA_THRESHOLD_TypeDef imdc_reg_0x4c = {.d32 = IMDC->TX_FIFO_DMA_THRESHOLD};
    imdc_reg_0x4c.b.tx_fifo_dma_threshold = IMDC_struct_init->tx_fifo_dma_threshold;
    imdc_reg_0x4c.b.tx_dma_enable = IMDC_struct_init->tx_fifo_dma_enable;
    IMDC->TX_FIFO_DMA_THRESHOLD = imdc_reg_0x4c.d32;

    RX_FIFO_INT_THRESHOLD_TypeDef imdc_reg_0x50 = {.d32 = IMDC->RX_FIFO_INT_THRESHOLD};
    imdc_reg_0x50.b.rx_fifo_int_threshold = IMDC_struct_init->rx_fifo_int_threshold;
    IMDC->RX_FIFO_INT_THRESHOLD = imdc_reg_0x50.d32;

    TX_FIFO_INT_THRESHOLD_TypeDef imdc_reg_0x54 = {.d32 = IMDC->RX_FIFO_INT_THRESHOLD};
    imdc_reg_0x54.b.tx_fifo_int_threshold = IMDC_struct_init->tx_fifo_int_threshold;
    IMDC->TX_FIFO_INT_THRESHOLD = imdc_reg_0x54.d32;
}

IMDC_ERROR IMDC_Decode(uint8_t *file, IMDC_decode_range *range, IMDC_DMA_config *dma_cfg)
{
    IMDC_ERROR err = IMDC_SUCCESS;
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
    uint32_t decompressed_data_size = (decompress_end_line - decompress_start_line + 1) *
                                      (decompress_end_column - decompress_start_column + 1)\
                                      * (header->algorithm_type.pixel_bytes + 2);

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
    hal_imdc_hw_handshake_init(&IMDC_struct_init);
    IMDC_Init(&IMDC_struct_init);


    uint32_t DMA_compressed_data_size_word = ((compressed_data_size % 4) ?
                                              (compressed_data_size / 4 + 1) : (compressed_data_size / 4));
    uint32_t DMA_decompressed_data_size_word = ((decompressed_data_size % 4) ?
                                                (decompressed_data_size / 4 + 1) : (decompressed_data_size / 4));
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
        RX_GDMA_InitStruct.GDMA_Multi_Block_Struct = (uint32_t)RX_GDMA_LLIStruct;
    }
    GDMA_Init(RX_DMA, &RX_GDMA_InitStruct);
    if (rx_block_num)
    {
        for (int i = 0; i < rx_block_num; i++)
        {
            if (i == rx_block_num - 1)
            {
                RX_GDMA_LLIStruct[i].SAR = RX_GDMA_InitStruct.GDMA_SourceAddr + 65535 * 4 * i;
                RX_GDMA_LLIStruct[i].DAR = (uint32_t)(&IMDC->RX_FIFO);
                RX_GDMA_LLIStruct[i].LLP = 0;
                /* configure low 32 bit of CTL register */
                RX_GDMA_LLIStruct[i].CTL_LOW = (BIT(0)
                                                | (RX_GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                                | (GDMA_DataSize_Word << 4)
                                                | (RX_GDMA_InitStruct.GDMA_DestinationInc << 7)
                                                | (RX_GDMA_InitStruct.GDMA_SourceInc << 9)
                                                | (RX_GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                                | (RX_GDMA_InitStruct.GDMA_SourceMsize << 14)
                                                | (RX_GDMA_InitStruct.GDMA_DIR << 20));
                /* configure high 32 bit of CTL register */
                uint32_t block_size = DMA_compressed_data_size_word - 65535 * i;
                RX_GDMA_LLIStruct[i].CTL_HIGH = block_size;
            }
            else
            {
                RX_GDMA_LLIStruct[i].SAR = RX_GDMA_InitStruct.GDMA_SourceAddr + 65535 * 4 * i;
                RX_GDMA_LLIStruct[i].DAR = (uint32_t)(&IMDC->RX_FIFO);
                RX_GDMA_LLIStruct[i].LLP = (uint32_t)&RX_GDMA_LLIStruct[i + 1];
                /* configure low 32 bit of CTL register */
                RX_GDMA_LLIStruct[i].CTL_LOW = hal_imdc_get_dma_ctl_low_int(RX_DMA);
                /* configure high 32 bit of CTL register */
                RX_GDMA_LLIStruct[i].CTL_HIGH = 65535;
            }
        }
    }

    GDMA_InitTypeDef TX_GDMA_InitStruct;
    uint32_t tx_block_num = 0;
    if (DMA_decompressed_data_size_word >= 65535)
    {
        tx_block_num = DMA_decompressed_data_size_word / 65535;
        if (DMA_decompressed_data_size_word % 65535)
        {
            tx_block_num = tx_block_num + 1;
        }
    }
    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&TX_GDMA_InitStruct);
    TX_GDMA_InitStruct.GDMA_ChannelNum          = dma_cfg->TX_DMA_channel_num;
    TX_GDMA_InitStruct.GDMA_BufferSize          = DMA_decompressed_data_size_word;
    TX_GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_PeripheralToMemory;
    TX_GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    TX_GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
    TX_GDMA_InitStruct.GDMA_SourceMsize         =
        GDMA_Msize_8;                         // 8 msize for source msize
    TX_GDMA_InitStruct.GDMA_DestinationMsize    =
        GDMA_Msize_8;                         // 8 msize for destiantion msize
    TX_GDMA_InitStruct.GDMA_DestinationDataSize =
        GDMA_DataSize_Word;                   // 32 bit width for destination transaction
    TX_GDMA_InitStruct.GDMA_SourceDataSize      =
        GDMA_DataSize_Word;                   // 32 bit width for source transaction
    TX_GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)(&IMDC->TX_FIFO);
    TX_GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)dma_cfg->output_buf;
    hal_imdc_tx_handshake_init(&TX_GDMA_InitStruct);
    if (tx_block_num)
    {
        TX_GDMA_InitStruct.GDMA_BufferSize = 65535;
        TX_GDMA_InitStruct.GDMA_Multi_Block_Mode = LLI_TRANSFER;
        TX_GDMA_InitStruct.GDMA_Multi_Block_En = 1;
        TX_GDMA_InitStruct.GDMA_Multi_Block_Struct = (uint32_t)TX_GDMA_LLIStruct;
    }
    GDMA_Init(TX_DMA, &TX_GDMA_InitStruct);
    if (tx_block_num)
    {
        for (int i = 0; i < tx_block_num; i++)
        {
            if (i == tx_block_num - 1)
            {
                TX_GDMA_LLIStruct[i].SAR = (uint32_t)(&IMDC->TX_FIFO);
                TX_GDMA_LLIStruct[i].DAR = (uint32_t)dma_cfg->output_buf + 65535 * 4 * i;
                TX_GDMA_LLIStruct[i].LLP = 0;
                /* configure low 32 bit of CTL register */
                TX_GDMA_LLIStruct[i].CTL_LOW = (BIT(0)
                                                | (TX_GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                                | (GDMA_DataSize_Word << 4)
                                                | (TX_GDMA_InitStruct.GDMA_DestinationInc << 7)
                                                | (TX_GDMA_InitStruct.GDMA_SourceInc << 9)
                                                | (TX_GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                                | (TX_GDMA_InitStruct.GDMA_SourceMsize << 14)
                                                | (TX_GDMA_InitStruct.GDMA_DIR << 20));
                /* configure high 32 bit of CTL register */
                uint32_t block_size = DMA_decompressed_data_size_word - 65535 * i;
                TX_GDMA_LLIStruct[i].CTL_HIGH = block_size;
            }
            else
            {
                TX_GDMA_LLIStruct[i].SAR = (uint32_t)(&IMDC->TX_FIFO);
                TX_GDMA_LLIStruct[i].DAR = (uint32_t)dma_cfg->output_buf + 65535 * 4 * i;
                TX_GDMA_LLIStruct[i].LLP = (uint32_t)&TX_GDMA_LLIStruct[i + 1];
                /* configure low 32 bit of CTL register */
                TX_GDMA_LLIStruct[i].CTL_LOW = hal_imdc_get_dma_ctl_low_int(TX_DMA);
                /* configure high 32 bit of CTL register */
                TX_GDMA_LLIStruct[i].CTL_HIGH = 65535;
            }
        }
    }
    GDMA_Cmd(dma_cfg->RX_DMA_channel_num, ENABLE);
    GDMA_Cmd(dma_cfg->TX_DMA_channel_num, ENABLE);
    IMDC_ClearINTPendingBit(IMDC_DECOMPRESS_FINISH_INT);
    IMDC_INTConfig(IMDC_DECOMPRESS_FINISH_INT, ENABLE);
    IMDC_MaskINTConfig(IMDC_DECOMPRESS_FINISH_INT, DISABLE);

    IMDC_ClearINTPendingBit(IMDC_DECOMPRESS_ERROR_INT);
    IMDC_INTConfig(IMDC_DECOMPRESS_ERROR_INT, ENABLE);
    IMDC_MaskINTConfig(IMDC_DECOMPRESS_ERROR_INT, DISABLE);

    IMDC_Cmd(ENABLE);
    IMDC_Run(ENABLE);
    while (IMDC->IMDC_CTL0 & BIT0);
    if (IMDC_GetINTStatus(IMDC_DECOMPRESS_ERROR_INT))
    {
        err =  IMDC_ERROR_DECODE_FAIL;
    }
    if (hal_imdc_get_dma_busy_state(dma_cfg->RX_DMA_channel_num))
    {
        GDMA_SuspendCmd(TX_DMA, ENABLE);
        GDMA_SuspendCmd(RX_DMA, ENABLE);
        hal_imdc_wait_dma_idle(RX_DMA);
        hal_imdc_wait_dma_idle(TX_DMA);
        GDMA_Cmd(dma_cfg->RX_DMA_channel_num, DISABLE);
        GDMA_Cmd(dma_cfg->TX_DMA_channel_num, DISABLE);
    }
    IMDC_RxFifoClear();
    IMDC_Cmd(DISABLE);
    while (!(IMDC->IMDC_CTL1 & BIT29));
    IMDC_TxFifoClear();
    return err;
}

IMDC_ERROR IMDC_Decode_Ex(uint8_t *file, IMDC_decode_range *range, IMDC_DMA_config *dma_cfg,
                          IMDC_INT_CFG_t int_cfg)
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
    uint32_t decompressed_data_size = (decompress_end_line - decompress_start_line + 1) *
                                      (decompress_end_column - decompress_start_column + 1)\
                                      * (header->algorithm_type.pixel_bytes + 2);

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
    hal_imdc_hw_handshake_init(&IMDC_struct_init);
    IMDC_Init(&IMDC_struct_init);


    uint32_t DMA_compressed_data_size_word = ((compressed_data_size % 4) ?
                                              (compressed_data_size / 4 + 1) : (compressed_data_size / 4));
    uint32_t DMA_decompressed_data_size_word = ((decompressed_data_size % 4) ?
                                                (decompressed_data_size / 4 + 1) : (decompressed_data_size / 4));
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
        RX_GDMA_InitStruct.GDMA_Multi_Block_Struct = (uint32_t)RX_GDMA_LLIStruct;
    }
    GDMA_Init(RX_DMA, &RX_GDMA_InitStruct);
    if (rx_block_num)
    {
        for (int i = 0; i < rx_block_num; i++)
        {
            if (i == rx_block_num - 1)
            {
                RX_GDMA_LLIStruct[i].SAR = RX_GDMA_InitStruct.GDMA_SourceAddr + 65535 * 4 * i;
                RX_GDMA_LLIStruct[i].DAR = (uint32_t)(&IMDC->RX_FIFO);
                RX_GDMA_LLIStruct[i].LLP = 0;
                /* configure low 32 bit of CTL register */
                RX_GDMA_LLIStruct[i].CTL_LOW = (BIT(0)
                                                | (RX_GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                                | (GDMA_DataSize_Byte << 4)
                                                | (RX_GDMA_InitStruct.GDMA_DestinationInc << 7)
                                                | (RX_GDMA_InitStruct.GDMA_SourceInc << 9)
                                                | (RX_GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                                | (RX_GDMA_InitStruct.GDMA_SourceMsize << 14)
                                                | (RX_GDMA_InitStruct.GDMA_DIR << 20));
                /* configure high 32 bit of CTL register */
                uint32_t block_size = compressed_data_size - 65535 * 4 * i;
                RX_GDMA_LLIStruct[i].CTL_HIGH = block_size;
            }
            else
            {
                RX_GDMA_LLIStruct[i].SAR = RX_GDMA_InitStruct.GDMA_SourceAddr + 65535 * 4 * i;
                RX_GDMA_LLIStruct[i].DAR = (uint32_t)(&IMDC->RX_FIFO);
                RX_GDMA_LLIStruct[i].LLP = (uint32_t)&RX_GDMA_LLIStruct[i + 1];
                /* configure low 32 bit of CTL register */
                RX_GDMA_LLIStruct[i].CTL_LOW = hal_imdc_get_dma_ctl_low_int(RX_DMA);
                /* configure high 32 bit of CTL register */
                RX_GDMA_LLIStruct[i].CTL_HIGH = 65535;
            }
        }
    }

    GDMA_InitTypeDef TX_GDMA_InitStruct;
    uint32_t tx_block_num = 0;
    if (DMA_decompressed_data_size_word >= 65535)
    {
        tx_block_num = DMA_decompressed_data_size_word / 65535;
        if (DMA_decompressed_data_size_word % 65535)
        {
            tx_block_num = tx_block_num + 1;
        }
    }
    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&TX_GDMA_InitStruct);
    TX_GDMA_InitStruct.GDMA_ChannelNum          = dma_cfg->TX_DMA_channel_num;
    TX_GDMA_InitStruct.GDMA_BufferSize          = DMA_decompressed_data_size_word;
    TX_GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_PeripheralToMemory;
    TX_GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    TX_GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
    TX_GDMA_InitStruct.GDMA_SourceMsize         =
        GDMA_Msize_8;                         // 8 msize for source msize
    TX_GDMA_InitStruct.GDMA_DestinationMsize    =
        GDMA_Msize_8;                         // 8 msize for destiantion msize
    TX_GDMA_InitStruct.GDMA_DestinationDataSize =
        GDMA_DataSize_Word;                   // 32 bit width for destination transaction
    TX_GDMA_InitStruct.GDMA_SourceDataSize      =
        GDMA_DataSize_Word;                   // 32 bit width for source transaction
    TX_GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)(&IMDC->TX_FIFO);
    TX_GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)dma_cfg->output_buf;
    hal_imdc_tx_handshake_init(&TX_GDMA_InitStruct);
    if (tx_block_num)
    {
        TX_GDMA_InitStruct.GDMA_BufferSize = 65535;
        TX_GDMA_InitStruct.GDMA_Multi_Block_Mode = LLI_TRANSFER;
        TX_GDMA_InitStruct.GDMA_Multi_Block_En = 1;
        TX_GDMA_InitStruct.GDMA_Multi_Block_Struct = (uint32_t)TX_GDMA_LLIStruct;
    }
    GDMA_Init(TX_DMA, &TX_GDMA_InitStruct);
    if (tx_block_num)
    {
        for (int i = 0; i < tx_block_num; i++)
        {
            if (i == tx_block_num - 1)
            {
                TX_GDMA_LLIStruct[i].SAR = (uint32_t)(&IMDC->TX_FIFO);
                TX_GDMA_LLIStruct[i].DAR = (uint32_t)dma_cfg->output_buf + 65535 * 4 * i;
                TX_GDMA_LLIStruct[i].LLP = 0;
                /* configure low 32 bit of CTL register */
                TX_GDMA_LLIStruct[i].CTL_LOW = (BIT(0)
                                                | (TX_GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                                | (GDMA_DataSize_Word << 4)
                                                | (TX_GDMA_InitStruct.GDMA_DestinationInc << 7)
                                                | (TX_GDMA_InitStruct.GDMA_SourceInc << 9)
                                                | (TX_GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                                | (TX_GDMA_InitStruct.GDMA_SourceMsize << 14)
                                                | (TX_GDMA_InitStruct.GDMA_DIR << 20));
                /* configure high 32 bit of CTL register */
                uint32_t block_size = DMA_decompressed_data_size_word - 65535 * i;
                TX_GDMA_LLIStruct[i].CTL_HIGH = block_size;
            }
            else
            {
                TX_GDMA_LLIStruct[i].SAR = (uint32_t)(&IMDC->TX_FIFO);
                TX_GDMA_LLIStruct[i].DAR = (uint32_t)dma_cfg->output_buf + 65535 * 4 * i;
                TX_GDMA_LLIStruct[i].LLP = (uint32_t)&TX_GDMA_LLIStruct[i + 1];
                /* configure low 32 bit of CTL register */
                TX_GDMA_LLIStruct[i].CTL_LOW = hal_imdc_get_dma_ctl_low_int(TX_DMA);
                /* configure high 32 bit of CTL register */
                TX_GDMA_LLIStruct[i].CTL_HIGH = 65535;
            }
        }
    }
    GDMA_Init(TX_DMA, &TX_GDMA_InitStruct);

    GDMA_Cmd(dma_cfg->RX_DMA_channel_num, ENABLE);
    GDMA_Cmd(dma_cfg->TX_DMA_channel_num, ENABLE);

    if (int_cfg.b.imdc_decompress_finish_int)
    {
        IMDC_ClearINTPendingBit(IMDC_DECOMPRESS_FINISH_INT);
        IMDC_INTConfig(IMDC_DECOMPRESS_FINISH_INT, ENABLE);
        IMDC_MaskINTConfig(IMDC_DECOMPRESS_FINISH_INT, DISABLE);
    }
    if (int_cfg.b.imdc_decompress_error_int)
    {
        IMDC_ClearINTPendingBit(IMDC_DECOMPRESS_ERROR_INT);
        IMDC_INTConfig(IMDC_DECOMPRESS_ERROR_INT, ENABLE);
        IMDC_MaskINTConfig(IMDC_DECOMPRESS_ERROR_INT, DISABLE);
    }
    if (int_cfg.b.line_decompress_finish_int)
    {
        IMDC_ClearINTPendingBit(IMDC_LINE_DECOMPRESS_FINISH_INT);
        IMDC_INTConfig(IMDC_LINE_DECOMPRESS_FINISH_INT, ENABLE);
        IMDC_MaskINTConfig(IMDC_LINE_DECOMPRESS_FINISH_INT, DISABLE);
    }
    if (int_cfg.b.rx_fifo_overflow_int)
    {
        IMDC_ClearINTPendingBit(IMDC_RX_FIFO_OVERFLOW_INT);
        IMDC_INTConfig(IMDC_RX_FIFO_OVERFLOW_INT, ENABLE);
        IMDC_MaskINTConfig(IMDC_RX_FIFO_OVERFLOW_INT, DISABLE);
    }
    if (int_cfg.b.rx_fifo_threshold_int)
    {
        IMDC_ClearINTPendingBit(IMDC_RX_FIFO_THRESHOLD_INT);
        IMDC_INTConfig(IMDC_RX_FIFO_THRESHOLD_INT, ENABLE);
        IMDC_MaskINTConfig(IMDC_RX_FIFO_THRESHOLD_INT, DISABLE);
    }
    if (int_cfg.b.tx_fifo_threshold_int)
    {
        IMDC_ClearINTPendingBit(IMDC_TX_FIFO_THRESHOLD_INT);
        IMDC_INTConfig(IMDC_TX_FIFO_THRESHOLD_INT, ENABLE);
        IMDC_MaskINTConfig(IMDC_TX_FIFO_THRESHOLD_INT, DISABLE);
    }
    if (int_cfg.b.tx_fifo_underflow_int)
    {
        IMDC_ClearINTPendingBit(IMDC_TX_FIFO_UNDERFLOW_INT);
        IMDC_INTConfig(IMDC_TX_FIFO_UNDERFLOW_INT, ENABLE);
        IMDC_MaskINTConfig(IMDC_TX_FIFO_UNDERFLOW_INT, DISABLE);
    }

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = IMDC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    IMDC_Cmd(ENABLE);
    IMDC_Run(ENABLE);
    return IMDC_SUCCESS;
}

IMDC_ERROR IMDC_Decode_Direct(uint8_t *file, IMDC_decode_range *range, IMDC_DMA_config *dma_cfg)
{
    return hal_imdc_decode_direct_int(file, range, dma_cfg, RX_GDMA_LLIStruct);
}
/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/
