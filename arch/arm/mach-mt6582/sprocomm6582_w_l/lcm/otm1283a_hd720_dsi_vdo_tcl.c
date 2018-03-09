/*
 * Module author : Yuvraj
 * For Power Plus
 *
 */

#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
        #include <string.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH		(720)
#define FRAME_HEIGHT		(1280)
#define LCM_ID			0x1283

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

unsigned static int lcm_esd_test = FALSE;

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util ;

#define SET_RESET_PIN(v)    			(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 				(lcm_util.udelay(n))
#define MDELAY(n) 				(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V3(para_tbl,size,force_update)        lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define   LCM_DSI_CMD_MODE						0

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};



static LCM_setting_table lcm_initialization_setting[] = {
    { 0x15, 0x00, 0x01, {0x00}},
    { 0x39, 0xFF, 0x03, {0x12, 0x83, 0x01}},
    { 0x15, 0x00, 0x01, {0x80}},
    { 0x39, 0xFF, 0x02, {0x12, 0x83}},
    { 0x15, 0x00, 0x01, {0xA2}},
    { 0x15, 0xC1, 0x01, {0x08}},
    { 0x15, 0x00, 0x01, {0xA4}},
    { 0x15, 0xC1, 0x01, {0xF0}},
    { 0x15, 0x00, 0x01, {0x80}},
    { 0x15, 0xC4, 0x01, {0x30}},
    { 0x15, 0x00, 0x01, {0x8A}},
    { 0x15, 0xC4, 0x01, {0x40}},
    { 0x15, 0x00, 0x01, {0x80}},
    { 0x39, 0xC0, 0x09, {0x00, 0x64, 0x00, 0x0F, 0x11, 0x00, 0x64, 0x0F, 0x11}},
    { 0x15, 0x00, 0x01, {0x90}},
    { 0x39, 0xC0, 0x06, {0x00, 0x55, 0x00, 0x01, 0x00, 0x04}},
    { 0x15, 0x00, 0x01, {0xA4}},
    { 0x15, 0xC0, 0x01, {0x00}},
    { 0x15, 0x00, 0x01, {0xB3}},
    { 0x39, 0xC0, 0x02, {0x00, 0x50}},
    { 0x15, 0x00, 0x01, {0x81}},
    { 0x15, 0xC1, 0x01, {0x66}},
    { 0x15, 0x00, 0x01, {0x80}},
    { 0x15, 0xC4, 0x01, {0x30}},
    { 0x15, 0x00, 0x01, {0x81}},
    { 0x39, 0xC4, 0x02, {0x83, 0x02}},
    { 0x15, 0x00, 0x01, {0x90}},
    { 0x15, 0xC4, 0x01, {0x49}},
    { 0x15, 0x00, 0x01, {0xB9}},
    { 0x15, 0xB0, 0x01, {0x51}},
    { 0x15, 0x00, 0x01, {0xC6}},
    { 0x15, 0xB0, 0x01, {0x03}},
    { 0x15, 0x00, 0x01, {0xA4}},
    { 0x15, 0xC0, 0x01, {0x00}},
    { 0x15, 0x00, 0x01, {0x87}},
    { 0x15, 0xC4, 0x01, {0x18}},
    { 0x15, 0x00, 0x01, {0xB0}},
    { 0x15, 0xC6, 0x01, {0x03}},
    { 0x15, 0x00, 0x01, {0x90}},
    { 0x39, 0xF5, 0x04, {0x02, 0x11, 0x02, 0x11}},
    { 0x15, 0x00, 0x01, {0x90}},
    { 0x15, 0xC5, 0x01, {0x50}},
    { 0x15, 0x00, 0x01, {0x94}},
    { 0x15, 0xC5, 0x01, {0x66}},
    { 0x15, 0x00, 0x01, {0xB2}},
    { 0x39, 0xF5, 0x08, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    { 0x15, 0x00, 0x01, {0x94}},
    { 0x15, 0xF5, 0x01, {0x02}},
    { 0x15, 0x00, 0x01, {0xBA}},
    { 0x15, 0xF5, 0x01, {0x03}},
    { 0x15, 0x00, 0x01, {0xB4}},
    { 0x15, 0xC5, 0x01, {0xC0}},
    { 0x15, 0x00, 0x01, {0xA0}},
    { 0x39, 0xC4, 0x0E, {0x05, 0x10, 0x04, 0x02, 0x05, 0x15, 0x11, 0x05, 0x10, 0x07, 0x02, 0x05, 0x15, 0x11}},
    { 0x15, 0x00, 0x01, {0xB0}},
    { 0x39, 0xC4, 0x02, {0x66, 0x66}},
    { 0x15, 0x00, 0x01, {0x91}},
    { 0x39, 0xC5, 0x02, {0x19, 0x50}},
    { 0x15, 0x00, 0x01, {0xB0}},
    { 0x39, 0xC5, 0x02, {0x04, 0xB8}},
    { 0x15, 0x00, 0x01, {0xB5}},
    { 0x39, 0xC5, 0x08, {0x03, 0xE8, 0x40, 0x03, 0xE8, 0x40, 0x80, 0x00}},
    { 0x15, 0x00, 0x01, {0x80}},
    { 0x39, 0xCB, 0x0B, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    { 0x15, 0x00, 0x01, {0x90}},
    { 0x39, 0xCB, 0x0F, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00}},
    { 0x15, 0x00, 0x01, {0xA0}},
    { 0x39, 0xCB, 0x0F, {0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00}},
    { 0x15, 0x00, 0x01, {0xB0}},
    { 0x39, 0xCB, 0x0F, {0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00}},
    { 0x15, 0x00, 0x01, {0xC0}},
    { 0x39, 0xCB, 0x0F, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x05}},
    { 0x15, 0x00, 0x01, {0xD0}},
    { 0x39, 0xCB, 0x0F, {0x05, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x05}},
    { 0x15, 0x00, 0x01, {0xE0}},
    { 0x39, 0xCB, 0x0E, {0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x05, 0x05, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00}},
    { 0x15, 0x00, 0x01, {0xF0}},
    { 0x39, 0xCB, 0x0B, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    { 0x15, 0x00, 0x01, {0x80}},
    { 0x39, 0xCC, 0x0F, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x10, 0x0E}},
    { 0x15, 0x00, 0x01, {0x90}},
    { 0x39, 0xCC, 0x0F, {0x0C, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    { 0x15, 0x00, 0x01, {0xA0}},
    { 0x39, 0xCC, 0x0E, {0x00, 0x00, 0x00, 0x09, 0x00, 0x0F, 0x0D, 0x0B, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00}},
    { 0x15, 0x00, 0x01, {0xB0}},
    { 0x39, 0xCC, 0x0F, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x10, 0x0E}},
    { 0x15, 0x00, 0x01, {0xC0}},
    { 0x39, 0xCC, 0x0F, {0x0C, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    { 0x15, 0x00, 0x01, {0xD0}},
    { 0x39, 0xCC, 0x0E, {0x00, 0x00, 0x00, 0x09, 0x00, 0x0F, 0x0D, 0x0B, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00}},
    { 0x15, 0x00, 0x01, {0x80}},
    { 0x39, 0xCE, 0x0C, {0x87, 0x03, 0x06, 0x86, 0x03, 0x06, 0x85, 0x03, 0x06, 0x84, 0x03, 0x06}},
    { 0x15, 0x00, 0x01, {0x90}},
    { 0x39, 0xCE, 0x0E, {0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00}},
    { 0x15, 0x00, 0x01, {0xA0}},
    { 0x39, 0xCE, 0x0E, {0x38, 0x05, 0x84, 0xFE, 0x00, 0x06, 0x00, 0x38, 0x04, 0x84, 0xFF, 0x00, 0x06, 0x00}},
    { 0x15, 0x00, 0x01, {0xB0}},
    { 0x39, 0xCE, 0x0E, {0x38, 0x03, 0x85, 0x00, 0x00, 0x06, 0x00, 0x38, 0x02, 0x85, 0x01, 0x00, 0x06, 0x00}},
    { 0x15, 0x00, 0x01, {0xC0}},
    { 0x39, 0xCE, 0x0E, {0x38, 0x01, 0x85, 0x02, 0x00, 0x06, 0x00, 0x38, 0x00, 0x85, 0x03, 0x00, 0x06, 0x00}},
    { 0x15, 0x00, 0x01, {0xD0}},
    { 0x39, 0xCE, 0x0E, {0x30, 0x00, 0x85, 0x04, 0x00, 0x06, 0x00, 0x30, 0x01, 0x85, 0x05, 0x00, 0x06, 0x00}},
    { 0x15, 0x00, 0x01, {0x80}},
    { 0x39, 0xCF, 0x0E, {0x70, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00}},
    { 0x15, 0x00, 0x01, {0x90}},
    { 0x39, 0xCF, 0x0E, {0x70, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00}},
    { 0x15, 0x00, 0x01, {0xA0}},
    { 0x39, 0xCF, 0x0E, {0x70, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00}},
    { 0x15, 0x00, 0x01, {0xB0}},
    { 0x39, 0xCF, 0x0E, {0x70, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00}},
    { 0x15, 0x00, 0x01, {0xC0}},
    { 0x39, 0xCF, 0x0B, {0x01, 0x01, 0x20, 0x20, 0x00, 0x00, 0x01, 0x80, 0x00, 0x03, 0x08}},
    { 0x15, 0x00, 0x01, {0x00}},
    { 0x39, 0xD8, 0x02, {0xBC, 0xBC}},
    { 0x15, 0x00, 0x01, {0x00}},
    { 0x15, 0xD9, 0x01, {0xB6}},
    { 0x15, 0x00, 0x01, {0x00}},
    { 0x39, 0xE1, 0x10, {0x04, 0x09, 0x0E, 0x0C, 0x04, 0x0E, 0x08, 0x08, 0x05, 0x08, 0x0D, 0x06, 0x0A, 0x13, 0x10, 0x0A}},
    { 0x15, 0x00, 0x01, {0x00}},
    { 0x39, 0xE2, 0x10, {0x04, 0x09, 0x0E, 0x0B, 0x04, 0x0E, 0x08, 0x08, 0x05, 0x08, 0x0D, 0x06, 0x0A, 0x13, 0x10, 0x0A}},
    { 0x15, 0x00, 0x01, {0x00}},
    { 0x39, 0xFF, 0x03, {0xFF, 0xFF, 0xFF}},
    { 0x15, 0x35, 0x01, {0x00}},
    { 0x15, 0x00, 0x01, {0x00}},
    { 0x15, 0x29, 0x01, {0x00}},
    { 0x05, 0x35, 0x00, {}},
    { 0x05, 0x11, 0x00, {}},
    { 0x00, 0xFF, 0x78, {0x00}},
    { 0x05, 0x29, 0x00, {}},
    { 0x00, 0xFF, 0x14, {0x00}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

	for(i = 0; i < count; i++) {

		unsigned cmd;
		cmd = table[i].cmd;

	switch (cmd) {

		case REGFLAG_DELAY :
 			MDELAY(table[i].count);
   			break;

		case REGFLAG_END_OF_TABLE :
   			break;

		default:
			dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
			MDELAY(2);
		}
	}

}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS * util)
{
  memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS * params)
{
	        memset(params, 0, sizeof(LCM_PARAMS));

	        params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif

		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_FOUR_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting
		params->dsi.intermediat_buffer_num = 2;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.vertical_sync_active				= 3;
		params->dsi.vertical_backporch					= 14;
		params->dsi.vertical_frontporch					= 16;
		params->dsi.vertical_active_line				= FRAME_HEIGHT;

		params->dsi.horizontal_sync_active				= 4;
		params->dsi.horizontal_backporch				= 34;
		params->dsi.horizontal_frontporch				= 24;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		params->dsi.PLL_CLOCK=210;

}

static void lcm_init(void)
{
  SET_RESET_PIN(0);
  MDELAY(20);
  SET_RESET_PIN(1);
  MDELAY(20);

//  dsi_set_cmdq_V3(0xC0B50B84, 130, 1);// 0xC0B50B84 -> is lcm_table
  dsi_set_cmdq_V3(lcm_initialization_setting, sizeof(lcm_initialization_setting)/sizeof(lcm_initialization_setting[0]), 1);
}

static void lcm_suspend(void)
{
  unsigned int data_array[16];

  data_array[0] = 0x00280500;
  dsi_set_cmdq(data_array, 1, 1);
  data_array[0] = 0x00100500;
  dsi_set_cmdq(data_array, 1, 1);
}

static void lcm_resume(void)
{
  unsigned int data_array[16];

  data_array[0] = 0x00290500;
  dsi_set_cmdq(data_array, 1, 1);
  data_array[0] = 0x00110500;
  dsi_set_cmdq(data_array, 1, 1);
}

static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);

}

static unsigned int lcm_compare_id(void)
{
  return 1;
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------

LCM_DRIVER otm1283a_hd720_dsi_vdo_tcl_lcm_drv =
{
        .name		= "otm1283a_hd720_dsi_vdo_tcl",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
	.update         = lcm_update,
#endif
};
