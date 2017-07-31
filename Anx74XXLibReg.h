///******************************************************************************
//The contents of this file are provided under the conditions defined
//by the AA-001946-LA-5-Software_License_Agreement.pdf.
//Copyright (c) 2017, Analogix Semiconductor, Inc.
///******************************************************************************
//slave addr: 0x7e, 0x6e, 0x64,0x62
#define FIRMWARE_CTRL 0xB2 
#define IRQ_BITS_MASK_OFFSET 0xB6
#define IRQ_REG_OFFSET 0xB7
#define SYSTEM_STATUS_OFFSET 0xB8
#define CC_STATUS_OFFSET 0xB9
#define CONFIG_RDO_MAX_VOLTAGE_OFFSET 0xAC
#define CONFIG_RDO_MAX_POWER_OFFSET 0xAD
#define CONFIG_RDO_MIN_POWER_OFFSET 0xAE
#define RDO_MAX_VOLT_OFFSET 0xAF
#define RDO_MAX_POWER_OFFSET 0xB0

#define TX_BUFFER_OFFSET 0xC0
#define INTR_ALERT_0  0xCB
#define INTR_ALERT_1  0xCC
#define R_RAM_CTRL 0x05

//Bits definition
#define VCONN_STATUS 0x04
/*0: vbus off; 1: vbus on*/
#define VBUS_STATUS 0x08
/*0: host; 1:device*/
#define DATA_ROLE 0x20

#define TRY_SNK_SRC_DISABLE 0xF3

//constant value definition
#define TRANSMIT_BUFFER_SIZE 32
#define RETRY_COUNT 3
#define DELAY_TIME 1000 //us
#define MSG_HEADER_INDEX 0
#define MSG_TYPE_INDEX 1
#define MSG_DATA_INDEX 2
#define MSG_CHECKSUM_LEN 1
#define MSG_TYPE_LEN 1
#define MSG_HEADER_LEN 1
#define MSG_DATA_OBJECT_SIZE 4
#define HIGH_4_BITS_MASK 0x0F
#define SHIFT_4_BITS 4

typedef enum
{
	VALUE_OPEN = 0,
	VALUE_RD = 1,
	VALUE_RA = 2,
	VALUE_USBDEFAULT = 4,
	VALUE_1500MA = 8,
	VALUE_3000MA = 0x0C
}ANX74XX_CC_VALUE;

typedef enum
{
	TYPE_PWR_SRC_CAP = 0x00,
	TYPE_PWR_SNK_CAP = 0x01,
	TYPE_DP_SNK_IDENTITY = 0x02,
	TYPE_SVID = 0x03,
	TYPE_MAX
}ANX74XX_MSG_TYPE;

//Discovery Identity Command Response definition
#define IDENTITY_OBJECT_COUNT 4
typedef enum
{
	IDENTITY_ID_HEADER_INDEX,
	IDENTITY_CERT_STAT_VDO_INDEX,
	IDENTITY_PRODUCT_VDO_INDEX,
	IDENTITY_AMA_VDO_INDEX
}ANX74XX_IDENTITY_OBJ_INDEX;
#define USB_VENDOR_ID 0x0000
#define AS_USB_DEVICE 1
#define AS_USB_HOST 1
#define NOT_USB_DEVICE 0
#define NOT_USB_HOST 0
#define AMA_MODE 5
#define MODAL_SUPPORTED 1
#define USB_IF_COMPLIANCE 0x000000
#define BCD_DEVICE 0x0000
#define USB_PRODUCT_ID 0x0000
#define HW_VERSION 0x0A
#define FW_VERSION 0x01
#define TX1_CONFIGURABLE 1
#define TX2_CONFIGURABLE 1
#define RX1_CONFIGURABLE 1
#define RX2_CONFIGURABLE 1
#define TX1_FIXED 0
#define TX2_FIXED 0
#define RX1_FIXED 0
#define RX2_FIXED 0
typedef enum
{
	VCONN_POWER_1W,
	VCONN_POWER_15W,
	VCONN_POWER_2W,
	VCONN_POWER_3W,
	VCONN_POWER_4W,
	VCONN_POWER_5W,
	VCONN_POWER_6W
}ANX74XX_VCONN_POWER;

#define VCONN_REQUIRED 1
#define VBUS_REQUIRED 1
#define VCONN_NOT_REQUIRED 0
#define VBUS_NOT_REQUIRED 0
typedef enum
{
	USB_SIGNAL_20,
	USB_SIGNAL_GEN1_20,
	USB_SIGNAL_GEN1_GEN2_20,
	USB_SIGNAL_BILLBOARD
}ANX74XX_USB_SIGNAL_TYPE;

//ID Header
#define ID_HEADER(vid, modal, pt, dev, host) \
(vid | (modal << 26) | (pt << 27) | (dev << 30) | (host << 31))
#define CERT_STAT_VDO(ui) (ui)
#define PRODUCT_VDO(bcd, pid) (bcd | (pid << 16))
#define ATERNATE_MODE_VDO(hw, fw, tx1, tx2, rx1, rx2, vpower, vconn, vbus, sup) \
(sup | (vbus << 3) | (vconn << 4) | (vpower << 5) | (rx2 << 8) | (rx1 << 9) | \
 (tx2 << 10) | (tx1 << 11) | (fw << 24) | (hw << 28))


