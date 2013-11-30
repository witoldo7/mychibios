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

#include "ch.h"
#include "hal.h"
#include "gfx.h"

#include "unlocker.h"

GEventMouse ev;

struct unlocker_rings_t unlocker_rings[UNLOCKER_COLS][UNLOCKER_ROWS];
uint8_t unlock_sequence[UNLOCKER_COLS * UNLOCKER_ROWS];

inline void drawRing(coord_t x, coord_t y)
{
  gdispFillCircle(x, y, RING_DIAMETER / 2,     RING_OUTER_COLOR);
  gdispFillCircle(x, y, RING_DIAMETER / 2 - 3, BACKGROUND_COLOR);
  gdispFillCircle(x, y,                     3, RING_INNER_COLOR);
}

inline void drawRingHL(coord_t x, coord_t y)
{
  gdispFillCircle(x, y, RING_DIAMETER / 2,     RING_OUTER_HL_COLOR);
  gdispFillCircle(x, y, RING_DIAMETER / 2 - 3, BACKGROUND_COLOR);
  gdispFillCircle(x, y,                     3, RING_INNER_HL_COLOR);
}

inline void drawRingError(coord_t x, coord_t y) {
  gdispFillCircle(x, y, RING_DIAMETER / 2,     RING_OUTER_ERR_COLOR);
  gdispFillCircle(x, y, RING_DIAMETER / 2 - 3, BACKGROUND_COLOR);
  gdispFillCircle(x, y,                     3, RING_INNER_ERR_COLOR);
}

inline void drawRingOk(coord_t x, coord_t y)
{
  gdispFillCircle(x, y, RING_DIAMETER / 2,     RING_OUTER_OK_COLOR);
  gdispFillCircle(x, y, RING_DIAMETER / 2 - 3, BACKGROUND_COLOR);
  gdispFillCircle(x, y,                     3, RING_INNER_OK_COLOR);
}

/*
 * Redraws lines after each ring is checked.
 */
void redrawLines(void)
{
  uint8_t i, j;
  i = 0;
  while (i < UNLOCKER_COLS) {
    j = 0;
    while (j < UNLOCKER_ROWS) {
      if (unlocker_rings[i][j].has_prev
        && unlocker_rings[i][j].prev_x
        && unlocker_rings[i][j].prev_y)
      {
          gdispDrawLine(
            unlocker_rings[i][j].prev_x, unlocker_rings[i][j].prev_y,
            unlocker_rings[i][j].x, unlocker_rings[i][j].y,
            LINE_COLOR);
          if (unlocker_rings[i][j].prev_x == unlocker_rings[i][j].x) {
              gdispDrawLine(
                unlocker_rings[i][j].prev_x - 1, unlocker_rings[i][j].prev_y,
                unlocker_rings[i][j].x - 1, unlocker_rings[i][j].y,
                LINE_COLOR);
              gdispDrawLine(
                unlocker_rings[i][j].prev_x + 1, unlocker_rings[i][j].prev_y,
                unlocker_rings[i][j].x + 1, unlocker_rings[i][j].y,
                LINE_COLOR);
          } else {
            gdispDrawLine(
              unlocker_rings[i][j].prev_x, unlocker_rings[i][j].prev_y - 1,
              unlocker_rings[i][j].x, unlocker_rings[i][j].y - 1,
              LINE_COLOR);
            gdispDrawLine(
              unlocker_rings[i][j].prev_x, unlocker_rings[i][j].prev_y + 1,
              unlocker_rings[i][j].x, unlocker_rings[i][j].y + 1,
              LINE_COLOR);
          }
          gdispFillCircle(unlocker_rings[i][j].x, unlocker_rings[i][j].y,           3, LINE_COLOR);
          gdispFillCircle(unlocker_rings[i][j].prev_x, unlocker_rings[i][j].prev_y, 3, LINE_COLOR);
      }
      j++;
    }
    i++;
  }
}

void drawRingsError(void)
{
  gdispClear(BACKGROUND_COLOR);

  uint8_t i, j;
  i = 0;
  while (i < UNLOCKER_COLS) {
    j = 0;
    while (j < UNLOCKER_ROWS) {
      drawRingError(unlocker_rings[i][j].x, unlocker_rings[i][j].y);
      j++;
    }
    i++;
  }
}

void drawRingsOk(void)
{
  gdispClear(BACKGROUND_COLOR);

  uint8_t i, j;
  i = 0;
  while (i < UNLOCKER_COLS) {
    j = 0;
    while (j < UNLOCKER_ROWS) {
      drawRingOk(unlocker_rings[i][j].x, unlocker_rings[i][j].y);
      j++;
    }
    i++;
  }
}

/*
 * resetRings() redraws rings and resets unlocker_rings structure.
 * Position of each ring is calculated with respect to width and height
 * of the screen.
 */
