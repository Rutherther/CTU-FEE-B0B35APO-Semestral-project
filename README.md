This is code from a semestral project for computer architecture.
It works on a MicroZed evaluation kit.
This kit contains:
- XilinX Zynq-7000
- Dual ARM Cortex-A9
- a display, knobs, buttons, LEDs
- and more

![MicroZed evaluation kit](mzapo-sdl/mzapo.png "MicroZed evaluation kit")

It is running Linux operating system.
 
# Assignment (the crossed out parts were not finished)
The semestral project will be separated to 3 individual programs.
The main program will be a file browser. It will be possible to browse
through the whole root file system ~~as well as an external device~~.
The browser will be able to ~~perform simple operations such as copying,
moving or removing files.~~ It will be able to execute arbitrary files
and support opening some file formats in specified programs.

The second part of the project will be an image viewer. The file browser
will be able to open image files using this program. 
It will support jpeg, png and ppm formats.
It will be possible to use the rotation encoders to move a cursor
over the image. It will be possible to zoom to the cursor.

The last part of the project will be a text file viewer. This file viewer
may open any file, but is intended mainly for plain text formats.
Again, it will be possible to zoom in on the text and move through the file.

For faster debugging, a simple program that may be executed on personal computer
will be made. SDL window will be used. 

## Evaluation ofthe completion of the assignment
The file browser unfortunately does not support operations such as moving, removing or copying files.
It cannot mount external devices as planned. Only the root file system may be browsed through.
However it may open files using mime types as planned.
On the other hand, the text as well as image browsers support all features that
were in the assignment.

# Manual
## File browser
### Function
The program may browse through the root file system.
It may execute files and open images or plain text files.
A context menu may be used to open any file in the plain text viewer.

The program contains instructions accessible from the main menu.

### Virtual windows
The program consists of windows that may be switched.

### Default menu
It's possible to either enter the file browser
or read the instructions in the main menu.

#### File browser
The file explorer displays what folder you are currently in and the files in it.
One can navigate between folders, view context menus or launch files.

#### Context menu
You can run the file in the context menu or view it in the text viewer.

#### Dialog
A dialog box will typically be shown when an error occurs.
It contains the error that occurred.

### File browser entry
The file browser does not have any arguments.

Usage:
`
./file-browser
`

### LED strip
The LED strip shows the vertical position in the file explorer,
to show how many files still need to be gone through before the end
folder.

### Error conditions
Errors are reported via a dialog box.

### Controls

#### Standard input
- `h`, `j`, `k`, `l` - move the view around the file (left, down, up, right)
- `y`, `i`, `o`, `p` - move full screen for faster navigation (left, down, up, right)
- `z`, `x` - zoom in and out of text
- `n` - return the view to the top of the file list
- `m` - view to the end of the file list
- `e` - exit the program
- `c` - file context menu
- `v` - confirm selection (launch file / switch folder)

#### Rotary encoders
- First (red) - horizontal view shift
- Second (green) - vertical view shift
- Third (blue) - zoom in or out of text
- First encoder button - end program / move back
- Second encoder button - file context menu
- Third encoder button - confirm selection (start file / switch folder)
## Text file viewer
### Functions
The program displays the text from a text file on the LCD display, it can be moved around with the rotary encoders.
If the program does not receive the text file, it is possible that it will not display it correctly.

### Running
The program cannot be run without arguments, it always needs to be run with the path to the text file.
Otherwise the program will end up with the error condition shown on the RGB LEDs.

`
./text-viewer [path-to-file]
`

### LED strip
The LED strip shows how much data is already loaded during the loading phase.
After loading, it shows the vertical position in the file according to how much the file is shifted.

### Error conditions
If the program does not start correctly, one of the RGB LEDs will light up red.

The left RGB LED indicates that no argument has been passed. The right RGB LED means that the file failed to load,
does not exist or has no access rights. A specific error message appears on the standard output.

### Control

#### Standard input
- `h`, `j`, `k`, `l` - move the view around the file (left, down, up, right)
- `y`, `i`, `o`, `p` - move full screen for faster navigation (left, down, up, right)
- `z`, `x` - zoom in and out of text
- `n` - return the view to the beginning of the file
- `m` - view to the end of the file
- `e` - exit the program

