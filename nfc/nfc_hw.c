/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <hardware/hardware.h>
#include <hardware/nfc.h>

#if 0
static uint8_t pn544_eedata_settings[][4] = {
    // DIFFERENTIAL_ANTENNA

    // RF Settings
    {0x00, 0x9B, 0xD1, 0x0D}, // Tx consumption higher than 0x0D (average 50mA)
    {0x00, 0x9B, 0xD2, 0x24}, // GSP setting for this threshold
    {0x00, 0x9B, 0xD3, 0x0A}, // Tx consumption higher than 0x0A (average 40mA)
    {0x00, 0x9B, 0xD4, 0x22}, // GSP setting for this threshold
    {0x00, 0x9B, 0xD5, 0x08}, // Tx consumption higher than 0x08 (average 30mA)
    {0x00, 0x9B, 0xD6, 0x1E}, // GSP setting for this threshold
    {0x00, 0x9B, 0xDD, 0x1C}, // GSP setting for this threshold
    {0x00, 0x9B, 0x84, 0x13}, // ANACM2 setting

    // Enable PBTF
    {0x00, 0x98, 0x00, 0x3F}, // SECURE_ELEMENT_CONFIGURATION - No Secure Element
    {0x00, 0x9F, 0x0A, 0x05}, // SWP_PBTF_RFLD  --> RFLEVEL Detector for PBTF
    {0x00, 0x9E, 0xD1, 0xA1}, //

    // Change RF Level Detector ANARFLDWU
    {0x00, 0x99, 0x23, 0x00}, // Default Value is 0x01

    // Low-power polling
    {0x00, 0x9E, 0x74, 0xB0}, // Default Value is 0x00, bits 0->2: sensitivity (0==max, 6==min),
                              // bit 3: RFU,
                              // bits 4,5 hybrid low-power: # of low-power polls per regular poll
                              // bit 6: RFU
                              // bit 7: (0 -> disabled, 1 -> enabled)
    {0x00, 0x9F, 0x28, 0x01}, // bits 0->7: # of measurements per low-power poll

    // Polling Loop - Card Emulation Timeout
    {0x00, 0x9F, 0x35, 0x14}, // Time for which PN544 stays in Card Emulation mode after leaving RF field
    {0x00, 0x9F, 0x36, 0x60}, // Default value 0x0411 = 50 ms ---> New Value : 0x1460 = 250 ms

    //LLC Timer
    {0x00, 0x9C, 0x31, 0x00}, // Guard host time-out in ms (MSB)
    {0x00, 0x9C, 0x32, 0xC8}, // Guard host time-out in ms (LSB)
    {0x00, 0x9C, 0x19, 0x40}, // Max RX retry (PN544=>host?)
    {0x00, 0x9C, 0x1A, 0x40}, // Max TX retry (PN544=>host?)

    {0x00, 0x9C, 0x0C, 0x00}, //
    {0x00, 0x9C, 0x0D, 0x00}, //
    {0x00, 0x9C, 0x12, 0x00}, //
    {0x00, 0x9C, 0x13, 0x00}, //

    //WTX for LLCP communication
    {0x00, 0x98, 0xA2, 0x0E}, // Max value: 14 (default value: 09)

    //SE GPIO
    {0x00, 0x98, 0x93, 0x40},

    // Set NFCT ATQA
    {0x00, 0x98, 0x7D, 0x02},
    {0x00, 0x98, 0x7E, 0x00},

    // Set NFC-F poll RC=0x00
    {0x00, 0x9F, 0x9A, 0x00},
};
#endif

/*
 *	implemented from stock rom nfc by @munjeni 2017
 */

static uint8_t EEDATA_Settings_Common[][4] = {
	{0x00, 0x9B, 0xD1, 0x0D},
	{0x00, 0x9B, 0xD2, 0x36},
	{0x00, 0x9B, 0xD3, 0x0A},
	{0x00, 0x9B, 0xD4, 0x22},
	{0x00, 0x9B, 0xD5, 0x08},
	{0x00, 0x9B, 0xD6, 0x1E},
	{0x00, 0x9B, 0xDD, 0x1C},
	{0x00, 0x9B, 0x84, 0x13},
	{0x00, 0x98, 0x00, 0x3E},
	{0x00, 0x9F, 0x0A, 0x05},
	{0x00, 0x9E, 0xD1, 0xA1},
	{0x00, 0x99, 0x23, 0x00},
	{0x00, 0x9E, 0x74, 0xB0},
	{0x00, 0x9F, 0x28, 0x01},
	{0x00, 0x9F, 0x35, 0x14},
	{0x00, 0x9F, 0x36, 0x60},
	{0x00, 0x9C, 0x31, 0x00},
	{0x00, 0x9C, 0x32, 0x00},
	{0x00, 0x9C, 0x19, 0x40},
	{0x00, 0x9C, 0x1A, 0x40},
	{0x00, 0x9C, 0x0C, 0x00},
	{0x00, 0x9C, 0x0D, 0x00},
	{0x00, 0x9C, 0x12, 0x00},
	{0x00, 0x9C, 0x13, 0x00},
	{0x00, 0x98, 0xA2, 0x0E},
	{0x00, 0x98, 0x93, 0x40},
	{0x00, 0x98, 0x7D, 0x02},
	{0x00, 0x98, 0x7E, 0x00},
	{0x00, 0x9F, 0x9A, 0x00},
	{0x00, 0x98, 0x05, 0x05},
	{0x00, 0x98, 0x06, 0x05},
	{0x00, 0x98, 0x07, 0x20},
	{0x00, 0x98, 0x9D, 0x27},
	{0x00, 0x98, 0x9E, 0x3F},
	{0x00, 0x98, 0x9F, 0x2D},
	{0x00, 0x98, 0x01, 0x17},
	{0x00, 0x99, 0x77, 0xFF},
	{0x00, 0x99, 0x7A, 0x3E},
	{0x00, 0x99, 0x29, 0xFF},
	{0x00, 0x99, 0x2A, 0xFF},
	{0x00, 0x99, 0x2B, 0xFF},
	{0x00, 0x9B, 0x85, 0xFF},
	{0x00, 0x9B, 0x18, 0x00},
	{0x00, 0x9F, 0x1B, 0x00},
	{0x00, 0x9F, 0x1F, 0x00},
	{0x00, 0x9F, 0x1C, 0x01},
	{0x00, 0x9F, 0x20, 0x01},
};

