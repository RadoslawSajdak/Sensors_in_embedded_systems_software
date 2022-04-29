#ifndef __BMP280_MAP_H__
#define __BMP280_MAP_H__

#define BMP280_dig_T1           0x88
#define BMP280_dig_P1           0x8E
#define BMP280_dig_T_size       6
#define BMP280_dig_P_size       20
#define BMP280_dig_size         2


#define BMP280_REG_ID           0xD0
#define BMP280_ID_VAL           0x58

#define BMP280_REG_RESET        0xE0
#define BMP280_RESET_VAL        0xB6

#define BMP280_REG_STATUS       0xF3
#define BMP280_STATUS_MEASURING_MASK    (1 << 3)    // ‘1’ whenever a conversion is running
#define BMP280_STATUS_IMUPDATE_MASK     (1 << 0)    // ‘1’ when the NVM data are being copied to image registers
#define BMP280_STATUS_DONE              0x00

#define BMP280_REG_CTRL_MEAS    0xF4
#define BMP280_CTRL_OSRS_T_MASK         (0x07 << 5) // temperature sampling mask
#define BMP280_CTRL_OSRS_T00            (0 << 5)    // skip temperature sampling
#define BMP280_CTRL_OSRS_T01            (1 << 5)    // 1x temperature sampling
#define BMP280_CTRL_OSRS_T02            (2 << 5)    // 2x temperature sampling
#define BMP280_CTRL_OSRS_T04            (3 << 5)    // 4x temperature sampling
#define BMP280_CTRL_OSRS_T08            (4 << 5)    // 8x temperature sampling
#define BMP280_CTRL_OSRS_T16            (5 << 5)    // 16x temperature sampling

#define BMP280_CTRL_OSRS_P_MASK         (0x07 << 2) // pressure sampling mask
#define BMP280_CTRL_OSRS_P00            (0 << 2)    // skip pressure sampling
#define BMP280_CTRL_OSRS_P01            (1 << 2)    // 1x pressure sampling
#define BMP280_CTRL_OSRS_P02            (2 << 2)    // 2x pressure sampling
#define BMP280_CTRL_OSRS_P04            (3 << 2)    // 4x pressure sampling
#define BMP280_CTRL_OSRS_P08            (4 << 2)    // 8x pressure sampling
#define BMP280_CTRL_OSRS_P16            (5 << 2)    // 16x pressure sampling

#define BMP280_CTRL_MODE_MASK           0x03        // mode mask
#define BMP280_CTRL_SLEEP               0x00        // sleep mode - no measures
#define BMP280_CTRL_FORCED              0x01        // forced mode - single measure -after-> sleep
#define BMP280_CTRL_NORMAL              0x03        // normal mode - regural sampling

#define BMP280_REG_CONFIG       0xF5
#define BMP280_CONFIG_SB_MASK           (0x7F << 5) // standby in normal mode mask
#define BMP280_CONFIG_SB0000MS          (0 << 5)    // 0,5ms standby in normal mode
#define BMP280_CONFIG_SB0062MS          (1 << 5)    // 62ms standby in normal mode
#define BMP280_CONFIG_SB0125MS          (2 << 5)    // 125ms standby in normal mode
#define BMP280_CONFIG_SB0250MS          (3 << 5)    // 250ms standby in normal mode
#define BMP280_CONFIG_SB0500MS          (4 << 5)    // 500ms standby in normal mode
#define BMP280_CONFIG_SB1000MS          (5 << 5)    // 1s standby in normal mode
#define BMP280_CONFIG_SB2000MS          (6 << 5)    // 2s standby in normal mode
#define BMP280_CONFIG_SB4000MS          (7 << 5)    // 4s standby in normal mode


#define BMP280_REG_PRESS_MSB    0xF7
#define BMP280_REG_PRESS_LSB    0xF8
#define BMP280_REG_PRESS_XLSB   0xF9


#define BMP280_REG_TEMP_MSB     0xFA
#define BMP280_REG_TEMP_LSB     0xFB
#define BMP280_REG_TEMP_XLSB    0xFC



#endif