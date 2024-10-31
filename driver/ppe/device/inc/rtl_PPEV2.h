/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl_PPEV2.h
* \brief    This file provides all the PPE 2.0 firmware functions.
* \details
* \author   feihu wang
* \date     2023-10-17
* \version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef RTL_PPEV2_H
#define RTL_PPEV2_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "rtl_PPEV2_reg.h"

/*============================================================================*
 *                         PPEV2 Registers Memory Map
 *============================================================================*/
typedef struct
{
    __IO uint32_t REG_GLB_STATUS;                       /* offset 0x00*/
    __IO uint32_t REG_LYR_ENABLE;                       /* offset 0x04*/
    __IO uint32_t REG_LD_CFG;                           /* offset 0x08*/
    __IO uint32_t REG_LL_CFG;                           /* offset 0x0C*/
    __IO uint32_t REG_LLP;                              /* offset 0x10*/
    __I  uint32_t REG_RESERVED1;                        /* offset 0x14*/
    __IO uint32_t REG_SECURE;                           /* offset 0x18*/
    __IO uint32_t REG_LINE_OVER_INDEX;                  /* offset 0x1C*/
    __I  uint32_t REG_RELEASE_DATE;                     /* offset 0x20*/
    __I  uint32_t REG_RTL_VER;                          /* offset 0x24*/
    __I  uint32_t REG_RESERVED2[6];                     /* offset 0x28-3C*/
    __I  uint32_t REG_INTR_STATUS;                      /* offset 0x40*/
    __I  uint32_t REG_INTR_RAW;                         /* offset 0x44*/
    __O  uint32_t REG_INTR_CLR;                         /* offset 0x48*/
    __IO uint32_t REG_INTR_MASK;                        /* offset 0x4C*/
    __I  uint32_t REG_BUS_ERR_DETAIL;                   /* offset 0x50*/
    __I  uint32_t REG_DIV0_ERR_DETAIL;                  /* offset 0x54*/
    __I  uint32_t REG_RESERVED3[2];                     /* offset 0x58-0x5C*/
} PPEV2_Typedef;

typedef struct
{
    __IO uint32_t REG_LYR0_ADDR;                /* offset 0x60*/
    __I  uint32_t REG_RESERVED1;                /* offset 0x64*/
    __IO uint32_t REG_CANVAS_SIZE;              /* offset 0x68*/
    __IO uint32_t REG_LYR0_PIC_CFG;             /* offset 0x6C*/
    __IO uint32_t REG_BACKGROUND;               /* offset 0x70*/
    __IO uint32_t REG_LYR0_BUS_CFG;             /* offset 0x74*/
    __IO uint32_t REG_LYR0_HS_CFG;              /* offset 0x78*/
    __I  uint32_t REG_RESERVED2;                /* offset 0x7C*/
} PPEV2_ResultLayer_Typedef;


