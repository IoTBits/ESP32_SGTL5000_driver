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

#ifndef _SGTL5000_H_
#define _SGTL5000_H_

#define			SGTL5000_CHIP_ID							0x0000
#define			SGTL5000_CHIP_DIG_POWER						0x0002
#define			SGTL5000_CHIP_CLK_CTRL						0x0004
#define			SGTL5000_CHIP_I2S_CTRL						0x0006
#define			SGTL5000_CHIP_SSS_CTRL						0x000A
#define			SGTL5000_CHIP_ADCDAC_CTRL					0x000E
#define			SGTL5000_CHIP_DAC_VOL						0x0010
#define			SGTL5000_CHIP_PAD_STRENGTH					0x0014
#define			SGTL5000_CHIP_ANA_ADC_CTRL					0x0020
#define			SGTL5000_CHIP_ANA_HP_CTRL					0x0022
#define			SGTL5000_CHIP_ANA_CTRL						0x0024
#define			SGTL5000_CHIP_LINREG_CTRL					0x0026
#define			SGTL5000_CHIP_REF_CTRL						0x0028
#define			SGTL5000_CHIP_MIC_CTRL						0x002A
#define			SGTL5000_CHIP_LINE_OUT_CTRL					0x002C
#define			SGTL5000_CHIP_LINE_OUT_VOL					0x002E
#define			SGTL5000_CHIP_ANA_POWER						0x0030
#define			SGTL5000_CHIP_PLL_CTRL						0x0032
#define			SGTL5000_CHIP_CLK_TOP_CTRL					0x0034
#define			SGTL5000_SHIP_ANA_STATUS					0x0036
#define			SGTL5000_CHIP_ANA_TEST1						0x0038
#define			SGTL5000_CHIP_ANA_TEST2						0x003A
#define			SGTL5000_CHIP_SHORT_CTRL					0x003C
#define			SGTL5000_DAP_CONTROL						0x0100
#define			SGTL5000_DAP_PEQ							0x0102
#define			SGTL5000_DAP_BASS_ENHANCE					0x0104
#define			SGTL5000_DAP_BASS_ENHANCE_CTRL				0x0106
#define			SGTL5000_DAP_AUDIO_EQ						0x0108
#define			SGTL5000_DAP_SGTL_SURROUND					0x010A
#define			SGTL5000_DAP_FILTER_COEF_ACCESS				0x010C
#define			SGTL5000_DAP_COEF_WR_B0_MSB					0x010E
#define			SGTL5000_DAP_COEF_WR_B0_LSB					0x0110
#define			SGTL5000_DAP_AUDIO_EQ_BASS_BAND0			0x0116
#define			SGTL5000_DAP_AUDIO_EQ_BAND1					0x0118
#define			SGTL5000_DAP_AUDIO_EQ_BAND2					0x011A
#define			SGTL5000_DAP_AUDIO_EQ_BAND3					0x011C
#define			SGTL5000_DAP_AUDIO_EQ_TREBLE_BAND4			0x011E
#define			SGTL5000_DAP_MAIN_CHAN						0x0120
#define			SGTL5000_DAP_MIX_CHAN						0x0122
#define			SGTL5000_DAP_AVC_CTRL						0x0124
#define			SGTL5000_DAP_AVC_THRESHOLD					0x0126
#define			SGTL5000_DAP_AVC_ATTACK						0x0128
#define			SGTL5000_DAP_AVC_DECAY						0x012A

#define			SGTL5000_DAP_COEF_WR_B1_MSB					0x012C
#define			SGTL5000_DAP_COEF_WR_B1_LSB					0x012E
#define			SGTL5000_DAP_COEF_WR_B2_MSB					0x0130
#define			SGTL5000_DAP_COEF_WR_B2_LSB					0x0132
#define			SGTL5000_DAP_COEF_WR_A1_MSB					0x0134
#define			SGTL5000_DAP_COEF_WR_A1_LSB					0x0136
#define			SGTL5000_DAP_COEF_WR_A2_MSB					0x0138
#define			SGTL5000_DAP_COEF_WR_A2_LSB					0x013A

#endif
