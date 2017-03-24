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
//#include "driver/i2c.h"
#include "sgtl5000.h"
#include "audiobit.h"
#include "sounds.h"

void audiobit_play (void)
{
    const char *ptr = (const char *)audiobit_music;
    unsigned int offset = 0;
    signed short samples[256], i;

    while (offset<NUM_ELEMENTS*2)
    {
        memcpy (samples, ptr+offset, sizeof(samples));

        for (i=0; i<256; i++)
            samples[i] = samples[i] + 32768;
        i2s_write_bytes (I2S_NUM, (const char*) samples, 512, portMAX_DELAY);
        offset = offset+512;
    }
    //i2s_write_bytes (I2S_NUM, (const char*)audiobit_music, 256, portMAX_DELAY);
}
//int i2s_write_bytes(i2s_port_t i2s_num, const char *src, size_t size, TickType_t ticks_to_wait);

void audiobit_i2s_init ()
{
    //for 36Khz sample rates, we create 100Hz sine wave, every cycle need 36000/100 = 360 samples (4-bytes each sample)
    //using 6 buffers, we need 60-samples per buffer
    //2-channels, 16-bit each channel, total buffer is 360*4 = 1440 bytes
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,                                  // Only TX
        .sample_rate = AUDIOBIT_SAMPLERATE,
        .bits_per_sample = AUDIOBIT_BITSPERSAMPLE,                                                  //16-bit per channel
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           //2-channels
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 6,
        .dma_buf_len = 512,                                                      //
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1                                //Interrupt level 1
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = AUDIOBIT_BCK,
        .ws_io_num = AUDIOBIT_LRCLK,
        .data_out_num = AUDIOBIT_DOUT,
        .data_in_num = AUDIOBIT_DIN                                                       //Not used
    };

    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);

    // Enable MCLK output
    WRITE_PERI_REG(PIN_CTRL, READ_PERI_REG(PIN_CTRL)&0xFFFFFFF0);
    PIN_FUNC_SELECT (PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);

    // Reconfigure the clock rates for accurate clock output if needed
    // Output 512*Fs at MCLK with 48kHz 16bps
    //WRITE_PERI_REG(I2S_CLKM_CONF_REG(I2S_NUM), (1<<0)|(2<<8)|(11<<12)|(1<<16));

    //PIN_FUNC_SELECT (PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD_CLK_OUT2);
}

// ##################################################################


/* Basic initialization, DAC enable:

• Start condition
• Device address with the R/W bit cleared to indicate write 
• Send two bytes for the 16 bit register address (most significant byte first)
• Stop Condition followed by start condition (or a single restart condition)
• Device address with the R/W bit set to indicate read 
• Read two bytes from the addressed register (most significant byte first)
• Stop condition
*/

esp_err_t audiobit_poweron_init (void)
{
    uint16_t readval, writeval;

    // No response from address at all? Exit!
    if (audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ID, &readval) == ESP_FAIL)
        return ESP_FAIL;

    // Check if this is SGTL5000 (AudioBit)
    if ((readval&0xFF00) != 0xA000)
        return ESP_FAIL;

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_DIG_POWER, 0x0031);

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_CLK_CTRL, 0x0022);

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_I2S_CTRL, 0x0130);

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_SSS_CTRL, 0x0020);

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ADCDAC_CTRL, 0x0000);

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_DAC_VOL, 0x3D3D);

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_PAD_STRENGTH, 0x02AA);

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_ADC_CTRL, 0x0000);

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_HP_CTRL, 0x1818);

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_CTRL, 0x0101);

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_LINREG_CTRL, 0x0000);

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_REF_CTRL, 0x01F0);

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_MIC_CTRL, 0x0000);

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_LINE_OUT_CTRL, 0x0000);

    // 
    audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_LINE_OUT_VOL, 0x0000);

    // 
    if (audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_POWER, 0x403C) == 0)
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
    uint8_t *byte_val = reg_val;
    uint16_t val;   // Value read from register
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
