/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ch.h"
#include "hal.h"
#include "test.h"

#include "chprintf.h"
#include "shell.h"

#include "usbcfg.h"
#include "oneWire.h"
#include "ds1820b.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>


/* Virtual serial port over USB.*/
static SerialUSBDriver SDU1;

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WA_SIZE(2048)
#define TEST_WA_SIZE    THD_WA_SIZE(256)
#define ARRAY_LEN(a) (sizeof(a)/sizeof(a[0])) 

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]) {
  size_t n, size;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: mem\r\n");
    return;
  }
  n = chHeapStatus(NULL, &size);
  chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
  chprintf(chp, "heap fragments   : %u\r\n", n);
  chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]) {
  static const char *states[] = {THD_STATE_NAMES};
  Thread *tp;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: threads\r\n");
    return;
  }
  chprintf(chp, "    addr    stack prio refs     state time\r\n");
  tp = chRegFirstThread();
  do {
    chprintf(chp, "%.8lx %.8lx %4lu %4lu %9s %lu\r\n",
            (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
            (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
            states[tp->p_state], (uint32_t)tp->p_time);
    tp = chRegNextThread(tp);
  } while (tp != NULL);
}

static void cmd_test(BaseSequentialStream *chp, int argc, char *argv[]) {
  Thread *tp;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: test\r\n");
    return;
  }
  tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriority(),
                           TestThread, chp);
  if (tp == NULL) {
    chprintf(chp, "out of memory\r\n");
    return;
  }
  chThdWait(tp);
}

static OneWireDriver owDrv;
static const OneWireConfig owCfg = { .dqPort = GPIOD,
				     .dqPad =  GPIOD_PIN8,
				     .dqAlternate = 7,
				     .uartd = &UARTD3 };
static OneWireRomAddress romAddr[8];
static bool_t searched = FALSE;
static bool_t initialized = FALSE;
static void cmd_oneWire(BaseSequentialStream *chp, int argc, char *argv[]) {
  if ((argc != 1) && (argc != 2))
    goto usage;
  
  if (initialized == FALSE) {
    oneWireInit(&owDrv, &owCfg);
    initialized = TRUE;
  }

  if (strcmp (argv[0], "rom") == 0) {
    OneWireRomAddress rom;
    bool_t res = oneWireReadRom (&owDrv, &rom);
    chprintf(chp, "read rom : 0x%x 0x%x 0x%x 0x%x     0x%x 0x%x 0x%x 0x%x %s\r\n",
		rom.addr[0], rom.addr[1], rom.addr[2], rom.addr[3], rom.addr[4], 
		rom.addr[5], rom.addr[6], rom.addr[7],
		res ? "SUCCESS" : "FAILED"); 
  } else if (strcmp (argv[0], "t") == 0) {   
    if (searched == FALSE) {
      oneWireSearchRom (&owDrv, FALSE, romAddr, ARRAY_LEN(romAddr));
      searched = TRUE;
    }
    uint8_t prec = 12;
    if (argc == 2) {
      prec = atoi (argv[1]);
    }
    
    for (uint8_t i=0; i< ARRAY_LEN(romAddr); i++) {
      if (romAddr[i].addr[0] == 0x28) {
	  ds1820BInit (&owDrv, &(romAddr[i]), prec);
	  const float temp =ds1820BGetTemp (&owDrv, &(romAddr[i]));
	  chprintf(chp, "sensor %d temp =%.2f\r\n", i, temp); 
      }
    }
  } else if (strcmp (argv[0], "dt") == 0) {   
    if (searched == FALSE) {
      oneWireSearchRom (&owDrv, FALSE, romAddr, ARRAY_LEN(romAddr));
      searched = TRUE;
    }
    uint8_t prec = 12;
    if (argc == 2) {
      prec = atoi (argv[1]);
    }
    
    for (uint8_t i=0; i< ARRAY_LEN(romAddr); i++) {
      if (romAddr[i].addr[0] == 0x28) {
	ds1820BInit (&owDrv, &(romAddr[i]), prec);
	ds1820BAskTemp (&owDrv, &(romAddr[i]));
      }
    }
    
    chThdSleepMilliseconds(750);

   for (uint8_t i=0; i< ARRAY_LEN(romAddr); i++) {
      if (romAddr[i].addr[0] == 0x28) {
	const float temp = ds1820BGGetTempFromRam (&owDrv, &(romAddr[i]));
	chprintf(chp, "sensor %d temp(ram) =%.2f\r\n", i, temp); 
      }
    }





  } else if (strcmp (argv[0], "search") == 0) {
    bool_t cond = FALSE;
    if (argc == 2) {
      cond  = atoi (argv[1]);
    }
    oneWireSearchRom (&owDrv, cond, romAddr, ARRAY_LEN(romAddr));
    searched = TRUE;
    for (uint8_t i=0; i< ARRAY_LEN(romAddr); i++) {
      chprintf(chp, "addr[%d] = 0x%x 0x%x 0x%x 0x%x     0x%x 0x%x 0x%x 0x%x\r\n",
	       i, romAddr[i].addr[0], romAddr[i].addr[1], romAddr[i].addr[2], romAddr[i].addr[3], 
	       romAddr[i].addr[4], romAddr[i].addr[5], romAddr[i].addr[6], romAddr[i].addr[7]);
    }
  } else {
    goto usage;
  }
  return;
 usage:
  chprintf (chp, "Usage: ow rom, search, temp\r\n");
}

static const ShellCommand commands[] = {
  {"mem", cmd_mem},
  {"threads", cmd_threads},
  {"test", cmd_test},
  {"ow", cmd_oneWire},
  {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SDU1,
  commands
};



/*===========================================================================*/
/* Initialization and main thread.                                           */
/*===========================================================================*/

/*
 * Application entry point.
 */
int main(void) {
  Thread *shelltp = NULL;

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Shell manager initialization.
   */
  shellInit();

  /*
   * Initializes a serial-over-USB CDC driver.
   */
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  /*
   * Activates the USB driver and then the USB bus pull-up on D+.
   * Note, a delay is inserted in order to not have to disconnect the cable
   * after a reset.
   */
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1000);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);


  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state, when the button is
   * pressed the test procedure is launched with output on the serial
   * driver 2.
   */
  while (TRUE) {
    palTogglePad(GPIOD, GPIOD_LED4);
    if (!shelltp) {
      if (SDU1.config->usbp->state == USB_ACTIVE) {
        /* Spawns a new shell.*/
        shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
      }
    }
    else {
      /* If the previous shell exited.*/
      if (chThdTerminated(shelltp)) {
        /* Recovers memory of the previous shell.*/
        chThdRelease(shelltp);
        shelltp = NULL;
      }
    }
    chThdSleepMilliseconds(500);
  }
}
