#include "os_mem.h"
#include "string.h"
#include "hal_imdc.h"
#include "hal_imdc_int.h"
#include "rtl_imdc_int.h"
#include "rtl_imdc.h"

static uint8_t high_speed_dma = 0xA5, low_speed_dma = 0xA5;

void hal_dma_copy(hal_imdc_dma_info *info, uint8_t *src, uint8_t *dst)
{
    bool use_LLI = true;
    if ((info->length == info->src_stride && info->length == info->dst_stride) || info->height == 1)
    {
        use_LLI = false;
    }
    uint32_t dma_height = info->height;
    uint32_t buffer_size, total_size = 0;
    uint8_t m_size = 0, data_size = 0;
    if (info->length % 4 == 0 || use_LLI == false)
    {
        data_size = GDMA_DataSize_Word;
        m_size = GDMA_Msize_16;
        if (use_LLI)
        {
            buffer_size = info->length / 4;
        }
        else
        {
            buffer_size = info->length * info->height / 4;
            if (info->length % 4)
            {
                buffer_size += 1;
            }
            if (buffer_size > 65535)
            {
                use_LLI = true;
                dma_height = buffer_size / 65535;
                if (buffer_size % 65535)
                {
                    dma_height += 1;
                }
                total_size = buffer_size;
                buffer_size = 65535;
            }
        }
    }
    else if (info->length % 2 == 0)
    {
        data_size = GDMA_DataSize_HalfWord;
        m_size = GDMA_Msize_32;
        buffer_size = info->length / 2;
    }
    else
    {
        data_size = GDMA_DataSize_Byte;
        m_size = GDMA_Msize_64;
        buffer_size = info->length;
    }

    GDMA_LLIDef *GDMA_LLIStruct;
    if (use_LLI)
    {
        GDMA_LLIStruct = os_mem_alloc(RAM_TYPE_DATA_ON, dma_height * sizeof(GDMA_LLIDef));
        if (GDMA_LLIStruct == NULL)
        {
            assert_param(GDMA_LLIStruct != NULL);
        }
        else
        {
            memset(GDMA_LLIStruct, 0, dma_height * sizeof(GDMA_LLIDef));
        }
    }
    uint32_t start_address = (uint32_t)src;
    uint32_t dest_address = (uint32_t)dst;
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    GDMA_ChannelTypeDef *dma_channel = hal_imdc_get_dma_channel_int(high_speed_dma);
    GDMA_InitTypeDef RX_GDMA_InitStruct;
    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&RX_GDMA_InitStruct);
    RX_GDMA_InitStruct.GDMA_ChannelNum          = high_speed_dma;
    RX_GDMA_InitStruct.GDMA_BufferSize          = buffer_size;
    RX_GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToMemory;
    RX_GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    RX_GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
    RX_GDMA_InitStruct.GDMA_SourceMsize         =
        m_size;                         // 8 msize for source msize
    RX_GDMA_InitStruct.GDMA_DestinationMsize    =
        GDMA_Msize_32;                         // 8 msize for destiantion msize
    RX_GDMA_InitStruct.GDMA_DestinationDataSize =
        GDMA_DataSize_HalfWord;                   // 32 bit width for destination transaction
    RX_GDMA_InitStruct.GDMA_SourceDataSize      =
        data_size;                   // 32 bit width for source transaction
    RX_GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)start_address;
    RX_GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)dest_address;

    if (use_LLI)
    {
        RX_GDMA_InitStruct.GDMA_Multi_Block_Mode = LLI_TRANSFER;
        RX_GDMA_InitStruct.GDMA_Multi_Block_En = 1;
        RX_GDMA_InitStruct.GDMA_Multi_Block_Struct = (uint32_t)GDMA_LLIStruct;
    }

    GDMA_Init(dma_channel, &RX_GDMA_InitStruct);
    if (use_LLI && GDMA_LLIStruct != NULL)
    {
        for (int i = 0; i < dma_height; i++)
        {
            if (i == dma_height - 1)
            {
                GDMA_LLIStruct[i].SAR = start_address + info->src_stride * i;
                GDMA_LLIStruct[i].DAR = (uint32_t)dest_address + info->dst_stride * i;
                GDMA_LLIStruct[i].LLP = 0;
                /* configure low 32 bit of CTL register */
                GDMA_LLIStruct[i].CTL_LOW = (BIT(0)
                                             | (RX_GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                             | (data_size << 4)
                                             | (RX_GDMA_InitStruct.GDMA_DestinationInc << 7)
                                             | (RX_GDMA_InitStruct.GDMA_SourceInc << 9)
                                             | (RX_GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                             | (RX_GDMA_InitStruct.GDMA_SourceMsize << 14)
                                             | (RX_GDMA_InitStruct.GDMA_DIR << 20));
                /* configure high 32 bit of CTL register */
                GDMA_LLIStruct[i].CTL_HIGH = buffer_size;
            }
            else
            {
                GDMA_LLIStruct[i].SAR = start_address + info->src_stride * i;
                GDMA_LLIStruct[i].DAR = (uint32_t)dest_address + info->dst_stride * i;
                GDMA_LLIStruct[i].LLP = (uint32_t)&GDMA_LLIStruct[i + 1];
                /* configure low 32 bit of CTL register */
                GDMA_LLIStruct[i].CTL_LOW = hal_imdc_get_dma_ctl_low_int(dma_channel);
                /* configure high 32 bit of CTL register */
                if (total_size == 0)
                {
                    GDMA_LLIStruct[i].CTL_HIGH = buffer_size;
                }
                else
                {
                    GDMA_LLIStruct[i].CTL_HIGH = total_size - i * buffer_size;
                }
            }
        }
    }
    GDMA_INTConfig(high_speed_dma, GDMA_INT_Transfer, ENABLE);
    GDMA_Cmd(high_speed_dma, ENABLE);
    while (GDMA_GetTransferINTStatus(high_speed_dma) != SET);
    GDMA_ClearINTPendingBit(high_speed_dma, GDMA_INT_Transfer);
    if (use_LLI && GDMA_LLIStruct != NULL)
    {
        os_mem_free(GDMA_LLIStruct);
    }
}

bool hal_imdc_decompress(hal_imdc_decompress_info *info, uint8_t *dst)
{
    IMDC_file_header *header = (IMDC_file_header *)info->raw_data_address;
    IMDC_decode_range range;
    range.start_column = info->start_column;
    range.end_column = info->end_column;
    range.start_line = info->start_line;
    range.end_line = info->end_line;

    IMDC_DMA_config dma_cfg = {0};
    dma_cfg.output_buf = (uint32_t *)dst;
    dma_cfg.RX_DMA_channel_num = low_speed_dma;
    dma_cfg.TX_DMA_channel_num = high_speed_dma;
    dma_cfg.TX_FIFO_INT_threshold = 8;
    dma_cfg.RX_FIFO_INT_threshold = 8;
    IMDC_ERROR err = IMDC_Decode((uint8_t *)header, &range, &dma_cfg);
    if (err != IMDC_SUCCESS)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool hal_imdc_decompress_rect(hal_imdc_decompress_info *info, uint8_t *dst)
{
    uint32_t dst_start_address = (uint32_t)dst;
    RCC_PeriphClockCmd(APBPeriph_IMDC, APBPeriph_IMDC_CLOCK, ENABLE);
    IMDC_DMA_config config = {0};
    IMDC_DMA_config *dma_cfg = &config;
    dma_cfg->output_buf = (uint32_t *)dst_start_address;
    dma_cfg->TX_DMA_channel_num = low_speed_dma;
    dma_cfg->RX_DMA_channel_num = high_speed_dma;
    dma_cfg->TX_FIFO_INT_threshold = 8;
    dma_cfg->RX_FIFO_INT_threshold = 8;
    uint32_t compressed_data_start_address = info->raw_data_address;
    IMDC_file_header *header = (IMDC_file_header *)compressed_data_start_address;

    uint32_t decompress_start_line = info->start_line;
    uint32_t decompress_end_line = info->end_line;
    uint32_t decompress_start_column = info->start_column;
    uint32_t decompress_end_column = info->end_column;
    uint32_t buffer_size = info->length / 4;
    uint32_t start_line_address = IMDC_Get_Line_Start_Address(compressed_data_start_address,
                                                              decompress_start_line);
    uint32_t compressed_data_size = IMDC_Get_Line_Start_Address(compressed_data_start_address,
                                                                decompress_end_line + 1) - start_line_address;
    IMDC_ERROR err = IMDC_SUCCESS;
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
    /* Configure DMA */
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    GDMA_InitTypeDef RX_GDMA_InitStruct;
    GDMA_ChannelTypeDef *RX_DMA = hal_imdc_get_dma_channel_int(dma_cfg->RX_DMA_channel_num);
    GDMA_ChannelTypeDef *TX_DMA = hal_imdc_get_dma_channel_int(dma_cfg->TX_DMA_channel_num);
    GDMA_LLIDef *RX_GDMA_LLIStruct;
    GDMA_LLIDef *TX_GDMA_LLIStruct;
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
        RX_GDMA_LLIStruct = os_mem_alloc(RAM_TYPE_DATA_ON, sizeof(GDMA_LLIDef) * rx_block_num);
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
    uint32_t tx_block_num = decompress_end_line - decompress_start_line + 1;
    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&TX_GDMA_InitStruct);
    TX_GDMA_InitStruct.GDMA_ChannelNum          = dma_cfg->TX_DMA_channel_num;
    TX_GDMA_InitStruct.GDMA_BufferSize          = buffer_size;
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
        TX_GDMA_LLIStruct = os_mem_alloc(RAM_TYPE_DATA_ON, sizeof(GDMA_LLIDef) * tx_block_num);
        TX_GDMA_InitStruct.GDMA_BufferSize = buffer_size;
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
                TX_GDMA_LLIStruct[i].DAR = (uint32_t)dma_cfg->output_buf + info->dst_stride * i;
                TX_GDMA_LLIStruct[i].LLP = 0;
                /* configure low 32 bit of CTL register */
                TX_GDMA_LLIStruct[i].CTL_LOW = (BIT(0)
                                                | (TX_GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                                | (TX_GDMA_InitStruct.GDMA_SourceDataSize << 4)
                                                | (TX_GDMA_InitStruct.GDMA_DestinationInc << 7)
                                                | (TX_GDMA_InitStruct.GDMA_SourceInc << 9)
                                                | (TX_GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                                | (TX_GDMA_InitStruct.GDMA_SourceMsize << 14)
                                                | (TX_GDMA_InitStruct.GDMA_DIR << 20));
                /* configure high 32 bit of CTL register */
                TX_GDMA_LLIStruct[i].CTL_HIGH = buffer_size;
            }
            else
            {
                TX_GDMA_LLIStruct[i].SAR = (uint32_t)(&IMDC->TX_FIFO);
                TX_GDMA_LLIStruct[i].DAR = (uint32_t)dma_cfg->output_buf + info->dst_stride * i;
                TX_GDMA_LLIStruct[i].LLP = (uint32_t)&TX_GDMA_LLIStruct[i + 1];
                /* configure low 32 bit of CTL register */
                TX_GDMA_LLIStruct[i].CTL_LOW = hal_imdc_get_dma_ctl_low_int(TX_DMA);
                /* configure high 32 bit of CTL register */
                TX_GDMA_LLIStruct[i].CTL_HIGH = buffer_size;
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
    if (rx_block_num)
    {
        os_mem_free(RX_GDMA_LLIStruct);
    }
    if (tx_block_num)
    {
        os_mem_free(TX_GDMA_LLIStruct);
    }
    if (err != IMDC_SUCCESS)
    {
        return false;
    }
    else
    {
        return true;
    }
}


void hal_dma_channel_init(uint8_t *high_speed_channel, uint8_t *low_speed_channel)
{
    hal_imdc_channel_init_int(high_speed_channel, low_speed_channel);
    high_speed_dma = *high_speed_channel;
    low_speed_dma = *low_speed_channel;
}