typedef struct
{
    __IO uint32_t REG_LYRx_ADDR;                                /* offset 0x00 + X * 0x80*/
    __IO uint32_t REG_RESERVED1;                                /* offset 0x04 + X * 0x80*/
    __IO uint32_t REG_LYRx_PIC_SIZE;                            /* offset 0x08 + X * 0x80*/
    __IO uint32_t REG_LYRx_PIC_CFG;                             /* offset 0x0C + X * 0x80*/
    __IO uint32_t REG_LYRx_FIXED_COLOR;                         /* offset 0x10 + X * 0x80*/
    __IO uint32_t REG_LYRx_BUS_CFG;                             /* offset 0x14 + X * 0x80*/
    __IO uint32_t REG_LYRx_HS_CFG;                              /* offset 0x18 + X * 0x80*/
    __IO uint32_t REG_LYRx_WIN_MIN;                             /* offset 0x1C + X * 0x80*/
    __IO uint32_t REG_LYRx_WIN_MAX;                             /* offset 0x20 + X * 0x80*/
    __IO uint32_t REG_LYRx_KEY_MIN;                             /* offset 0x24 + X * 0x80*/
    __IO uint32_t REG_LYRx_KEY_MAX;                             /* offset 0x28 + X * 0x80*/
    __IO uint32_t REG_LYRx_TRANS_MATRIX_E11;                    /* offset 0x2C + X * 0x80*/
    __IO uint32_t REG_LYRx_TRANS_MATRIX_E12;                    /* offset 0x30 + X * 0x80*/
    __IO uint32_t REG_LYRx_TRANS_MATRIX_E13;                    /* offset 0x34 + X * 0x80*/
    __IO uint32_t REG_LYRx_TRANS_MATRIX_E21;                    /* offset 0x38 + X * 0x80*/
    __IO uint32_t REG_LYRx_TRANS_MATRIX_E22;                    /* offset 0x3C + X * 0x80*/
    __IO uint32_t REG_LYRx_TRANS_MATRIX_E23;                    /* offset 0x40 + X * 0x80*/
    __IO uint32_t REG_LYRx_TRANS_MATRIX_E31;                    /* offset 0x44 + X * 0x80*/
    __IO uint32_t REG_LYRx_TRANS_MATRIX_E32;                    /* offset 0x48 + X * 0x80*/
    __IO uint32_t REG_LYRx_TRANS_MATRIX_E33;                    /* offset 0x4C + X * 0x80*/
    __IO uint32_t REG_RESERVED2[12];                            /* offset (0x50-0x7C) + X * 0x80*/
} PPEV2_Input_Layer_Typedef;

/*============================================================================*
 *                         PPEV2 Declaration
 *============================================================================*/
#define PPEV2_REG_BASE                              0x40005000UL
#define PPEV2_Result_Layer_BASE                     (PPEV2_REG_BASE + 0x60)
#define PPEV2_Input_Layer1_BASE                     (PPEV2_REG_BASE + 1 * 0x80)
#define PPEV2_Input_Layer2_BASE                     (PPEV2_REG_BASE + 2 * 0x80)
#define PPEV2_Input_Layer3_BASE                     (PPEV2_REG_BASE + 3 * 0x80)
#define PPEV2_Input_Layer4_BASE                     (PPEV2_REG_BASE + 4 * 0x80)

#define PPEV2                                           ((PPEV2_Typedef *)PPEV2_REG_BASE)
#define PPEV2_ResultLayer                               ((PPEV2_ResultLayer_Typedef *)PPEV2_Result_Layer_BASE)
#define PPEV2_InputLayer1                               ((PPEV2_Input_Layer_Typedef *)PPEV2_Input_Layer1_BASE)
#define PPEV2_InputLayer2                               ((PPEV2_Input_Layer_Typedef *)PPEV2_Input_Layer2_BASE)
#define PPEV2_InputLayer3                               ((PPEV2_Input_Layer_Typedef *)PPEV2_Input_Layer3_BASE)
#define PPEV2_InputLayer4                               ((PPEV2_Input_Layer_Typedef *)PPEV2_Input_Layer4_BASE)

#define PPEV2_MAX_INPUTLAYER                          0x4
#define PPEV2_LINKLIST_GLB_REG_NUM                    3UL
#define PPEV2_LINKLIST_RESULT_REG_NUM                 4UL
#define PPEV2_LINKLIST_INPUT_REG_NUM                  17UL
#define PPEV2_LLI_MAX                                 (PPEV2_LINKLIST_GLB_REG_NUM + PPEV2_LINKLIST_RESULT_REG_NUM + PPEV2_LINKLIST_INPUT_REG_NUM * PPEV2_MAX_INPUTLAYER)

/** \defgroup 87X2G_PPEV2       PPEV2
  * \brief
  * \{
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** \defgroup PPEV2_Exported_Constants PPEV2 Exported Constants
  * \brief
  * \{
  */

typedef enum
{
    PPEV2_INPUT_1 = 0x1,
    PPEV2_INPUT_2 = 0x2,
    PPEV2_INPUT_3 = 0x3,
    PPEV2_INPUT_4 = 0x4,

} PPEV2_INPUT_LAYER_INDEX;

