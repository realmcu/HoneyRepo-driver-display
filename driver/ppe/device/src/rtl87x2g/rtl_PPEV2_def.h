/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl_ppev2_def.h
* \brief    PPEV2 related definitions for RTL8762G
* \details
* \author
* \date     2023-11-15
* \version  v1.1
* *********************************************************************************************************
*/

#ifndef RTL_PPEV2_DEF_H
#define RTL_PPEV2_DEF_H

#include "utils/rtl_utils.h"
#include "address_map.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/*============================================================================*
 *                         PPEV2 Registers Memory Map
 *============================================================================*/

/*============================================================================*
 *                         PPEV2 Registers and Field Descriptions
 *============================================================================*/
/* Peripheral:PPEV2*/
/* Description:PPEV2register defines */

/* Register: PPEV2_REG_GLB_STATUS -----------------------------------------------------*/
/* Description: . Offset: 0x0000. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t run_state:     2;
        const  uint32_t reserved:     13;
        const  uint32_t chn_inact:     17;
    } b;
} PPEV2_REG_GLB_STATUS_TypeDef;

/* Register: PPEV2_REG_LYR_ENABLE -----------------------------------------------------*/
/* Description: . Offset: 0x0004. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const  uint32_t result_lyr_en:     1;
        uint32_t input_lyr_en:     15;
        const  uint32_t reserved:     16;
    } b;
} PPEV2_REG_LYR_ENABLE_TypeDef;

/* Register: PPEV2_REG_LD_CFG -----------------------------------------------------*/
/* Description: . Offset: 0x0008. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t reload_en:     16;
        uint32_t set_valid:     1;
        uint32_t auto_clr:     1;
        const  uint32_t reserved:     14;
    } b;
} PPEV2_REG_LD_CFG_TypeDef;

/* Register: PPEV2_REG_LL_CFG -----------------------------------------------------*/
/* Description: . Offset: 0x000C. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t ll_en:     16;
        const  uint32_t ll_arsize:     3;
        uint32_t ll_incr:     1;
        uint32_t ll_arcache:     4;
        uint32_t ll_max_arlen_log:     3;
        const  uint32_t reserved:     1;
        uint32_t ll_prior:     4;
    } b;
} PPEV2_REG_LL_CFG_TypeDef;

/* Register: PPEV2_REG_LLP -----------------------------------------------------*/
/* Description: . Offset: 0x0010. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const  uint32_t reserved:     2;
        uint32_t llp:     30;
    } b;
} PPEV2_REG_LLP_TypeDef;

/* Register: PPEV2_REG_SECURE -----------------------------------------------------*/
/* Description: . Offset: 0x0018. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t secure:     1;
        const  uint32_t rsvd:     31;
    } b;
} PPEV2_REG_SECURE_TypeDef;

/* Register: PPEV2_REG_LINE_OVER_INDEX -----------------------------------------------------*/
/* Description: . Offset: 0x001C. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t line_over_int_index:     16;
        const  uint32_t result_lyr_current_line_index:     16;
    } b;
} PPEV2_REG_LINE_OVER_INDEX_TypeDef;

/* Register: PPEV2_REG_RELEASE_DATE -----------------------------------------------------*/
/* Description: . Offset: 0x0020. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const  uint32_t release_date:     32;
    } b;
} PPEV2_REG_RELEASE_DATE_TypeDef;

/* Register: PPEV2_REG_RTL_VER -----------------------------------------------------*/
/* Description: . Offset: 0x0024. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const  uint32_t rtl_ver:     32;
    } b;
} PPEV2_REG_RTL_VER_TypeDef;

/* Register: PPEV2_REG_INTR_STATUS -----------------------------------------------------*/
/* Description: . Offset: 0x0040. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const  uint32_t all_over:     1;
        const  uint32_t fr_over:     1;
        const  uint32_t load_over:     1;
        const  uint32_t line_over:     1;
        const  uint32_t susp_inac:     1;
        const  uint32_t secure_err:     1;
        const  uint32_t reserved:     1;
        const  uint32_t bus_err:     1;
        const  uint32_t div0_err:     1;
        const  uint32_t rsvd:     23;
    } b;
} PPEV2_REG_INTR_STATUS_TypeDef;

/* Register: PPEV2_REG_INTR_RAW -----------------------------------------------------*/
/* Description: . Offset: 0x0044. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const  uint32_t all_over:     1;
        const  uint32_t fr_over:     1;
        const  uint32_t load_over:     1;
        const  uint32_t line_over:     1;
        const  uint32_t susp_inac:     1;
        const  uint32_t secure_err:     1;
        const  uint32_t reserved:     1;
        const  uint32_t bus_err:     1;
        const  uint32_t div0_err:     1;
        const  uint32_t rsvd:     23;
    } b;
} PPEV2_REG_INTR_RAW_TypeDef;

/* Register: PPEV2_REG_INTR_CLR -----------------------------------------------------*/
/* Description: . Offset: 0x0048. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t all_over:     1;
        uint32_t fr_over:     1;
        uint32_t load_over:     1;
        uint32_t line_over:     1;
        uint32_t susp_inac:     1;
        uint32_t secure_err:     1;
        const  uint32_t reserved:     1;
        uint32_t bus_err:     1;
        uint32_t div0_err:     1;
        const  uint32_t rsvd:     23;
    } b;
} PPEV2_REG_INTR_CLR_TypeDef;

/* Register: PPEV2_REG_INTR_MASK -----------------------------------------------------*/
/* Description: . Offset: 0x004C. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t all_over:     1;
        uint32_t fr_over:     1;
        uint32_t load_over:     1;
        uint32_t line_over:     1;
        uint32_t susp_inac:     1;
        uint32_t secure_err:     1;
        const  uint32_t reserved:     1;
        uint32_t bus_err:     1;
        uint32_t div0_err:     1;
        const  uint32_t rsvd:     23;
    } b;
} PPEV2_REG_INTR_MASK_TypeDef;

/* Register: PPEV2_REG_BUS_ERR_DETAIL -----------------------------------------------------*/
/* Description: . Offset: 0x0050. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const  uint32_t bus_err_detail:     17;
        const  uint32_t rsvd:     15;
    } b;
} PPEV2_REG_BUS_ERR_DETAIL_TypeDef;

/* Register: PPEV2_REG_DIV0_ERR_DETAIL -----------------------------------------------------*/
/* Description: . Offset: 0x0054. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        const  uint32_t rsvd:     1;
        const  uint32_t div0_err_detail:     15;
        const  uint32_t rsvd1:     16;
    } b;
} PPEV2_REG_DIV0_ERR_DETAIL_TypeDef;

/* Register: PPEV2_REG_LYR0_ADDR -----------------------------------------------------*/
/* Description: . Offset: 0x0060. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t  d8[4];
    struct
    {
        uint32_t addr:     32;
    } b;
} PPEV2_REG_LYR0_ADDR_TypeDef;

/* Register: PPEV2_REG_CANVAS_SIZE -----------------------------------------------------*/
/* Description: . Offset: 0x0068. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t canvas_width:     16;
        uint32_t canvas_height:     16;
    } b;
} PPEV2_REG_CANVAS_SIZE_TypeDef;

/* Register: PPEV2_REG_LYR0_PIC_CFG -----------------------------------------------------*/
/* Description: . Offset: 0x006C. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t format:     8;
        uint32_t background_blend: 1;
        const  uint32_t rsvd:     7;
        uint32_t line_length:     16;
    } b;
} PPEV2_REG_LYR0_PIC_CFG_TypeDef;

/* Register: PPEV2_REG_BACKGROUND -----------------------------------------------------*/
/* Description: . Offset: 0x0070. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t background:     32;
    } b;
} PPEV2_REG_BACKGROUND_TypeDef;

/* Register: PPEV2_REG_LYR0_BUS_CFG -----------------------------------------------------*/
/* Description: . Offset: 0x0074. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t awsize:     3;
        uint32_t incr:     1;
        uint32_t awcache:     4;
        uint32_t max_awlen_log:     3;
        const  uint32_t rsvd:     1;
        uint32_t awqos:     4;
        const  uint32_t rsvd1:     16;
    } b;
} PPEV2_REG_LYR0_BUS_CFG_TypeDef;

/* Register: PPEV2_REG_LYR0_HS_CFG -----------------------------------------------------*/
/* Description: . Offset: 0x0078. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t hs_en:     1;
        const        uint32_t hw_hs:     1;
        uint32_t polar:     1;
        const        uint32_t rsvd:     1;
        uint32_t msize_log:     4;
        uint32_t hw_index:     8;
        const        uint32_t rsvd1:     16;
    } b;
} PPEV2_REG_LYR0_HS_CFG_TypeDef;

/* Register: PPEV2_REG_LYRx_ADDR -----------------------------------------------------*/
/* Description: . Offset: 0x0000 +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t addr:     32;
    } b;
} PPEV2_REG_LYRx_ADDR_TypeDef;

/* Register: PPEV2_REG_LYRx_PIC_SIZE -----------------------------------------------------*/
/* Description: . Offset: 0x0008 +  x *0x0080. Address: . */

typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t pic_width:     16;
        uint32_t pic_height:     16;
    } b;
} PPEV2_REG_LYRx_PIC_SIZE_TypeDef;

/* Register: PPEV2_REG_LYRx_PIC_CFG -----------------------------------------------------*/
/* Description: . Offset: 0x000C +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t format:     8;
        uint32_t pic_src:     1;
        const  uint32_t rsvd3:     1;
        uint32_t input_lyr_read_matrix_size:     1;
        const  uint32_t rsvd2:     1;
        uint32_t color_key_mode:     2;
        const  uint32_t rsvd1:     2;
        uint32_t line_length:     16;
    } b;
} PPEV2_REG_LYRx_PIC_CFG_TypeDef;

/* Register: PPEV2_REG_LYRx_FIXED_COLOR -----------------------------------------------------*/
/* Description: . Offset: 0x0010 +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t const_pixel:     32;
    } b;
} PPEV2_REG_LYRx_FIXED_COLOR_TypeDef;

/* Register: PPEV2_REG_LYRx_BUS_CFG -----------------------------------------------------*/
/* Description: . Offset: 0x0014 +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t arsize:     3;
        uint32_t incr:     1;
        uint32_t arcache:     4;
        uint32_t max_arlen_log:     3;
        const  uint32_t rsvd:     1;
        uint32_t arqos:     4;
        const  uint32_t rsvd1:     16;
    } b;
} PPEV2_REG_LYRx_BUS_CFG_TypeDef;

/* Register: PPEV2_REG_LYRx_HS_CFG -----------------------------------------------------*/
/* Description: . Offset: 0x0018 +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t hs_en:     1;
        uint32_t hw_hs:     1;
        uint32_t polar:     1;
        const  uint32_t rsvd:     1;
        uint32_t msize_log:     4;
        uint32_t hw_index:     8;
        const  uint32_t rsvd1:     16;
    } b;
} PPEV2_REG_LYRx_HS_CFG_TypeDef;

/* Register: PPEV2_REG_LYRx_WIN_MIN -----------------------------------------------------*/
/* Description: . Offset: 0x001C +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t win_x_min:     16;
        uint32_t win_y_min:     16;
    } b;
} PPEV2_REG_LYRx_WIN_MIN_TypeDef;

/* Register: PPEV2_REG_LYRx_WIN_MAX -----------------------------------------------------*/
/* Description: . Offset: 0x0020 +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t win_x_max:     16;
        uint32_t win_y_max:     16;
    } b;
} PPEV2_REG_LYRx_WIN_MAX_TypeDef;

/* Register: PPEV2_REG_LYRx_KEY_MIN -----------------------------------------------------*/
/* Description: . Offset: 0x0024 +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t color_key_min:     24;
        const  uint32_t rsvd:     8;
    } b;
} PPEV2_REG_LYRx_KEY_MIN_TypeDef;

/* Register: PPEV2_REG_LYRx_KEY_MAX -----------------------------------------------------*/
/* Description: . Offset: 0x0028 +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t color_key_max:     24;
        const  uint32_t rsvd:     8;
    } b;
} PPEV2_REG_LYRx_KEY_MAX_TypeDef;

/* Register: PPEV2_REG_LYRx_TRANS_MATRIX_E11 -----------------------------------------------------*/
/* Description: . Offset: 0x002C +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t matrix_e11:     32;
    } b;
} PPEV2_REG_LYRx_TRANS_MATRIX_E11_TypeDef;

/* Register: PPEV2_REG_LYRx_TRANS_MATRIX_E12 -----------------------------------------------------*/
/* Description: . Offset: 0x0030 +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t matrix_e12:     32;
    } b;
} PPEV2_REG_LYRx_TRANS_MATRIX_E12_TypeDef;

/* Register: PPEV2_REG_LYRx_TRANS_MATRIX_E13 -----------------------------------------------------*/
/* Description: . Offset: 0x0034 +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t matrix_e13:     32;
    } b;
} PPEV2_REG_LYRx_TRANS_MATRIX_E13_TypeDef;

/* Register: PPEV2_REG_LYRx_TRANS_MATRIX_E21 -----------------------------------------------------*/
/* Description: . Offset: 0x0038 +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t matrix_e21:     32;
    } b;
} PPEV2_REG_LYRx_TRANS_MATRIX_E21_TypeDef;

/* Register: PPEV2_REG_LYRx_TRANS_MATRIX_E22 -----------------------------------------------------*/
/* Description: . Offset: 0x003C +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t matrix_e22:     32;
    } b;
} PPEV2_REG_LYRx_TRANS_MATRIX_E22_TypeDef;

/* Register: PPEV2_REG_LYRx_TRANS_MATRIX_E23 -----------------------------------------------------*/
/* Description: . Offset: 0x0040 +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t matrix_e23:     32;
    } b;
} PPEV2_REG_LYRx_TRANS_MATRIX_E23_TypeDef;

/* Register: PPEV2_REG_LYRx_TRANS_MATRIX_E31 -----------------------------------------------------*/
/* Description: . Offset: 0x0044 +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t matrix_e31:     32;
    } b;
} PPEV2_REG_LYRx_TRANS_MATRIX_E31_TypeDef;

/* Register: PPEV2_REG_LYRx_TRANS_MATRIX_E32 -----------------------------------------------------*/
/* Description: . Offset: 0x0048 +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t matrix_e32:     32;
    } b;
} PPEV2_REG_LYRx_TRANS_MATRIX_E32_TypeDef;

/* Register: PPEV2_REG_LYRx_TRANS_MATRIX_E33 -----------------------------------------------------*/
/* Description: . Offset: 0x004C +  x *0x0080. Address: . */
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint32_t matrix_e33:     32;
    } b;
} PPEV2_REG_LYRx_TRANS_MATRIX_E33_TypeDef;

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* RTL_PPEV2_DEF_H */