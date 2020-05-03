/*
 * simplestopwatch
 * Copyright © 2020 Victor Ocampo
 * All rights reserved
 */

#include "sdl-life.h"

// This function initializes the cells as a pointer
bool initCells(void) {
        cells = (cellState_t *)calloc((gridHeight * gridWidth), sizeof(cellState_t));
        if(cells == NULL) {
                SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Memory allocation failed!");
                return false;
        }

        return true;
}

// This function updates the size of the grid based on the new width and height
bool updateCells(int w, int h) {
        blockHeight = roundUp((float)h / (float)(endOfDisplayedGridHeight - startOfDisplayedGridHeight));
        blockWidth = roundUp((float)w / (float)(endOfDisplayedGridWidth - startOfDisplayedGridWidth));

        return false;
}

// Writes to a cell
void writeCell(size_t row, size_t col, cellState_t state) {
        *(cells + (col + (row * gridWidth))) = state;
}

// Reads a cell's state
cellState_t readCell(size_t row, size_t col) {
        return *(cells + (col + (row * gridWidth)));
}

// Returns the amount of cells around a certain cell
uint8_t cellsAroundCell(size_t row, size_t col) {
        uint8_t r, i;

        size_t l[8][2] = {
                {row-1, col-1}, {row-1, col}, {row-1, col+1},
                {row, col-1}, {row, col+1},
                {row+1, col-1}, {row+1, col}, {row+1, col+1}
        };


        r = 0;

        for(i = 0; i < 8; i++) {
                if(row == 0 && (i == 0 || i == 1 || i == 2));
                else if(row == gridHeight && (i == 5 || i == 6 || i == 7));
                else if(col == 0 && (i == 0 || i == 3 || i == 5));
                else if(col == gridWidth && (i == 2 || i == 4 || i == 7));
                else if(readCell(l[i][0], l[i][1]) == live || readCell(l[i][0], l[i][1]) == potentialDead) {
                        r++;
                }
        }

        return r;
}

// This function proceeds to the next generation "enforcing" the rules in some sense
void enforceRules(void) {
        size_t r, c, cellsAround;

        for(r = 25; r < gridHeight; r++) {
                for(c = 0; c < gridWidth; c++) {
                        cellsAround = cellsAroundCell(r, c);

                        if(readCell(r, c) == live) {
                                if(cellsAround < 2) // less than two cells as neighbors results in death (by isolation)
                                        writeCell(r, c, potentialDead);
                                else if(cellsAround >= 4) // more than four cells as neighbors or more results in death (by overpopulation)
                                        writeCell(r, c, potentialDead);
                        }

                        else if(readCell(r, c) == dead) {
                                if(cellsAround == 3)
                                        writeCell(r, c, potentialLive); // exactly three cells as neighbors result in a birth of a new cell
                        }
                }
        }

        // The reason for the state potentialDead / potentialAlive is because when a cell dies it must be still counted as cell that is alive when counting neighbors
        // ... that might mean that potentialLive is useless TODO: if it's useless remove it!
        for(r = 0; r < gridHeight; r++) {
                for(c = 0; c < gridWidth; c++) {
			 if(readCell(r, c) == potentialDead)
                                writeCell(r, c, dead);
                        else if(readCell(r, c) == potentialLive)
                                writeCell(r, c, live);
                }
        }
}

// This displays one cell on the window
void displayCell(cellState_t status, SDL_Rect location) {
        if(status == live) {
                SDL_SetRenderDrawColor(gRenderer, filledR, filledG, filledB, 0);

                if(fillCell)
                        SDL_RenderFillRect(gRenderer, &location);
                else
                        SDL_RenderDrawRect(gRenderer, &location);
        }
        
        else {
                SDL_SetRenderDrawColor(gRenderer, emptyR, emptyG, emptyB, 0);
                SDL_RenderDrawRect(gRenderer, &location);
        }
}

// This displays all of the cells on the window
void displayAllCells(void) {
        size_t x, y;
        SDL_Rect location;

        location.w = blockWidth;
        location.h = blockHeight;
        location.x = 0;
        location.y = 0;

        for(y = startOfDisplayedGridHeight; y < endOfDisplayedGridHeight; y++) {
                for(x = startOfDisplayedGridWidth; x < endOfDisplayedGridWidth; x++) {
                        displayCell(readCell(y, x), location);

                        location.x += location.w;
                }

                location.y += location.h;
                location.x = 0;
        }
}
