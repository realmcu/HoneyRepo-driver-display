/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* \file     rtl_imdc.h
* \brief    This file provides all the LCDC HANDLER firmware functions.
* \details
* \author   astor zhang
* \date     2023-10-17
* \version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef RTL_IMDC_H
#define RTL_IMDC_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include "rtl_imdc_def.h"

/** \defgroup IMDC       IMDC
  * \brief
  * \{
  */
/*============================================================================*
 *                         IMDC Declaration
 *============================================================================*/
/** \defgroup IMDC_Exported_Constants IMDC Exported Constants
  * \brief
  * \{
  */

/** \defgroup IMDC_Declaration IMDC Declaration
  * \{
  * \ingroup  IMDC_Exported_Constants
  */

#define IMDC                           ((IMDC_TypeDef *)IMDC_REG_BASE)

/** End of IMDC_Declaration
  * \}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
typedef enum
{
    IMDC_RLE = 0x00,
    IMDC_FASTLZ = 0x01,
    IMDC_YUV_SAMPLE_BLUR_FASTLZ = 0x02,
    IMDC_YUV_SAMPLE_BLUR = 0x03,
} IMDC_ALGORITHM;

typedef enum
{
    YUV444_SAMPLE = 0x00,
    YUV422_SAMPLE = 0x01,
    YUV411_SAMPLE = 0x02,
} IMDC_YUV_SAMPLE_TYPE;

typedef enum
{
    YUV_BLUR_0BIT = 0x00,
    YUV_BLUR_1BIT = 0x01,
    YUV_BLUR_2BIT = 0x02,
    YUV_BLUR_4BIT = 0x03,
} IMDC_YUV_BLUR_BIT;

typedef enum
{
    PIXEL_SIZE_16BIT = 0x00,
    PIXEL_SIZE_24BIT = 0x01,
    PIXEL_SIZE_32BIT = 0x02,
} IMDC_PIXEL_SIZE;

typedef enum
{
    THROW_AWAY_0BYTE = 0x00,
    THROW_AWAY_1BYTE = 0x01,
    THROW_AWAY_2BYTE = 0x02,
    THROW_AWAY_3BYTE = 0x03,
} IMDC_THROW_AWAY_SIZE;

typedef enum
{
    RUN_LENGTH_SIZE_0BYTE = 0x00,
    RUN_LENGTH_SIZE_1BYTE = 0x01,
    RUN_LENGTH_SIZE_2BYTE = 0x02,
} IMDC_RLE_RUNLENGTH_SIZE;

typedef enum
{
    IMDC_SUCCESS = 0x0,
    IMDC_ERROR_NULL_INPUT,
    IMDC_ERROR_DECODE_FAIL,
    IMDC_ERROR_START_EXCEED_BOUNDARY,
    IMDC_ERROR_END_EXCEED_BOUNDARY,
    IMDC_ERROR_START_LARGER_THAN_END,
    IMDC_ERROR_INVALID_PARAM,
} IMDC_ERROR;

typedef enum
{
    IMDC_HW_HANDSHAKE_DMA = 0x0,
    IMDC_HW_HANDSHAKE_PPE = 0x1,
} IMDC_HW_HANDSHAKE;

typedef struct
{
    struct
    {
        uint8_t algorithm: 2;
        uint8_t feature_1: 2;
        uint8_t feature_2: 2;
        uint8_t pixel_bytes: 2;
    } algorithm_type;
    uint8_t reserved[3];
    uint32_t raw_pic_width;
    uint32_t raw_pic_height;
} IMDC_file_header;

typedef struct
{
    uint32_t start_line;
    uint32_t end_line;
    uint32_t start_column;
    uint32_t end_column;
} IMDC_decode_range;

typedef struct
{
    uint32_t RX_DMA_channel_num;
    uint32_t RX_FIFO_INT_threshold;
    uint32_t TX_DMA_channel_num;
    uint32_t TX_FIFO_INT_threshold;
    uint32_t *output_buf;
} IMDC_DMA_config;

typedef struct
{
    IMDC_ALGORITHM algorithm_type;
    IMDC_THROW_AWAY_SIZE head_throw_away_byte_num;
    IMDC_PIXEL_SIZE pic_pixel_size;
    IMDC_RLE_RUNLENGTH_SIZE pic_length2_size;
    IMDC_RLE_RUNLENGTH_SIZE pic_length1_size;
    IMDC_YUV_BLUR_BIT yuv_blur_bit;
    IMDC_YUV_SAMPLE_TYPE yuv_sample_type;
    IMDC_HW_HANDSHAKE hw_handshake;
    uint32_t pic_decompress_height;
    uint32_t pic_raw_width;
    uint32_t tx_column_start;
    uint32_t tx_column_end;
    uint32_t compressed_data_size;
    uint32_t rx_fifo_dma_enable;
    uint32_t tx_fifo_dma_enable;
    uint8_t rx_fifo_dma_threshold;
    uint8_t tx_fifo_dma_threshold;
    uint8_t tx_fifo_int_threshold;
    uint8_t rx_fifo_int_threshold;
} IMDC_InitTypeDef;

typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t imdc_decompress_error_int: 1;
        __IO uint32_t tx_fifo_underflow_int: 1;
        __IO uint32_t tx_fifo_threshold_int: 1;
        __IO uint32_t rx_fifo_overflow_int: 1;
        __IO uint32_t rx_fifo_threshold_int: 1;
        __IO uint32_t line_decompress_finish_int: 1;
        __IO uint32_t imdc_decompress_finish_int: 1;
        __I uint32_t reserved_0: 25;
    } b;
} IMDC_INT_CFG_t;

/**
 * \defgroup    IMDC_FIFO_Width IMDC FIFO Width
 * \{
 * \ingroup     IMDC_Exported_Constants
 */
