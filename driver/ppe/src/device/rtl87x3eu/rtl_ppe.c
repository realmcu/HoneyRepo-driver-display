/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* \file     rtl_PPE.c
* \brief    This file provides all the the PPE 2.0 firmware functions.
* \details
* \author   feihu wang
* \date     2023-10-17
* \version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "rtl_ppe.h"
#include "string.h"
#include "math.h"
#include "ppe_simulation.h"
#include "trace.h"

/*============================================================================*
 *                          Private Macros
 *============================================================================*/
#define USE_PPE_MAT     1
#define ABS(x)               (((x) < 0)    ? -(x) :  (x))
#define EPS                  1.1920929e-7f

/*============================================================================*
 *                          Private Variables
 *============================================================================*/
typedef struct
{
    float p[3];
} ppe_pox_t;

/*============================================================================*
 *                          Private Functions
 *============================================================================*/
static bool inv_matrix2complement(ppe_matrix_t *matrix, uint32_t *comp);
static bool ppe_get_block_size(ppe_matrix_t *matrix, uint8_t pixel_size, uint16_t* block_width, uint16_t* block_height);
static uint32_t* INDEX_CLUT = NULL;

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void ppe_get_identity(ppe_matrix_t *matrix);
bool check_inverse(ppe_matrix_t *matrix);
#if USE_PPE_MAT
void ppe_translate(float x, float y, ppe_matrix_t *matrix);
void ppe_scale(float scale_x, float scale_y, ppe_matrix_t *matrix);
void ppe_rotate(float degrees, ppe_matrix_t *matrix);
void ppe_perspective(float px, float py, ppe_matrix_t *matrix);
int  ppe_get_transform_matrix(ppe_point4_t src, ppe_point4_t dst, ppe_matrix_t *mat);
void ppe_matrix_inverse(ppe_matrix_t *matrix);
void ppe_mat_multiply(ppe_matrix_t *matrix, ppe_matrix_t *mult);
#endif

void PPE_CLK_ENABLE_IN_DLPS(FunctionalState NewState)
{
    if (NewState != ENABLE)
    {
        /*clock disable*/
        uint32_t reg_value = *(uint32_t *)(0x40000234UL);
        reg_value &= (~BIT26);
        *(uint32_t *)(0x40000234UL) = reg_value;
    }
    else
    {
        /*clock enable*/
        uint32_t reg_value = *(uint32_t *)(0x40000234UL);
        reg_value |= BIT26;
        *(uint32_t *)(0x40000234UL) = reg_value;
    }
}
 
void PPE_CLK_ENABLE(FunctionalState NewState)
{
 
    if (NewState != ENABLE)
    {
        /*clock disable*/
        uint32_t reg_value = *(uint32_t *)(0x40000234UL);
        reg_value &= (~BIT27);
        *(uint32_t *)(0x40000234UL) = reg_value;
 
        /*function disable*/
        reg_value = *(uint32_t *)(0x40000210UL);
        reg_value &= ~(BIT24);
        *(uint32_t *)(0x40000210UL) = reg_value;
    }
    else
    {
        /*clock enable*/
        uint32_t reg_value = *(uint32_t *)(0x40000234UL);
        reg_value |= (BIT27 | BIT26);
        *(uint32_t *)(0x40000234UL) = reg_value;
 
        /*function enable*/
        reg_value = *(uint32_t *)(0x40000210UL);
        reg_value |= (BIT24);
        *(uint32_t *)(0x40000210UL) = reg_value;
    }
}

/*enable PPE FSM*/
void PPE_Cmd(FunctionalState NewState)
{
    PPE_REG_GLB_STATUS_TypeDef ppe_reg_glb_status_0x00 = {.d32 = PPE->REG_GLB_STATUS};

    if (NewState == DISABLE)
    {
        ppe_reg_glb_status_0x00.b.run_state = 0x0;
    }
    else
    {
        ppe_reg_glb_status_0x00.b.run_state = 0x1;
    }

    PPE->REG_GLB_STATUS = ppe_reg_glb_status_0x00.d32;
}

void PPE_ResultLayer_Init(PPE_ResultLayer_Init_Typedef *PPE_ResultLyaer_Init_Struct)
{
    PPE_REG_LYR0_ADDR_TypeDef ppe_reg_lyr0_addr_0x98 = {.d32 = PPE_ResultLayer->REG_LYR0_ADDR};
    ppe_reg_lyr0_addr_0x98.b.addr                = PPE_ResultLyaer_Init_Struct->Layer_Address;
    PPE_ResultLayer->REG_LYR0_ADDR               = ppe_reg_lyr0_addr_0x98.d32;


    PPE_REG_LYR0_PIC_CFG_TypeDef  ppe_reg_lyr0_pic_cfg_0x80 = {.d32 = PPE_ResultLayer->REG_LYR0_PIC_CFG};
    ppe_reg_lyr0_pic_cfg_0x80.b.format                  = PPE_ResultLyaer_Init_Struct->Color_Format;
    PPE_ResultLayer->REG_LYR0_PIC_CFG                   = ppe_reg_lyr0_pic_cfg_0x80.d32;

    PPE_REG_LYR0_BUS_CFG_TypeDef ppe_reg_lyr0_bus_cfg_0x90 = {.d32 = PPE_ResultLayer->REG_LYR0_BUS_CFG};
    ppe_reg_lyr0_bus_cfg_0x90.b.incr                   = PPE_ResultLyaer_Init_Struct->LayerBus_Inc;
    ppe_reg_lyr0_bus_cfg_0x90.b.axsize                 = 0x2;
    ppe_reg_lyr0_bus_cfg_0x90.b.max_awlen_log          = 0x7;
    PPE_ResultLayer->REG_LYR0_BUS_CFG                  = ppe_reg_lyr0_bus_cfg_0x90.d32;

    PPE_REG_RELOAD_CFG_TypeDef ppe_reg_ld_cfg_0x08  = {.d32 = PPE->REG_RELOAD_CFG};
    ppe_reg_ld_cfg_0x08.b.input_lyr_1_reload_en  =  0;
    ppe_reg_ld_cfg_0x08.b.input_lyr_2_reload_en  =  0;
    PPE->REG_RELOAD_CFG                         =    ppe_reg_ld_cfg_0x08.d32;

    PPE_REG_LL_CFG_TypeDef ppe_reg_ll_cfg_0x0c = {.d32 = PPE->REG_LL_CFG};
    ppe_reg_ll_cfg_0x0c.b.input_lyr_1_ll_en = 0;
    ppe_reg_ll_cfg_0x0c.b.input_lyr_2_ll_en = 0;
    PPE->REG_LL_CFG                        = ppe_reg_ll_cfg_0x0c.d32;
    
    PPE_REG_LYR0_WIN_MIN_TypeDef ppe_reg_lyr0_win_min_0x88 = {.d32 = PPE_ResultLayer->REG_LYR0_WIN_MIN};
    ppe_reg_lyr0_win_min_0x88.b.win_x_min = PPE_ResultLyaer_Init_Struct->Layer_Window_Xmin;
    ppe_reg_lyr0_win_min_0x88.b.win_y_min = PPE_ResultLyaer_Init_Struct->Layer_Window_Ymin;
    PPE_ResultLayer->REG_LYR0_WIN_MIN = ppe_reg_lyr0_win_min_0x88.d32;
    
    PPE_REG_LYR0_WIN_MAX_TypeDef ppe_reg_lyr0_win_max_0x8c = {.d32 = PPE_ResultLayer->REG_LYR0_WIN_MAX};
    ppe_reg_lyr0_win_max_0x8c.b.win_x_max = PPE_ResultLyaer_Init_Struct->Layer_Window_Xmax;
    ppe_reg_lyr0_win_max_0x8c.b.win_y_max = PPE_ResultLyaer_Init_Struct->Layer_Window_Ymax;
    PPE_ResultLayer->REG_LYR0_WIN_MAX = ppe_reg_lyr0_win_max_0x8c.d32;
    
    PPE_REG_LYR0_LINE_LEN_TypeDef ppe_reg_lyr0_line_len_0xa0 = {.d32 = PPE_ResultLayer->REG_LYR0_LINE_LEN};
    ppe_reg_lyr0_line_len_0xa0.b.line_len = PPE_ResultLyaer_Init_Struct->Line_Length;
    PPE_ResultLayer->REG_LYR0_LINE_LEN = ppe_reg_lyr0_line_len_0xa0.d32;
    
    PPE_REG_LYR0_BLK_SIZE_TypeDef ppe_reg_lyr0_line_len_0xa4 = {.d32 = PPE_ResultLayer->REG_LYR0_BLK_SIZE};
    ppe_reg_lyr0_line_len_0xa4.b.width = PPE_ResultLyaer_Init_Struct->Block_Width;
    ppe_reg_lyr0_line_len_0xa4.b.height = PPE_ResultLyaer_Init_Struct->Block_Height;
    PPE_ResultLayer->REG_LYR0_BLK_SIZE = ppe_reg_lyr0_line_len_0xa4.d32;
}

