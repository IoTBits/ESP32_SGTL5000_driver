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

#ifndef _AUDIOBIT_H_
#define _AUDIOBIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <esp_types.h>
#include "esp_err.h"
#include "driver/i2c.h"
#include "driver/i2s.h"
#include "soc/soc.h"
#include "sgtl5000.h"

#define AUDIOBIT_VDDD			1800
#define AUDIOBIT_EXT_VDDD		1
#define AUDIOBIT_VDDIO			3300
#define AUDIOBIT_VDDA			1800

#define AUDIOBIT_I2C_SCL_IO		19    /*!< gpio number for I2C master clock */
#define AUDIOBIT_I2C_SDA_IO		18    /*!< gpio number for I2C master data  */
#define AUDIOBIT_I2C_NUM 		1   /*!< I2C port number for master dev */
#define AUDIOBIT_I2C_FREQ_HZ    100000     /*!< I2C master clock frequency */
#define AUDIOBIT_I2C_ADDR  		0x0A    /*!< slave address for AudioBit */

// NOTE: Do not change I2S num right now because it will affect MCLK output!
#define I2S_NUM         		(0)
// MCLK still implemented in direct routine, don't change it!
#define AUDIOBIT_MCLK			0
#define	AUDIOBIT_LRCLK			25
#define AUDIOBIT_BCK			26
#define AUDIOBIT_DOUT			22
#define AUDIOBIT_DIN			-1
#define AUDIOBIT_SAMPLERATE		48000
#define AUDIOBIT_BITSPERSAMPLE	16

#define WRITE_BIT  				I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT   				I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN   			0x1     /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS  			0x0     /*!< I2C master will not check ack from slave */
#define ACK_VAL    				0x0         /*!< I2C ack value */
#define NACK_VAL   				0x1         /*!< I2C nack value */

#define I2C_MASTER_TX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0   /*!< I2C master do not need buffer */


esp_err_t audiobit_poweron_init (void);
esp_err_t audiobit_write_reg (i2c_port_t i2c_num, uint16_t reg_addr, uint16_t reg_val);
esp_err_t audiobit_read_reg (i2c_port_t i2c_num, uint16_t reg_addr, uint16_t *reg_val);
void audiobit_i2c_init();
void audiobit_i2s_init();
void audiobit_play (void);


#ifdef __cplusplus
}
#endif
#endif
