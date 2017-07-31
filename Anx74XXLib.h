typedef enum
{
    ANX74XX_FW_FEATURE_CTRL_BITMAP_AUTO_PD_EN            = 1 << 1,   //bit 1
    ANX74XX_FW_FEATURE_CTRL_BITMAP_TRY_SRC_ENABLE        = 1 << 2,   //bit 2
    ANX74XX_FW_FEATURE_CTRL_BITMAP_TRY_SNK_ENABLE        = 1 << 3,   //bit 3       
    ANX74XX_FW_FEATURE_CTRL_BITMAP_GOTO_SAFE5V_EN        = 1 << 4,   //bit 4
}ANX74XX_FW_FEATURE_FLAG_BITMAP_TYPE;

typedef enum
{
    ANX74XX_IRQ_BITMAP_RECVD_MSG_INT            = 1 << 0,   //bit 0
    ANX74XX_IRQ_BITMAP_VCONN_CHG_INT            = 1 << 2,   //bit 2
    ANX74XX_IRQ_BITMAP_VBUS_CHG_INT             = 1 << 3,   //bit 3
    ANX74XX_IRQ_BITMAP_CC_STATUS_CHG_INT        = 1 << 4,   //bit 4
    ANX74XX_IRQ_BITMAP_DATA_ROLE_CHG_INT        = 1 << 5,   //bit 5
    ANX74XX_IRQ_BITMAP_PR_C_GOT_POWER           = 1 << 6    //bit 6
}ANX74XX_IRQ_BITMAP_TYPE;

typedef enum
{
    ANX74XX_CCX_STATUS_SRC_OPEN                 = 0x0,
    ANX74XX_CCX_STATUS_SRC_RD                   = 0x1,
    ANX74XX_CCX_STATUS_SRC_RA                   = 0x2,
    ANX74XX_CCX_STATUS_SNK_DEFAULT              = 0x4,
    ANX74XX_CCX_STATUS_SNK_POWER1P5             = 0x8,
    ANX74XX_CCX_STATUS_SNK_POIWER3P0            = 0xC
}ANX74XX_CCX_STATUS_TYPE;

typedef enum
{
    ANX74XX_PORT_ROLE_DRP,
    ANX74XX_PORT_ROLE_DFP_ONLY,
    ANX74XX_PORT_ROLE_UFP_ONLY,
}ANX74XX_PORT_ROLE_TYPE;

typedef struct
{
    ANX74XX_CCX_STATUS_TYPE     CC1;
    ANX74XX_CCX_STATUS_TYPE     CC2;
}ANX74XX_CC_STATUS

/*
    API to enable disable firmware feature
    @param slave_addr - I2C Slave Address of the HW
    @param feature_ctrl_bitmap - bitmap to enable/disable the feature
    
    @return EFI_SUCCESS if the operation is successful. 
            Other Error code if error has been detected
 */
EFI_STATUS 
EFIAPI
anx74xx_set_fw_feature_control(
    UINT8 slave_addr, 
    ANX74XX_FW_FEATURE_FLAG_BITMAP_TYPE feature_ctrl_bitmap
);

/*
    API to configure parameters for power negotiation
    @param slave_addr - I2C Slave Address of the HW
    @param max_voltage_mv - max voltage in unit of mili-volt
    @param max_power_mw - max power in unit of mili-watt
    @param min_power_mw - min power in unit of mili-watt
    
    @return EFI_SUCCESS if the operation is successful. 
            Other Error code if error has been detected    
 */
EFI_STATUS
EFIAPI
anx74xx_set_power_negotiation_parameter(
    UINT8 slave_addr, 
    UINT32 max_voltage_mv, 
    UINT32 max_power_mw, 
    UINT32 min_power_mw
);

/*
    API to read RDO from HW
    @param slave_addr - I2C Slave Address of the HW
    @param max_voltage_mv - max voltage in unit of mili-volt
    @param max_power_mw - max power in unit of mili-watt
    
    @return EFI_SUCCESS if the operation is successful. 
            Other Error code if error has been detected    
 */
