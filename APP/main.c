
#include "ch.h"
#include "hal.h"
#include "test.h"
#include "oneWire.h"
#include "ds1820b.h"
#include "unlocker.h"
#include "gfx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>


 
//kurka 1 wire def
#define ARRAY_LEN(a) (sizeof(a)/sizeof(a[0])) 

static GHandle   ghConsole;
 
static void createWidgets(void) {
	GWidgetInit	wi;
 
	// Apply some default values for GWIN
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	wi.g.show = TRUE;
 
	// Apply the console parameters	
	wi.g.width = 215;
	wi.g.height = 280;
	wi.g.y = 20;
	wi.g.x = 10;
 
	// Create the actual console
	ghConsole = gwinConsoleCreate(NULL, &wi.g);
 
	// Set the console fore- and background colors
	gwinSetColor(ghConsole, Yellow);
	gwinSetBgColor(ghConsole, Black);
 
	// Clear the console
	gwinClear(ghConsole);
}
 
int main(void) {
//	uint16_t i;

	// one wire initialization
	static OneWireDriver owDrv;
	static const OneWireConfig owCfg = { .dqPort = GPIOB,
	                                     .dqPad =  10,
	                                     .dqAlternate = 7,
	                                     .uartd = &UARTD3 };
	static OneWireRomAddress romAddr[8];
	static bool_t searched = FALSE;
	static bool_t initialized = FALSE;
	
	

	// Initialize the uGFX and the underlying system
	gfxInit();

	// Lock screen 	
uint8_t secret_sequence[UNLOCKER_COLS * UNLOCKER_ROWS];
  displayUnlockerSetup(secret_sequence);
 	displayUnlocker(secret_sequence);
	
	// Set the widget defaults
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gdispClear(Black);
 
	// Create the widget
	createWidgets();
 
	// Print to the console
	gwinPrintf(ghConsole, "Witamy w konsoli!\r\n\n");
	gwinPrintf(ghConsole, "Odczyt temp z czujnikow DS18B20.\r\n");
	//initialize ds18b20	
	if (initialized == FALSE) {
	    oneWireInit(&owDrv, &owCfg);
	    initialized = TRUE;
	}

	if (searched == FALSE) {
      		oneWireSearchRom (&owDrv, FALSE, romAddr, ARRAY_LEN(romAddr));
      		searched = TRUE;
	}
	while (1) {	oneWireSearchRom (&owDrv, FALSE, romAddr, ARRAY_LEN(romAddr));

	   	for (uint8_t i=0; i< ARRAY_LEN(romAddr); i++) {
			if (romAddr[i].addr[0] == 0x28) {
		  		ds1820BInit (&owDrv, &(romAddr[i]), 10);
		 		const float temp =ds1820BGetTemp (&owDrv, &(romAddr[i]));
				gwinPrintf(ghConsole, "czujnik %d temp =%.2f\r\n", i, temp);      	
				}
		}
	 
		gfxSleepMilliseconds(800);
	}
	 
	return 0;
}
