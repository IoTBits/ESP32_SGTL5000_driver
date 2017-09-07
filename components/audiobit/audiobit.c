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

#include <string.h>
#include "sgtl5000.h"
#include "audiobit.h"
#include "soc/rtc.h"
#include "soc/soc.h"

static i2s_config_t audiobit_i2s_config = {
    .mode = I2S_MODE_MASTER | I2S_MODE_TX,                                  // Only TX
    .sample_rate = AUDIOBIT_SAMPLERATE,                                     // Default: 48kHz
    .bits_per_sample = AUDIOBIT_BITSPERSAMPLE,                              //16-bit per channel
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           //2-channels
    .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
    .dma_buf_count = 6,
    .dma_buf_len = 512,                                                      //
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1                                //Interrupt level 1
};

static i2s_pin_config_t audiobit_pin_config = {
    .bck_io_num = AUDIOBIT_BCK,
    .ws_io_num = AUDIOBIT_LRCLK,
    .data_out_num = AUDIOBIT_DOUT,
    .data_in_num = AUDIOBIT_DIN                                                       //Not used
};


// ##################################################################

/*
    Set the AudioBit according to sample rate, bit depth, and calculate MCLK
    
    arg_sample_rate: 48000, 8000, etc
    arg_bits_per_sample: 16, 24, etc
*/
esp_err_t audiobit_configure_stream (uint32_t arg_sample_rate, uint32_t arg_bits_per_sample)
{
    // NOTE: To save power, MCLK is always minimum possible, i.e. 256*Fs
    audiobit_i2s_config.sample_rate = arg_sample_rate;
    audiobit_i2s_config.bits_per_sample = arg_bits_per_sample;

    return i2s_driver_install (I2S_NUM, &audiobit_i2s_config, 0, NULL);
}

/*
    Set surround sound ON/OFF
    0: Surround OFF
    1-8: Surround effect level
*/
esp_err_t audiobit_set_surround_sound (uint8_t surround)
{
    if (surround == 0)      // No surround sound effect
        {
            if (audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_DAP_SGTL_SURROUND, 0x0000) == 0)
                return ESP_OK;
            else
                return ESP_FAIL;
        }

    if (surround >8) surround = 8;
    surround -= 1;          // Surround between 0-7 now

    if (audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_DAP_SGTL_SURROUND, 0x3|(surround<<4)) == 0)
            return ESP_OK;
        else
            return ESP_FAIL;
}

/*
    Set microphone bias resistor
    0: hi-Z, 1: 2K, 2: 4K, 3: 8K
*/
esp_err_t audiobit_set_mic_resistor (uint8_t bias)
{
    uint16_t readval;
    
    if (bias > 0x03)
        bias = 0;           // Invalid value, disable bias!

    audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_MIC_CTRL, &readval);
    readval &= 0xFCFF;
    readval |= (bias << 8);
    if (audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_MIC_CTRL, readval) == 0)
        return ESP_OK;
    else
        return ESP_FAIL;
}

/*
    Set microphone bias voltage (in 250mV steps)
    Range: 1250 - 3000 mV
*/
esp_err_t audiobit_set_mic_voltage (uint16_t voltage)
{
    uint16_t readval;
    
    if (voltage < 1250)
        voltage = 1250;
    else if (voltage > 3000)
        voltage = 3000;

    if (voltage > AUDIOBIT_VDDA - 200)
        return ESP_FAIL;

    voltage -= 1250;
    voltage /= 250;

    audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_MIC_CTRL, &readval);
    readval &= 0xFF8F;
    readval |= (voltage << 4);
    if (audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_MIC_CTRL, readval) == 0)
        return ESP_OK;
    else
        return ESP_FAIL;
}

/*
    Set microphone gain in steps of 10dB each
    Range: 0 to 3
    0: 0dB, 1: +20dB... +40dB
*/
esp_err_t audiobit_set_mic_gain (uint8_t gain)
{
    uint16_t readval;

    if (gain > 3)
        gain = 3;

    audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_MIC_CTRL, &readval);
    readval &= 0xFFFC;
    readval |= gain;
    if (audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_MIC_CTRL, readval) == 0)
        return ESP_OK;
    else
        return ESP_FAIL;
}

/*
    Mute the headphone output
*/
esp_err_t audiobit_mute_headphone (void)
{
    uint16_t readval;

    audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_CTRL, &readval);
    readval |= 0x0010;
    if (audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_CTRL, readval) == 0)
    return ESP_OK;
else
    return ESP_FAIL;
}

/*
    Unmute the headphone output
*/
esp_err_t audiobit_unmute_headphone (void)
{
    uint16_t readval;

    audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_CTRL, &readval);
    readval &= 0xFFEF;
    if (audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_CTRL, readval) == 0)
    return ESP_OK;
