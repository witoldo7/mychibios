/*
 * Copyright (c) 2013, Mateusz Tomaszkiewicz <silentdemon@gmail.com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _UNLOCKER_H_
#define _UNLOCKER_H_

/* Number of columns and rows displayed.*/
#define UNLOCKER_COLS 3
#define UNLOCKER_ROWS 3

/* Outer size of a ring.*/
#define RING_DIAMETER        40
/* Width of touch-active rectangle.*/
#define RING_ACTIVE_AREA     28

/* Colors.*/
#define BACKGROUND_COLOR     White

#define RING_OUTER_COLOR     Black
#define RING_INNER_COLOR     Black

#define RING_OUTER_HL_COLOR  Yellow
#define RING_INNER_HL_COLOR  Yellow

#define RING_OUTER_ERR_COLOR Red
#define RING_INNER_ERR_COLOR Red

#define RING_OUTER_OK_COLOR  Lime
#define RING_INNER_OK_COLOR  Lime

#define LINE_COLOR           Gray

/* Delay introduced to display comparison result - fail or success.*/
#define RESULT_DELAY_MS 1200

struct unlocker_rings_t {
  coord_t x;
  coord_t y;
  uint8_t checked;
  uint8_t has_prev;
  coord_t prev_x;
  coord_t prev_y;
};

void displayUnlockerSetup(uint8_t* secret_sequence);
void displayUnlocker(uint8_t* secret_sequence);

#endif
