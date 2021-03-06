/*
 * simplestopwatch
 * Copyright © 2020 Victor Ocampo
 * All rights reserved
 */

#include "sdl-life.h"

int main(int argc, char *args[]) {
	SDL_Thread *renderThreadID;
	SDL_Event e;
	int fnameN, // The index of the file name in the args list
	    ruleN; // The index of the rule string in the args list
	size_t i;

	paused = true;

	fnameN = 0;
	ruleN = 0; 

	startOfDisplayedGridHeight = 50;
	startOfDisplayedGridWidth = 50;
	endOfDisplayedGridHeight = 100;
	endOfDisplayedGridWidth = 100;
	gridHeight = 200;
	gridWidth = 200;

	fpsCap = FPSCAP;

	emptyR = EMPTY_GRID_R;
	emptyG = EMPTY_GRID_G;
	emptyB = EMPTY_GRID_B;

	filledR = FILLED_GRID_R;
	filledG = FILLED_GRID_B;
	filledB = FILLED_GRID_B;

	fillCell = false;

	// This for loop parses through the command line arguments
	for(i = 1; i < argc; i++) {
		if(*args[i] == '-') {
			if(!strcmp(args[i], "-fps"))
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
			else if(!strcmp(args[i], "-f") || !strcmp(args[i], "-file")) {
				i++;
				fnameN = i;
			}
			else if(!strcmp(args[i], "-rulestring") || !strcmp(args[i], "-rules") || !strcmp(args[i], "-r")) {
				i++;
				ruleN = i;
			}
			else
				usage();
		}

		else
			usage();
	}

	if(!initSdl())
		die(__FILE__, __LINE__, "Failed to initialize SDL!");

	if(!initCells())
		die(__FILE__, __LINE__, "Failed to initialize cells!");

	if(fnameN != 0) 
		readFile(args[fnameN]);

	if(ruleN != 0)
		setRules(args[ruleN]);
	else
		setRules(NULL);

	quitLoop = false;

	renderThreadID = SDL_CreateThread(renderThread, "render thread", (void *)"Render Thread");

	while(!quitLoop) {
		while(SDL_PollEvent(&e)) {
			switch(e.type) {
				case SDL_QUIT:
					quitLoop = true;
				case SDL_WINDOWEVENT:
					if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
						// We update the screen here even though this is not the render thread so that the window resizes 
						// independent of the fps cap. Actually the FPS cap is more of a "gps" cap: generations per second cap.
						
						resizeWindow();
					}
					break;
				case SDL_KEYDOWN:
					if(e.key.keysym.sym == SDLK_SPACE && e.key.keysym.mod == KMOD_NONE) {
						paused = !paused;

						SDL_SemWait(gLock); // Semaphore necessary because we are using gRenderer and gWindow

						SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);

						SDL_RenderClear(gRenderer);

						displayAllCells();

						SDL_RenderPresent(gRenderer);

						SDL_SemPost(gLock);

					}
					else if(e.key.keysym.sym == SDLK_w && e.key.keysym.mod == KMOD_NONE && startOfDisplayedGridHeight > 0) {
						startOfDisplayedGridHeight--;
						endOfDisplayedGridHeight--;

						resizeWindow();
					}
					else if(e.key.keysym.sym == SDLK_s && e.key.keysym.mod == KMOD_NONE && endOfDisplayedGridHeight < gridHeight) {
						startOfDisplayedGridHeight++;
						endOfDisplayedGridHeight++;

						resizeWindow();
					}
					else if(e.key.keysym.sym == SDLK_a && e.key.keysym.mod == KMOD_NONE && startOfDisplayedGridWidth > 0) {
						startOfDisplayedGridWidth--;
						endOfDisplayedGridWidth--;

						resizeWindow();
					}
					else if(e.key.keysym.sym == SDLK_d && e.key.keysym.mod == KMOD_NONE && endOfDisplayedGridWidth < gridWidth) {
						startOfDisplayedGridWidth++;
						endOfDisplayedGridWidth++;

						resizeWindow();
					}
					else if(e.key.keysym.sym == SDLK_PLUS && (endOfDisplayedGridHeight - startOfDisplayedGridHeight) > 1 
							&& (endOfDisplayedGridWidth - startOfDisplayedGridWidth) > 2 &&
							e.key.keysym.mod == KMOD_NONE) {
						zoomIn();

						resizeWindow();
					}
					else if(e.key.keysym.sym == SDLK_MINUS && startOfDisplayedGridHeight > 0 && startOfDisplayedGridWidth > 0 &&
							e.key.keysym.mod == KMOD_NONE) {
						zoomOut();

						resizeWindow();
					}

					break;
				case SDL_MOUSEWHEEL:
					if(e.wheel.y > 0 && (endOfDisplayedGridHeight - startOfDisplayedGridHeight) > 1 
							&& (endOfDisplayedGridWidth - startOfDisplayedGridWidth) > 2) {
						zoomIn();
					}
					else if(e.wheel.y < 0 && startOfDisplayedGridHeight > 0 && startOfDisplayedGridWidth > 0) {
						zoomOut();
					}

					resizeWindow();

					break;
				case SDL_MOUSEBUTTONDOWN:
					if(e.button.button == SDL_BUTTON_LEFT) {
						writeCell((e.button.y / blockHeight) + startOfDisplayedGridHeight, (e.button.x / blockWidth) + startOfDisplayedGridWidth, live);

						SDL_SemWait(gLock); // Semaphore necessary because we are using gRenderer and gWindow

						SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);

						SDL_RenderClear(gRenderer);

						displayAllCells();

						SDL_RenderPresent(gRenderer);

						SDL_SemPost(gLock);
					}
					else if(e.button.button == SDL_BUTTON_RIGHT) {
						writeCell((e.button.y / blockHeight) + startOfDisplayedGridHeight, (e.button.x / blockWidth) + startOfDisplayedGridWidth, dead);

						SDL_SemWait(gLock); // Semaphore necessary because we are using gRenderer and gWindow

						SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);

						SDL_RenderClear(gRenderer);

						displayAllCells();

						SDL_RenderPresent(gRenderer);

						SDL_SemPost(gLock);
					}

					break;
			}
		}

		SDL_Delay(500);
	}

	closeSdl();

	SDL_WaitThread(renderThreadID, NULL);

	return EXIT_SUCCESS;
}