else
    return ESP_FAIL;
}

/*
    Set DAC and ADC reference voltage (VAG) in millivolts
    Range: 800 mV to 1575 mV
*/
esp_err_t audiobit_set_ref (uint16_t vag_voltage)
{
    uint16_t readval;

    if (vag_voltage < 800)
        vag_voltage = 800;
    else if (vag_voltage > 1575)
        vag_voltage = 1575;

    vag_voltage /= 25;
    vag_voltage -= 32;
    vag_voltage = (vag_voltage & 0x001F) << 4;

    audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_REF_CTRL, &readval);
    readval &= 0xFE0F;
    readval |= vag_voltage;
    if (audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_REF_CTRL, readval) == 0)
    return ESP_OK;
else
    return ESP_FAIL;
}

/*
    Check if the module is connected and initialized (ready)
    Call this before any other operation, and after setting up
    I2C and I2S interfaces.

    Note: MCLK must be active for this check to return ESP_OK
*/
esp_err_t audiobit_check_module (void)
{
    uint16_t readval;

    audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ID, &readval);
    if ((readval & 0xFF00) == 0xAA00)
        return ESP_OK;
    else
        return ESP_FAIL;
}

/*
    Set digital volume for DAC output.
    Leave this value at 0dB if HP volume can be used to control output

    Volume: 0 dB to -90 dB
    Recommended (default): 0 dB
*/
esp_err_t audiobit_set_digital_volume (int8_t left_vol, int8_t right_vol)
{
    int8_t left, right;

    left = (-2*left_vol) + 0x3C;
    right = (-2*right_vol) + 0x3C;

    if (audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_DAC_VOL, (right << 8)|left) == 0)
        return ESP_OK;
    else
        return ESP_FAIL;
}

/*
    Set volume for headphone output driver.
    Leave this value at 0dB for best audio quality, with DAC volume = 0 dB
    
    For 32 ohm headset in capless mode, -17dB is a good initial value
    Volume: +12 dB to -50 dB
    Recommended (default): 0 dB
*/
esp_err_t audiobit_set_headphone_volume (int8_t left_vol, int8_t right_vol)
{
    int8_t left, right;

    left = ((-2*left_vol) + 0x18) & 0x7F;
    right = ((-2*right_vol) + 0x18) & 0x7F;

    if (audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_HP_CTRL, (right << 8)|left) == 0)
        return ESP_OK;
    else
        return ESP_FAIL;
}

/*
    Set digital pad drive strength. Setting high drive strength causes
    more noise and more power rail noise.
    Pad drive strength:
    0 = disable
    1 = low
    2 = medium
    3 = high
*/
esp_err_t audiobit_pin_drive_strength (uint8_t i2c_strength, uint8_t i2s_strength)
{
    uint16_t strength;

    i2c_strength &= 0x03;
    i2s_strength &= 0x03;
    strength = i2c_strength|(i2c_strength << 2);
    strength |= (i2s_strength << 4)|(i2s_strength << 6)|(i2s_strength << 8);

    if (audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_HP_CTRL, strength) == 0)
        return ESP_OK;
    else
        return ESP_FAIL;
}

/* Basic initialization, DAC enable:

• Start condition
• Device address with the R/W bit cleared to indicate write 
• Send two bytes for the 16 bit register address (most significant byte first)
• Stop Condition followed by start condition (or a single restart condition)
• Device address with the R/W bit set to indicate read 
• Read two bytes from the addressed register (most significant byte first)
• Stop condition
*/

esp_err_t audiobit_playback_init (void)
{
    uint8_t retval=0;
    uint16_t readval;

    // Read chip ID
    retval = audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ID, &readval);
    printf("AudioBit chip ID: %d, return code: %d\n", readval, retval);

    // Digital power control
    // Enable I2S data in and DAC
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_DIG_POWER, 0x0021);
    printf("Power up I2S and DAC, err_code: %d\n", retval);

    // 48kHz sample rate, with CLKM = 256*Fs = 12.288000 MHz
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_CLK_CTRL, 0x0008);
    printf("Power up I2S and DAC, err_code: %d\n", retval);

    // I2S mode = , LRALIGN = 0, LRPOL = 0
    // 32*Fs is SCLK rate, 16 bits/sample, I2S is slave, no PLL used
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_I2S_CTRL, 0x0130);
    printf("I2S configured, err_code: %d\n", retval);

    // I2S in -> DAC output, rest left at default
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_SSS_CTRL, 0x0010);
    printf("Attach I2S in to DAC, err_code: %d\n", retval);

    // Unmute DAC, no volume ramp enabled
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ADCDAC_CTRL, 0x0000);
    printf("Unmute DAC, err_code: %d\n", retval);

    // DAC volume is 0dB for both channels
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_DAC_VOL, 0x3C3C);
    printf("DAC volume -0.5dB, err_code: %d\n", retval);

    // Moderate drive strength (4mA) for all pads
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_PAD_STRENGTH, 0x02AA);
    printf("Moderate drive strength for pads, err_code: %d\n", retval);

    // Headphone output volume is -17dB each
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_HP_CTRL, 0x3A3A);
    printf("HP out volume is -17dB, err_code: %d\n", retval);

    // Unmute HP, ZCD disabled, rest mute
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_CTRL, 0x0101);
    printf("Unmute HP, err_code: %d\n", retval);

    // VAG_VAL = 0.8V + 100mV = 0.9V
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_REF_CTRL, 0x0040);
    printf("Check VAG = 0.9V!, err_code: %d\n", retval);

    // Capless HP and DAC on
    // Stereo DAC with external VDDD source
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_POWER, 0x40FC);
    printf("Power up all analog sections, err_code: %d\n", retval);

    if (retval == 0)
        return ESP_OK;
    else
        return ESP_FAIL;
}