#define IMDC_TX_FIFO_WIDTH             32
#define IMDC_RX_FIFO_WIDTH             32

/** End of IMDC_FIFO_Width
  * \}
  */

/**
 * \defgroup    IMDC_FIFO_Depth IMDC FIFO Depth
 * \{
 * \ingroup     IMDC_Exported_Constants
 */
#define IMDC_TX_FIFO_DEPTH             16
#define IMDC_RX_FIFO_DEPTH             16

/** End of IMDC_FIFO_Depth
  * \}
  */

/**
 * \defgroup    IMDC_Interrupts_Definition IMDC Interrupts Definition
 * \{
 * \ingroup     IMDC_Exported_Constants
 */
#define IMDC_DECOMPRESS_FINISH_INT                                 ((uint32_t)BIT(6))
#define IMDC_LINE_DECOMPRESS_FINISH_INT                            ((uint32_t)BIT(5))
#define IMDC_RX_FIFO_THRESHOLD_INT                                 ((uint32_t)BIT(4))
#define IMDC_RX_FIFO_OVERFLOW_INT                                  ((uint32_t)BIT(3))
#define IMDC_TX_FIFO_THRESHOLD_INT                                 ((uint32_t)BIT(2))
#define IMDC_TX_FIFO_UNDERFLOW_INT                                  ((uint32_t)BIT(1))
#define IMDC_DECOMPRESS_ERROR_INT                                  ((uint32_t)BIT(0))

#define IS_IMDC_INT(INT)              (((INT) == IMDC_DECOMPRESS_FINISH_INT)         || \
                                       ((INT) == IMDC_LINE_DECOMPRESS_FINISH_INT)  || \
                                       ((INT) == IMDC_RX_FIFO_THRESHOLD_INT)         || \
                                       ((INT) == IMDC_RX_FIFO_OVERFLOW_INT)  || \
                                       ((INT) == IMDC_TX_FIFO_THRESHOLD_INT)      || \
                                       ((INT) == IMDC_TX_FIFO_UNDERFLOW_INT)         || \
                                       ((INT) == IMDC_DECOMPRESS_ERROR_INT)

/** End of IMDC_Interrupts_Definition
  * \}
  */

