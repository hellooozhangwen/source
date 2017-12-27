/*
 *  Whqx E600GAC reference board support
 *
 *  Copyright (C) 2014 Matthias Schiffer <mschiffer@universe-factory.net>
 *  Copyright (C) 2015 Sven Eckelmann <sven@open-mesh.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "pci.h"



#define E600GAC_GPIO_LED_DS4	  4  /***INTERNET_LED***/
#define E600GAC_GPIO_LED_DS10	  13 /***SYS_LED***/
#define E600GAC_GPIO_LED_LINK2	  15 /*** WAN port4***/
#define E600GAC_GPIO_LED_LINK1	  16 /*** LAN port0***/
#define E600GAC_GPIO_LED3_GREEN	  11 /***green led***/
#define E600GAC_GPIO_LED3_RED	  12 /***red led***/
#define E600GAC_GPIO_LED3_BLUE    14 /***blue led***/
#define E600GAC_GPIO_BIN_RESET    17

/*gpio mux */
#define E600GAC_GPIO_OUT_MUX_LED_LINK5      45
#define E600GAC_GPIO_OUT_MUX_LED_LINK1      41
#define E600GAC_GPIO_OUT_MUX_LED_LINK2      42
#define E600GAC_GPIO_OUT_MUX_LED_LINK3      43
#define E600GAC_GPIO_OUT_MUX_LED_LINK4      44

#define E600GAC_GPIO_BTN_WPS	17

#define E600GAC_KEYS_POLL_INTERVAL	20	/* msecs */
#define E600GAC_KEYS_DEBOUNCE_INTERVAL	(3 * E600GAC_KEYS_POLL_INTERVAL)

#define E600GAC_MAC0_OFFSET	0x1000

static struct gpio_led e600gac_leds_gpio[] __initdata = {
	{
		.name		= "e600gac:green:ds4",
		.gpio		= E600GAC_GPIO_LED_DS4,
		.active_low	= 1,
	},{
		.name		= "e600gac:green:ds10",
		.gpio		= E600GAC_GPIO_LED_DS10,
		.active_low	= 1,
	},{
		.name		= "e600gac:green:link2",
		.gpio		= E600GAC_GPIO_LED_LINK2,
		.active_low	= 1,
	},{
		.name		= "e600gac:green:link1",
		.gpio		= E600GAC_GPIO_LED_LINK1,
		.active_low	= 1,
	},{
		.name		= "e600gac:control:red",
		.gpio		= E600GAC_GPIO_LED3_RED,
		.active_low	= 1,
	},{
		.name		= "e600gac:control:blue",
		.gpio		= E600GAC_GPIO_LED3_BLUE,
		.active_low	= 1,
	},{
		.name		= "e600gac:control:green",
		.gpio		= E600GAC_GPIO_LED3_GREEN,
		.active_low	= 1,
	},
};

static struct gpio_keys_button e600gac_gpio_keys[] __initdata = {
	{
		.desc		= "RESET button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = E600GAC_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= E600GAC_GPIO_BIN_RESET,
		.active_low	= 1,
	},
};

static void __init e600gac_gpio_led_setup(void)
{
	ath79_gpio_direction_select(E600GAC_GPIO_LED_LINK2, true);
	ath79_gpio_direction_select(E600GAC_GPIO_LED_LINK1, true);

	ath79_gpio_output_select(E600GAC_GPIO_LED_LINK2,
			E600GAC_GPIO_OUT_MUX_LED_LINK5);
	ath79_gpio_output_select(E600GAC_GPIO_LED_LINK1,
			E600GAC_GPIO_OUT_MUX_LED_LINK1);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(e600gac_leds_gpio),
			e600gac_leds_gpio);
	ath79_register_gpio_keys_polled(-1, E600GAC_KEYS_POLL_INTERVAL,
			ARRAY_SIZE(e600gac_gpio_keys),
			e600gac_gpio_keys);
}

static void __init e600gac_setup(void)
{
	u8 *mac = (u8 *)KSEG1ADDR(0x1f040400);
        u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_register_m25p80(NULL);

	e600gac_gpio_led_setup();	

	ath79_register_usb();

	ath79_setup_ar933x_phy4_switch(false, false);
	
	ath79_register_wmac(art + E600GAC_MAC0_OFFSET, NULL);
	
	ath79_register_pci();

	ath79_register_mdio(0, 0x0);
	ath79_register_mdio(1, 0x0);

	ath79_init_mac(ath79_eth1_data.mac_addr, mac, 0);
	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 1);

	/* WAN port */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_register_eth(0);

	/* LAN ports */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_switch_data.phy4_mii_en = 1;
	ath79_register_eth(1);
}

MIPS_MACHINE(ATH79_MACH_E600GAC, "E600GAC", "E600GAC", e600gac_setup);
