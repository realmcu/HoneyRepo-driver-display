#ifndef RTL_eDPI_REG_H
#define RTL_eDPI_REG_H

#include <stdint.h>
#include <stdbool.h>
#include "rtl876x.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ================================================================================ */
/* ============= Refence: DC.xlsx =============== */
/* ================================================================================ */
typedef struct
{
    __IO uint32_t  RESERVED1;                               //0X00
    __IO uint32_t  EDPI_SYNC_WIDTH;                         //0X04
    __IO uint32_t  EDPI_ABACK_PORCH;                        //0X08
    __IO uint32_t  EDPI_AACTIVE;                            //0X0C
    __IO uint32_t  EDPI_TOTAL;                              //0X10
    __IO uint32_t  EDPI_SYNC_POL;                           //0X14
    __IO uint32_t  EDPI_INT_MASK;                           //0X18
    __IO uint32_t  EDPI_INT_FLAG;                           //0X1C
    __IO uint32_t  EDPI_INT_CLR;                            //0X20
    __IO uint32_t  EDPI_LINE_INT_POS;                       //0X24
    __IO uint32_t  EDPI_PIXEL_POS;                          //0X28
    __IO uint32_t  EDPI_SYNC_STATUS;                        //0X2C
    __IO uint32_t  EDPI_COLOR_MAP;                          //0X30
    __IO uint32_t  EDPI_OP_MODE;                            //0X34
    __IO uint32_t  EDPI_VIDEO_CTL;                          //0X38
    __IO uint32_t  EDPI_CMD_CTL;                            //0X3C
    __IO uint32_t  EDPI_CMD_MAX_LATENCY;                    //0X40
    __IO uint32_t  DPI_LINNE_BUFFER_PIXEL_NUM;              //0X44
    __IO uint32_t  DPI_LINE_BUFFER_PIXEL_THRESHOLD;         //0X48
    __IO uint32_t  RESERVED2;                               //0X4C
    __IO uint32_t  EDPI_DIV_PAR;                            //0X50
    __IO uint32_t  RESERVED3;                               //0X54
    __IO uint32_t  EDPI_RGB_COMPATIBLE;                     //0X58
} LCDC_EDPI_TypeDef;