/**
 * \defgroup    IMDC_ALGO IMDC ALGO
 * \{
 * \ingroup     IMDC_Exported_Constants
 */
#define IMDC_ALGO_RLE                      (0)
#define IMDC_ALGO_FASTLZ                   (BIT(0))
#define IMDC_ALGO_YUV_BLUR_FASTLZ          (BIT(1))
#define IMDC_ALGO_YUV_BLUR                 (BIT(1)|BIT(0))
#define IS_IMDC_ALGORITHM(ALGO)             (((ALGO) == IMDC_ALGO_RLE) || ((ALGO) == IMDC_ALGO_FASTLZ)      ||\
                                             ((ALGO) == IMDC_ALGO_YUV_BLUR_FASTLZ) || ((ALGO) == IMDC_ALGO_YUV_BLUR))

/** End of IMDC_ALGO
  * \}
  */

/**
 * \defgroup    IMDC_PIXEL_SIZE IMDC PIXEL SIZE
 * \{
 * \ingroup     IMDC_Exported_Constants
 */
#define IMDC_PIXEL_16BIT                   (0)
#define IMDC_PIXEL_24BIT                   (BIT(0))
#define IMDC_PIXEL_32BIT                   (BIT(1))
#define IS_IMDC_PIXEL_BYTES(NUM)           (((NUM) == IMDC_PIXEL_16BIT) || ((NUM) == IMDC_PIXEL_24BIT) || ((NUM) == IMDC_PIXEL_32BIT))

/** End of IMDC_PIXEL_SIZE
  * \}
  */

/**
 * \defgroup    IMDC_RLE_SIZE_LEN IMDC RLE SIZE LEN
 * \{
 * \ingroup     IMDC_Exported_Constants
 */
#define IMDC_RLE_1BYTE_LEN                 (BIT(0))
#define IMDC_RLE_2BYTE_LEN                 (BIT(1))
#define IS_IMDC_RLE_BYTE_LEN(LEN)          (((LEN) == IMDC_RLE_1BYTE_LEN) || ((LEN) == IMDC_RLE_2BYTE_LEN))

/** End of IMDC_RLE_SIZE_LEN
  * \}
  */

/**
 * \defgroup    IMDC_YUV_BLUR_SIZE IMDC YUV BLUR SIZE
 * \{
 * \ingroup     IMDC_Exported_Constants
 */
#define IMDC_YUV_BLUR_0BIT                 (0)
#define IMDC_YUV_BLUR_1BIT                 (BIT(0))
#define IMDC_YUV_BLUR_2BIT                 (BIT(1))
#define IMDC_YUV_BLUR_4BIT                 (BIT(0)|BIT(1))

#define IS_IMDC_YUB_BLUR_BIT(NUM)          (((NUM) == IMDC_YUV_BLUR_0BIT) || ((NUM) == IMDC_YUV_BLUR_1BIT)      ||\
                                            ((NUM) == IMDC_YUV_BLUR_2BIT) || ((NUM) == IMDC_YUV_BLUR_4BIT))

/** End of IMDC_YUV_BLUR_SIZE
  * \}
  */

/**
 * \defgroup    IMDC_YUV_TYPE IMDC YUV TYPE
 * \{
 * \ingroup     IMDC_Exported_Constants
 */
#define IMDC_YUV_TYPE_444                  (0)
#define IMDC_YUV_TYPE_422                  (BIT(0))
#define IMDC_YUV_TYPE_411                  (BIT(1))

#define IS_IMDC_YUV_TYPE(TYPE)             (((TYPE) == IMDC_YUV_TYPE_444) || ((TYPE) == IMDC_YUV_TYPE_422) || ((TYPE) == IMDC_YUV_TYPE_411))

/** End of IMDC_YUV_TYPE
  * \}
  */

/** End of IMDC_Exported_Constants
  * \}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** \defgroup IMDC_Exported_Functions IMDC Exported Functions
  * \brief
  * \{
  */

/**
 * \brief  Clear TX FIFO
 */
void IMDC_TxFifoClear(void);

/**
 * \brief  Clear RX FIFO
 */
