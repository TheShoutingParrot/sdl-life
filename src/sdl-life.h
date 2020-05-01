/*
 * sdl-life
 * Copyright © 2020 Victor Ocampo
 * All rights reserved
 */

#ifndef __SDL_LIFE_H
#define __SDL_LIFE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define WINDOW_NAME     "sdl-life"
#define WINDOWW         640
#define WINDOWH         480

#define FPSCAP          15              // Default fps to be capped on

#define EMPTY_GRID_R    0x00
#define EMPTY_GRID_G    0x00
#define EMPTY_GRID_B    0x00

#define FILLED_GRID_R   0xff
#define FILLED_GRID_G   0xff
#define FILLED_GRID_B   0xff

typedef enum cellState {
        dead,
        live,
        potentialDead,
        potentialLive
} cellState_t;

SDL_Window              *gWindow;
SDL_Renderer            *gRenderer;

SDL_sem                 *gLock; // This is a general semaphore, used for gRenderer and gWindow

size_t                  blockHeight,    // Height of cell
                        blockWidth,     // Width of cell
                        gridHeight,     // Height of the whole grid
                        gridWidth,      // Width of the whole grid
                        fpsCap;

uint8_t                 emptyR,
                        emptyG,
                        emptyB,
                        filledR,
                        filledG,
                        filledB;

cellState_t             *cells;

bool                    quitLoop,
                        fillCell;

bool initSdl(void);
void closeSdl(void);
SDL_Texture *loadTexture(const char *path);
bool loadMedia(void);

int eventThread(void *data);
int renderThread(void *data);

bool initCells(void);
bool updateCells(int w, int h);
void writeCell(size_t row, size_t col, cellState_t state);
cellState_t readCell(size_t row, size_t col);
uint8_t cellsAroundCell(size_t row, size_t col);
void enforceRules(void);

void readFile(const char *fname);

void displayCell(cellState_t status, SDL_Rect location);
void displayAllCells(void);

int roundUp(float n);

void usage(void);
void die(const char *f, const size_t l, const char *fmt, ...);

#endif