void PPE_InputLayer_Init(PPE_INPUT_LAYER_INDEX intput_layer_index, \
                           PPE_InputLayer_Init_Typedef *PPE_InputLayer_Init_Struct)
{
    PPE_Input_Layer_Typedef *input_layer;
    switch (intput_layer_index)
    {
        case PPE_INPUT_1:
            {
                input_layer = PPE_InputLayer1;
                break;
            }
        case PPE_INPUT_2:
            {
                input_layer = PPE_InputLayer2;
                break;
            }
        default:
            assert_param(NULL != NULL);
            break;
    }

    PPE_REG_LYRx_PIC_CFG_TypeDef ppe_reg_lyrx_pic_cfg_t     = {.d32 = input_layer->REG_LYRx_PIC_CFG};
    ppe_reg_lyrx_pic_cfg_t.b.format      =       PPE_InputLayer_Init_Struct->Pixel_Color_Format;
    ppe_reg_lyrx_pic_cfg_t.b.pic_src                    = PPE_InputLayer_Init_Struct->Pixel_Source;
    ppe_reg_lyrx_pic_cfg_t.b.interpolation = PPE_InputLayer_Init_Struct->Source_Interpolation;
    if(intput_layer_index == PPE_INPUT_2)
    {
        ppe_reg_lyrx_pic_cfg_t.b.abf  = PPE_InputLayer_Init_Struct->Blend_Method;
    }
    else
    {
        ppe_reg_lyrx_pic_cfg_t.b.abf  = PPE_BLEND_SRC;
    }
    ppe_reg_lyrx_pic_cfg_t.b.key_mode  = PPE_InputLayer_Init_Struct->Color_Key_Mode;
    ppe_reg_lyrx_pic_cfg_t.b.key_en = PPE_InputLayer_Init_Struct->Color_Key_Enable.key_enable;
    input_layer->REG_LYRx_PIC_CFG                         = ppe_reg_lyrx_pic_cfg_t.d32;
    
    PPE_REG_LYRx_CONST_PIX_TypeDef ppe_reg_lyrx_const_pix_t = {.d32 = input_layer->REG_LYRx_CONST_PIX};
    ppe_reg_lyrx_const_pix_t.b.const_pixel =  PPE_InputLayer_Init_Struct->Const_Pixel;
    input_layer->REG_LYRx_CONST_PIX                         = ppe_reg_lyrx_const_pix_t.d32;
    
    PPE_REG_LYRx_WIN_MIN_TypeDef ppe_reg_lyrx_win_min_t = {.d32 = input_layer->REG_LYRx_WIN_MIN};
    ppe_reg_lyrx_win_min_t.b.win_x_min              = PPE_InputLayer_Init_Struct->Layer_Window_Xmin;
    ppe_reg_lyrx_win_min_t.b.win_y_min              = PPE_InputLayer_Init_Struct->Layer_Window_Ymin;
    input_layer->REG_LYRx_WIN_MIN                     = ppe_reg_lyrx_win_min_t.d32;

    PPE_REG_LYRx_WIN_MAX_TypeDef ppe_reg_lyrx_win_max_t = {.d32 = input_layer->REG_LYRx_WIN_MAX};
    ppe_reg_lyrx_win_max_t.b.win_x_max              = PPE_InputLayer_Init_Struct->Layer_Window_Xmax;
    ppe_reg_lyrx_win_max_t.b.win_y_max              = PPE_InputLayer_Init_Struct->Layer_Window_Ymax;
    input_layer->REG_LYRx_WIN_MAX                     = ppe_reg_lyrx_win_max_t.d32;
    
    PPE_REG_LYRx_BUS_CFG_TypeDef ppe_reg_lyrx_bus_cfg_t = {.d32 = input_layer->REG_LYRx_BUS_CFG};
    ppe_reg_lyrx_bus_cfg_t.b.incr                   = PPE_InputLayer_Init_Struct->LayerBus_Inc;
    //PPE_InputLayer_Init_Struct->Cache_Enable = false;
    if(PPE_InputLayer_Init_Struct->Cache_Enable)
    {
        ppe_reg_lyrx_bus_cfg_t.b.arcache = 0x06;
    }
    else
    {
        ppe_reg_lyrx_bus_cfg_t.b.arcache = 0x00;
    }
    input_layer->REG_LYRx_BUS_CFG                     = ppe_reg_lyrx_bus_cfg_t.d32;
    
    PPE_REG_LYRx_ADDR_TypeDef ppe_reg_lyrx_addr_t = {.d32 = input_layer->REG_LYRx_ADDR};
    ppe_reg_lyrx_addr_t.b.addr                = PPE_InputLayer_Init_Struct->Layer_Address;
    input_layer->REG_LYRx_ADDR                  = ppe_reg_lyrx_addr_t.d32;
    
    PPE_REG_LYRx_LINE_LEN_TypeDef ppe_reg_lyrx_line_len_t = {.d32 = input_layer->REG_LYRx_LINE_LEN};
    ppe_reg_lyrx_line_len_t.b.line_len = PPE_InputLayer_Init_Struct->Line_Length;
    input_layer->REG_LYRx_LINE_LEN = ppe_reg_lyrx_line_len_t.d32;

    PPE_REG_LYRx_PIC_SIZE_TypeDef ppe_reg_lyrx_pic_size_t = {.d32 = input_layer->REG_LYRx_PIC_SIZE};
    ppe_reg_lyrx_pic_size_t.b.width               = PPE_InputLayer_Init_Struct->Pic_Width;
    ppe_reg_lyrx_pic_size_t.b.height              = PPE_InputLayer_Init_Struct->Pic_Height;
    input_layer->REG_LYRx_PIC_SIZE                      = ppe_reg_lyrx_pic_size_t.d32;

    PPE_REG_LYRx_KEY_MIN_TypeDef ppe_reg_lyrx_key_min_t = {.d32 = input_layer->REG_LYRx_KEY_MIN};
    ppe_reg_lyrx_key_min_t.b.r          =  PPE_InputLayer_Init_Struct->Color_Key_MIN_R;
    ppe_reg_lyrx_key_min_t.b.g          =  PPE_InputLayer_Init_Struct->Color_Key_MIN_G;
    ppe_reg_lyrx_key_min_t.b.b          =  PPE_InputLayer_Init_Struct->Color_Key_MIN_B;
    input_layer->REG_LYRx_KEY_MIN                     = ppe_reg_lyrx_key_min_t.d32;

    PPE_REG_LYRx_KEY_MAX_TypeDef ppe_reg_lyrx_key_max_t = {.d32 = input_layer->REG_LYRx_KEY_MAX};
    ppe_reg_lyrx_key_max_t.b.r          = PPE_InputLayer_Init_Struct->Color_Key_MAX_R;
    ppe_reg_lyrx_key_max_t.b.g          = PPE_InputLayer_Init_Struct->Color_Key_MAX_G;
    ppe_reg_lyrx_key_max_t.b.b          = PPE_InputLayer_Init_Struct->Color_Key_MAX_B;
    input_layer->REG_LYRx_KEY_MAX                     = ppe_reg_lyrx_key_max_t.d32;
    
    PPE_REG_LYRx_KEY_REPLACE_TypeDef ppe_reg_lyrx_key_replace_t = {.d32 = input_layer->REG_LYRx_KEY_REPLACE};
    ppe_reg_lyrx_key_replace_t.b.a = PPE_InputLayer_Init_Struct->Color_Key_Replace_A;
    ppe_reg_lyrx_key_replace_t.b.b = PPE_InputLayer_Init_Struct->Color_Key_Replace_B;
    ppe_reg_lyrx_key_replace_t.b.g = PPE_InputLayer_Init_Struct->Color_Key_Replace_G;
    ppe_reg_lyrx_key_replace_t.b.r = PPE_InputLayer_Init_Struct->Color_Key_Replace_R;
    input_layer->REG_LYRx_KEY_REPLACE = ppe_reg_lyrx_key_replace_t.d32;

    input_layer->REG_LYRx_TRANS_MATRIX_E11            =
        PPE_InputLayer_Init_Struct->Transfer_Matrix_E11;
    input_layer->REG_LYRx_TRANS_MATRIX_E12            =
        PPE_InputLayer_Init_Struct->Transfer_Matrix_E12;
    input_layer->REG_LYRx_TRANS_MATRIX_E13            =
        PPE_InputLayer_Init_Struct->Transfer_Matrix_E13;
    input_layer->REG_LYRx_TRANS_MATRIX_E21            =
        PPE_InputLayer_Init_Struct->Transfer_Matrix_E21;
    input_layer->REG_LYRx_TRANS_MATRIX_E22            =
        PPE_InputLayer_Init_Struct->Transfer_Matrix_E22;
    input_layer->REG_LYRx_TRANS_MATRIX_E23            =
        PPE_InputLayer_Init_Struct->Transfer_Matrix_E23;
    input_layer->REG_LYRx_TRANS_MATRIX_E31            =
        PPE_InputLayer_Init_Struct->Transfer_Matrix_E31;
    input_layer->REG_LYRx_TRANS_MATRIX_E32            =
        PPE_InputLayer_Init_Struct->Transfer_Matrix_E32;
    input_layer->REG_LYRx_TRANS_MATRIX_E33            =
        PPE_InputLayer_Init_Struct->Transfer_Matrix_E33;
}


void PPE_ResultLayer_StructInit(PPE_ResultLayer_Init_Typedef *PPE_ResultLyaer_Init_Struct)
{
    PPE_ResultLyaer_Init_Struct->AXSize                         = 0x02;
    PPE_ResultLyaer_Init_Struct->Color_Format             = PPE_ABGR8888;
    PPE_ResultLyaer_Init_Struct->Block_Width                 = 32;
    PPE_ResultLyaer_Init_Struct->Block_Height                    = 32;
    PPE_ResultLyaer_Init_Struct->MAX_AXLEN_LOG = 0x07;
    PPE_ResultLyaer_Init_Struct->LayerBus_Inc                     = PPE_AWBURST_INC;
}

void PPE_InputLayer_StructInit(PPE_INPUT_LAYER_INDEX intput_layer_index, \
                                 PPE_InputLayer_Init_Typedef *PPE_InputLayer_Init_Struct)
{
}

void PPE_InputLayer_enable(PPE_INPUT_LAYER_INDEX intput_layer_index, FunctionalState NewState)
{
    PPE_REG_LYR_ENABLE_TypeDef     ppe_reg_lyr_enable_0x04     = {.d32 = PPE->REG_LYR_ENABLE};
    uint32_t input_index = (uint32_t)intput_layer_index;

    if (input_index == PPE_INPUT_1)
    {
        if (NewState != DISABLE)
        {
            ppe_reg_lyr_enable_0x04.b.input_lyr_1_en = 1;
        }
        else
        {
            ppe_reg_lyr_enable_0x04.b.input_lyr_1_en = 0;
        }
    }
    
    else if (input_index == PPE_INPUT_2)
    {
        if (NewState != DISABLE)
        {
            ppe_reg_lyr_enable_0x04.b.input_lyr_2_en = 1;
        }
        else
        {
            ppe_reg_lyr_enable_0x04.b.input_lyr_2_en = 0;
        }
    }
    else
    {
        assert_param(intput_layer_index == PPE_INPUT_1 || intput_layer_index == PPE_INPUT_2);
    }

    PPE->REG_LYR_ENABLE =  ppe_reg_lyr_enable_0x04.d32;
}

FunctionalState PPE_Get_Interrupt_Status(PPE_INTERRUPT PPE_INT)
{
    if (PPE->REG_INTR_STATUS & BIT(PPE_INT))
    {
        return ENABLE;
    }
    else
    {
        return DISABLE;
    }
}