static uint8_t EEDATA_Settings_C2[][4] = {
	{0x00, 0x9F, 0x09, 0x00},
	{0x00, 0x9F, 0xC8, 0x01},
	{0x00, 0x98, 0x08, 0xFF},
	{0x00, 0x9B, 0xF5, 0x22},
	{0x00, 0x9B, 0xF6, 0x22},
	{0x00, 0x9B, 0xF7, 0x33},
	{0x00, 0x9B, 0xF8, 0x55},
};

static uint8_t EEDATA_Settings_C3[][4] = {
	{0x00, 0x9B, 0x65, 0xFB},
	{0x00, 0x9C, 0x05, 0x04},
	{0x00, 0x9C, 0x29, 0x04},
	{0x00, 0x9E, 0x77, 0x01},
	{0x00, 0x9E, 0x90, 0x80},
	{0x00, 0x9F, 0x19, 0x60},
	{0x00, 0x9F, 0x1A, 0x00},
	{0x00, 0x9E, 0xB9, 0x00},
	{0x00, 0x9F, 0xA8, 0x00},
	{0x00, 0x9F, 0xAA, 0x00},
	{0x00, 0x9F, 0xAC, 0x01},
};

static uint8_t EEDATA_Settings_C3_EasyCard[][4] = {
	{0x00, 0x9B, 0x65, 0xFB},
	{0x00, 0x9C, 0x05, 0x04},
	{0x00, 0x9C, 0x29, 0x04},
	{0x00, 0x9E, 0x77, 0x01},
	{0x00, 0x9E, 0x90, 0x80},
	{0x00, 0x9F, 0x19, 0xFF},
	{0x00, 0x9F, 0x1A, 0xFF},
	{0x00, 0x9E, 0xB9, 0x00},
	{0x00, 0x9F, 0xA8, 0x00},
	{0x00, 0x9F, 0xAA, 0x00},
	{0x00, 0x9F, 0xAC, 0x01},
};

static int pn544_close(hw_device_t *dev) {
	free(dev);

	return 0;
}

static int nfc_open(const struct hw_module_t *module, const char *name, struct hw_device_t **device)
{
	if (strcmp(name, NFC_PN544_CONTROLLER) == 0)
	{
		nfc_pn544_device_t *dev = calloc(1, sizeof(nfc_pn544_device_t));

		if (dev == NULL)
			return -12;

		dev->common.tag = HARDWARE_DEVICE_TAG;
		dev->common.version = 0;
		dev->common.module = (struct hw_module_t *)module;
		dev->common.module->reserved[0] = (uint32_t)(sizeof(EEDATA_Settings_C2) / 4);
		dev->common.module->reserved[1] = (uint32_t)&EEDATA_Settings_C2;
		dev->common.module->reserved[2] = (uint32_t)(sizeof(EEDATA_Settings_C3) / 4);
		dev->common.module->reserved[3] = (uint32_t)&EEDATA_Settings_C3;
		dev->common.module->reserved[4] = (uint32_t)(sizeof(EEDATA_Settings_C3_EasyCard) / 4);
		dev->common.module->reserved[5] = (uint32_t)&EEDATA_Settings_C3_EasyCard;
		dev->common.close = pn544_close;
		dev->num_eeprom_settings = sizeof(EEDATA_Settings_Common) / 4;
		dev->eeprom_settings = (uint8_t *)EEDATA_Settings_Common;
		dev->linktype = PN544_LINK_TYPE_I2C;
		dev->device_node = "/dev/pn544";
		dev->enable_i2c_workaround = 0;
		dev->i2c_device_address = 0;
		*device = (struct hw_device_t *)dev;

		return 0;
	}
	else
		return -EINVAL;
}

static struct hw_module_methods_t nfc_module_methods = {
	.open = nfc_open,
};

struct nfc_module_t HAL_MODULE_INFO_SYM = {
	.common = {
		.tag = HARDWARE_MODULE_TAG,
		.version_major = 1,
		.version_minor = 0,
		.id = NFC_HARDWARE_MODULE_ID,
		.name = "Sony stockrom NFC HW HAL",
		.author = "The Android Open Source Project",
		.methods = &nfc_module_methods,
	},
};


