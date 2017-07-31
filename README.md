# LibertyUEFI
1. Here are the reference codes, and they didn't be compiled. User who uses the codes, needs put them in UEFI build environment and fix build errors.
2. User needs implement the below 3 functions in file Anx74XXLib.c:
INT8 anx74XX_read_data(UINT8 slave_addr, UINT8 offset, UINT8 *data_out, UINT8 size_out)
UINT8 anx74XX_write_data(UINT8 slave_addr, UINT8 offset, UINT8 *data_in, UINT8 size_in)
VOID delay_us(UINT32 us)

3. Below API cannot be supported in Liberty:
EFI_STATUS
EFIAPI
anx74xx_get_noncompliant_cable_detect_result(
    UINT8 slave_addr, 
    BOOLEAN *noncompliant_cable_detected
);
