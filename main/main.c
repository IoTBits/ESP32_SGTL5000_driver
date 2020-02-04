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
#include <string.h>
#include "audiobit.h"
#include "sounds.h"
#include "driver/i2s.h"

void audio_play_task (void *pvParameter)
{
    const char *ptr = (const char *)audiobit_music;
    unsigned int offset = 0;
    signed short samples[256], i;
    size_t written;

    // Play array from signed, 16-bit stereo file
    while (offset<NUM_ELEMENTS*2)
    {
        memcpy (samples, ptr+offset, sizeof(samples));

        for (i=0; i<256; i++)
            samples[i] = samples[i] + 32768;
        i2s_write (I2S_NUM, (const char*) samples, 512, &written, portMAX_DELAY);
        offset = offset+512;
    }

    printf("Done playing demo file. Reset to re-play!\n");

    // Silence here
    while (1)
    {
        for (i=0; i<256; i++)
            samples[i] = 0;
        i2s_write (I2S_NUM, (const char*) samples, 512, &written, portMAX_DELAY);
    }
}

void app_main()
{
    ets_delay_us(300000);       // Wait for complete boot-up
    audiobit_i2s_init ();
    ets_delay_us(1000);             // Allow some MCLK cycles for AudioBit

    audiobit_i2c_init ();       // Init interface to AudioBit

    if (audiobit_playback_init () == ESP_OK)
        printf("I2S and I2C setup is completed for playback!\n");
    else
        printf("Seems like AudioBit is not connected properly!\n");

    // Create a task to play audio by loading DMA buffers
    // Not loading in time may cause muting or glitches
    xTaskCreate(&audio_play_task, "audio_play_task", 4096, NULL, 8, NULL);
}