FunctionalState PPE_Get_Raw_Interrupt_Status(PPE_INTERRUPT PPE_INT)
{
    if (PPE->REG_INTR_RAW & BIT(PPE_INT))
    {
        return ENABLE;
    }
    else
    {
        return DISABLE;
    }
}
void PPE_Clear_Interrupt(PPE_INTERRUPT PPE_INT)
{
    PPE->REG_INTR_CLR |= BIT(PPE_INT);
}

void PPE_Mask_Interrupt(PPE_INTERRUPT PPE_INT, FunctionalState NewState)
{
    if (NewState != ENABLE)
    {
        PPE->REG_INTR_MASK |= BIT(PPE_INT);
    }
    else
    {
        PPE->REG_INTR_MASK &= ~BIT(PPE_INT);
    }
}

void PPE_Mask_All_Interrupt(FunctionalState NewState)
{
    if (NewState != ENABLE)
    {
        PPE->REG_INTR_MASK |= (BIT(PPE_ALL_OVER_INT) \
                                 | BIT(PPE_FRAME_OVER_INT) \
                                 | BIT(PPE_LOAD_OVER_INT) \
                                 | BIT(PPE_LINE_OVER_INT) \
                                 | BIT(PPE_SUSPEND_IACTIVE_INT) \
                                 | BIT(PPE_SECURE_ERROR_INT) \
                                 | BIT(PPE_BUS_ERROR_INT) \
                                 | BIT(PPE_DIV0_ERR_INT) \
                                );
    }
    else
    {
        PPE->REG_INTR_MASK &= ~(BIT(PPE_ALL_OVER_INT) \
                                  | BIT(PPE_FRAME_OVER_INT) \
                                  | BIT(PPE_LOAD_OVER_INT) \
                                  | BIT(PPE_LINE_OVER_INT) \
                                  | BIT(PPE_SUSPEND_IACTIVE_INT) \
                                  | BIT(PPE_SECURE_ERROR_INT) \
                                  | BIT(PPE_BUS_ERROR_INT) \
                                  | BIT(PPE_DIV0_ERR_INT) \
                                 );
    }
}

uint8_t PPE_Get_Pixel_Size(PPE_PIXEL_FORMAT format)
{
    if (format <= PPE_RGBX8888)
    {
        return 32;
    }
    else if (format <= PPE_RGBX4444)
    {
        return 16;
    }
    else if (format <= PPE_RGBX2222)
    {
        return 8;
    }
    else if (format <= PPE_RGBX5658)
    {
        return 24;
    }
    else if (format <= PPE_RGBX5551)
    {
        return 16;
    }
    else if (format <= PPE_RGB888)
    {
        return 24;
    }
    else if (format <= PPE_RGB565)
    {
        return 16;
    }
    else if (format <= PPE_X8 || format == PPE_I8)
    {
        return 8;
    }
    else if (format <= PPE_X4 || format == PPE_I4)
    {
        return 4;
    }
    else if (format <= PPE_X2 || format == PPE_I2)
    {
        return 2;
    }
    else if (format <= PPE_X1 || format == PPE_I1)
    {
        return 1;
    }
    else if (format <= PPE_RGBX6668)
    {
        return 32;
    }
    else if (format <= PPE_RGB565_S)
    {
        return 16;
    }
    else
    {
        return 0;
    }
}

void PPE_Finish(void)
{
    while (((PPE_REG_GLB_STATUS_TypeDef)PPE->REG_GLB_STATUS).b.run_state);
}

PPE_ERR PPE_buffer_init(ppe_buffer_t *buffer)
{
    if(buffer == NULL)
    {
        return PPE_ERR_INVALID_PARAMETER;
    }
    else
    {
        memset(buffer, 0, sizeof(ppe_buffer_t));
    }
    return PPE_SUCCESS;
}

PPE_ERR PPE_set_color_key(ppe_buffer_t *image, PPE_color_key_config *config)
{
    if(image == NULL || config == NULL)
    {
        return PPE_ERR_INVALID_PARAMETER;
    }
    memcpy(&image->color_key_config, config, sizeof(PPE_color_key_config));
    return PPE_SUCCESS;
}


