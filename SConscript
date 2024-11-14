# for module compiling
Import('RTK_SDK_ROOT')
Import('RTK_IC_TYPE')
import os
from building import *
import menu_config

# get current directory
cwd  = GetCurrentDir()
objs = []
list = os.listdir(cwd)
parent_dir = os.path.dirname(cwd)

src = Split("""
""")

include_path = []
libs = ['']

if GetDepend(['CONFIG_REALTEK_LCDC_DBIB']):
    src += ['driver/lcdc/device/src/rtl_common/rtl_lcdc_dbib.c']
if GetDepend(['CONFIG_REALTEK_LCDC_DBIC']):
    src += ['driver/lcdc/device/src/rtl_common/rtl_lcdc_dbic.c']
if GetDepend(['CONFIG_REALTEK_LCDC_DSI']):
    src += ['driver/mipi/device/src/rtl_common/rtl_lcdc_dsi.c']
if GetDepend(['CONFIG_REALTEK_LCDC_EDPI']) :
    src += ['driver/lcdc/device/src/rtl_common/rtl_lcdc_edpi.c']
if GetDepend(['CONFIG_REALTEK_LCDC']):
    src += ['driver/lcdc/device/src/rtl_common/rtl_lcdc.c']
if GetDepend(['CONFIG_REALTEK_PPE']):
    src += ['driver/ppe/device/src/rtl87x2g/rtl_ppe.c']
if GetDepend(['CONFIG_REALTEK_RAMLESS_QSPI']):
    src += ['driver/lcdc/device/src/rtl_common/rtl_ramless_qspi.c']

if  GetDepend(['CONFIG_REALTEK_IMDC']) or GetDepend(['CONFIG_REALTEK_IDU']) :
    src += ['driver/idu/device/src/rtl_common/rtl_imdc.c']
    src += ['driver/idu/device/src/rtl_common/hal_imdc.c']
    if RTK_IC_TYPE == 'rtl87x2g':
        src += ['driver/idu/device/src/rtl87x2g/hal_imdc_int.c']
        src += ['driver/idu/device/src/rtl87x2g/rtl_imdc_int.c']

if GetDepend(['CONFIG_REALTEK_SEGCOM']):
    src += ['driver/segcom/device/src/rtl_common/rtl876x_segcom.c']


include_path += [cwd,
        cwd + '/driver/lcdc/device/inc',
        cwd + '/driver/lcdc/device/src/' + RTK_IC_TYPE,
        cwd + '/driver/idu/device/inc',
        cwd + '/driver/idu/device/src/' + RTK_IC_TYPE,
        cwd + '/driver/mipi/device/inc',
        cwd + '/driver/ppe/device/inc/' + RTK_IC_TYPE,
        cwd + '/driver/segcom/device/inc']


group = DefineGroup('peripheral', src, depend = [''], CPPPATH = include_path,LIBS = [''], LIBPATH = [''])

for d in list:
    path = os.path.join(cwd, d)
    if os.path.isfile(os.path.join(path, 'SConscript')):
        group = group + SConscript(os.path.join(d, 'SConscript'))

Return('group')