

#include "ch.h"
#include "hal.h"
#include "gfx.h"
#include "oneWire.h"
#include "ds1820b.h"
#include "unlocker.h"
#include "dht11.h"


#define MAXTIMINGS 85
#define DHT_PIN 4
#define DHTPORT GPIOA
int dht11_dat[5] = {0,0,0,0,0};








#define ARRAY_LEN(a) (sizeof(a)/sizeof(a[0])) 
static OneWireDriver owDrv;
static const OneWireConfig owCfg = { .dqPort = GPIOB,
	                             .dqPad =  10,
	               .dqAlternate = 7,
	                             .uartd = &UARTD3 };
static OneWireRomAddress romAddr[8];


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

// 1 wire thread
static WORKING_AREA(waThread1, 256);
static msg_t Thread1(void *arg) {
	  (void)arg;
		// one wire initialization
		oneWireInit(&owDrv, &owCfg);
		// Search 1 wire bus
		oneWireSearchRom (&owDrv, FALSE, romAddr, ARRAY_LEN(romAddr));

		while (1) {	

	   	for (uint8_t i=0; i< ARRAY_LEN(romAddr); i++) {
			if (romAddr[i].addr[0] == 0x28) {
		  		ds1820BInit (&owDrv, &(romAddr[i]), 10);
		 		const float temp =ds1820BGetTemp (&owDrv, &(romAddr[i]));
				gwinPrintf(ghConsole, "czujnik %d temp =%.2f\r\n", i, temp);      	
				}
		}
			chThdSleepMilliseconds(1000);		
		}

	  return (msg_t)0;
}

static dht11_data last_dht11;

void read_dht11_dat()
{
	
  uint8_t laststate = true;
  uint8_t counter = 0;
  uint8_t j = 0, i;
  float f; // fahrenheit

  dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;
	
	palSetPadMode(DHTPORT, DHT_PIN, PAL_MODE_OUTPUT_PUSHPULL); //output
	palSetPad(DHTPORT, DHT_PIN); //high
	chThdSleepMilliseconds(1000);

	// palSetPadMode(DHTPORT, DHT_PIN, PAL_MODE_OUTPUT_PUSHPULL);
 
	 palClearPad(DHTPORT, DHT_PIN);

	chThdSleepMilliseconds(18);

	palSetPad(DHTPORT, DHT_PIN);

	 palSetPadMode(DHTPORT, DHT_PIN, PAL_MODE_INPUT);
	chThdSleepMicroseconds(40);


  // detect change and read data
  for ( i=0; i< MAXTIMINGS; i++) {
    counter = 0;
    while (palReadPad(DHTPORT, DHT_PIN) == laststate) {
      counter++;
      chThdSleepMicroseconds(1);;
      if (counter == 255) {
        break;
      }
    }
    laststate = palReadPad(DHTPORT, DHT_PIN);

    if (counter == 255) break;

    // ignore first 3 transitions
    if ((i >= 4) && (i%2 == 0)) {
      // shove each bit into the storage bytes
      dht11_dat[j/8] <<= 1;
      if (counter > 16)
        dht11_dat[j/8] |= 1;
      j++;
    }
  }

  // check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
  // print it out if data is good
  if ((j >= 40) && 
      (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF)) ) {
    f = dht11_dat[2] * 9. / 5. + 32;
   gwinPrintf(ghConsole, "Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n", 
            dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f);
  }
  else
  {
 gwinPrintf(ghConsole, "Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n", 
            dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f);
   gwinPrintf(ghConsole,"Data not good, skip\n");
  }
}


int main(void)
{
 // System initialization
  halInit();
  chSysInit();
  // GFX subsystem initaialization
  gfxInit();

  /* Initialize mouse and calibrate it if needed.*/
  //ginputGetMouse(0);

	// Lock screen 
 // uint8_t secret_sequence[UNLOCKER_COLS * UNLOCKER_ROWS] = {1,4,8,6,2,5,7,0,0};
 // displayUnlocker(secret_sequence);


  // Set the widget defaults
  gwinSetDefaultFont(gdispOpenFont("UI2"));
  gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
  gdispClear(Black);

  // Create the widget
  createWidgets();

  // Print to the console
  gwinPrintf(ghConsole, "Witamy w konsoli!\r\n\n");
  gwinPrintf(ghConsole, "Odczyt temp z czujnikow DS18B20 co 1s.\r\n");
  //dht 111 cfg	
	//init_dht11();

  //chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
 // const int maxlen = 40;
  //char buf[maxlen];

  while (TRUE) {
    chThdSleepMilliseconds(1500);
/*
  // DHT-11 local temp
  dht11_status status = read_dht11(&last_dht11);
  if(status == success){
  gwinPrintf(ghConsole, "%02d.%01d,%02d.%01d,",
           last_dht11.humidity_integral, last_dht11.humidity_decimal,
           last_dht11.temp_integral, last_dht11.temp_decimal);
	}
gwinPrintf(ghConsole, "status %d,",status);

  */
read_dht11_dat();
}
return 0;
}






