/*
Website: www.iot-bits.com

Copyright (C) 2016, IoTBits (Author: Pratik Panda), all right reserved.
E-mail: hello@PratikPanda.com
Personal website: www.PratikPanda.com

The code referencing this license is open source software. Redistribution
and use of the code in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    Redistribution of original and modified source code must retain the 
    above copyright notice, this condition and the following disclaimer.

    This code (or modifications) in source or binary forms may NOT be used
    in a commercial application without obtaining permission from the Author.

This software is provided by the copyright holder and contributors "AS IS"
and any warranties related to this software are DISCLAIMED. The copyright 
owner or contributors are NOT LIABLE for any damages caused by use of this 
software.
*/

#include <stdio.h>
#include "audiobit.h"

/*
 * Pin assignment:
 * - master:
 *    GPIO18 is assigned as the data signal of i2c master port
 *    GPIO19 is assigned as the clock signal of i2c master port
 * - no need to add external pull-up resistors, driver will enable internal pull-up resistors.
 */
/*
esp_err_t scanaddress (i2c_port_t i2c_num, uint8_t num)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // Start condition
    i2c_master_start(cmd);
    // Address + Write bit
    i2c_master_write_byte(cmd, num, ACK_CHECK_EN);
    // MSB for reg address
    i2c_master_stop(cmd);

    // Execute and return status
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}
*/
void init_test_sgtl5000 (void)
{
    uint8_t retval=0;
    uint16_t readval;

    // Read chip ID
    retval = audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ID, &readval);
    printf("Read chip ID: %d, return code: %d\n", readval, retval);

    // Digital power control
    // En data in, DAC
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_DIG_POWER, 0x0021);
    printf("Power up I2S and DAC, err_code: %d\n", retval);

    // 48kHz sample rate, with CLKM = 256*Fs
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_CLK_CTRL, 0x0008);
    printf("Power up I2S and DAC, err_code: %d\n", retval);

    // I2S mode, data starts 1 sck after LRCLK transition
    // 32Fs is SCLK rate, I2S is slave, no PLL used
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_I2S_CTRL, 0x0130);
    printf("I2S configured, err_code: %d\n", retval);

    // I2S in -> DAC output
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_SSS_CTRL, 0x0010);
    printf("Attach I2S in to DAC, err_code: %d\n", retval);

    // Unmute DAC, no volume ramp enabled
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ADCDAC_CTRL, 0x0000);
    printf("Unmute DAC, err_code: %d\n", retval);

    // DAC volume is -0.5dB for both channels
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_DAC_VOL, 0x3C3C);
    printf("DAC volume -0.5dB, err_code: %d\n", retval);

    // Moderate drive strength (4mA) for all pads
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_PAD_STRENGTH, 0x02AA);
    printf("Moderate drive strength for pads, err_code: %d\n", retval);

    // Headphone output volume is -5.5dB each
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_HP_CTRL, 0x3A3A);
    printf("HP out volume is 0dB, err_code: %d\n", retval);

    // Unmute HP, ZCD disabled, rest mute
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_CTRL, 0x0101);
    printf("Unmute HP, err_code: %d\n", retval);

    // VAG_VAL = 0.8V + 100mV = 0.9V
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_REF_CTRL, 0x0040);
    printf("Check VAG = 0.8V!, err_code: %d\n", retval);

    // Capless HP and DAC on
    // Stereo DAC with external VDDD source
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_POWER, 0x40FC);
    printf("Power up all analog sections, err_code: %d\n", retval);

}

void app_main()
{
    //i2c_master_init();
    
    uint8_t retval=0;
    uint16_t readval;

    //delay_us (100000);
    //delay_us(50000);
    audiobit_i2s_init ();
    //delay_us(50000);
    audiobit_i2c_init ();
    init_test_sgtl5000 ();

    while (1)
        audiobit_play();
    //xTaskCreate(i2c_test_task, "i2c_test_task_1", 1024 * 2, (void* ) 1, 10, NULL);
}