#include "trace.h"
PPE_ERR PPE_Blit_Inverse(ppe_buffer_t *dst, ppe_buffer_t *src, uint8_t *output, ppe_matrix_t *inverse,
                             ppe_rect_t *rect, PPE_BLEND_METHOD method)
{
    if (dst->address == NULL)
    {
        return PPE_ERR_NULL_TARGET;
    }
    if (src->address == NULL)
    {
        return PPE_ERR_NULL_SOURCE;
    }
    if ((src->win_x_max <= src->win_x_min) || (src->win_y_max <= src->win_y_min))
    {
        return PPE_ERR_INVALID_RANGE;
    }
    if (rect != NULL)
    {
        if (rect->h == 0 || rect->w == 0)
        {
            return PPE_SUCCESS;
        }
    }
    if (inverse == NULL)
    {
        return PPE_ERR_INVALID_MATRIX;
    }
    uint32_t comp[9];
    if (!check_inverse(inverse))
    {
        return PPE_ERR_INVALID_MATRIX;
    }
    else
    {
        if (!inv_matrix2complement(inverse, comp))
        {
            return PPE_SUCCESS;
        }
    }

    if (src->opacity == 0)
    {
        return PPE_SUCCESS;
    }

    uint32_t color = 0;
    if (src->format >= PPE_A8 && src->format <= PPE_X1)
    {
        color = (src->const_color & 0xFFFFFF) + ((((src->const_color >> 24) & 0xFF) * src->opacity / 0xFF) << 24);
    }
    else
    {
        color = ((src->opacity & 0xFF) * ((src->const_color & 0xFF000000) >> 24) / 0xFF << 24) + \
                    (src->const_color & 0xFFFFFF);
    }
    PPE_CLK_ENABLE(ENABLE);
    PPE_REG_GLB_STATUS_TypeDef     ppe_reg_blg_status_0x00;
    PPE_REG_LYR_ENABLE_TypeDef     ppe_reg_lyr_enable_0x04;
    
    /*PPE Global initialization*/

    /*input layer 2 initilization*/
    PPE_InputLayer_enable(PPE_INPUT_2, ENABLE);           // logic enable of layer
    PPE_InputLayer_Init_Typedef PPE_input_layer2_init = {.Color_Key_Enable.key_enable = 0};
    PPE_InputLayer_StructInit(PPE_INPUT_2, &PPE_input_layer2_init);
    PPE_input_layer2_init.Layer_Address                   = (uint32_t)src->address;
    PPE_input_layer2_init.Pic_Height                      = (uint32_t)src->height;
    PPE_input_layer2_init.Pic_Width                       = (uint32_t)src->width;
    PPE_input_layer2_init.Line_Length                     = PPE_input_layer2_init.Pic_Width *
                                                              PPE_Get_Pixel_Size(src->format);
    PPE_input_layer2_init.Pixel_Source                = PPE_LAYER_SRC_FROM_DMA;
    PPE_input_layer2_init.Const_Pixel                 = color;
    PPE_input_layer2_init.Pixel_Color_Format              = src->format;
    if(method == PPE_BLEND_BYPASS)
    {
        PPE_input_layer2_init.Blend_Method = PPE_BLEND_SRC;
    }
    else
    {
        PPE_input_layer2_init.Blend_Method = method;
    }
    if (src->high_quality)
    {
        PPE_input_layer2_init.Source_Interpolation                = PPV2_SRC_BILINEAR;
    }
    else
    {
        PPE_input_layer2_init.Source_Interpolation                = PPV2_SRC_NEAREST_NEIGHBOUR;
    }
    PPE_input_layer2_init.Color_Key_Enable.key_enable     = src->color_key_config.key_enable.key_enable;
    if (PPE_input_layer2_init.Color_Key_Enable.key_enable)
    {
        PPE_input_layer2_init.Color_Key_Mode                  = src->color_key_config.key_mode;
        PPE_input_layer2_init.Color_Key_MAX_B = src->color_key_config.key_range.B_max;
        PPE_input_layer2_init.Color_Key_MAX_G = src->color_key_config.key_range.G_max;
        PPE_input_layer2_init.Color_Key_MAX_R = src->color_key_config.key_range.R_max;
        PPE_input_layer2_init.Color_Key_MIN_B = src->color_key_config.key_range.B_min;
        PPE_input_layer2_init.Color_Key_MIN_G = src->color_key_config.key_range.G_min;
        PPE_input_layer2_init.Color_Key_MIN_R = src->color_key_config.key_range.R_min;
        PPE_input_layer2_init.Color_Key_Replace_A = src->color_key_config.key_replace.channel_replace.a_replace;
        PPE_input_layer2_init.Color_Key_Replace_B = src->color_key_config.key_replace.channel_replace.b_replace;
        PPE_input_layer2_init.Color_Key_Replace_G = src->color_key_config.key_replace.channel_replace.g_replace;
        PPE_input_layer2_init.Color_Key_Replace_R = src->color_key_config.key_replace.channel_replace.r_replace;
    }
    PPE_input_layer2_init.LayerBus_Inc                    = PPE_AWBURST_INC;
    PPE_input_layer2_init.Layer_Window_Xmin               = src->win_x_min;
    PPE_input_layer2_init.Layer_Window_Xmax               = src->win_x_max;
    PPE_input_layer2_init.Layer_Window_Ymin               = src->win_y_min;
    PPE_input_layer2_init.Layer_Window_Ymax               = src->win_y_max;

    uint32_t ct = 0;
    PPE_input_layer2_init.Transfer_Matrix_E11             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E12             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E13             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E21             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E22             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E23             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E31             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E32             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E33             = comp[ct++];
    PPE_input_layer2_init.Cache_Enable = false;
    PPE_InputLayer_Init(PPE_INPUT_2, &PPE_input_layer2_init);
    PPE_InputLayer_Init_Typedef PPE_input_layer1_init = {.Color_Key_Enable.key_enable = 0};
    if (PPE_BLEND_BYPASS != method)
    {
        /*input layer 1 initilization*/
        PPE_InputLayer_enable(PPE_INPUT_1, ENABLE);           // logic enable of layer
        PPE_InputLayer_StructInit(PPE_INPUT_1, &PPE_input_layer1_init);
        PPE_input_layer1_init.Layer_Address                   = dst->address;
        PPE_input_layer1_init.Pic_Width                      = (uint32_t)dst->width;
        PPE_input_layer1_init.Pic_Height                       = (uint32_t)dst->height;
        PPE_input_layer1_init.Line_Length                     = dst->stride * PPE_Get_Pixel_Size(dst->format);
        PPE_input_layer1_init.Pixel_Source                    = PPE_LAYER_SRC_FROM_DMA;
        PPE_input_layer1_init.Pixel_Color_Format              = dst->format;
        PPE_input_layer1_init.Const_Pixel                     = 0xFFFFFFFF;
        PPE_input_layer1_init.LayerBus_Inc                    = PPE_AWBURST_INC;
        if(rect == NULL)
        {
            PPE_input_layer1_init.Layer_Window_Xmin               = 0;
            PPE_input_layer1_init.Layer_Window_Xmax               = dst->width - 1;
            PPE_input_layer1_init.Layer_Window_Ymin               = 0;
            PPE_input_layer1_init.Layer_Window_Ymax               = dst->height - 1;
        }
        else
        {
            PPE_input_layer1_init.Layer_Window_Xmin               = rect->x;
            PPE_input_layer1_init.Layer_Window_Xmax               = rect->x + rect->w - 1;
            PPE_input_layer1_init.Layer_Window_Ymin               = rect->y;
            PPE_input_layer1_init.Layer_Window_Ymax               = rect->y + rect->h - 1;
        }
        PPE_input_layer1_init.Cache_Enable                    = false;
        PPE_input_layer1_init.Blend_Method                    = PPE_BLEND_SRC;
        PPE_input_layer1_init.Color_Key_Enable.key_enable     = dst->color_key_config.key_enable.key_enable;
        if (PPE_input_layer1_init.Color_Key_Enable.key_enable)
        {
            PPE_input_layer1_init.Color_Key_Mode                  = dst->color_key_config.key_mode;
            PPE_input_layer1_init.Color_Key_MAX_B = dst->color_key_config.key_range.B_max;
            PPE_input_layer1_init.Color_Key_MAX_G = dst->color_key_config.key_range.G_max;
            PPE_input_layer1_init.Color_Key_MAX_R = dst->color_key_config.key_range.R_max;
            PPE_input_layer1_init.Color_Key_MIN_B = dst->color_key_config.key_range.B_min;
            PPE_input_layer1_init.Color_Key_MIN_G = dst->color_key_config.key_range.G_min;
            PPE_input_layer1_init.Color_Key_MIN_R = dst->color_key_config.key_range.R_min;
            PPE_input_layer1_init.Color_Key_Replace_A = dst->color_key_config.key_replace.channel_replace.a_replace;
            PPE_input_layer1_init.Color_Key_Replace_B = dst->color_key_config.key_replace.channel_replace.b_replace;
            PPE_input_layer1_init.Color_Key_Replace_G = dst->color_key_config.key_replace.channel_replace.g_replace;
            PPE_input_layer1_init.Color_Key_Replace_R = dst->color_key_config.key_replace.channel_replace.r_replace;
        }

        PPE_input_layer1_init.Transfer_Matrix_E11             = 0x10000;
        PPE_input_layer1_init.Transfer_Matrix_E12             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E13             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E21             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E22             = 0x10000;
        PPE_input_layer1_init.Transfer_Matrix_E23             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E31             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E32             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E33             = 0x10000;
        PPE_InputLayer_Init(PPE_INPUT_1, &PPE_input_layer1_init);
    }
    else
    {
        PPE_InputLayer_enable(PPE_INPUT_1, DISABLE);
    }

    PPE_ResultLayer_Init_Typedef PPE_ResultLayer0_Init;
    PPE_ResultLayer_StructInit(&PPE_ResultLayer0_Init);
    if(output == NULL)
    {
        PPE_ResultLayer0_Init.Layer_Address                   = dst->address;
    }
    else
    {
        PPE_ResultLayer0_Init.Layer_Address                   = (uint32_t)output;
    }
    if(rect == NULL)
    {
        PPE_ResultLayer0_Init.Layer_Window_Xmin               = 0;
        PPE_ResultLayer0_Init.Layer_Window_Xmax               = dst->width - 1;
        PPE_ResultLayer0_Init.Layer_Window_Ymin               = 0;
        PPE_ResultLayer0_Init.Layer_Window_Ymax               = dst->height - 1;
    }
    else
    {
        PPE_ResultLayer0_Init.Layer_Window_Xmin               = rect->x;
        PPE_ResultLayer0_Init.Layer_Window_Xmax               = rect->x + rect->w - 1;
        PPE_ResultLayer0_Init.Layer_Window_Ymin               = rect->y;
        PPE_ResultLayer0_Init.Layer_Window_Ymax               = rect->y + rect->h - 1;
    }
    PPE_ResultLayer0_Init.Line_Length                     = dst->stride * PPE_Get_Pixel_Size(dst->format);
    PPE_ResultLayer0_Init.Color_Format                    = dst->format;
    PPE_ResultLayer0_Init.LayerBus_Inc                    = PPE_AWBURST_INC;

    if (PPE_ResultLayer0_Init.Block_Width > PPE_ResultLayer0_Init.Layer_Window_Xmax - PPE_ResultLayer0_Init.Layer_Window_Xmin + 1)
    {
        PPE_ResultLayer0_Init.Block_Width = PPE_ResultLayer0_Init.Layer_Window_Xmax - PPE_ResultLayer0_Init.Layer_Window_Xmin + 1;
    }
    if (PPE_ResultLayer0_Init.Block_Height > PPE_ResultLayer0_Init.Layer_Window_Ymax - PPE_ResultLayer0_Init.Layer_Window_Ymin + 1)
    {
        PPE_ResultLayer0_Init.Block_Height = PPE_ResultLayer0_Init.Layer_Window_Ymax - PPE_ResultLayer0_Init.Layer_Window_Ymin + 1;
    }
//    for(int i = 5; i >= 0; i--)
//    {
//        DIRECT_LOG("table width %d", 1, block_width[i]);
//        if(PPE_ResultLayer0_Init.Block_Width >= block_width[i])
//        {
//            PPE_ResultLayer0_Init.Block_Width = block_width[i];
//            //DIRECT_LOG("change width %d", 1, PPE_ResultLayer0_Init.Block_Width);
//            break;
//        }
//    }
//    PPE_ResultLayer0_Init.Block_Width = 1;
//    PPE_ResultLayer0_Init.Block_Height = 1;
//    PPE_Clear_Interrupt(PPE_DIV0_ERR_INT);
//    PPE_Mask_Interrupt(PPE_DIV0_ERR_INT, ENABLE);
//    PPE_Mask_All_Interrupt(ENABLE);
    extern uint32_t sys_timestamp_get_from_hw_timer_us(void);
    uint32_t time1, time2;
    *(uint32_t*)(PPE_REG_BASE + 0x838) = 1;
    *(uint32_t*)(PPE_REG_BASE + 0x938) = 1;
    *(uint32_t*)(PPE_REG_BASE + 0xA38) = 1;
    *(uint32_t*)(PPE_REG_BASE + 0xB38) = 1;
    
//    DBG_DIRECT("0x234 0x%08x, 0x210 0x%08x", *(uint32_t *)(0x40000234UL),  *(uint32_t *)(0x40000210UL));
//    
//    uint32_t* data = (uint32_t*)PPE;
//    for(int i = 0; i < 0x80; i++)
//    {
//        DBG_DIRECT("offset %08x = 0x%08x", i * 4, data[i]);
//    }
//    uint16_t block_width[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
//    uint16_t block_height[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
//    for (int i = 0; i < 6; i++)
//    {
//        for(int j = 0; j < 6; j++)
//        {
//            if(block_width[i] * block_height[j] * PPE_Get_Pixel_Size(dst->format) / PPE_BYTE_SIZE > 4096)
//            {
//                continue;
//            }
            if(PPE_Get_Pixel_Size(src->format) == 16)
            {                
                PPE_ResultLayer0_Init.Block_Width = 32; //block_width[i];
                PPE_ResultLayer0_Init.Block_Height = 32; //block_height[j];
            }
            else if(PPE_Get_Pixel_Size(src->format) == 24)
            {
                if(PPE_Get_Pixel_Size(dst->format) == 16)
                {
                    PPE_ResultLayer0_Init.Block_Width = 16;
                    PPE_ResultLayer0_Init.Block_Height = 24;
                }
                else
                {
                    PPE_ResultLayer0_Init.Block_Width = 12;
                    PPE_ResultLayer0_Init.Block_Height = 24;
                }
            }
            else if(PPE_Get_Pixel_Size(src->format) == 32)
            {
                if(PPE_Get_Pixel_Size(dst->format) == 16)
                {
                    PPE_ResultLayer0_Init.Block_Width = 16;
                    PPE_ResultLayer0_Init.Block_Height = 24;
                }
                else
                {
                    PPE_ResultLayer0_Init.Block_Width = 16;
                    PPE_ResultLayer0_Init.Block_Height = 16;
                }
            }
            else if(PPE_Get_Pixel_Size(src->format) <= 8)
            {
                PPE_ResultLayer0_Init.Block_Width = 64;
                PPE_ResultLayer0_Init.Block_Height = 64;
            }
            PPE_ResultLayer_Init(&PPE_ResultLayer0_Init);
//            uint32_t* PPE_REG = (uint32_t*)PPE;
//            for(int i = 0; i < sizeof(PPE_Typedef) / 4; i++)
//            {
//                DBG_DIRECT("GLOBAL 0x%04x, value: 0x%08x", i * 4, *(PPE_REG + i));
//            }
//            uint32_t* PPE_RESULT_REG = (uint32_t*)PPE_ResultLayer;
//            for(int i = 0; i < sizeof(PPE_ResultLayer_Typedef) / 4; i++)
//            {
//                DBG_DIRECT("RESULT 0x%04x, value: 0x%08x", i * 4, *(PPE_RESULT_REG + i));
//            }
//            
//            uint32_t* PPE_LAYER1_REG = (uint32_t*)PPE_InputLayer1;
//            for(int i = 0; i < sizeof(PPE_Input_Layer_Typedef) / 4; i++)
//            {
//                DBG_DIRECT("LAYER1 0x%04x, value: 0x%08x", i * 4, *(PPE_LAYER1_REG + i));
//            }
//            uint32_t* PPE_LAYER2_REG = (uint32_t*)PPE_InputLayer2;
//            for(int i = 0; i < sizeof(PPE_Input_Layer_Typedef) / 4; i++)
//            {
//                DBG_DIRECT("LAYER2 0x%04x, value: 0x%08x", i * 4, *(PPE_LAYER2_REG + i));
//            }
            time1 = sys_timestamp_get_from_hw_timer_us();
            PPE_Cmd(ENABLE);
            while (((PPE_REG_GLB_STATUS_TypeDef)PPE->REG_GLB_STATUS).b.run_state)
            {
            }
            time2 = sys_timestamp_get_from_hw_timer_us();
            DBG_DIRECT("duration %d block width %d, height %d", (time2 - time1) / 40, PPE_ResultLayer0_Init.Block_Width, PPE_ResultLayer0_Init.Block_Height);
            
            uint32_t G1_cache_cnt = *(uint32_t*)(PPE_REG_BASE + 0x810);
            uint32_t G2_cache_cnt = *(uint32_t*)(PPE_REG_BASE + 0x910);
            uint32_t G3_cache_cnt = *(uint32_t*)(PPE_REG_BASE + 0xA10);
            uint32_t G4_cache_cnt = *(uint32_t*)(PPE_REG_BASE + 0xB10);
            
            uint32_t G1_hit_cnt = *(uint32_t*)(PPE_REG_BASE + 0x814);
            uint32_t G2_hit_cnt = *(uint32_t*)(PPE_REG_BASE + 0x914);
            uint32_t G3_hit_cnt = *(uint32_t*)(PPE_REG_BASE + 0xA14);
            uint32_t G4_hit_cnt = *(uint32_t*)(PPE_REG_BASE + 0xB14);
            
            DBG_DIRECT("cache G1 %d, G2 %d, G3 %d, G4 %d, total %d", G1_cache_cnt, G2_cache_cnt, G3_cache_cnt, G4_cache_cnt, \
                                    G1_cache_cnt + G2_cache_cnt + G3_cache_cnt + G4_cache_cnt);
//            DBG_DIRECT("hit G1 %d, G2 %d, G3 %d, G4 %d, total %d", G1_hit_cnt, G2_hit_cnt, G3_hit_cnt, G4_hit_cnt, \
//                                    G1_hit_cnt + G2_hit_cnt + G3_hit_cnt + G4_hit_cnt);
            DBG_DIRECT("cache from psram %d", (G1_cache_cnt + G2_cache_cnt + G3_cache_cnt + G4_cache_cnt) - (G1_hit_cnt + G2_hit_cnt + G3_hit_cnt + G4_hit_cnt));
            *(uint32_t*)(PPE_REG_BASE + 0x80C) = 0xFFFFFFFF;
            *(uint32_t*)(PPE_REG_BASE + 0x90C) = 0xFFFFFFFF;
            *(uint32_t*)(PPE_REG_BASE + 0xA0C) = 0xFFFFFFFF;
            *(uint32_t*)(PPE_REG_BASE + 0xB0C) = 0xFFFFFFFF;
            
//        }
//    }
    return PPE_SUCCESS;
}