void IMDC_RxFifoClear(void);

/**
 * \brief  Clear internal memory
 */
void IMDC_AllMemClear(void);

/**
 * \brief  Enable/Disable IMDC interrupt
 * \param[in] IMDC_INT_EN         interrupt to be operated.
 * \param[in] state               enable/disable interrupt.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        IMDC_INTConfig(IMDC_DECOMPRESS_FINISH_INT, ENABLE);
    }
 * \endcode
 */
void IMDC_INTConfig(uint32_t IMDC_INT_EN, FunctionalState state);

/**
 * \brief  Get interrupt status of IMDC
 * \param[in] IMDC_INT         interrupts to be obtained.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        IMDC_INT_CFG_t internal_int_cfg = {.d32 = 0};
        internal_int_cfg.b.imdc_decompress_finish_int = 1;
        internal_int_cfg.b.imdc_decompress_error_int = 1;
        IMDC_INT_CFG_t ret_int = IMDC_GetINTStatus(internal_int_cfg);
        if(rev.b.imdc_decompress_finish_int)
        {
            ...
        }
    }
 * \endcode
 */
ITStatus IMDC_GetINTStatus(uint32_t IMDC_INT);

/**
 * \brief  Mask/Unmask IMDC interrupt
 * \param[in] IMDC_INT_MSK         interrupts to be masked.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        IMDC_INT_CFG_t internal_int_cfg = {.d32 = 0};
        internal_int_cfg.b.imdc_decompress_finish_int = 1;
        internal_int_cfg.b.imdc_decompress_error_int = 1;
        IMDC_MaskINTConfig(internal_int_cfg);
    }
 * \endcode
 */
void IMDC_MaskINTConfig(uint32_t IMDC_INT_MSK, FunctionalState state);

/**
 * \brief  Clear pending IMDC interrupt
 * \param[in] IMDC_INT         interrupts to be cleared.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        IMDC_INT_CFG_t internal_int_cfg = {.d32 = 0};
        internal_int_cfg.b.imdc_decompress_finish_int = 1;
        internal_int_cfg.b.imdc_decompress_error_int = 1;
        IMDC_MaskINTConfig(internal_int_cfg);
    }
 * \endcode
 */
void IMDC_ClearINTPendingBit(uint32_t IMDC_INT);

/**
 * \brief  Get IMDC internal buffer state
 * \return buffer state
 * \retval 0        Not empty.
 * \retval 1        Empty.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        ITStatus ret = IMDC_GetInternalBufferState();
        if(ret)
        {
            ...
        }
    }
 * \endcode
 */
ITStatus IMDC_GetInternalBufferState(void);

/**
 * \brief  Get vertical counter of decompress process
 * \return counter value starts from 0
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        uint32_t cnt = IMDC_GetDecompressLine();
        DBG_DIRECT("%d lines has been decompressed", cnt + 1);
    }
 * \endcode
 */
uint32_t IMDC_GetDecompressLine(void);

/**
 * \brief  Get horizontal counter of decompress process
 * \return counter value starts from 0
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        uint32_t cnt = IMDC_GetHorizontalByteNum();
        DBG_DIRECT("%d pixels has been decompressed", cnt + 1);
    }
 * \endcode
 */
uint32_t IMDC_GetHorizontalByteNum(void);

/**
 * \brief  the size of compressed data which DMA will transmit from FLASH, measured in byte
 * \return data size
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        uint32_t data_size = IMDC_GetCompressedSize();
        DBG_DIRECT("%d bytes will be decompressed", data_size);
    }
 * \endcode
 */
uint32_t IMDC_GetCompressedSize(void);

