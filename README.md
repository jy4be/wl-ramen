# Description

This is a simle radial menu for wayland, inspired by dmenu. It reads new-line seperated values from STDIN, creates a dial for the user to select an option, and prints the selection to STDOUT.
It can also show simple images on the dial, and the dial itself is fairly customizeable.

Your wayland compositor needs to support the Layer Shell Protocol, you also need to have the freetype2 library installed on your system.
Displays images using the brilliant stb image library: https://github.com/nothings/stb/tree/master

# Usage

Simply invoke the program with `wl-menu`, and pipe some options separated with new-lines into it, for instance with `echo`.
The input can either be simple strings, like
```
Option 1
Option 2
Option 3
```
or can contain paths to images displayed on the dials, seperated with a colon:
```
Opt. 1:image1.png
Opt. 2:image2.png
Opt. 2:image2.png
```
Selection of an options is done with the mouse; Left click selects a highlighted colour, right click closes the menu without choosing anything.

There is a plethora of options and flags to change the design of the dial:

| **Flag**      | **Use** |
|---------------|----------|
| -f [Fontfile] | Use specified font instead of the standard font|
| -r [radius]   | Set the radius from the center to the start of the dial|
| -d [width]    | Set the width of the dial |
| -cb [colour]  | Set the backgrounf color  |
| -cs [colour]  | Set the foreground/highlight/text colour |
| -b [center/below/none] | Set the position of banner displaying the highlighted option |
| -bs [size] | Set the fontsize |
| -s | Instead of fading out the colour towards the end, paint a solid colour |
| -i [size] | Set the size of the icons for the dial options. Dials will be resized to square dimensions |

The maximum amount of dial options is 16 by standard, but can be changed in code in the file `types.h`.

# Compiling

Use `make release` to build with optimizations, or just `make` to build without. The fisnished binary will be in /build.
You may also build with `make debug` to enable debug flags. The binary is standalone, aside from the freetype2 library, and of course wayland.

# Screenshots and examples
Simple Dial
`echo -e "Opt 1\nOpt 2\nOpt 3\nOpt 4" | ./build/wl_RaMen`

Dial with changed options
`echo -e "Opt 1\nOpt 2\nOpt 3\nOpt 4" | ./build/wl_RaMen -r 150 -d 200 -b center -s -cb "#40a0a0a0" -cs "#206080"`

Dial with icons (Using some Adwaita Icons)
`echo -e "Opt 1:Adwaita/16x16/places/folder.png\nOpt 2:Adwaita/16x16/places/user-home.png\nOpt 3:Adwaita/16x16/places/user-desktop.png" | ./build/wl_RaMen`