typedef enum
{
    PPEV2_NON_SECURE_MODE = 0X0,
    PPEV2_SECURE_MODE         = 0x1,
} PPEV2_SECURE;

typedef enum
{
    PPEV2_AWBURST_FIXED     = 0X0,
    PPEV2_AWBURST_INC         = 0X1,
} PPEV2_AWBURST;

typedef enum
{
    PPEV2_HW_HS_DISABLE     = 0X0,
    PPEV2_HW_HS_ENABLE         = 0X1,
} PPEV2_HW_HS;

typedef enum
{
    PPEV2_HW_HS_ACTIVE_HIGH     = 0X0,
    PPEV2_HW_HS_ACTIVE_LOW         = 0X1,
} PPEV2_HW_HS_POL;

typedef enum
{
    PPEV2_COLOR_KEY_DISABLE     = 0X0,
    PPEV2_COLOR_KEY_INSIDE         = 0X2,
    PPEV2_COLOR_KEY_OUTSIDE   = 0X3,
} PPEV2_COLOR_KEY_MODE;

typedef enum
{
    PPEV2_LAYER_SRC_CONST             = 0X0,
    PPEV2_LAYER_SRC_FROM_DMA         = 0X1,
} PPEV2_PIXEL_SOURCE;

typedef enum
{
    PPEV2_DMA_HW_HANDSHAKE,
    PPEV2_DMA_SW_HANDSHAKE,
} PPEV2_DMA_HANDSHAKE;

typedef enum
{
    PPEV2_MSIZE_1,
    PPEV2_MSIZE_2,
    PPEV2_MSIZE_4,
    PPEV2_MSIZE_8,
    PPEV2_MSIZE_16,
    PPEV2_MSIZE_32,
    PPEV2_MSIZE_64,
    PPEV2_MSIZE_128,
    PPEV2_MSIZE_256,
    PPEV2_MSIZE_512,
    PPEV2_MSIZE_1024,
} PPEV2_MSIZE_LOG;

typedef enum
{
    PPEV2_HS_IMDC_Tx,
} PPEV2_HW_HANDSHAKE_INDEX;

typedef enum
{
    PPEV2_MAX_AXLEN_0,
    PPEV2_MAX_AXLEN_1,
    PPEV2_MAX_AXLEN_3,
    PPEV2_MAX_AXLEN_7,
    PPEV2_MAX_AXLEN_15,
    PPEV2_MAX_AXLEN_31,
    PPEV2_MAX_AXLEN_63,
    PPEV2_MAX_AXLEN_127,
} PPEV2_MAX_AXLEN;

typedef enum
{
    PPV2_READ_MATRIX_1X1,
    PPV2_READ_MATRIX_2X2,
} PPEV2_READ_MATRIX_SIZE;


typedef enum
{
    PPEV2_ABGR8888 = 0x0,
    PPEV2_ARGB8888,
    PPEV2_XBGR8888,
    PPEV2_XRGB8888,
    PPEV2_BGRA8888,
    PPEV2_RGBA8888,
    PPEV2_BGRX8888,
    PPEV2_RGBX8888,
    PPEV2_ABGR4444,
    PPEV2_ARGB4444,
    PPEV2_XBGR4444,
    PPEV2_XRGB4444,
    PPEV2_BGRA4444,
    PPEV2_RGBA4444,
    PPEV2_BGRX4444,
    PPEV2_RGBX4444,
    PPEV2_ABGR2222,
    PPEV2_ARGB2222,
    PPEV2_XBGR2222,
    PPEV2_XRGB2222,
    PPEV2_BGRA2222,
    PPEV2_RGBA2222,
    PPEV2_BGRX2222,
    PPEV2_RGBX2222,
    PPEV2_ABGR8565,
    PPEV2_ARGB8565,
    PPEV2_XBGR8565,
    PPEV2_XRGB8565,
    PPEV2_BGRA5658,
    PPEV2_RGBA5658,
    PPEV2_BGRX5658,
    PPEV2_RGBX5658,
    PPEV2_ABGR1555,
    PPEV2_ARGB1555,
    PPEV2_XBGR1555,
    PPEV2_XRGB1555,
    PPEV2_BGRA5551,
    PPEV2_RGBA5551,
    PPEV2_BGRX5551,
    PPEV2_RGBX5551,
    PPEV2_BGR888,
    PPEV2_RGB888,
    PPEV2_BGR565,
    PPEV2_RGB565,
    PPEV2_A8,
    PPEV2_X8,
    PPEV2_ABGR8666 = 0x32,
    PPEV2_ARGB8666,
    PPEV2_XBGR8666,
    PPEV2_XRGB8666,
    PPEV2_BGRA6668,
    PPEV2_RGBA6668,
    PPEV2_BGRX6668,
    PPEV2_RGBX6668,
} PPEV2_PIXEL_FORMAT;

