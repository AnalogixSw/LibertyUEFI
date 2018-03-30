///******************************************************************************
//The contents of this file are provided under the conditions defined
//by the LICENSE.txt.
//Copyright (c) 2017, Analogix Semiconductor, Inc.
///******************************************************************************

#include "Anx74XXLib.h"
#include "Anx74XXLibReg.h"

//User needs to update these values according the device system
//--begin
UINT32 g_source_capabilities[1] = {
//5V,1.5A,Dual-role power, USB Communicateions Capable,
//data role swap, untruncked support(PD3.0)
   PDO_FIXED(PD_VOLTAGE_5V, PD_CURRENT_1500MA,
   (PDO_TYPE_FIXED | PDO_FIXED_DUAL_ROLE | PDO_FIXED_COMM_CAP |\
   PDO_FIXED_DATA_SWAP | PDO_FIXED_UNCHUNKED_SUPPORTED))
};
UINT32 g_sink_capabilities[4] = {
   /*5V, 3A, Fixed */
   PDO_FIXED(PD_VOLTAGE_5V, PD_CURRENT_3A,
   (PDO_TYPE_FIXED | PDO_FIXED_DUAL_ROLE | PDO_FIXED_COMM_CAP |\
   PDO_FIXED_DATA_SWAP | PDO_FIXED_SUSPEND_HIGHER)),
   /*12V, 3A */
   PDO_FIXED(PD_VOLTAGE_12V, PD_CURRENT_3A,
   	(PDO_TYPE_FIXED | PDO_FIXED_DUAL_ROLE | PDO_FIXED_COMM_CAP |\
   	PDO_FIXED_DATA_SWAP)),
   /*20v,3A */
   PDO_FIXED(PD_VOLTAGE_20V, PD_CURRENT_3A,
   	(PDO_TYPE_FIXED | PDO_FIXED_DUAL_ROLE | PDO_FIXED_COMM_CAP | \
   	PDO_FIXED_DATA_SWAP)),
   PDO_VAR(PD_VOLTAGE_5V, PD_VOLTAGE_20V, PD_CURRENT_3A)
};
UINT8 g_SVID[4] = { 0x00, 0x00, 0x00, 0xff };
//VDO in Discovery Modes response
//DFP_D-capable
UINT8 g_DP_Capabilities[4] = { 0xC6, 0x00, 0x00, 0x00 };
//Identity response
UINT8 g_Indentity[12] = {
	0x00, 0x00, 0x00, 0x90, //0x90,	snk_id_hdr, only host; 0xD0, host/device, PDUSB peripheral */
	0x00, 0x00, 0x00, 0x00,	/*snk_cert*/
	0x00, 0x00, 0x00, 0x00 /*snk_prd*/
};
//--end

VOID efuse_read(UINT8 slave_addr, UINT8 offset, UINT8 len, UINT8 *buf)
{
	//slave_addr is 0x32
	anx74XX_read_data(slave_addr, EFUSE_READ_DATA_0 + offset, buf, len);
}
UINT8 is_calibrated(UINT8 slave_addr)
{
	UINT8 buf[3];
	efuse_read(slave_addr, 0, 3, buf);
	if (!buf[0] || !buf[1] || !buf[2])
		return 0;
	return 1;
}


UINT8 get_chip_rev(UINT8 slave_addr)
{
	UINT8 ver = 0;
	//slave_addr is 0x32
	anx74XX_read_data(slave_addr, HARDWARE_CHIP_REV_OFFSET, &ver, 1);
	return ver;
}

