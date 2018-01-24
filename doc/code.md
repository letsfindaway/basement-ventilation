# Code walk - Explaining architecture and details of the program

At the latest when looking into the code you easily find out that I'm from Germany. There are some German class names and comments, but I think the structure of the program is clear enough that this is not a problem. Additionally I will explain some points to you here on this page.

As for every sketch the main routines are located in the [basement-ventilation.ino](../basement-ventilation.ino) source file. Here is the `setup` and the `loop` function located.

At the very beginning we `#include "pins.h"` and that is also the place where to look when you wonder which pin of the Feather is connected to what.

I then created a set of classes which wrap functionality:

* `Klima` is for sensor reading and computation of the absolute humidity
* `Raum` represents a room and makes the proper decisions for the window
* `Motor` controls the motors of the actuators and the window position
* `Anzeige` is responsible for the display and the touch screen
* `Wlan` provides WiFi connectivity and contains an NTP client and a simple web server
* `Log` provides logging capabilities to the serial console and via UDP to a remote computer
* `Settings` manages persistent configuration values

In the beginning I set up a set of related objects:

* first the `Klima` objects are created for the three sensors
* Then two `Raum` objects are created, each referring to two sensors, the specific one inside and the common one outside
* Also the objects `Anzeige` and `Wlan` are created here.

Then the pins are configured and the `setup` method of these classes is called.