PPE_ERR PPE_Blit_Inverse_Simulate(ppe_buffer_t *dst, ppe_buffer_t *src, ppe_matrix_t *matrix, ppe_matrix_t *inverse,
                             ppe_rect_t *rect, PPE_BLEND_METHOD method)
{
    if (dst->address == NULL)
    {
        return PPE_ERR_NULL_TARGET;
    }
    if (src->address == NULL)
    {
        return PPE_ERR_NULL_SOURCE;
    }
    if ((src->win_x_max <= src->win_x_min) || (src->win_y_max <= src->win_y_min))
    {
        return PPE_ERR_INVALID_RANGE;
    }
    if (rect != NULL)
    {
        if (rect->h == 0 || rect->w == 0)
        {
            return PPE_SUCCESS;
        }
    }
    if (inverse == NULL)
    {
        return PPE_ERR_INVALID_MATRIX;
    }
    uint32_t comp[9];
    if (!check_inverse(inverse))
    {
        return PPE_ERR_INVALID_MATRIX;
    }
    else
    {
        if (!inv_matrix2complement(inverse, comp))
        {
            return PPE_SUCCESS;
        }
    }

    if (src->opacity == 0)
    {
        return PPE_SUCCESS;
    }

    uint32_t color = 0;
    if (src->format >= PPE_A8 && src->format <= PPE_X1)
    {
        color = (src->const_color & 0xFFFFFF) + ((((src->const_color >> 24) & 0xFF) * src->opacity / 0xFF) << 24);
    }
    else
    {
        color = ((src->opacity & 0xFF) * ((src->const_color & 0xFF000000) >> 24) / 0xFF << 24) + \
                    (src->const_color & 0xFFFFFF);
    }

    PPE_CLK_ENABLE(ENABLE);
    PPE_REG_GLB_STATUS_TypeDef     ppe_reg_blg_status_0x00;
    PPE_REG_LYR_ENABLE_TypeDef     ppe_reg_lyr_enable_0x04;


    /*PPE Global initialization*/

    /*input layer 2 initilization*/
    PPE_InputLayer_enable(PPE_INPUT_2, ENABLE);           // logic enable of layer
    PPE_InputLayer_Init_Typedef PPE_input_layer2_init = {.Color_Key_Enable.key_enable = 0};
    PPE_InputLayer_StructInit(PPE_INPUT_2, &PPE_input_layer2_init);
    PPE_input_layer2_init.Layer_Address                   = (uint32_t)src->address;
    PPE_input_layer2_init.Pic_Height                      = (uint32_t)src->height;
    PPE_input_layer2_init.Pic_Width                       = (uint32_t)src->width;
    PPE_input_layer2_init.Line_Length                     = PPE_input_layer2_init.Pic_Width *
                                                              PPE_Get_Pixel_Size(src->format);
    PPE_input_layer2_init.Pixel_Source                = PPE_LAYER_SRC_FROM_DMA;
    PPE_input_layer2_init.Const_Pixel                 = color;
    PPE_input_layer2_init.Pixel_Color_Format              = src->format;
    if(PPE_input_layer2_init.Pixel_Color_Format >= PPE_I8 && PPE_input_layer2_init.Pixel_Color_Format <= PPE_I1)
    {
        PPE_input_layer2_init.Index_Table = INDEX_CLUT;
    }
    PPE_input_layer2_init.Blend_Method = method;
    if (src->high_quality)
    {
        PPE_input_layer2_init.Source_Interpolation                = PPV2_SRC_BILINEAR;
    }
    else
    {
        PPE_input_layer2_init.Source_Interpolation                = PPV2_SRC_NEAREST_NEIGHBOUR;
    }
    PPE_input_layer2_init.Color_Key_Enable.key_enable     = src->color_key_config.key_enable.key_enable;
    if (PPE_input_layer2_init.Color_Key_Enable.key_enable)
    {
        PPE_input_layer2_init.Color_Key_Mode                  = src->color_key_config.key_mode;
        PPE_input_layer2_init.Color_Key_MAX_B = src->color_key_config.key_range.B_max;
        PPE_input_layer2_init.Color_Key_MAX_G = src->color_key_config.key_range.G_max;
        PPE_input_layer2_init.Color_Key_MAX_R = src->color_key_config.key_range.R_max;
        PPE_input_layer2_init.Color_Key_MIN_B = src->color_key_config.key_range.B_min;
        PPE_input_layer2_init.Color_Key_MIN_G = src->color_key_config.key_range.G_min;
        PPE_input_layer2_init.Color_Key_MIN_R = src->color_key_config.key_range.R_min;
        PPE_input_layer2_init.Color_Key_Replace_A = src->color_key_config.key_replace.channel_replace.a_replace;
        PPE_input_layer2_init.Color_Key_Replace_B = src->color_key_config.key_replace.channel_replace.b_replace;
        PPE_input_layer2_init.Color_Key_Replace_G = src->color_key_config.key_replace.channel_replace.g_replace;
        PPE_input_layer2_init.Color_Key_Replace_R = src->color_key_config.key_replace.channel_replace.r_replace;
    }
    PPE_input_layer2_init.LayerBus_Inc                    = PPE_AWBURST_INC;
    PPE_input_layer2_init.Layer_Window_Xmin               = src->win_x_min;
    PPE_input_layer2_init.Layer_Window_Xmax               = src->win_x_max;
    PPE_input_layer2_init.Layer_Window_Ymin               = src->win_y_min;
    PPE_input_layer2_init.Layer_Window_Ymax               = src->win_y_max;

    uint32_t ct = 0;
    PPE_input_layer2_init.Transfer_Matrix_E11             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E12             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E13             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E21             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E22             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E23             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E31             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E32             = comp[ct++];
    PPE_input_layer2_init.Transfer_Matrix_E33             = comp[ct++];
    
//    PPE_input_layer2_init.Transfer_Matrix_E11             = 0xfffe4ad9;
//    PPE_input_layer2_init.Transfer_Matrix_E12             = 0x0004b22c;
//    PPE_input_layer2_init.Transfer_Matrix_E13             = 0xffeaa792;
//    PPE_input_layer2_init.Transfer_Matrix_E21             = 0x00019142;
//    PPE_input_layer2_init.Transfer_Matrix_E22             = 0x000011e9;
//    PPE_input_layer2_init.Transfer_Matrix_E23             = 0xffe7c9f5;
//    PPE_input_layer2_init.Transfer_Matrix_E31             = 0xfffffbf6;
//    PPE_input_layer2_init.Transfer_Matrix_E32             = 0x00000714;
//    PPE_input_layer2_init.Transfer_Matrix_E33             = 0x0000f5db;
    //PPE_InputLayer_Init(PPE_INPUT_2, &PPE_input_layer2_init);

    PPE_InputLayer_Init_Typedef PPE_input_layer1_init = {.Color_Key_Enable.key_enable = 0};
    if (1) //(mode == PPE_SRC_OVER_MODE || mode == PPE_CONST_MASK_MODE)
    {
        /*input layer 1 initilization*/
        PPE_InputLayer_enable(PPE_INPUT_1, ENABLE);           // logic enable of layer
        PPE_InputLayer_StructInit(PPE_INPUT_1, &PPE_input_layer1_init);
        PPE_input_layer1_init.Layer_Address                   = dst->address;
        PPE_input_layer1_init.Pic_Width                      = (uint32_t)dst->width;
        PPE_input_layer1_init.Pic_Height                       = (uint32_t)dst->height;
        PPE_input_layer1_init.Line_Length                     = dst->stride * PPE_Get_Pixel_Size(
                                                                      dst->format);
        PPE_input_layer1_init.Pixel_Source                    = PPE_LAYER_SRC_FROM_DMA;
        PPE_input_layer1_init.Pixel_Color_Format              = dst->format;
        PPE_input_layer1_init.Const_Pixel                     = 0xFFFFFFFF;
        PPE_input_layer1_init.LayerBus_Inc                    = PPE_AWBURST_INC;
        PPE_input_layer1_init.Layer_Window_Xmin               = 0;
        PPE_input_layer1_init.Layer_Window_Xmax               = dst->width - 1;
        PPE_input_layer1_init.Layer_Window_Ymin               = 0;
        PPE_input_layer1_init.Layer_Window_Ymax               = dst->height - 1;
        PPE_input_layer1_init.Cache_Enable                    = 0x6;
        PPE_input_layer1_init.Blend_Method                    = PPE_BLEND_SRC;
        PPE_input_layer1_init.Color_Key_Enable.key_enable     = dst->color_key_config.key_enable.key_enable;
        if (PPE_input_layer1_init.Color_Key_Enable.key_enable)
        {
            PPE_input_layer1_init.Color_Key_Mode                  = dst->color_key_config.key_mode;
            PPE_input_layer1_init.Color_Key_MAX_B = dst->color_key_config.key_range.B_max;
            PPE_input_layer1_init.Color_Key_MAX_G = dst->color_key_config.key_range.G_max;
            PPE_input_layer1_init.Color_Key_MAX_R = dst->color_key_config.key_range.R_max;
            PPE_input_layer1_init.Color_Key_MIN_B = dst->color_key_config.key_range.B_min;
            PPE_input_layer1_init.Color_Key_MIN_G = dst->color_key_config.key_range.G_min;
            PPE_input_layer1_init.Color_Key_MIN_R = dst->color_key_config.key_range.R_min;
            PPE_input_layer1_init.Color_Key_Replace_A = dst->color_key_config.key_replace.channel_replace.a_replace;
            PPE_input_layer1_init.Color_Key_Replace_B = dst->color_key_config.key_replace.channel_replace.b_replace;
            PPE_input_layer1_init.Color_Key_Replace_G = dst->color_key_config.key_replace.channel_replace.g_replace;
            PPE_input_layer1_init.Color_Key_Replace_R = dst->color_key_config.key_replace.channel_replace.r_replace;
        }

        PPE_input_layer1_init.Transfer_Matrix_E11             = 0x10000;
        PPE_input_layer1_init.Transfer_Matrix_E12             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E13             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E21             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E22             = 0x10000;
        PPE_input_layer1_init.Transfer_Matrix_E23             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E31             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E32             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E33             = 0x10000;
        //PPE_InputLayer_Init(PPE_INPUT_1, &PPE_input_layer1_init);
    }
    else if (1)//(mode == PPE_BYPASS_MODE)
    {
        PPE_InputLayer_enable(PPE_INPUT_1, ENABLE);           // logic enable of layer
        PPE_InputLayer_Init_Typedef PPE_input_layer1_init = {.Color_Key_Enable.key_enable = 0};
        PPE_InputLayer_StructInit(PPE_INPUT_1, &PPE_input_layer1_init);
        PPE_input_layer1_init.Layer_Address                   = NULL;
        PPE_input_layer1_init.Pic_Height                      = (uint32_t)rect->h;
        PPE_input_layer1_init.Pic_Width                       = (uint32_t)rect->w;
        PPE_input_layer1_init.Line_Length                     = dst->width * PPE_Get_Pixel_Size(
                                                                      dst->format) / PPE_BYTE_SIZE;
        PPE_input_layer1_init.Pixel_Source                    = PPE_LAYER_SRC_CONST;
        PPE_input_layer1_init.Pixel_Color_Format              = dst->format;
        PPE_input_layer1_init.Const_Pixel                     = 0x0;
        PPE_input_layer1_init.Source_Interpolation             = PPV2_SRC_NEAREST_NEIGHBOUR;
        PPE_input_layer1_init.LayerBus_Inc                    = PPE_AWBURST_INC;
        PPE_input_layer1_init.Layer_Window_Xmin               = 0;
        PPE_input_layer1_init.Layer_Window_Xmax               = dst->width;
        PPE_input_layer1_init.Layer_Window_Ymin               = 0;
        PPE_input_layer1_init.Layer_Window_Ymax               = dst->height;

        PPE_input_layer1_init.Transfer_Matrix_E11             = 0x10000;
        PPE_input_layer1_init.Transfer_Matrix_E12             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E13             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E21             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E22             = 0x10000;
        PPE_input_layer1_init.Transfer_Matrix_E23             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E31             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E32             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E33             = 0x10000;
        PPE_InputLayer_Init(PPE_INPUT_1, &PPE_input_layer1_init);
    }
    else
    {
        PPE_InputLayer_enable(PPE_INPUT_1, DISABLE);
    }


    PPE_ResultLayer_Init_Typedef PPE_ResultLayer0_Init;
    PPE_ResultLayer_StructInit(&PPE_ResultLayer0_Init);
    PPE_ResultLayer0_Init.Layer_Address                   = dst->address;
    if(rect == NULL)
    {
        PPE_ResultLayer0_Init.Layer_Window_Xmin               = 0;
        PPE_ResultLayer0_Init.Layer_Window_Xmax               = dst->width - 1;
        PPE_ResultLayer0_Init.Layer_Window_Ymin               = 0;
        PPE_ResultLayer0_Init.Layer_Window_Ymax               = dst->height - 1;
    }
    else
    {
        PPE_ResultLayer0_Init.Layer_Window_Xmin               = rect->x;
        PPE_ResultLayer0_Init.Layer_Window_Xmax               = rect->x + rect->w - 1;
        PPE_ResultLayer0_Init.Layer_Window_Ymin               = rect->y;
        PPE_ResultLayer0_Init.Layer_Window_Ymax               = rect->y + rect->h - 1;
    }
    PPE_ResultLayer0_Init.Line_Length                     = dst->stride * PPE_Get_Pixel_Size(dst->format);
    PPE_ResultLayer0_Init.Color_Format                    = dst->format;
    PPE_ResultLayer0_Init.LayerBus_Inc                    = PPE_AWBURST_INC;
    bool ret = ppe_get_block_size(matrix, PPE_Get_Pixel_Size(src->format), &PPE_ResultLayer0_Init.Block_Width, &PPE_ResultLayer0_Init.Block_Height);
    if(!ret)
    {
        return PPE_ERR_INVALID_PARAMETER;
    }
    PPE_ResultLayer_Init(&PPE_ResultLayer0_Init);

    //PPE_Cmd(ENABLE);
    //while (((PPE_REG_GLB_STATUS_TypeDef)PPE->REG_GLB_STATUS).b.run_state);
    uint32_t layer_en = (BIT0 | BIT1);
    PPE_InputLayer_Init_Typedef in_list[2];
    memcpy(&in_list[0], &PPE_input_layer1_init, sizeof(PPE_InputLayer_Init_Typedef));
    memcpy(&in_list[1], &PPE_input_layer2_init, sizeof(PPE_InputLayer_Init_Typedef));
    PPE_Simulation(&PPE_ResultLayer0_Init, in_list, layer_en);
    return PPE_SUCCESS;
}

