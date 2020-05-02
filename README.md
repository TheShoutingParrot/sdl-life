# sdl-life

This program is a simple graphical implementation of Conway's game of life which uses SDL2 and is written in C. Please remember that this implementation is very early in development. Look at the [TODO.md](TODO.md) file to see this program's TODO list

![Alt text](screenshots/screenshot-i3-5.png?raw=true "Glider")

## Installation

To install this program `make install` must be executed on the command prompt, this command will install the program to PREFIX/bin. This program has been only tested on Linux using X11.

## Features

This program currently lacks many features that are to be expected from a *full* game of life implementation.
This program has the following features:
* All Conway's game of life [rules](https://www.conwaylife.com/wiki/Conway%27s_Game_of_Life#Rules) working
* Pause and resume
* Support for reading [plaintext cell files](https://www.conwaylife.com/wiki/Plaintext) (.cells)

## Usage

Simple example: `sdl-life glider.cells`

Currently this program only reads and executes `.cells` files. If you don't give it a valid `.cells` file it will not work properly.

One reason why many patterns that have a some what big bounding box break or atleast don't work exactly how they are supposed to is that the grid is by default really small; change the grid's size with the arguments `-gridw` and `-gridh` for example `sdl-life -gridw 100 -gridh 100 gosperglidergun.cells`.

### Keybindings

	-Space		-Pause/Resume

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