/**
 * \brief  IMDC decode function, decode file from memory/flash
 * \param[in] file          address of source.
 * \param[in] range         decode range.
 * \param[in] dma_cfg       determine which DMA channel is selected.
 * \return operation result
 * \retval IMDC_SUCCESS    Operation success.
 * \retval others           Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        RCC_PeriphClockCmd(APBPeriph_IMDC, APBPeriph_IMDC_CLOCK, ENABLE);
        IMDC_file_header* header = ( IMDC_file_header*)file_data;
        DBG_DIRECT("algo = %d, blur %d, YUV %d", header->algorithm_type.algorithm, header->algorithm_type.feature_2, header->algorithm_type.feature_1);
        IMDC_decode_range range = {0, header->raw_pic_height - 1, 0, header->raw_pic_width - 1};
        IMDC_DMA_config dma_cfg;
        dma_cfg.output_buf = (uint32_t*)buf;
        dma_cfg.RX_DMA_channel_num = 0;
        dma_cfg.TX_DMA_channel_num = 1;
        IMDC_Decode(file_data, range, dma_cfg);
    }
 * \endcode
 */
IMDC_ERROR IMDC_Decode(uint8_t *file, IMDC_decode_range *range, IMDC_DMA_config *dma_cfg);

/**
 * \brief  IMDC decode function with interrupt, decode file from memory/flash
 * \param[in] file          address of source.
 * \param[in] range         decode range.
 * \param[in] dma_cfg       determine which DMA channel is selected.
 * \return operation result
 * \retval IMDC_SUCCESS    Operation started.
 * \retval others           Operation aborted.
 *
 * <b>Example usage</b>
 * \code{.c}
    void IMDC_Handler(void){
        if(IMDC_GetINTStatus(IMDC_DECOMPRESS_FINISH_INT_STATUS_MSK) == SET){
            IMDC_ClearINTPendingBit(IMDC_DECOMPRESS_FINISH_INT_STATUS_MSK);
            DBG_DIRECT("IMDC_IRQHandler line = %d\n",__LINE__);
            for(int i = 0; i < sizeof(decode_file_data); i++){
                if(decode_file_data[i] != buf[i]){

                    DBG_DIRECT("Decode Error at raw[%d] %x || out[%d] %x ",i, decode_file_data[i], i ,buf[i]);
                }
            }
            DBG_DIRECT("Decompress start bit is : %d", IMDC->IMDC_CTL0 & BIT0);
        }
        if(IMDC_GetINTStatus(IMDC_DECOMPRESS_ERROR_INT_STATUS_MSK) == SET){
            IMDC_ClearINTPendingBit(IMDC_DECOMPRESS_ERROR_INT_STATUS_MSK);
            DBG_DIRECT("IMDC_IRQHandler line = %d\n",__LINE__);
        }
    }
    void test_code(void){
        RCC_PeriphClockCmd(APBPeriph_IMDC, APBPeriph_IMDC_CLOCK, ENABLE);
        IMDC_file_header* header = ( IMDC_file_header*)file_data;
        DBG_DIRECT("algo = %d, blur %d, YUV %d", header->algorithm_type.algorithm, header->algorithm_type.feature_2, header->algorithm_type.feature_1);
        IMDC_decode_range range = {0, header->raw_pic_height - 1, 0, header->raw_pic_width - 1};
        IMDC_DMA_config dma_cfg;
        dma_cfg.output_buf = (uint32_t*)buf;
        dma_cfg.RX_DMA_channel_num = 0;
        dma_cfg.TX_DMA_channel_num = 1;
        IMDC_INT_CFG_t int_cfg = {.d32 = 0};
        int_cfg.b.imdc_decompress_finish_int = 1;
        int_cfg.b.imdc_decompress_error_int = 1;
        IMDC_Decode_Ex(file_data, range, dma_cfg, int_cfg);
    }
 * \endcode
 */
IMDC_ERROR IMDC_Decode_Ex(uint8_t *file, IMDC_decode_range *range, IMDC_DMA_config *dma_cfg,
                          IMDC_INT_CFG_t int_cfg);

/**
 * \brief  Enable IMDC
 * \param[in] state               enable or disable IMDC peripheral.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        IMDC_Cmd(ENABLE);
        IMDC_Run(ENABLE);
    }
 * \endcode
 */
void IMDC_Cmd(FunctionalState state);

/**
 * \brief  Start IMDC to decode, must envoke after enable IMDC
 * \param[in] state               enable or disable IMDC decode.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        IMDC_Cmd(ENABLE);
        IMDC_Run(ENABLE);
    }
 * \endcode
 */
