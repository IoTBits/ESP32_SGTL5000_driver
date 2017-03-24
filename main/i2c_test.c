
#include <stdio.h>
#include "audiobit.h"


/**
 * TEST CODE BRIEF
 *
 * This example will show you how to use I2C module by running two tasks on i2c bus:
 *
 * - read external i2c sensor, here we use a BH1750 light sensor(GY-30 module) for instance.
 * - Use one I2C port(master mode) to read or write the other I2C port(slave mode) on one ESP32 chip.
 *
 * Pin assignment:
 * - master:
 *    GPIO18 is assigned as the data signal of i2c master port
 *    GPIO19 is assigned as the clock signal of i2c master port
 * - no need to add external pull-up resistors, driver will enable internal pull-up resistors.
 */


// Returns ESP_ERR or ESP_FAIL


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
/*
void init_test_sgtl5000 (void)
{
    uint8_t retval=0;
    uint16_t readval;

    retval = audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ID, &readval);
    printf("I2C read returned: %d, err_code: %d\n", readval, retval);

    // NOTE: This next Write call is needed ONLY if VDDD is // externally driven
    // Turn off startup power supplies to save power (Clear bit 12 and 13) 
    // Write CHIP_ANA_POWER 0x4260
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_POWER, 0x4260);
    printf("I2C write, err_code: %d\n", retval);

    //------ Reference Voltage and Bias Current Configuration---------
    // NOTE: The value written in the next 2 Write calls is dependent 
    // on the VDDA voltage value. 
    // Set ground, ADC, DAC reference voltage (bits 8:4). The value should 
    // be set to VDDA/2. This example assumes VDDA = 1.8V. VDDA/2 = 0.9V.
    // The bias current should be set to 50% of the nominal value (bits 3:1) 
    // Write CHIP_REF_CTRL 0x004E
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_REF_CTRL, 0x004E);
    printf("I2C write, err_code: %d\n", retval);


    // Set LINEOUT reference voltage to VDDIO/2 (1.65V) (bits 5:0) and
    // bias current (bits 11:8) to the recommended value of 0.36mA for 
    // 10kOhm load with 1nF capacitance 
    // Write CHIP_LINE_OUT_CTRL 0x0322
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_LINE_OUT_CTRL, 0x0322);
    printf("I2C write, err_code: %d\n", retval);

    //----------------Other Analog Block Configurations-----------------
    // Configure slow ramp up rate to minimize pop (bit 0) 
    // Write CHIP_REF_CTRL 0x004F
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_REF_CTRL, 0x004F);
    printf("I2C write, err_code: %d\n", retval);

    // Enable short detect mode for headphone left/right 
    // and center channel and set short detect current trip level
    // to 75mA 
    // Write CHIP_SHORT_CTRL 0x1106
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_SHORT_CTRL, 0x1106);
    printf("I2C write, err_code: %d\n", retval);

    // Enable Zero-cross detect if needed for HP_OUT (bit 5) and ADC (bit 1) 
    // Write CHIP_ANA_CTRL 0x0133
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_CTRL, 0x0133);
    printf("I2C write, err_code: %d\n", retval);

    //----------------Power up Inputs/Outputs/Digital Blocks------------
    // Power up LINEOUT, HP, ADC, DAC 
    // Write CHIP_ANA_POWER 0x6AFF
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ANA_POWER, 0x6AFF); =========
    printf("I2C write, err_code: %d\n", retval);

    // Power up desired digital blocks 
    // I2S_IN (bit 0), I2S_OUT (bit 1), DAP (bit 4), DAC (bit 5), 
    // ADC (bit 6) are powered on 
    // Write CHIP_DIG_POWER 0x0073
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_DIG_POWER, 0x0073);
    printf("I2C write, err_code: %d\n", retval);

    //--------------------Set LINEOUT Volume Level----------------------
    // Set the LINEOUT volume level based on voltage reference (VAG) 
    // values using this formula 
    // Value = (int)(40*log(VAG_VAL/LO_VAGCNTRL) + 15) 
    // Assuming VAG_VAL and LO_VAGCNTRL is set to 0.9V and 1.65V respectively, the 
    // left LO volume (bits 12:8) and right LO volume (bits 4:0) value should be set 
    // to 5 
    // Write CHIP_LINE_OUT_VOL 0x0505
    retval = audiobit_write_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_LINE_OUT_VOL, 0x0505);
    printf("I2C write, err_code: %d\n", retval);

// Configure SYS_FS clock to 48kHz 
// Configure MCLK_FREQ to 256*Fs 
Modify CHIP_CLK_CTRL->SYS_FS 0x0002 // bits 3:2
Modify CHIP_CLK_CTRL->MCLK_FREQ 0x0000 // bits 1:0
    retval = audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ID, &readval);
    printf("I2C read returned: %d, err_code: %d\n", readval, retval);
// Configure the I2S clocks in master mode 
// NOTE: I2S LRCLK is same as the system sample clock 
Modify CHIP_I2S_CTRL->MS 0x0001 // bit 7
===== write CHIP_I2S_CTRL 0x0008 for all of the above
    retval = audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ID, &readval);
    printf("I2C read returned: %d, err_code: %d\n", readval, retval);

// Example 1: I2S_IN -> DAP -> DAC -> LINEOUT, HP_OUT
// Route I2S_IN to DAP 
Modify CHIP_SSS_CTRL->DAP_SELECT
    retval = audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ID, &readval);
    printf("I2C read returned: %d, err_code: %d\n", readval, retval);
// Route DAP to DAC 
Modify CHIP_SSS_CTRL->DAC_SELECT
0x0001 // bits 7:6 
0x0003 // bits 5:4
===== select DAC input from I2S input, write CHIP_SSS_CTRL 0x0010
    retval = audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ID, &readval);
    printf("I2C read returned: %d, err_code: %d\n", readval, retval);
// Select DAC as the input to HP_OUT 
Modify CHIP_ANA_CTRL->SELECT_HP 0x0000 // bit 6

    retval = audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ID, &readval);
    printf("I2C read returned: %d, err_code: %d\n", readval, retval);
// Enable DAP block 
// NOTE: DAP will be in a pass-through mode if none of DAP 
// sub-blocks are enabled. 
Modify DAP_CONTROL->DAP_EN 0x0001 // bit 0

    retval = audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ID, &readval);
    printf("I2C read returned: %d, err_code: %d\n", readval, retval);
By default the I2S port on the chip is configured for 24-bits of data in I2S format with SCLK set for 64*Fs.
This can be modified by setting various bit-fields in CHIP_I2S_CTRL register.

//---------------- Input Volume Control--------------------
// Configure ADC left and right analog volume to desired default. 
// Example shows volume of 0dB 
Write CHIP_ANA_ADC_CTRL 0x0000
    retval = audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ID, &readval);
    printf("I2C read returned: %d, err_code: %d\n", readval, retval);

//---------------- Volume and Mute Control--------------------
// Configure HP_OUT left and right volume to minimum, unmute 
// HP_OUT and ramp the volume up to desired volume. 
Write CHIP_ANA_HP_CTRL 0x7F7F 
Modify CHIP_ANA_CTRL->MUTE_HP 0x0000 // bit 4
Write CHIP_ANA_HP_CTRL usCurrentVol;
    retval = audiobit_read_reg (AUDIOBIT_I2C_NUM, SGTL5000_CHIP_ID, &readval);
    printf("I2C read returned: %d, err_code: %d\n", readval, retval);

// LINEOUT and DAC volume control 
Modify CHIP_ANA_CTRL->MUTE_LO 0x0000 // bit 8 
// Configure DAC left and right digital volume. Example shows 
// volume of 0dB 
Write CHIP_DAC_VOL 0x3C3C 
Modify CHIP_ADCDAC_CTRL->DAC_MUTE_LEFT 0x0000 // bit 2
Modify CHIP_ADCDAC_CTRL->DAC_MUTE_RIGHT 0x0000 // bit 3

*/