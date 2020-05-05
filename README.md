# sdl-life

This program is a simple graphical implementation of Conway's game of life which uses SDL2 and is written in C. Please remember that this implementation is very early in development. Look at the [TODO.md](TODO.md) file to see this program's TODO list

![Alt text](screenshots/screenshot-i3-5.png?raw=true "Glider")

## Features

This program currently lacks many features that are to be expected from a *full* game of life implementation.
This program has the following features:
* All Conway's game of life [rules](https://www.conwaylife.com/wiki/Conway%27s_Game_of_Life#Rules) working
* Pause and resume
* Zoom in and out
* Move around the grid
* Support for reading [plaintext cell files](https://www.conwaylife.com/wiki/Plaintext) (.cells)
* Add or remove cells by clicking on them

## Installation

To install this program `make install` must be executed on the command prompt, this command will install the program to PREFIX/bin. This program has been only tested on Linux using X11.

## Usage

This program is an implementation of Conway's game of life. You can run this just by executing `sdl-life` on the command line. To add cells just left click and you will see a new cell on the grid, if the program isn't paused it will probably disappear immediately after it appears; press the space bar to pause or resume the program. A list of all the [keybindings](#features) is below.

If you want to see [ready made patterns](https://www.conwaylife.com/wiki/Pattern) in action you can do that if you have a valid [.cells](https://www.conwaylife.com/wiki/Plaintext) file containing a pattern, then just execute `sdl-life -f pattern.cells` and you will see the pattern stored on the file in action. Currently only the [plaintext / .cells format](https://www.conwaylife.com/wiki/Plaintext) is supported for reading patterns. Reading the most common file format for storing large patterns, the [Run Length Encoded](https://www.conwaylife.com/wiki/Run_Length_Encoded) file format, should be supported soon.

### Keybindings

	-Space					-Pause/Resume
	-w/a/s/d				-Move around the grid (up/down/left/right)
	-Mouse scroll or PLUS (+) / MINUS (-)	-Zoom in and out
	-Left click				-Add cell
	-Right click				-Remove cell

## Screenshots

![Alt text](screenshots/screenshot-gnome-1.png?raw=true "Title")
![Alt text](screenshots/screenshot-gnome-2.png?raw=true "Title")

sdl-life as seen on Ubuntu gnome.

![Alt text](screenshots/screenshot-i3-1.png?raw=true "Title")
![Alt text](screenshots/screenshot-i3-2.png?raw=true "Title")
![Alt text](screenshots/screenshot-i3-3.png?raw=true "Title")

As you can see sdl-life supports a plentitude of colors.

![Alt text](screenshots/screenshot-i3-4.png?raw=true "Title")
## License information

This project is licensed under the terms of the [BSD 2-Clause license](https://opensource.org/licenses/BSD-2-Clause), see the [LICENSE](LICENSE) file for further information.