void IMDC_Run(FunctionalState state);

/**
 * \brief  Initialize IMDC with parameters filled in structure
 * \param[in] IMDC_init_struct      pointer to initialize structure
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        IMDC_InitTypeDef IMDC_struct_init;
        IMDC_struct_init.algorithm_type            = IMDC_ALGO_RLE;
        IMDC_struct_init.head_throw_away_byte_num  = THROW_AWAY_0BYTE;
        IMDC_struct_init.pic_pixel_size            = PIXEL_SIZE_16BIT;
        IMDC_struct_init.pic_decompress_height     = 100;
        IMDC_struct_init.pic_raw_width             = 500;
        IMDC_struct_init.tx_column_start           = 15;
        IMDC_struct_init.tx_column_end             = 300 ;
        IMDC_struct_init.compressed_data_size      = compressed_data_size;
        IMDC_struct_init.pic_length2_size          = IMDC_RLE_1BYTE_LEN;
        IMDC_struct_init.pic_length1_size          = IMDC_RLE_1BYTE_LEN;
        IMDC_struct_init.yuv_blur_bit              = 0;
        IMDC_struct_init.yuv_sample_type           = 0;
        IMDC_struct_init.rx_fifo_dma_enable        = (uint32_t)ENABLE;
        IMDC_struct_init.tx_fifo_dma_enable        = (uint32_t)ENABLE;
        IMDC_struct_init.rx_fifo_dma_threshold     = IMDC_RX_FIFO_DEPTH / 2;
        IMDC_struct_init.tx_fifo_dma_threshold     = IMDC_TX_FIFO_DEPTH / 2;
        IMDC_struct_init.rx_fifo_int_threshold     = 8;
        IMDC_struct_init.tx_fifo_int_threshold     = 8;
        IMDC_struct_init.hw_handshake              = IMDC_HW_HANDSHAKE_DMA;
        IMDC_Init(&IMDC_struct_init);
    }
 * \endcode
 */
void IMDC_Init(IMDC_InitTypeDef *IMDC_init_struct);

/**
 * \brief  Decode compressed data and send directly to PPE
 * \param[in] file          address of source.
 * \param[in] range         decode range.
 * \param[in] dma_cfg       determine which DMA channel is selected.
 * \return operation result
 * \retval IMDC_SUCCESS    Operation success.
 * \retval others           Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        RCC_PeriphClockCmd(APBPeriph_IMDC, APBPeriph_IMDC_CLOCK, ENABLE);
        IMDC_file_header* header = ( IMDC_file_header*)file_data;
        IMDC_decode_range range = {0, header->raw_pic_height - 1, 0, header->raw_pic_width - 1};
        IMDC_DMA_config dma_cfg;
        dma_cfg.output_buf = (uint32_t*)buf;
        dma_cfg.RX_DMA_channel_num = 0;
        IMDC_Decode_Direct(file_data, range, dma_cfg);
        //Note: Continue with PPE operation
    }
 * \endcode
 */
IMDC_ERROR IMDC_Decode_Direct(uint8_t *file, IMDC_decode_range *range,
                              IMDC_DMA_config *dma_cfg);

/**
 * \brief  Get the start address of certain line in compressed file
 * \param[in] compressed_start_address          start address of entire compressed file.
 * \param[in] line_number                       specified line number
 * \return start address of specified line
 * \retval 0                Invalid line number.
 * \retval others           Valid address.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        uint32_t file_address = (uint32_t)SAMPLE_FILE;
        uint32_t line_addr = IMDC_Get_Line_Start_Address(file_address, 100);
    }
 * \endcode
 */
uint32_t IMDC_Get_Line_Start_Address(uint32_t compressed_start_address,
                                     uint32_t line_number);

/** End of IMDC_Exported_Functions
  * \}
  */

/** End of IMDC
  * \}
  */

#ifdef __cplusplus
}
#endif

#endif /* RTL_IMDC_H */

/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/