PPE_ERR PPE_Mask(ppe_buffer_t *dst, uint32_t color, ppe_rect_t *rect)
{
    if (dst->address == NULL)
    {
        return PPE_ERR_NULL_TARGET;
    }
    if (rect->h == 0 || rect->w == 0)
    {
        return PPE_SUCCESS;
    }

    PPE_CLK_ENABLE(ENABLE);
    PPE_REG_GLB_STATUS_TypeDef     ppe_reg_blg_status_0x00;
    PPE_REG_LYR_ENABLE_TypeDef     ppe_reg_lyr_enable_0x04;

    /*input layer 2 initilization*/
    PPE_InputLayer_enable(PPE_INPUT_2, ENABLE);           // logic enable of layer
    PPE_InputLayer_Init_Typedef PPE_input_layer2_init = {.Color_Key_Enable.key_enable = 0};
    PPE_InputLayer_StructInit(PPE_INPUT_2, &PPE_input_layer2_init);
    PPE_input_layer2_init.Layer_Address                   = NULL;
    PPE_input_layer2_init.Pic_Height                      = rect->h;
    PPE_input_layer2_init.Pic_Width                       = rect->w;
    PPE_input_layer2_init.Line_Length                     = rect->w * 32;
    PPE_input_layer2_init.Pixel_Color_Format              = PPE_ABGR8888;
    PPE_input_layer2_init.Pixel_Source                    = PPE_LAYER_SRC_CONST;
    PPE_input_layer2_init.Const_Pixel                     = color;
    PPE_input_layer2_init.Color_Key_Mode                  = PPE_COLOR_KEY_INSIDE;
    PPE_input_layer2_init.Source_Interpolation             = PPV2_SRC_BILINEAR;

    PPE_input_layer2_init.LayerBus_Inc                    = PPE_AWBURST_INC;
    if(rect != NULL)
    {
        PPE_input_layer2_init.Layer_Window_Xmin               = 0;
        PPE_input_layer2_init.Layer_Window_Xmax               = rect->w - 1;
        PPE_input_layer2_init.Layer_Window_Ymin               = 0;
        PPE_input_layer2_init.Layer_Window_Ymax               = rect->h - 1;
    }

    PPE_input_layer2_init.Transfer_Matrix_E11             = 0x10000;
    PPE_input_layer2_init.Transfer_Matrix_E12             = 0;
    PPE_input_layer2_init.Transfer_Matrix_E13             = 0;
    PPE_input_layer2_init.Transfer_Matrix_E21             = 0;
    PPE_input_layer2_init.Transfer_Matrix_E22             = 0x10000;
    PPE_input_layer2_init.Transfer_Matrix_E23             = 0;
    PPE_input_layer2_init.Transfer_Matrix_E31             = 0;
    PPE_input_layer2_init.Transfer_Matrix_E32             = 0;
    PPE_input_layer2_init.Transfer_Matrix_E33             = 0x10000;
    PPE_InputLayer_Init(PPE_INPUT_2, &PPE_input_layer2_init);

    if ((color & 0xFF000000) != 0xFF000000)
    {
        /*input layer 1 initilization*/
        PPE_InputLayer_enable(PPE_INPUT_1, ENABLE);           // logic enable of layer
        PPE_InputLayer_Init_Typedef PPE_input_layer1_init = {.Color_Key_Enable.key_enable = 0};
        PPE_InputLayer_StructInit(PPE_INPUT_1, &PPE_input_layer1_init);
        PPE_input_layer1_init.Layer_Address                   = dst->address +
                                                                  (rect->y * dst->width + rect->x) * PPE_Get_Pixel_Size(dst->format) / PPE_BYTE_SIZE;
        PPE_input_layer1_init.Pic_Height                      = (uint32_t)rect->h;
        PPE_input_layer1_init.Pic_Width                       = (uint32_t)rect->w;
        PPE_input_layer1_init.Line_Length                     = dst->width * PPE_Get_Pixel_Size(
                                                                      dst->format) / PPE_BYTE_SIZE;
        PPE_input_layer1_init.Pixel_Source                    = PPE_LAYER_SRC_FROM_DMA;
        PPE_input_layer1_init.Pixel_Color_Format              = dst->format;
        PPE_input_layer1_init.Const_Pixel                     = 0xFFFFFFFF;
        PPE_input_layer1_init.LayerBus_Inc                    = PPE_AWBURST_INC;
        PPE_input_layer1_init.Layer_Window_Xmin               = 0;
        PPE_input_layer1_init.Layer_Window_Xmax               = dst->width;
        PPE_input_layer1_init.Layer_Window_Ymin               = 0;
        PPE_input_layer1_init.Layer_Window_Ymax               = dst->height;

        PPE_input_layer1_init.Transfer_Matrix_E11             = 0x10000;
        PPE_input_layer1_init.Transfer_Matrix_E12             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E13             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E21             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E22             = 0x10000;
        PPE_input_layer1_init.Transfer_Matrix_E23             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E31             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E32             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E33             = 0x10000;
        PPE_InputLayer_Init(PPE_INPUT_1, &PPE_input_layer1_init);
    }
    else
    {
        PPE_InputLayer_enable(PPE_INPUT_1, ENABLE);           // logic enable of layer
        PPE_InputLayer_Init_Typedef PPE_input_layer1_init = {.Color_Key_Enable.key_enable = 0};
        PPE_InputLayer_StructInit(PPE_INPUT_1, &PPE_input_layer1_init);
        PPE_input_layer1_init.Layer_Address                   = NULL;
        PPE_input_layer1_init.Pic_Height                      = (uint32_t)rect->h;
        PPE_input_layer1_init.Pic_Width                       = (uint32_t)rect->w;
        PPE_input_layer1_init.Line_Length                     = dst->width * PPE_Get_Pixel_Size(
                                                                      dst->format) / PPE_BYTE_SIZE;
        PPE_input_layer1_init.Pixel_Source                    = PPE_LAYER_SRC_CONST;
        PPE_input_layer1_init.Pixel_Color_Format              = dst->format;
        PPE_input_layer1_init.Const_Pixel                     = 0x0;
        PPE_input_layer1_init.LayerBus_Inc                    = PPE_AWBURST_INC;
        PPE_input_layer1_init.Layer_Window_Xmin               = 0;
        PPE_input_layer1_init.Layer_Window_Xmax               = dst->width;
        PPE_input_layer1_init.Layer_Window_Ymin               = 0;
        PPE_input_layer1_init.Layer_Window_Ymax               = dst->height;

        PPE_input_layer1_init.Transfer_Matrix_E11             = 0x10000;
        PPE_input_layer1_init.Transfer_Matrix_E12             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E13             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E21             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E22             = 0x10000;
        PPE_input_layer1_init.Transfer_Matrix_E23             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E31             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E32             = 0;
        PPE_input_layer1_init.Transfer_Matrix_E33             = 0x10000;
        PPE_InputLayer_Init(PPE_INPUT_1, &PPE_input_layer1_init);
    }


    PPE_ResultLayer_Init_Typedef PPE_ResultLayer0_Init;
    PPE_ResultLayer_StructInit(&PPE_ResultLayer0_Init);
    PPE_ResultLayer0_Init.Layer_Address                   = dst->address +
                                                              (rect->y * dst->width + rect->x) * PPE_Get_Pixel_Size(dst->format) / PPE_BYTE_SIZE;
    PPE_ResultLayer0_Init.Line_Length                     = dst->width * PPE_Get_Pixel_Size(
                                                                  dst->format) / PPE_BYTE_SIZE;
    PPE_ResultLayer0_Init.Color_Format                    = dst->format;
    PPE_ResultLayer0_Init.LayerBus_Inc                    = PPE_AWBURST_INC;
    PPE_ResultLayer_Init(&PPE_ResultLayer0_Init);

    PPE_Cmd(ENABLE);
    while (((PPE_REG_GLB_STATUS_TypeDef)PPE->REG_GLB_STATUS).b.run_state);
    return PPE_SUCCESS;
}

