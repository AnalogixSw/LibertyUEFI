///******************************************************************************
//The contents of this file are provided under the conditions defined
//by the AA-001946-LA-5-Software_License_Agreement.pdf.
//Copyright (c) 2017, Analogix Semiconductor, Inc.
///******************************************************************************
#include "Anx74XXLib.h"
#include "Anx74XXLibReg.h"


UINT8 anx74XX_read_data(UINT8 slave_addr, UINT8 offset, UINT8 *data_out, UINT8 size_out)
{
	//TODO: need implement this
	//read data through I2C, returns count of read out.
	return 0;
}


UINT8 anx74XX_write_data(UINT8 slave_addr, UINT8 offset, UINT8 *data_in, UINT8 size_in)
{
	//TODO: need implement this
	//write data through I2C, returns count of write in.
	return 0;
}

VOID delay_us(UINT32 us)
{
	//TODO: need implement this
}

EFI_STATUS 
EFIAPI
anx74xx_set_fw_feature_control(
    UINT8 slave_addr, 
    ANX74XX_FW_FEATURE_FLAG_BITMAP_TYPE feature_ctrl_bitmap
)
{
	UINT8 reg = 0;
	UINT8 ret = anx74XX_read_data(slave_addr, FIRMWARE_CTRL, &reg, 1);
	if (ret != 1)
		return EFI_ERROR;
	switch(feature_ctrl_bitmap)
	{
		case ANX74XX_FW_FEATURE_CTRL_BITMAP_TRY_SRC_ENABLE:
		reg = reg & TRY_SNK_SRC_DISABLE;
		break;
		case ANX74XX_FW_FEATURE_CTRL_BITMAP_TRY_SNK_ENABLE:
		reg = reg & TRY_SNK_SRC_DISABLE;
		break;
		default:
		break;
		
	}
	reg |= feature_ctrl_bitmap;
	anx74XX_write_data(slave_addr, FIRMWARE_CTRL, &reg, 1);
	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
anx74xx_set_power_negotiation_parameter(
    UINT8 slave_addr, 
    UINT32 max_voltage_mv, 
    UINT32 max_power_mw, 
    UINT32 min_power_mw
)
{
	UINT8 max_volt = max_voltage_mv/100;
	UINT8 max_power = max_power_mw/500;
	UINT8 min_power = min_power_mw/500;
	UINT8 ret = 
	anx74XX_write_data(slave_addr, CONFIG_RDO_MAX_VOLTAGE_OFFSET, &max_volt, 1);
	if (ret != 1)
	{
		return EFI_ERROR;
	}
	ret = 
	anx74XX_write_data(slave_addr, CONFIG_RDO_MAX_POWER_OFFSET, &max_power, 1);
	if (ret != 1)
	{
		return EFI_ERROR;
	}
	ret = 
	anx74XX_write_data(slave_addr, CONFIG_RDO_MIN_POWER_OFFSET, &min_power, 1);
	if (ret != 1)
	{
		return EFI_ERROR;
	}
	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
anx74xx_get_rdo_info(
    UINT8 slave_addr,
    UINT32 *max_voltage_mv, 
    UINT32 *max_power_mw,    
)
{
	UINT8 max_volt = 0;
	UINT8 max_power = 0;
	UINT8 ret = anx74XX_read_data(slave_addr, RDO_MAX_VOLT_OFFSET, &max_volt, 1);
	if (ret != 1)
	{
		return EFI_ERROR;
	}
	ret = anx74XX_read_data(slave_addr, RDO_MAX_POWER_OFFSET, &max_power, 1);
	if (ret != 1)
	{
		return EFI_ERROR;
	}
	*max_voltage_mv = max_volt * 100;
	*max_power_mw = max_power * 500;
	return EFI_SUCCESS;
}

VOID memory_copy(UINT8* des, UINT8* src, UINT8 len)
{
	for(UINT8 i = 0; i < len; i++)
	{
		des[i] = src[i];
	}
}

UINT8 cac_checksum(UINT8 *data, UINT8 len)
{
	UINT8 i;
	UINT8 checksum;

	checksum = 0;
	for (i = 0; i < len; i++)
		checksum += *(data + i);

	return (UINT8) (0 - checksum);
}

UINT8 send_pd_msg(UINT8 slave_addr, UINT8* data, UINT8 len, UINT8 type)
{
	UINT8 c, sending_len;
	UINT8 write_data_buf[TRANSMIT_BUFFER_SIZE];
	/* full, return 0 */
	write_data_buf[MSG_HEADER_INDEX] = (len + MSG_TYPE_LEN);	/* cmd */
	write_data_buf[MSG_TYPE_INDEX] = type;
	if (data)
		memory_copy(write_data_buf + MSG_DATA_INDEX, data, len);
	/* cmd + checksum */
	write_data_buf[len + MSG_TYPE_LEN + MSG_HEADER_LEN] = cac_checksum(write_data_buf, len + MSG_TYPE_LEN + MSG_HEADER_LEN);

	sending_len = write_data_buf[MSG_HEADER_INDEX] + MSG_HEADER_LEN + MSG_CHECKSUM_LEN;

	anx74XX_read_data(slave_addr, TX_BUFFER_OFFSET, &c, 1);

	/* retry*/
	if (retry_count && c) {
		UINT8 count = RETRY_COUNT;

		while (count) {
			delay_us(DELAY_TIME);
			anx74XX_read_data(slave_addr, TX_BUFFER_OFFSET, &c, 1);
			if (c == 0)
				break;
			count--;
		}
	}


	if (c == 0) {
		anx74XX_write_data(slave_addr, TX_BUFFER_OFFSET + MSG_HEADER_LEN,
			 &write_data_buf[MSG_TYPE_INDEX], sending_len - MSG_HEADER_LEN);
		anx74XX_write_data(slave_addr, TX_BUFFER_OFFSET,
			write_data_buf[MSG_HEADER_INDEX], MSG_HEADER_LEN);
	}

	return 0;
}

EFI_STATUS
EFIAPI
anx74xx_set_source_cap(
    UINT8 slave_addr, 
    UINT32 *p_src_cap, 
    UINT32 no_of_src_cap
)
{
	if (p_src_cap)
	{
		send_pd_msg(slave_addr, (UINT8 *)p_src_cap, no_of_src_cap*MSG_DATA_OBJECT_SIZE, TYPE_PWR_SRC_CAP);
	}
	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
anx74xx_set_sink_cap(
    UINT8 slave_addr, 
    UINT32 *p_snk_cap, 
    UINT32 no_of_snk_cap
)
{
	if (p_snk_cap)
	{
		send_pd_msg(slave_addr, (UINT8 *)p_snk_cap, no_of_snk_cap*MSG_DATA_OBJECT_SIZE, TYPE_PWR_SNK_CAP);
	}
	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
anx74xx_get_noncompliant_cable_detect_result(
    UINT8 slave_addr, 
    BOOLEAN *noncompliant_cable_detected
)
{
	//TBD, cannot support now
	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
anx74xx_set_irq_mask(
    UINT8 slave_addr, 
    ANX74XX_IRQ_BITMAP_TYPE irq_mask_bitmap
)
{
	//1 mask, 0 unmask, here is set mask to mask related interrupt
	UINT8 mask = 0;
	UINT8 ret = anx74XX_read_data(slave_addr, IRQ_BITS_MASK_OFFSET, &mask, 1);
	if (ret != 1)
	{
		return EFI_ERROR;
	}
	mask | = irq_mask_bitmap;
	ret = 
	anx74XX_write_data(slave_addr, IRQ_BITS_MASK_OFFSET, &mask, 1);
	if (ret != 1)
	{
		return EFI_ERROR;
	}
	return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
anx74xx_set_irq_unmask(
    UINT8 slave_addr, 
    ANX74XX_IRQ_BITMAP_TYPE irq_mask_bitmap
)
{
	//1 mask, 0 unmask, here is set mask to unmask related interrupt
	UINT8 mask = 0;
	UINT8 ret = anx74XX_read_data(slave_addr, IRQ_BITS_MASK_OFFSET, &mask, 1);
	if (ret != 1)
	{
		return EFI_ERROR;
	}
	mask & = (~irq_mask_bitmap);
	ret = 
	anx74XX_write_data(slave_addr, IRQ_BITS_MASK_OFFSET, &mask, 1);
	if (ret != 1)
	{
		return EFI_ERROR;
	}
	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
anx74xx_get_irq_status(
    UINT8 slave_addr, 
    ANX74XX_IRQ_BITMAP_TYPE *irq_status_bitmap
)
{
	//irq register may include several status changes at a time,
	//so here return the irq register value, user need to check what status included.
	UINT8 irq = 0;
	UINT8 ret = anx74XX_read_data(slave_addr, IRQ_REG_OFFSET, (UINT8 *)irq_status_bitmap, 1);
	if (ret != 1)
	{
		return EFI_ERROR;
	}
	//clear the register
	if (*irq_status_bitmap)
	{
		ret = 
		anx74XX_write_data(slave_addr, IRQ_REG_OFFSET, &irq, 1);
		if (ret != 1)
		{
			return EFI_ERROR;
		}
	}
//	if (irq & ANX74XX_IRQ_BITMAP_RECVD_MSG_INT)
//	{
//		*irq_status_bitmap = ANX74XX_IRQ_BITMAP_RECVD_MSG_INT;
//	}
	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
anx74xx_get_cc_status(
    UINT8 slave_addr,
    ANX74XX_CC_STATUS *cc_status
)
{
	UINT8 status = 0;
	UINT8 cc1 = 0;
	UINT8 cc2 = 0;
	UINT8 ret = anx74XX_read_data(slave_addr, CC_STATUS_OFFSET, &status, 1);
	if (ret != 1)
	{
		return EFI_ERROR;
	}
	cc1 = status & HIGH_4_BITS_MASK;
	cc2 = ((status >> SHIFT_4_BITS) & HIGH_4_BITS_MASK);
	if((cc1 == VALUE_OPEN) && (cc2 == VALUE_OPEN))
	{
		cc_status->CC1 = ANX74XX_CCX_STATUS_SRC_OPEN;
		cc_status->CC2 = ANX74XX_CCX_STATUS_SRC_OPEN;
	}
	else if (cc1 == VALUE_OPEN)
	{
		//cc1 is src.open
		cc_status->CC1 = ANX74XX_CCX_STATUS_SRC_OPEN;
		if (cc2 == VALUE_RD)
		{
			//cc2 is src.rd, sink attached, UFP
			cc_status->CC2 = ANX74XX_CCX_STATUS_SRC_RD;
		}
		else if (cc2 == VALUE_RA)
		{
			//cc2 is src.ra, powered cable without sink
			cc_status->CC2 = ANX74XX_CCX_STATUS_SRC_RA;
		}
		else if (cc2 == VALUE_3000MA)
		{
			cc_status->CC2 = ANX74XX_CCX_STATUS_SNK_POIWER3P0;
		}
		else if (cc2 == VALUE_1500MA)
		{
			cc_status->CC2 = ANX74XX_CCX_STATUS_SNK_POWER1P5;
		}
		else
		{
			if (cc2 == VALUE_USBDEFAULT)
				cc_status->CC2 = ANX74XX_CCX_STATUS_SNK_DEFAULT;
		}
	}
	else if (cc1 == VALUE_RD)
	{
		//cc1 is src.rd
		cc_status->CC1 = ANX74XX_CCX_STATUS_SRC_RD;
		if (cc2 == VALUE_OPEN)
		{
			// ufp
			cc_status->CC2 = ANX74XX_CCX_STATUS_SRC_OPEN;
		}
		else if (cc2 == VALUE_RD)
		{
			//src.rd
			cc_status->CC2 = ANX74XX_CCX_STATUS_SRC_RD;
		}
		else if (cc2 == VALUE_RA)
		{
			//src.ra
			cc_status->CC2 = ANX74XX_CCX_STATUS_SRC_RA;
		}
	}
	else if (cc1 == VALUE_RA)
	{
		//cc1 is src.ra
		cc_status->CC1 = ANX74XX_CCX_STATUS_SRC_RA;
		if (cc2 == VALUE_OPEN)
		{
			//powered able without ufp
			cc_status->CC2 = ANX74XX_CCX_STATUS_SRC_OPEN;
		}
		else if (cc2 == VALUE_RD)
		{
			//src.rd, powered cable with ufp
			cc_status->CC2 = ANX74XX_CCX_STATUS_SRC_RD;
		}
		else if (cc2 == VALUE_RA)
		{
			//src.ra, audio accessory
			cc_status->CC2 = ANX74XX_CCX_STATUS_SRC_RA;
		}
	}
	else
	{
		if ((cc1 == VALUE_USBDEFAULT))
		{
			//usbdefault
			cc_status->CC1 = ANX74XX_CCX_STATUS_SNK_DEFAULT;
		}
		else if ((cc1 == VALUE_1500MA))
		{
			//1500ma
			cc_status->CC1 = ANX74XX_CCX_STATUS_SNK_POWER1P5;
		}
		else if ((cc1 == VALUE_3000MA))
		{
			//3000ma
			cc_status->CC1 = ANX74XX_CCX_STATUS_SNK_POIWER3P0;
		}
	}

	
	return EFI_SUCCESS;
	
}

EFI_STATUS
EFIAPI
anx74xx_get_misc_status(
    UINT8 slave_addr,
    BOOLEAN *is_dfp,
    BOOELAN *is_power_provider,
    BOOLEAN *is_vconn_on
)
{
	UINT8 status = 0;
	UINT8 ret = anx74XX_read_data(slave_addr, SYSTEM_STATUS_OFFSET, &status, 1);
	if (ret != 1)
	{
		return EFI_ERROR;
	}
	*is_dfp = (status & DATA_ROLE) ? TRUE : FALSE;
	*is_power_provider = (status & VBUS_STATUS) ? TRUE : FALSE;
	*is_vconn_on = (status & VCONN_STATUS) ? TRUE : FALSE;
	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
anx74xx_set_port_role(
    UINT8 slave_addr,
    ANX74XX_PORT_ROLE_TYPE  port_role
)
{
	//Set port role need send Identity message when initializing
	UINT32 identity[IDENTITY_OBJECT_COUNT] = {0};
	UINT8 data_role = 1;
	switch(port_role)
	{
		case ANX74XX_PORT_ROLE_DRP:
		identity[IDENTITY_ID_HEADER_INDEX] = ID_HEADER(USB_VENDOR_ID,
					MODAL_SUPPORTED, AMA_MODE, AS_USB_DEVICE, AS_USB_HOST);
		break;
		case ANX74XX_PORT_ROLE_DFP_ONLY:
		identity[IDENTITY_ID_HEADER_INDEX] = ID_HEADER(USB_VENDOR_ID,
					MODAL_SUPPORTED, AMA_MODE, NOT_USB_DEVICE, AS_USB_HOST);
		break;
		case ANX74XX_PORT_ROLE_UFP_ONLY:
		identity[IDENTITY_ID_HEADER_INDEX] = ID_HEADER(USB_VENDOR_ID,
					MODAL_SUPPORTED, AMA_MODE, AS_USB_DEVICE, NOT_USB_HOST);
		break;
		default:
		return EFI_ERROR;
	}
	identity[IDENTITY_CERT_STAT_VDO_INDEX] = CERT_STAT_VDO(USB_IF_COMPLIANCE);
	identity[IDENTITY_PRODUCT_VDO_INDEX] = PRODUCT_VDO(BCD_DEVICE,
											USB_PRODUCT_ID);
	identity[IDENTITY_AMA_VDO_INDEX] = ATERNATE_MODE_VDO(HW_VERSION,
							FW_VERSION, TX1_FIXED, TX2_FIXED, RX1_FIXED,
							RX2_FIXED, VCONN_POWER_15W, VCONN_REQUIRED,
							VBUS_REQUIRED, USB_SIGNAL_GEN1_20);
	send_pd_msg(slave_addr, (UINT8 *)identity,
				IDENTITY_OBJECT_COUNT*MSG_DATA_OBJECT_SIZE,
				TYPE_DP_SNK_IDENTITY);
	return EFI_SUCCESS;
}