typedef enum
{
    PPEV2_BYPASS_MODE,
    PPEV2_TRANSPARENT_MODE,
    PPEV2_SRC_OVER_MODE,
    PPEV2_CONST_MASK_MODE,
} PPEV2_BLEND_MODE;

typedef enum
{
    PPEV2_SUCCESS = 0x0,
    PPEV2_ERR_NULL_TARGET,
    PPEV2_ERR_NULL_SOURCE,
    PPEV2_ERR_INVALID_MATRIX,
    PPEV2_ERR_INVALID_RANGE,
} PPEV2_err;

typedef struct
{
    FunctionalState         SetValid_AutoClear;
    uint32_t                LLP;
    PPEV2_SECURE            Secure_En;
} PPEV2_Init_Typedef;

typedef struct
{
    uint32_t                            Layer_Address;
    uint32_t                            Canvas_Height;
    uint32_t                            Canvas_Width;
    uint32_t                            Line_Length;
    PPEV2_PIXEL_FORMAT                  Color_Format;
    uint32_t                            BackGround;
    PPEV2_AWBURST                       LayerBus_Inc;
    PPEV2_MAX_AXLEN                     Max_Axlen;
    FunctionalState                     MultiFrame_Reload_En;
    FunctionalState                     MultiFrame_LLP_En;
    PPEV2_HW_HS                         Layer_HW_Handshake_En;
    PPEV2_HW_HANDSHAKE_INDEX            Layer_HW_Handshake_Index;
    PPEV2_HW_HS_POL                     Layer_HW_Handshake_Polarity;
    PPEV2_MSIZE_LOG                     Layer_HW_Handshake_MsizeLog;
} PPEV2_ResultLayer_Init_Typedef;

typedef struct
{
    uint32_t                            Layer_Address;
    uint32_t                            Pic_Height;
    uint32_t                            Pic_Width;
    uint32_t                            Line_Length;
    PPEV2_PIXEL_SOURCE                  Pixel_Source;
    PPEV2_PIXEL_FORMAT                  Pixel_Color_Format;
    PPEV2_READ_MATRIX_SIZE              Read_Matrix_Size;
    uint32_t                            Const_Pixel;
    PPEV2_COLOR_KEY_MODE                Color_Key_Mode;
    uint32_t                            Color_Key_Min;
    uint32_t                            Color_Key_Max;
    FunctionalState                     MultiFrame_Reload_En;
    FunctionalState                     MultiFrame_LLP_En;
    PPEV2_AWBURST                       LayerBus_Inc;
    PPEV2_MAX_AXLEN                     Max_Axlen;
    PPEV2_HW_HS                         Layer_HW_Handshake_En;
    PPEV2_HW_HANDSHAKE_INDEX            Layer_HW_Handshake_Index;
    PPEV2_HW_HS_POL                     Layer_HW_Handshake_Polarity;
    PPEV2_MSIZE_LOG                     Layer_HW_Handshake_MsizeLog;
    uint16_t                            Layer_Window_Xmin;
    uint16_t                            Layer_Window_Xmax;
    uint16_t                            Layer_Window_Ymin;
    uint16_t                            Layer_Window_Ymax;
    uint32_t                            Transfer_Matrix_E11;
    uint32_t                            Transfer_Matrix_E12;
    uint32_t                            Transfer_Matrix_E13;
    uint32_t                            Transfer_Matrix_E21;
    uint32_t                            Transfer_Matrix_E22;
    uint32_t                            Transfer_Matrix_E23;
    uint32_t                            Transfer_Matrix_E31;
    uint32_t                            Transfer_Matrix_E32;
    uint32_t                            Transfer_Matrix_E33;
} PPEV2_InputLayer_Init_Typedef;