/* Write operation:

• Start condition 
• Device address with the R/W bit cleared to indicate write 
• Send two bytes for the 16 bit register address (most significant byte first)
• Send two bytes for the 16 bits of data to be written to the register (most significant byte first)
• Stop condition
*/

esp_err_t audiobit_write_reg (i2c_port_t i2c_num, uint16_t reg_addr, uint16_t reg_val)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    uint8_t dwr[4];

    // Start condition
    i2c_master_start(cmd);
    // Address + Write bit
    i2c_master_write_byte(cmd, (AUDIOBIT_I2C_ADDR<<1)|WRITE_BIT, ACK_CHECK_EN);
    // MSB for reg address
    //i2c_master_write_byte(cmd, (reg_addr>>8)&0xFF, ACK_CHECK_EN);
    dwr[0] = (reg_addr>>8)&0xFF;
    // LSB for reg address
    //i2c_master_write_byte(cmd, (reg_addr&0xFF), ACK_CHECK_EN);
    dwr[1] = (reg_addr&0xFF);
    // MSB for reg data
    //i2c_master_write_byte(cmd, (reg_val>>8)&0xFF, ACK_CHECK_EN);
    dwr[2] = (reg_val>>8)&0xFF;
    // LSB for reg data
    //i2c_master_write_byte(cmd, (reg_val&0xFF), ACK_CHECK_EN);
    dwr[3] = (reg_val&0xFF);
    i2c_master_write(cmd, dwr, 4, ACK_CHECK_EN);
    i2c_master_stop(cmd);

    // Execute and return status
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


/* Read operation:

• Start condition
• Device address with the R/W bit cleared to indicate write 
• Send two bytes for the 16 bit register address (most significant byte first)
• Stop Condition followed by start condition (or a single restart condition)
• Device address with the R/W bit set to indicate read 
• Read two bytes from the addressed register (most significant byte first)
• Stop condition
*/
esp_err_t audiobit_read_reg (i2c_port_t i2c_num, uint16_t reg_addr, uint16_t *reg_val)
{
    uint8_t *byte_val = reg_val;		// This will cause warning, please ignore
    esp_err_t ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // Start condition
    i2c_master_start(cmd);
    // Address + Write bit
    i2c_master_write_byte(cmd, (AUDIOBIT_I2C_ADDR<<1)|WRITE_BIT, ACK_CHECK_EN);
    // MSB for reg address
    i2c_master_write_byte(cmd, (reg_addr>>8)&0xFF, ACK_CHECK_EN);
    // LSB for reg address
    i2c_master_write_byte(cmd, (reg_addr&0xFF), ACK_CHECK_EN);

    // Restart (stop + start)
    i2c_master_start(cmd);

    // Address + read
    i2c_master_write_byte(cmd, (AUDIOBIT_I2C_ADDR<<1)|READ_BIT, ACK_CHECK_EN);
    
    // MSB for reg data
    i2c_master_read(cmd, byte_val + 1, 1, ACK_VAL);
    // LSB for reg data
    i2c_master_read_byte(cmd, byte_val, NACK_VAL);
    
    i2c_master_stop(cmd);
    // Execute and return status, should return 0
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief i2c master initialization
 */
void audiobit_i2c_init()
{
    int i2c_master_port = AUDIOBIT_I2C_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = AUDIOBIT_I2C_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = AUDIOBIT_I2C_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = AUDIOBIT_I2C_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void audiobit_i2s_init ()
{
    i2s_driver_install(I2S_NUM, &audiobit_i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &audiobit_pin_config);

    // Enable MCLK output
    WRITE_PERI_REG(PIN_CTRL, READ_PERI_REG(PIN_CTRL)&0xFFFFFFF0);
    PIN_FUNC_SELECT (PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
}