void resetRings(void)
{
  /* Calculating positions & drawing points.*/
  gdispClear(BACKGROUND_COLOR);

  coord_t height = gdispGetHeight();
  coord_t width = gdispGetWidth();

  coord_t start_x, start_y, delta;

  if (height < width) {
    delta = height / (UNLOCKER_ROWS + 1);
    start_x = (width - (delta * (UNLOCKER_COLS - 1))) / 2;
    start_y = delta;
  } else {
    delta = width / (UNLOCKER_COLS + 1);
    start_x = delta;
    start_y = (height - (delta * (UNLOCKER_ROWS - 1))) / 2;
  }

  uint8_t i, j;
  i = 0;
  while (i < UNLOCKER_COLS) {
    j = 0;
    while (j < UNLOCKER_ROWS) {
      unlocker_rings[i][j].x = start_x + delta * i;
      unlocker_rings[i][j].y = start_y + delta * j;
      unlocker_rings[i][j].checked = 0;
      unlocker_rings[i][j].has_prev = 0;
      unlocker_rings[i][j].prev_x = 0;
      unlocker_rings[i][j].prev_y = 0;

      drawRing(unlocker_rings[i][j].x, unlocker_rings[i][j].y);

      unlock_sequence[i + j * UNLOCKER_COLS] = 0;

      j++;
    }
    i++;
  }
}

/*
 * Main unlocker function.
 */
void unlocker(uint8_t* secret_sequence, uint8_t save_sequence)
{
  resetRings();

  uint8_t i, j;
  coord_t last_x = 0, last_y = 0;
  uint8_t last_i = 0, last_j = 0;
  uint8_t unlock_cnt = 0;

  while (TRUE) {
    ginputGetMouseStatus(0, &ev);

    /* If button (touch) is up.*/
    if (!(ev.current_buttons & GINPUT_MOUSE_BTN_LEFT)) {
      if (last_x != 0 && last_y != 0) {

        /* If we run in setup mode, save sequence and exit.*/
        i = 0;
        if (save_sequence) {
          while (i < UNLOCKER_COLS * UNLOCKER_ROWS) {
            secret_sequence[i] = unlock_sequence[i];
            i++;
          }
          drawRingsOk();
          gfxSleepMilliseconds(RESULT_DELAY_MS);
          break;
        }

        /* Sequence comparison: just drawn with secret one.*/
        i = 0;
        while (i < UNLOCKER_COLS * UNLOCKER_ROWS) {
          if (secret_sequence[i] != unlock_sequence[i]) {
            break;
          }
          i++;
        }

        /* If they don't match.*/
        if (i != UNLOCKER_COLS * UNLOCKER_ROWS) {
          drawRingsError();
          gfxSleepMilliseconds(RESULT_DELAY_MS);
          last_x = 0;
          last_y = 0;
          last_i = 0;
          last_j = 0;
          unlock_cnt = 0;
          resetRings();
          continue;
        } else {
          drawRingsOk();
          gfxSleepMilliseconds(RESULT_DELAY_MS);
          break;
        }
      } else {
        continue;
      }
    }

    /* Button still down.*/
    i = 0;
    while (i < UNLOCKER_COLS) {
      j = 0;
      while (j < UNLOCKER_ROWS) {
        if (unlocker_rings[i][j].checked == 0
          && unlocker_rings[i][j].x - RING_ACTIVE_AREA / 2 < ev.x
          && unlocker_rings[i][j].x + RING_ACTIVE_AREA / 2 > ev.x
          && unlocker_rings[i][j].y - RING_ACTIVE_AREA / 2 < ev.y
          && unlocker_rings[i][j].y + RING_ACTIVE_AREA / 2 > ev.y)
        {
          unlocker_rings[i][j].checked = 1;

          drawRingHL(unlocker_rings[i][j].x, unlocker_rings[i][j].y);

          if (last_x != 0 && last_y != 0) {
            unlocker_rings[i][j].prev_x = last_x;
            unlocker_rings[i][j].prev_y = last_y;
            unlocker_rings[i][j].has_prev = last_i + last_j * UNLOCKER_ROWS + 1;
            redrawLines();
          }
          last_x = unlocker_rings[i][j].x;
          last_y = unlocker_rings[i][j].y;
          last_i = i;
          last_j = j;

          unlock_sequence[unlock_cnt++] = i + j * UNLOCKER_ROWS + 1;
        }
        j++;
      }
      i++;
    }
  }
  return;
}

/*
 * Displays unlocker that saves drawn pattern for further comparison.
 */
void displayUnlockerSetup(uint8_t* secret_sequence)
{
  unlocker(secret_sequence, 1);
}

/*
 * Just displays unlocker.
 */
void displayUnlocker(uint8_t* secret_sequence)
{
  unlocker(secret_sequence, 0);
}