EFI_STATUS
EFIAPI
anx74xx_get_rdo_info(
    UINT8 slave_addr,
    UINT32 *max_voltage_mv, 
    UINT32 *max_power_mw,    
);

/*
    API to set Source capability PDO
    @param slave_addr - I2C Slave Address of the HW
    @param p_src_cap - pointer to a array of source PDOs
    @param no_of_src_cap - number of source cap are passed in the previous argument
    
    @return EFI_SUCCESS if the operation is successful. 
            Other Error code if error has been detected    
 */
EFI_STATUS
EFIAPI
anx74xx_set_source_cap(
    UINT8 slave_addr, 
    UINT32 *p_src_cap, 
    UINT32 no_of_src_cap
);
    
/*
    API to set Sink capability PDO
    @param slave_addr - I2C Slave Address of the HW
    @param p_snk_cap - pointer to a array of Sink PDOs
    @param no_of_snk_cap - number of sink cap are passed in the previous argument
    
    @return EFI_SUCCESS if the operation is successful. 
            Other Error code if error has been detected    
 */
EFI_STATUS
EFIAPI
anx74xx_set_sink_cap(
    UINT8 slave_addr, 
    UINT32 *p_snk_cap, 
    UINT32 no_of_snk_cap
);

/*
    API to get the value of global variable which indicating 
    non-compliant C-to-A cable is detected
    @param slave_addr - I2C Slave Address of the HW
    @param noncompliant_cable_detected - return True if non-compliant cable is detected
    
    @return EFI_SUCCESS if the operation is successful. 
            Other Error code if error has been detected    
 */
EFI_STATUS
EFIAPI
anx74xx_get_noncompliant_cable_detect_result(
    UINT8 slave_addr, 
    BOOLEAN *noncompliant_cable_detected
);

/*
    API to set irq_mask
    @param slave_addr - I2C Slave Address of the HW
    @param irq_mask_bitmap - bitmap to set the irq mask
    
    @return EFI_SUCCESS if the operation is successful. 
            Other Error code if error has been detected    
 */
EFI_STATUS
EFIAPI
anx74xx_set_irq_mask(
    UINT8 slave_addr, 
    ANX74XX_IRQ_BITMAP_TYPE irq_mask_bitmap
);

/*
    API to get irq status
    @param slave_addr - I2C Slave Address of the HW
    @param irq_status_bitmap - return irq status
    
    @return EFI_SUCCESS if the operation is successful. 
            Other Error code if error has been detected    
 */
EFI_STATUS
EFIAPI
anx74xx_get_irq_status(
    UINT8 slave_addr, 
    ANX74XX_IRQ_BITMAP_TYPE *irq_status_bitmap
);

/*
    API to get CC status
    @param slave_addr - I2C Slave Address of the HW
    @param cc_status - return cc status
    
    @return EFI_SUCCESS if the operation is successful. 
            Other Error code if error has been detected    
 */
EFI_STATUS
EFIAPI
anx74xx_get_cc_status(
    UINT8 slave_addr,
    ANX74XX_CC_STATUS *cc_status
);

/*
    API to get misc status, including Data role, Vbus Status and Vconn Status
    @param slave_addr - I2C Slave Address of the HW
    @param is_dfp - return TRUE if current data role is UFP
    @param is_power_provider - return TRUE if the port is provinding power
    @param is_vconn_on - return TRUE if the vconn is On
    
    @return EFI_SUCCESS if the operation is successful. 
            Other Error code if error has been detected    
 */
EFI_STATUS
EFIAPI
anx74xx_get_misc_status(
    UINT8 slave_addr,
    BOOLEAN *is_dfp,
    BOOELAN *is_power_provider,
    BOOLEAN *is_vconn_on
);

/*
    API to set port role to DRP, UFP oply or DFP only.
    @param slave_addr - I2C Slave Address of the HW
    @param port_role - port role to be set
    
    @return EFI_SUCCESS if the operation is successful. 
            Other Error code if error has been detected    
 */
EFI_STATUS
EFIAPI
anx74xx_set_port_role(
    UINT8 slave_addr,
    ANX74XX_PORT_ROLE_TYPE  port_role
);