// This function simply initializes SDL, creates the window (gWindow), creates the renderer to be used with the window (gRenderer) and finally creates a general semaphore (gLock)
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

// This function simply closes SDL by freeing/destroying some memory and quitting SDL
void closeSdl(void) {
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "SDL is shutting DOWN!");

	SDL_DestroySemaphore(gLock);
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);

	SDL_Quit();
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

		else {
			SDL_Delay(1000);
		}
	}
	
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "%s has finished\n", data);

	return 0;
}

void resizeWindow(void) {
	int w, h;

	SDL_SemWait(gLock); // Semaphore necessary because we are using gRenderer and gWindow

	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);

	SDL_RenderClear(gRenderer);

	SDL_GetWindowSize(gWindow, &w, &h);

	updateCells(w, h);
	displayAllCells();

	SDL_RenderPresent(gRenderer);

	SDL_SemPost(gLock);
}

void zoomIn(void) {
	startOfDisplayedGridHeight++;
	startOfDisplayedGridWidth++;
	endOfDisplayedGridHeight--;
	endOfDisplayedGridWidth--;
}

void zoomOut(void) {
	startOfDisplayedGridHeight--;
	startOfDisplayedGridWidth--;
	endOfDisplayedGridHeight++;
	endOfDisplayedGridWidth++;
}

// This function reads the cell file, currently can only read a plaintext cell (.cells) file
void readFile(const char *fname) {
	FILE *cellFile;
	size_t row, col, len;
	char currentCell;

	cellFile = fopen(fname, "r");

	if(cellFile == NULL)
		die(__FILE__, __LINE__, "Couldn't open file: %s\n", fname);

	row = startOfDisplayedGridHeight + 5;
	col = startOfDisplayedGridWidth + 5;

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
			col = startOfDisplayedGridWidth + 5;
		}
	}

	fclose(cellFile);
}

void setRules(const char *ruleString) {
	uint8_t i,
		n;

	i = 0;
	birthRule = (int8_t *)malloc(10);
	if(birthRule == NULL)
		die(__FILE__, __LINE__, "Failed to allocate memory");

	surviveRule = (int8_t *)malloc(10);
	if(surviveRule  == NULL)
		die(__FILE__, __LINE__, "Failed to allocate memory");

	if(ruleString == NULL) {
		*(birthRule) = 3;
		*(birthRule+1) = -1;

		*(surviveRule) = 2;
		*(surviveRule+1) = 3;
		*(surviveRule+2) = -1;
	}

	else if(tolower(*ruleString) == 'b') {
		n = 0;
		for(i = 1; i < strlen(ruleString); i++) {
			if(*(ruleString+i) == '/')
				break;
			else if(isdigit(*(ruleString+i)))
				*(birthRule+n++) = *(ruleString+i) - '0';
			else
				die(__FILE__, __LINE__, "This doesn't conform to the rulestring b/s notation");
		}

		*(birthRule+n++) = -1;
		if(!realloc(birthRule, n))
			die(__FILE__, __LINE__, "Failed to reallocate memory");

		n = 0;
		for(i = i+1; i < strlen(ruleString); i++) {
			if(tolower(*(ruleString+i)) == 's');
			else if(isdigit(*(ruleString+i)))
				*(surviveRule+n++) = *(ruleString+i) - '0';
			else
				die(__FILE__, __LINE__, "This rulestring doesn't conform to b/s notation (must be b/s or s/b notation)");
		}

		*(surviveRule+n++) = -1;
		if(!realloc(surviveRule, n))
			die(__FILE__, __LINE__, "Failed to reallocate memory");

	}

	else if(isdigit(*ruleString)) {
		n = 0;
		for(i = 0; i < strlen(ruleString); i++) {
			if(*(ruleString+i) == '/')
				break;
			else if(isdigit(*(ruleString+i)))
				*(surviveRule+n++) = *(ruleString+i) - '0';
			else
				die(__FILE__, __LINE__, "This rulestring doesn't conform to s/b notation (must be b/s or s/b notation)");
		}

		*(surviveRule+n++) = -1;
		if(!realloc(surviveRule, n))
			die(__FILE__, __LINE__, "Failed to reallocate memory");

		n = 0;
		for(i = i+1; i < strlen(ruleString); i++) {
			if(isdigit(*(ruleString+i)))
				*(birthRule+n++) = *(ruleString+i) - '0';
			else
				die(__FILE__, __LINE__, "This doesn't conform to the rulestring b/s notation");
		}

		*(birthRule+n++) = -1;
		if(!realloc(birthRule, n))
			die(__FILE__, __LINE__, "Failed to reallocate memory");
	}
}
