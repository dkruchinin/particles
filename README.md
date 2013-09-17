About
======

This is a simulation of the motion of N colliding particles working according to the laws of elastic collision.
The simulation uses event driven model which makes it pretty swift. This thing can be used to demonstrate the
motion of molecules in gas, Brownian motion, atomic diffusion and many other incredible things.

For more informatino read this: http://introcs.cs.princeton.edu/java/assignments/collisions.html


Building
======

Before you try to build this, you need:

* OSX: well, if you want to build it under linux you'll have to hack the makefile yourself. I hope I'll have enough time and enthusiasm to migrate this to cmake or something.
* Compiler supporting C++11
* GNU Make
* SDL2: http://www.libsdl.org/download-2.0.php

If you have all these just run

    % make


Usage
======

    % ./simulation
    Usage: ./simulation <width> <height> <config>

* width - obviously the width of the simulation screen
* height - obviously the hight of the simulation screen
* config - configuration file describing particles

Configuration file
------------------

Configuration file format is pretty simple, each line describes unique particle: its x and y coordinates, velocities,
radius, mass and color.
Line format:

    x y vx vy mass radius r g b

Where:
* x: relative x coordinate in range ```[0.0, 1.0]``` (absolute = width * x)
* y: relative y coordinate in range ```[0.0, 1.0]``` (absolute = height * y)
* vx: relative velocity along X axis ```[0.0, 1.0]``` (absolute = (width + height) / 2 * vx)
* vy: relative velocity along Y axis ```[0.0, 1.0]``` (absolute = (width + height) / 2 * vy)
* mass: mass of the particle ```[1, 100]```
* radius: relative radius of the particle ```[0.0, 1.0]``` (absolute = (width + height) / 2 * radius)
* r: red color value ```[0, 255]```
* g: green color value ```[0, 255]```
* b: blue color value ```[0, 255]```

Example of the valid configuration file

    # pendulum
    .1 .4 .03 0 10 .04 50 50 50
    .45 .4 0 0 10 .04 50 50 50
    .5501 .4 0 0 10 .04 50 50 50
    .6502 .4 0 0 10 .04 50 50 50

You can find a few ready to use configuration files in the configs directory:

    % ./simulation 600 600 configs/<something>

Controls
--------

* Space: pause/resume
* Up: increase speed
* Down decrease speed

Some examples
=============

Brownian motion:
-----------------

    % ./simulation 600 600 configs/brownian

![Sample](https://raw.github.com/dkruchinin/particles/master/misc/brownian.png)

1000 colliding particles:
-------------------------

    % ./simulation 600 600 configs/1000p

![Sample2](https://raw.github.com/dkruchinin/particles/master/misc/1000p.png)