void PPE_Register_CLUT(uint32_t* clut, uint16_t size)
{
    PPE->CLUT_INDEX = 0;
    for(int i = 0; i < size; i++)
    {
        PPE->CLUT_CONT = clut[i];
    }
    INDEX_CLUT = clut;
}

void ppe_get_identity(ppe_matrix_t *matrix)
{
    /* Set identify matrix. */
    matrix->m[0][0] = 1.0f;
    matrix->m[0][1] = 0.0f;
    matrix->m[0][2] = 0.0f;
    matrix->m[1][0] = 0.0f;
    matrix->m[1][1] = 1.0f;
    matrix->m[1][2] = 0.0f;
    matrix->m[2][0] = 0.0f;
    matrix->m[2][1] = 0.0f;
    matrix->m[2][2] = 1.0f;
}

#if USE_PPE_MAT
static const int16_t sin_table[] =
{
    0,     572,   1144,  1715,  2286,  2856,  3425,  3993,  4560,  5126,  5690,  6252,  6813,  7371,  7927,  8481,
    9032,  9580,  10126, 10668, 11207, 11743, 12275, 12803, 13328, 13848, 14364, 14876, 15383, 15886, 16383, 16876,
    17364, 17846, 18323, 18794, 19260, 19720, 20173, 20621, 21062, 21497, 21925, 22347, 22762, 23170, 23571, 23964,
    24351, 24730, 25101, 25465, 25821, 26169, 26509, 26841, 27165, 27481, 27788, 28087, 28377, 28659, 28932, 29196,
    29451, 29697, 29934, 30162, 30381, 30591, 30791, 30982, 31163, 31335, 31498, 31650, 31794, 31927, 32051, 32165,
    32269, 32364, 32448, 32523, 32587, 32642, 32687, 32722, 32747, 32762, 32767
};

static int16_t ppe_fix_sin(int16_t angle)
{
    int16_t ret = 0;
    angle       = angle % 360;

    if (angle < 0) { angle = 360 + angle; }

    if (angle < 90)
    {
        ret = sin_table[angle];
    }
    else if (angle >= 90 && angle < 180)
    {
        angle = 180 - angle;
        ret   = sin_table[angle];
    }
    else if (angle >= 180 && angle < 270)
    {
        angle = angle - 180;
        ret   = -sin_table[angle];
    }
    else     /*angle >=270*/
    {
        angle = 360 - angle;
        ret   = -sin_table[angle];
    }

    return ret;
}

static inline int16_t ppe_fix_cos(int16_t angle)
{
    return ppe_fix_sin(angle + 90);
}


void ppe_mat_multiply(ppe_matrix_t *matrix, ppe_matrix_t *mult)
{
    float m00, m01, m02, m10, m11, m12, m20, m21, m22;
    m00 = matrix->m[0][0];
    m01 = matrix->m[0][1];
    m02 = matrix->m[0][2];

    m10 = matrix->m[1][0];
    m11 = matrix->m[1][1];
    m12 = matrix->m[1][2];

    m20 = matrix->m[2][0];
    m21 = matrix->m[2][1];
    m22 = matrix->m[2][2];

    float t00, t01, t02, t10, t11, t12, t20, t21, t22;
    t00 = mult->m[0][0];
    t01 = mult->m[0][1];
    t02 = mult->m[0][2];

    t10 = mult->m[1][0];
    t11 = mult->m[1][1];
    t12 = mult->m[1][2];

    t20 = mult->m[2][0];
    t21 = mult->m[2][1];
    t22 = mult->m[2][2];
    /* Compute matrix entry. */
    //row = 0;
    matrix->m[0][0] = (m00 * t00) + (m01 * t10) + (m02 * t20);
    matrix->m[0][1] = (m00 * t01) + (m01 * t11) + (m02 * t21);
    matrix->m[0][2] = (m00 * t02) + (m01 * t12) + (m02 * t22);
    //row = 1;
    matrix->m[1][0] = (m10 * t00) + (m11 * t10) + (m12 * t20);
    matrix->m[1][1] = (m10 * t01) + (m11 * t11) + (m12 * t21);
    matrix->m[1][2] = (m10 * t02) + (m11 * t12) + (m12 * t22);
    ///row = 2;
    matrix->m[2][0] = (m20 * t00) + (m21 * t10) + (m22 * t20);
    matrix->m[2][1] = (m20 * t01) + (m21 * t11) + (m22 * t21);
    matrix->m[2][2] = (m20 * t02) + (m21 * t12) + (m22 * t22);
}

void ppe_translate(float x, float y, ppe_matrix_t *matrix)
{
    /* Set translation matrix. */
    ppe_matrix_t t = { { {1.0f, 0.0f, x},
            {0.0f, 1.0f, y},
            {0.0f, 0.0f, 1.0f}
        }
    };

    /* Multiply with current matrix. */
    ppe_mat_multiply(matrix, &t);
}

void ppe_scale(float scale_x, float scale_y, ppe_matrix_t *matrix)
{
    /* Set scale matrix. */
    ppe_matrix_t s = { { {scale_x, 0.0f, 0.0f},
            {0.0f, scale_y, 0.0f},
            {0.0f, 0.0f, 1.0f}
        }
    };

    /* Multiply with current matrix. */
    ppe_mat_multiply(matrix, &s);
}

void ppe_rotate(float degrees, ppe_matrix_t *matrix)
{
#ifndef M_PI
#define M_PI 3.1415926535898f
#endif
    /* Convert degrees into radians. */
    int16_t angle = (int)degrees;

    /* Compuet cosine and sine values. */
    float cos_angle = ppe_fix_cos(angle) / 32767.0f;
    float sin_angle = ppe_fix_sin(angle) / 32767.0f;

    /* Set rotation matrix. */
    ppe_matrix_t r = { { {cos_angle, -sin_angle, 0.0f},
            {sin_angle, cos_angle, 0.0f},
            {0.0f, 0.0f, 1.0f}
        }
    };

    /* Multiply with current matrix. */
    ppe_mat_multiply(matrix, &r);
}

