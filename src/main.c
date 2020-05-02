/*
 * simplestopwatch
 * Copyright © 2020 Victor Ocampo
 * All rights reserved
 */

/*
 * Here's a short TODO list:
 * 	PRIORITY 1: Add multithreading (one render thread and one event handling thread)	DONE!
 * 	PRIORITY 2: Add RLE file format support
 * 	PRIORITY 3: Add a zoom in and zoom out functionality
 *	PRIORITY 4: Add pause / resume functionality
 * 	PRIORITY 5: Add more comments to the code and generally add more documentation
 *
 *
 * If you can in any way help me do any of these things you are greatly appreciated!
 */

#include "sdl-life.h"

int main(int argc, char *args[]) {
	SDL_Thread *eventThreadID,
		   *renderThreadID;
	int fnameN; // The index of the file name in the args list
	size_t i;

	paused = false;

	fnameN = 0;

	gridHeight = 50;
	gridWidth = 50;

	fpsCap = FPSCAP;

	emptyR = EMPTY_GRID_R;
	emptyG = EMPTY_GRID_G;
	emptyB = EMPTY_GRID_B;

	filledR = FILLED_GRID_R;
	filledG = FILLED_GRID_B;
	filledB = FILLED_GRID_B;

	fillCell = false;

	for(i = 1; i < argc; i++) {
		if(*args[i] == '-') {
			if(!strcmp(args[i], "-gridw"))
				gridWidth = atoi(args[++i]);
			else if(!strcmp(args[i], "-gridh"))
				gridHeight = atoi(args[++i]);
			else if(!strcmp(args[i], "-fps"))
				fpsCap = atoi(args[++i]);
			else if(!strcmp(args[i], "-cellcolor")) {
				i++;
				if(!strcmp(args[i], "black")) {
					filledR = 0x00;
					filledG = 0x00;

					filledB = 0x00;
				}
				else if(!strcmp(args[i], "white")) {
					filledR = 0xff;
					filledG = 0xff;
					filledB = 0xff;
				}
				else if(!strcmp(args[i], "red")) {
					filledR = 0xff;
					filledG = 0x00;
					filledB = 0x00;
				}
				else if(!strcmp(args[i], "green")) {
					filledR = 0x00;
					filledG = 0xff;
					filledB = 0x00;
				}
				else if(!strcmp(args[i], "blue")) {
					filledR = 0x00;
					filledG = 0x00;
					filledB = 0xff;
				}
				else
					usage();
			}
			else if(!strcmp(args[i], "-fillcell")) {
				fillCell = true;
			}
			else if(!strcmp(args[i], "-grid")) {	
				emptyR = 0x55;
				emptyG = 0x55;
				emptyB = 0x55;
			}
			else
				usage();
		}

		else
			fnameN = i;
	}

	if(fnameN == 0) {
		die(__FILE__, __LINE__, "Cell file hasn't been defined");
	}

	if(!initSdl())
		die(__FILE__, __LINE__, "Failed to initialize SDL!");

	if(!initCells())
		die(__FILE__, __LINE__, "Failed to initialize cells!");

	readFile(args[fnameN]);

	quitLoop = false;

	eventThreadID = SDL_CreateThread(eventThread, "event thread", (void *)"Event Thread");
	renderThreadID = SDL_CreateThread(renderThread, "render thread", (void *)"Render Thread");

	while(!quitLoop);

	closeSdl();

	SDL_WaitThread(eventThreadID, NULL);
	SDL_WaitThread(renderThreadID, NULL);

	return EXIT_SUCCESS;
}

bool initSdl(void) {
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "The initialization process has begun");
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Failed to initialize SDL: %s", SDL_GetError());
		return false;
	}

	gWindow = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOWW, WINDOWH, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if(gWindow == NULL) {
		SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Failed to create the window: %s", SDL_GetError());
		return false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(gRenderer == NULL) {
		SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Failed to create the renderer: %s", SDL_GetError());
		return false;
	}

	gLock = SDL_CreateSemaphore(1);

	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "The initialization has finished");

	return true;
}

void closeSdl(void) {
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "SDL is shutting DOWN!");

	SDL_DestroySemaphore(gLock);
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);

	SDL_Quit();
}