typedef enum
{
    PPEV2_ALL_OVER_INT = 0x0,
    PPEV2_FRAME_OVER_INT,
    PPEV2_LOAD_OVER_INT,
    PPEV2_LINE_OVER_INT,
    PPEV2_SUSPEND_IACTIVE_INT,
    PPEV2_SECURE_ERROR_INT,
    PPEV2_BUS_ERROR_INT = 0x7,
    PPEV2_DIV0_ERR_INT,
} PPEV2_INTERRUPT;


typedef enum
{
    PPEV2_DISABLE = 0x0,
    PPEV2_ENABLE     = 0x1,
    PPEV2_SUSPEND_ALL_INA = 0x2,
    PPEV2_SUSPEND = 0x3,
} PPEV2_RUN_STATE;


typedef struct
{
    uint32_t LYR_ENABLE;
    uint32_t LL_CFG;
    uint32_t LLP;
} PPEV2_LLI_GLB;

typedef struct
{
    uint32_t LYR0_ADDR;
    uint32_t CANVAS_SIZE;
    uint32_t LYR0_PIC_CFG;
    uint32_t BACKGROUND;
} PPEV2_LLI_RESULT_LAYER;

typedef struct
{
    uint32_t LYRx_ADDR;
    uint32_t LYRx_PIC_SIZE;
    uint32_t LYRx_PIC_CFG;
    uint32_t LYRx_FIXED_COLOR;
    uint32_t LYRx_WIN_MIN;
    uint32_t LYRx_WIN_MAX;
    uint32_t LYRx_KEY_MIN;
    uint32_t LYRx_KEY_MAX;
    uint32_t LYRx_TRANS_MATRIX_E11;
    uint32_t LYRx_TRANS_MATRIX_E12;
    uint32_t LYRx_TRANS_MATRIX_E13;
    uint32_t LYRx_TRANS_MATRIX_E21;
    uint32_t LYRx_TRANS_MATRIX_E22;
    uint32_t LYRx_TRANS_MATRIX_E23;
    uint32_t LYRx_TRANS_MATRIX_E31;
    uint32_t LYRx_TRANS_MATRIX_E32;
    uint32_t LYRx_TRANS_MATRIX_E33;
} PPEV2_LLI_INPUT_LAYER;

typedef struct
{
    float m[3][3];    /*! The 3x3 matrix itself, in [row][column] order. */
} ppe_matrix_t;

typedef struct
{
    int x;
    int y;
    uint32_t w;
    uint32_t h;
} ppe_rect_t;

typedef struct
{
    int x;
    int y;
}
ppe_point_t;

/* Four 2D Point that form a polygon */
typedef ppe_point_t ppe_point4_t[4];

typedef struct
{
    uint32_t address;
    uint16_t width;
    uint16_t height;
    uint32_t const_color;
    uint16_t win_x_min;
    uint16_t win_x_max;
    uint16_t win_y_min;
    uint16_t win_y_max;
    uint32_t color_key_max;
    uint32_t color_key_min;
    PPEV2_COLOR_KEY_MODE color_key_enable;
    PPEV2_PIXEL_FORMAT format;
    uint8_t opacity;
    bool high_quality;
} ppe_buffer_t;

/** End of PPEV2_Exported_Constants
  * \}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** \defgroup PPEV2_Exported_Functions PPEV2 Exported Functions
  * \brief
  * \{
  */