void ppe_perspective(float px, float py, ppe_matrix_t *matrix)
{
    /* set prespective matrix */
    ppe_matrix_t p = { { {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {px, py, 1.0f}
        }
    };
    /* Multiply with current matrix. */
    ppe_mat_multiply(matrix, &p);
}

static int swap(float *a, float *b)
{
    float temp;
    if (a == NULL || b == NULL)
    {
        return -1;
    }
    temp = *a;
    *a = *b;
    *b = temp;
    return 0;
}

int ppe_get_transform_matrix(ppe_point4_t src, ppe_point4_t dst,
                             ppe_matrix_t *mat)
{
    float a[8][8], b[9], A[64];
    int i, j, k, m = 8, n = 1;
    int astep = 8, bstep = 1;
    float d;

    if (src == NULL || dst == NULL || mat == NULL)
    {
        return  -1;
    }

    for (i = 0; i < 4; ++i)
    {
        a[i][0] = a[i + 4][3] = src[i].x;
        a[i][1] = a[i + 4][4] = src[i].y;
        a[i][2] = a[i + 4][5] = 1;
        a[i][3] = a[i][4] = a[i][5] =
                                a[i + 4][0] = a[i + 4][1] = a[i + 4][2] = 0;
        a[i][6] = -src[i].x * dst[i].x;
        a[i][7] = -src[i].y * dst[i].x;
        a[i + 4][6] = -src[i].x * dst[i].y;
        a[i + 4][7] = -src[i].y * dst[i].y;
        b[i] = dst[i].x;
        b[i + 4] = dst[i].y;
    }
    for (i = 0; i < 8; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            A[8 * i + j] = a[i][j];
        }
    }

    for (i = 0; i < m; i++)
    {
        k = i;
        for (j = i + 1; j < m; j++)
            if (ABS(A[j * astep + i]) > ABS(A[k * astep + i]))
            {
                k = j;
            }
        if (ABS(A[k * astep + i]) < EPS)
        {
            return -1;
        }
        if (k != i)
        {
            for (j = i; j < m; j++)
            {
                swap(&A[i * astep + j], &A[k * astep + j]);
            }
            for (j = 0; j < n; j++)
            {
                swap(&b[i * bstep + j], &b[k * bstep + j]);
            }
        }
        d = -1 / A[i * astep + i];
        for (j = i + 1; j < m; j++)
        {
            float alpha = A[j * astep + i] * d;
            for (k = i + 1; k < m; k++)
            {
                A[j * astep + k] += alpha * A[i * astep + k];
            }
            for (k = 0; k < n; k++)
            {
                b[j * bstep + k] += alpha * b[i * bstep + k];
            }
        }
    }

    for (i = m - 1; i >= 0; i--)
        for (j = 0; j < n; j++)
        {
            float s = b[i * bstep + j];
            for (k = i + 1; k < m; k++)
            {
                s -= A[i * astep + k] * b[k * bstep + j];
            }
            b[i * bstep + j] = s / A[i * astep + i];
        }

    b[8] = 1;

    for (i = 0; i < 3; ++i)
    {
        for (j = 0; j < 3; ++j)
        {
            mat->m[i][j] = b[i * 3 + j];
        }
    }
    return 0;
}

void ppe_matrix_inverse(ppe_matrix_t *matrix)
{
    float m00, m01, m02, m10, m11, m12, m20, m21, m22;
    m00 = matrix->m[0][0];
    m01 = matrix->m[0][1];
    m02 = matrix->m[0][2];

    m10 = matrix->m[1][0];
    m11 = matrix->m[1][1];
    m12 = matrix->m[1][2];

    m20 = matrix->m[2][0];
    m21 = matrix->m[2][1];
    m22 = matrix->m[2][2];

    float detal = m00 * m11 * m22 + \
                  m01 * m12 * m20 + \
                  m02 * m10 * m21 - \
                  m00 * m12 * m21 - \
                  m01 * m10 * m22 - \
                  m02 * m11 * m20;

    matrix->m[0][0] = (m11 * m22 - m12 * m21) / detal;
    matrix->m[1][0] = -(m22 * m10 - m12 * m20) / detal;
    matrix->m[2][0] = (m10 * m21 - m20 * m11) / detal;

    matrix->m[0][1] = -(m01 * m22 - m02 * m21) / detal;
    matrix->m[1][1] = (m22 * m00 - m20 * m02) / detal;
    matrix->m[2][1] = -(m00 * m21 - m20 * m01) / detal;

    matrix->m[0][2] = (m01 * m12 - m11 * m02) / detal;
    matrix->m[1][2] = -(m00 * m12 - m02 * m10) / detal;
    matrix->m[2][2] = (m00 * m11 - m01 * m10) / detal;
}
#endif

bool check_inverse(ppe_matrix_t *matrix)
{
    return true;
}

bool inv_matrix2complement(ppe_matrix_t *matrix, uint32_t *comp)
{
    float *tf = (float *)matrix->m;
    int32_t tr[9];
    for (int i = 0; i < 9; i++)
    {
        tr[i] = tf[i] * 65536;
    }
    memcpy(comp, tr, 9 * sizeof(int32_t));
    return true;
}

static void pos_transfer(ppe_matrix_t *matrix, ppe_pox_t *pox)
{
    float m_row0, m_row1, m_row2;

    float a = pox->p[0];
    float b = pox->p[1];
    float c = pox->p[2];

    /* Process all rows. */
    m_row0 = matrix->m[0][0];
    m_row1 = matrix->m[0][1];
    m_row2 = matrix->m[0][2];
    pox->p[0] = (m_row0 * a) + (m_row1 * b) + (m_row2 * c);

    m_row0 = matrix->m[1][0];
    m_row1 = matrix->m[1][1];
    m_row2 = matrix->m[1][2];
    pox->p[1] = (m_row0 * a) + (m_row1 * b) + (m_row2 * c);

    m_row0 = matrix->m[2][0];
    m_row1 = matrix->m[2][1];
    m_row2 = matrix->m[2][2];
    pox->p[2] = (m_row0 * a) + (m_row1 * b) + (m_row2 * c);

    pox->p[0] = pox->p[0] / pox->p[2];
    pox->p[1] = pox->p[1] / pox->p[2];
    pox->p[2] = 1;
}

bool ppe_get_block_size(ppe_matrix_t *matrix, uint8_t pixel_size, uint16_t* block_width, uint16_t* block_height)
{
    float bw = 0, bh = 0;
    switch (pixel_size)
    {
        case 1:
            bw = 352;
            bh = 88;
        break;
        
        case 2:
            bw = 256;
            bh = 64;
        break;
        
        case 4:
            bw = 176;
            bh = 44;
        break;
        
        case 8:
            bw = 128;
            bh = 32;
        break;
        
        case 16:
            bw = 16;
            bh = 8;
        break;
        
        case 24:
        case 32:
            bw = 16;
            bh = 16;
        break;
        
        default:
            assert_param(NULL != NULL);
        break;
    }
    ppe_pox_t pox = {0.0f};
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;

    pox.p[0] = 0;
    pox.p[1] = 0;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    x_min = pox.p[0];
    x_max = pox.p[0];
    y_min = pox.p[1];
    y_max = pox.p[1];

    pox.p[0] = bw - 1;
    pox.p[1] = 0;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    pox.p[0] = 0;
    pox.p[1] = (bh - 1) * 1.0f;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    pox.p[0] = (bw - 1) * 1.0f;
    pox.p[1] = (bh - 1) * 1.0f;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    uint32_t w = 0, h = 0;
    w = floor(x_max) - ceil(x_min) + 1;
    h = floor(y_max) - ceil(y_min) + 1;

    if (w == 0 || h == 0)
    {
        return false;
    }
    *block_width = w;
    *block_height = h;
    return true;
}

bool ppe_get_area(ppe_rect_t *result_rect, ppe_rect_t *source_rect, ppe_matrix_t *matrix,
                  ppe_buffer_t *buffer)
{
    ppe_pox_t pox = {0.0f};
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;

    pox.p[0] = source_rect->x * 1.0f;
    pox.p[1] = source_rect->y * 1.0f;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    x_min = pox.p[0];
    x_max = pox.p[0];
    y_min = pox.p[1];
    y_max = pox.p[1];

    pox.p[0] = (source_rect->x + source_rect->w - 1) * 1.0f;
    pox.p[1] = source_rect->y * 1.0f;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    pox.p[0] = source_rect->x * 1.0f;
    pox.p[1] = (source_rect->y + source_rect->h - 1) * 1.0f;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    pox.p[0] = (source_rect->x + source_rect->w - 1) * 1.0f;
    pox.p[1] = (source_rect->y + source_rect->h - 1) * 1.0f;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    if (x_max <= -1 || x_min >= buffer->width * 1.0f || y_max <= -1 || y_min >= buffer->height * 1.0f)
    {
        result_rect->x = 0;
        result_rect->y = 0;
        result_rect->w = 0;
        result_rect->h = 0;
        return false;
    }
    if (x_min < 0)
    {
        x_min = 0;
    }
    else if (x_min > buffer->width - 1)
    {
        x_min = buffer->width - 1;
    }
    if (y_min < 0)
    {
        y_min = 0;
    }
    else if (y_min >= buffer->height)
    {
        y_min = buffer->height - 1;
    }
    result_rect->x = (int16_t)x_min;
    result_rect->y = (int16_t)y_min;

    if (x_max >= buffer->width)
    {
        x_max = buffer->width - 1;
    }
    else if (x_max < 0)
    {
        x_max = 0;
    }

    if (y_max >= buffer->height)
    {
        y_max = buffer->height - 1;
    }
    else if (y_max < 0)
    {
        y_max = 0;
    }

    result_rect->w = ceil(x_max) - result_rect->x + 1;
    result_rect->h = ceil(y_max) - result_rect->y + 1;

    if (result_rect->x + result_rect->w > buffer->width)
    {
        result_rect->w = buffer->width - result_rect->x;
    }
    if (result_rect->y + result_rect->h > buffer->height)
    {
        result_rect->h = buffer->height - result_rect->y;
    }
    if (isnan(y_min) || isnan(y_max) || isnan(x_min) || isnan(x_max) || result_rect->h == 0 ||
        result_rect->w == 0)
    {
        return false;
    }
    return true;
}
/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/