// This is the thread that handles all of the events
int eventThread(void *data) {
	SDL_Event e;
	int w, h;

	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "%s is starting...\n", data);

	do {
		while(SDL_PollEvent(&e)) {
			switch(e.type) {
				case SDL_QUIT:
					quitLoop = true;
				case SDL_WINDOWEVENT:
					if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
						// We update the screen here even though this is not the render thread so that the window resizes 
						// independent of the fps cap
						
						SDL_SemWait(gLock); // Semaphore necessary because we are using gRenderer and gWindow

						SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);

						SDL_RenderClear(gRenderer);

						SDL_GetWindowSize(gWindow, &w, &h);
						updateCells(w, h);
						displayAllCells();

						SDL_RenderPresent(gRenderer);

						SDL_SemPost(gLock);
					}
				case SDL_KEYDOWN:
					if(e.key.keysym.sym == SDLK_SPACE && e.key.keysym.mod == KMOD_NONE)
						paused = !paused;
			}
		}
	} while(!quitLoop);

	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "%s has finished\n", data);

	return 0;
}

// This is the thread that renders the cells to the screen
int renderThread(void *data) {
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "%s is starting...\n", data);

	size_t startTime, frameTicks;
	int w, h;

	SDL_SemWait(gLock);

	SDL_GetWindowSize(gWindow, &w, &h);
	updateCells(w, h);

	SDL_SemPost(gLock);

	while(!quitLoop) {
		if(!paused) {
			startTime = SDL_GetTicks();

			SDL_SemWait(gLock); // Semaphore necessary because we are using gRenderer and gWindow

			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);

			SDL_RenderClear(gRenderer);

			displayAllCells();
			enforceRules();

			SDL_RenderPresent(gRenderer);

			SDL_SemPost(gLock);

			frameTicks = SDL_GetTicks() - startTime;
			if(frameTicks < (1000 / fpsCap))
			       SDL_Delay((1000 / fpsCap) - frameTicks);	
		}
	}
	
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "%s has finished\n", data);

	return 0;
}

bool initCells(void) {
	cells = (cellState_t *)calloc((gridHeight * gridWidth), sizeof(cellState_t));
	if(cells == NULL) {
		SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Memory allocation failed!");
		return false;
	}

	return true;
}

bool updateCells(int w, int h) {
	blockHeight = roundUp((float)h / (float)gridHeight);
	blockWidth = roundUp((float)w / (float)gridWidth);

	return false;
}

void writeCell(size_t row, size_t col, cellState_t state) {
	*(cells + (col + (row * gridWidth))) = state;
}

cellState_t readCell(size_t row, size_t col) {
	return *(cells + (col + (row * gridWidth)));
}

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

void enforceRules(void) {
	size_t r, c, cellsAround;

	for(r = 0; r < gridHeight; r++) {
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

void readFile(const char *fname) {
	FILE *cellFile;
	size_t row, col, len;
	char currentCell;

	cellFile = fopen(fname, "r");

	if(cellFile == NULL)
		die(__FILE__, __LINE__, "Couldn't open file: %s\n", fname);

	row = 1;
	col = 1;

	len = strlen(fname);

	//TODO: .rle support!!!
	if((*(fname+(len-3)) == 'r') && (*(fname+(len-2)) == 'l') && (*(fname+(len-1)) == 'e')) {
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "The .rle format is currently not supported!\nThe only supported format is plaintext / .cells");
	}

	while((currentCell = fgetc(cellFile)) != EOF) {
		if(currentCell == '!') {
			while((currentCell = fgetc(cellFile)) != EOF)
				if(currentCell == '\n')
					break;
		}
		if(currentCell == '.') {
			col++;
		}
		if(currentCell == 'O') {
			writeCell(row, col++, live);
		}
		if(currentCell == '\n') {
			row++;
			col = 1;
		}
	}

	fclose(cellFile);
}

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

void displayAllCells(void) {
	size_t x, y;
	SDL_Rect location;

	location.w = blockWidth;
	location.h = blockHeight;
	location.x = 0;
	location.y = 0;

	for(y = 0; y < gridHeight; y++) {
		for(x = 0; x < gridWidth; x++) {
			displayCell(readCell(y, x), location);

			location.x += location.w;
		}

		location.y += location.h;
		location.x = 0;
	}
}