#### Rotary encoders
- First (red) - horizontal view shift
- Second (green) - vertical view shift
- Third (blue) - zoom in or out of text
- First encoder button - exit program
- Second encoder button - return the view to the beginning of the file
- Third encoder button - view to the end of the file
## Image viewer
### Functions
The program displays a rescaled image on the LCD that can be zoomed in using the rotary encoders or standard input.
When the program starts, the image is zoomed out to fit entirely on the screen.

### Starting up
The program cannot be run without arguments, it always needs to be run with the path to the image file.
Otherwise the program will end with the error condition shown on the RGB LEDs.

Usage:
`
./image-viewer [path-to-file]
`

### Supported formats
- jpg
- png
- ppm

Formats are differentiated by their headers, endings are not differentiated

### LED strip
The LED strip shows how much data is already loaded during the loading phase.
After loading, the horizontal position of the cursor in the image is displayed
for easier orientation at higher magnification.

### Error conditions
If the program does not start correctly, one of the RGB LEDs will light up red.

The left RGB LED indicates that no argument has been passed. The right RGB LED means that the file failed to load,
it does not exist, the format is not correct or there are no access rights to it. A specific error message appears on the standard output.

### Control
The program has a cursor that determines where to zoom in. The cursor appears when it is moved and a moment afterwards.

#### Standard input
- `h`, `j`, `k`, `l` - control the cursor or scroll across the image depending on the mode, see the m key (left, down, up, right)
- `z`, `x` - zoom in and out of the image (to where the cursor is)
- `r` - return to the original state (zooms out the image and sets the cursor in the middle of the image)
- `m` - switch between image or cursor movement modes (RGB LEDs light up green in image movement mode)
- `e` - exit the program

#### Rotary encoders
- First (red) - move the cursor horizontally or move the whole image horizontally (see button of the second encoder)
- Second (green) - move the cursor vertically or move the entire image vertically (see second encoder button)
- Third (blue) - zoom in and out
- First encoder button - exit the program
- Second encoder button - switch between image or cursor movement modes
- Third encoder button - return to the original zoom state

# Compilation, installation, startup
`Makefile` is used for compilation.

## Cross compilation for MZAPO
For cross compilation, `arm-gnueabihf-gcc` and dynamic libraries are needed
`libjpeg` version 62, `libpng` version 16 and `libmagic` version 1 for MZAPO.

After compilation, the files are saved in the `bin` folder, to run
copy them to MZAPO.

The copying can be done via ssh, you can use `make`, specifically
`
TARGET_IP=[ip-addr] make copy-executable
`
where after `ip-addr` the ip address of MZAPO needs to be inserted.
In basic mode, it is assumed that the SSH tunnel is used
and the `mzapo-root-key` (SSH key for mzapo) is located
on the path ``/.ssh/mzapo-root-key`. To change the basic settings
just change the configuration in the `Makefile` in the project root.
The possible configurations are commented out on the lines starting
`SSHOPTIONS`

You can use the serial port or ssh to boot. Alternatively, use `make`,
to launch the file explorer:
`
TARGET_IP=[ip-addr] make run
`
or to launch the `run-image-viewer` image viewer
and to launch the text file viewer `run-text-viewer`.
The ARG environment variable is used to pass the argument

For example, you can use
`
TARGET_IP=[ip-addr] ARG=/path/to/image make run-image-viewer
`
or
`
TARGET_IP=[ip-addr] ARG=/path/to/image make run-text-viewer
`

The program should be run with cwd in the folder where the binaries are located.
## Compiling for a personal computer
For better debugging it is possible to compile the program
for a personal computer where the display is simulated
using the SDL window.

To compile for a PC, the environment variable `COMPUTER` can be used.
It is then possible to use
`
COMPUTER=1 make
`

After compilation, the files are in the `bin` folder in the root of the project.
They can be run directly with the necessary parameters, see the manual.

You need to copy the files from the `copy/` folder to the `bin/` folder and run the program
with cwd in the folder where all the binary files are located