void PPEV2_Init(PPEV2_Init_Typedef *PPEV2_Init_Struct);

void PPEV2_ResultLayer_Init(PPEV2_ResultLayer_Init_Typedef *PPEV2_ResultLyaer_Init_Struct);

void PPEV2_InputLayer_Init(PPEV2_INPUT_LAYER_INDEX intput_layer_index,
                           PPEV2_InputLayer_Init_Typedef *PPEV2_InputLayer_Init_Struct);

void PPEV2_StructInit(PPEV2_Init_Typedef *PPEV2_Init_Struct);

void PPEV2_ResultLayer_StructInit(PPEV2_ResultLayer_Init_Typedef *PPEV2_ResultLyaer_Init_Struct);

void PPEV2_InputLayer_StructInit(PPEV2_INPUT_LAYER_INDEX intput_layer_index,
                                 PPEV2_InputLayer_Init_Typedef *PPEV2_InputLayer_Init_Struct);

void PPEV2_Cmd(FunctionalState NewState);

void PPEV2_InputLayer_enable(PPEV2_INPUT_LAYER_INDEX intput_layer_index, FunctionalState NewState);

FunctionalState PPEV2_Get_Interrupt_Status(PPEV2_INTERRUPT PPEV2_int);

FunctionalState PPEV2_Get_Raw_Interrupt_Status(PPEV2_INTERRUPT PPEV2_int);

void PPEV2_Clear_Interrupt(PPEV2_INTERRUPT PPEV2_int);

void PPEV2_Mask_Interrupt(PPEV2_INTERRUPT PPEV2_int, FunctionalState NewState);

void PPEV2_Mask_All_Interrupt(FunctionalState NewState);

PPEV2_err PPEV2_Blit(ppe_buffer_t *target, ppe_buffer_t *image, ppe_matrix_t *matrix,
                     PPEV2_BLEND_MODE mode);
PPEV2_err PPEV2_Blit_Inverse(ppe_buffer_t *dst, ppe_buffer_t *src, ppe_matrix_t *inverse,
                             ppe_rect_t *rect, PPEV2_BLEND_MODE mode);
PPEV2_err PPEV2_Blend_Handshake(ppe_buffer_t *dst, ppe_buffer_t *src, ppe_rect_t *rect);

void PPEV2_CLK_ENABLE(FunctionalState NewState);
void PPEV2_Finish(void);
uint8_t PPEV2_Get_Pixel_Size(PPEV2_PIXEL_FORMAT format);
void ppe_get_identity(ppe_matrix_t *matrix);
void ppe_translate(float x, float y, ppe_matrix_t *matrix);
void ppe_scale(float scale_x, float scale_y, ppe_matrix_t *matrix);
void ppe_rotate(float degrees, ppe_matrix_t *matrix);
void ppe_perspective(float px, float py, ppe_matrix_t *matrix);
int  ppe_get_transform_matrix(ppe_point4_t src, ppe_point4_t dst, ppe_matrix_t *mat);
void ppe_matrix_inverse(ppe_matrix_t *matrix);
void ppe_mat_multiply(ppe_matrix_t *matrix, ppe_matrix_t *mult);
bool ppe_get_area(ppe_rect_t *result_rect, ppe_rect_t *source_rect, ppe_matrix_t *matrix,
                  ppe_buffer_t *buffer);
PPEV2_err PPEV2_Blend_Multi(ppe_buffer_t *dst, ppe_buffer_t *src_1,
                            ppe_buffer_t *src_2, ppe_buffer_t *src_3, PPEV2_BLEND_MODE mode);
PPEV2_err PPEV2_Mask(ppe_buffer_t *dst, uint32_t color, ppe_rect_t *rect);
/** End of PPEV2_Exported_Functions
  * \}
  */

/** End of PPEV2
  * \}
  */

#ifdef __cplusplus
}
#endif

#endif /* RTL_PPEV2_H */

/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/