/* 0x00
    31:0    R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t reserved_0: 32;
    } b;
} EDPI_RESERVED1_TypeDef;



/* 0x04
    10:0    R/W    vsh                         0x0
    15:11   R      reserved2                   0x0
    27:16   R/W    hsw                         0x0
    31:28   R      reserved1                   0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t vsh: 11;
        __I uint32_t reserved_1: 5;
        __IO uint32_t hsw: 12;
        __I uint32_t reserved_0: 4;
    } b;
} EDPI_SYNC_WIDTH_TypeDef;



/* 0x08
    10:0    R/W    avbp                        0x0
    15:11   R      reserved2                   0x0
    27:16   R/W    ahbp                        0x0
    31:28   R      reserved1                   0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t avbp: 11;
        __I uint32_t reserved_1: 5;
        __IO uint32_t ahbp: 12;
        __I uint32_t reserved_0: 4;
    } b;
} EDPI_ABACK_PORCH_TypeDef;



/* 0x0C
    10:0    R/W    aah                         0x0
    15:11   R      reserved2                   0x0
    27:16   R/W    aaw                         0x0
    31:28   R      reserved1                   0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t aah: 11;
        __I uint32_t reserved_1: 5;
        __IO uint32_t aaw: 12;
        __I uint32_t reserved_0: 4;
    } b;
} EDPI_AACTIVE_TypeDef;



/* 0x10
    10:0    R/W    totalh                      0x0
    15:11   R      reserved2                   0x0
    27:16   R/W    totalw                      0x0
    31:28   R      reserved1                   0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t totalh: 11;
        __I uint32_t reserved_1: 5;
        __IO uint32_t totalw: 12;
        __I uint32_t reserved_0: 4;
    } b;
} EDPI_TOTAL_TypeDef;



/* 0x14
    28:0    R      reserved                    0x0
    29      R/W    depol                       0x1
    30      R/W    vspol                       0x1
    31      R/W    hspol                       0x1
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t reserved_0: 29;
        __IO uint32_t depol: 1;
        __IO uint32_t vspol: 1;
        __IO uint32_t hspol: 1;
    } b;
} EDPI_SYNC_POL_TypeDef;



/* 0x18
    0       R/W    lim                         0x1
    31:1    R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t lim: 1;
        __I uint32_t reserved_0: 31;
    } b;
} EDPI_INT_MASK_TypeDef;



/* 0x1C
    0       R      lif                         0x0
    31:1    R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t lif: 1;
        __I uint32_t reserved_0: 31;
    } b;
} EDPI_INT_FLAG_TypeDef;



/* 0x20
    0       R/W1C  clif                        0x0
    31:1    R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t clif: 1;
        __I uint32_t reserved_0: 31;
    } b;
} EDPI_INT_CLR_TypeDef;



/* 0x24
    10:0    R/W    lipos                       0x0
    31:11   R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t lipos: 11;
        __I uint32_t reserved_0: 21;
    } b;
} EDPI_LINE_INT_POS_TypeDef;



/* 0x28
    15:0    R      cypos                       0x0
    31:16   R      cxpos                       0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t cypos: 16;
        __I uint32_t cxpos: 16;
    } b;
} EDPI_PIXEL_POS_TypeDef;



/* 0x2C
    0       R      vdes                        0x0
    1       R      hdes                        0x0
    2       R      vsyncs                      0x0
    3       R      hsyncs                      0x0
    31:4    R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t vdes: 1;
        __I uint32_t hdes: 1;
        __I uint32_t vsyncs: 1;
        __I uint32_t hsyncs: 1;
        __I uint32_t reserved_0: 28;
    } b;
} EDPI_SYNC_STATUS_TypeDef;



/* 0x30
    1:0     R/W    color_map                   0x0
    31:2    R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t color_map: 2;
        __I uint32_t reserved_0: 30;
    } b;
} EDPI_COLOR_MAP_TypeDef;



/* 0x34
    0       R/W    op_mode                     0x0
    31:1    R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t op_mode: 1;
        __I uint32_t reserved_0: 31;
    } b;
} EDPI_OP_MODE_TypeDef;



/* 0x38
    0       R/W    up_en                       0x0
    1       R/W    clm_en                      0x0
    2       R/W    sd_en                       0x1
    8:3     R      reserved3                   0x0
    9       R/W    sd_pol                      0x1
    15:10   R      reserved2                   0x0
    16      R/W    sd_dis_fcolor               0x0
    17      R/W    sd_en_fcolor                0x0
    30:18   R      reserved1                   0x0
    31      R/W    lcd_arc                     0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t up_en: 1;
        __IO uint32_t clm_en: 1;
        __IO uint32_t sd_en: 1;
        __I uint32_t reserved_2: 6;
        __IO uint32_t sd_pol: 1;
        __I uint32_t reserved_1: 6;
        __IO uint32_t sd_dis_fcolor: 1;
        __IO uint32_t sd_en_fcolor: 1;
        __I uint32_t reserved_0: 13;
        __IO uint32_t lcd_arc: 1;
    } b;
} EDPI_VIDEO_CTL_TypeDef;



/* 0x3C
    0       R      halt                        0x0
    1       R/W    tear_req                    0x0
    31:2    R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t halt: 1;
        __IO uint32_t tear_req: 1;
        __I uint32_t reserved_0: 30;
    } b;
} EDPI_CMD_CTL_TypeDef;



/* 0x40
    23:0    R/W    max_latency                 0x0
    31:24   R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t max_latency: 24;
        __I uint32_t reserved_0: 8;
    } b;
} EDPI_CMD_MAX_LATENCY_TypeDef;



/* 0x44
    15:0    R/W    linne_buffer_pixel_num      0x0
    31:16   R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t linne_buffer_pixel_num: 16;
        __I uint32_t reserved_0: 16;
    } b;
} EDPI_LINNE_BUFFER_PIXEL_NUM_TypeDef;



/* 0x48
    15:0    R/W    line_buffer_pixel_threshold 0x0
    31:16   R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t line_buffer_pixel_threshold: 16;
        __I uint32_t reserved_0: 16;
    } b;
} EDPI_LINE_BUFFER_PIXEL_THRESHOLD_TypeDef;



/* 0x4C
    31:0    R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t reserved_0: 32;
    } b;
} EDPI_RESERVED2_TypeDef;



/* 0x50
    15:0    R/W    edpi_div_par                0x0
    31:16   R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t edpi_div_par: 16;
        __I uint32_t reserved_0: 16;
    } b;
} EDPI_DIV_PAR_TypeDef;



/* 0x54
    31:0    R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t reserved_0: 32;
    } b;
} EDPI_RESERVED3_TypeDef;



/* 0x58
    1:0     R/W    hsync_constant              0x01
    3:2     R/W    vsync_constant              0x01
    5:4     R/W    de_constant                 0x01
    31:6    R      reserved                    0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t hsync_constant: 2;
        __IO uint32_t vsync_constant: 2;
        __IO uint32_t de_constant: 2;
        __I uint32_t reserved_0: 26;
    } b;
} EDPI_RGB_COMPATIBLE_TypeDef;


#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* RTL_eDPI_REG_H */