VOID power_on(UINT8 spi_slave_addr, UIN8 tcpc_slave_addr)
{
	UINT8 val = 0;
	INT8 count = 200;
    //Make sure OCM is running
	while (val == 0)
	{
		//slave_addr is 0x32
		anx74XX_read_data(spi_slave_addr, OCM_VERSION, &val, 1);
		count--;
		if (count == 0)
			break;
	}
	UINT8 rev = get_chip_rev(spi_slave_addr);
	//chip may not calibrated, so here need to check calibration
	if (!is_calibrated(spi_slave_addr))
	{
		if (rev ==
			AC_CHIP_REV)
		{
			//27M clock
			val = 0xB7;
			anx74XX_write_data(tcpc_slave_addr, RING_OSC_CTRL, &val, 1);
			val = 0x26;
			//adc
			anx74XX_write_data(tcpc_slave_addr, ADC_CTRL_2, &val, 1);
		}
		else
		{
			//27M clock
			val = 0xCD;
			anx74XX_write_data(tcpc_slave_addr, RING_OSC_CTRL, &val, 1);
			val = 0x0f;
			//adc
			anx74XX_write_data(tcpc_slave_addr, ADC_CTRL_2, &val, 1);
		}
		//5.1K
		val = 0x11;
		anx74XX_write_data(tcpc_slave_addr, ANALOG_CTRL_6, &val, 1)
	}
	//configure CC status mask
	anx74xx_set_irq_unmask(spi_slave_addr, ANX74XX_IRQ_BITMAP_CC_STATUS_CHG_INT);
	//configure RDO, these parameters  need to be set addroding the device's power capability.
	anx74xx_set_power_negotiation_parameter(spi_slave_addr, PD_MAX_VOLTAGE_20V, PD_POWER_60W, PD_POWER_15W);
	//Send initial settings
	anx74xx_set_source_cap(spi_slave_addr, g_source_capabilities, 1);
	anx74xx_set_sink_cap(spi_slave_addr, g_sink_capabilities, 4);
	anx74xx_set_initial_msg_info(spi_slave_addr, g_Indentity, 12, TYPE_DP_SNK_IDENTITY);
	anx74xx_set_initial_msg_info(spi_slave_addr, g_SVID, 4, TYPE_SVID);
	anx74xx_set_initial_msg_info(spi_slave_addr, g_DP_Capabilities, 4, TYPE_DP_CAP);
}

VOID power_down(UINT8 spi_slave_addr, UIN8 tcpc_slave_addr)
{
	UINT8 val = 0;
	//ocm reset
	//slave_addr is 0x32
	anx74XX_read_data(spi_slave_addr, OCM_CTRL_0, &val, 1);
	val |= OCM_RESET;
	anx74XX_write_data(spi_slave_addr, OCM_CTRL_0, &val, 1);
	//enable drp toggle 
	val = 0x80;
	anx74XX_write_data(tcpc_slave_addr, ANALOG_CTRL_10, &val, 1);
	//CC with  RD
	val = 0x4A;
	anx74XX_write_data(tcpc_slave_addr, TCPC_ROLE_CONTROL, &val, 1);
	UINT8 rev = get_chip_rev(spi_slave_addr);
	if (rev == AA_CHIP_REV)
	{
		anx74XX_read_data(tcpc_slave_addr, ANALOG_CTRL_1, &val, 1);
		val = val |  TOGGLE_CTRL_MODE |R_TOGGLE_ENABLE;
		anx74XX_write_data(tcpc_slave_addr, ANALOG_CTRL_1, &val, 1);
		anx74XX_read_data(tcpc_slave_addr, ANALOG_CTRL_1, &val, 1);
		val = val | R_LATCH_TOGGLE_ENABLE;
		anx74XX_write_data(tcpc_slave_addr, ANALOG_CTRL_1, &val, 1);
	}
	else
	{
		if (rev == AC_CHIP_REV)
		{
			//DRP en
			val = 0x99;
			anx74XX_write_data(tcpc_slave_addr, TCPC_COMMAND_ADDR, &val, 1);
		}
	}
	//lpower down
	val = 0xff;
	anx74XX_write_data(tcpc_slave_addr, TCPC_COMMAND_ADDR, &val, 1);
}
