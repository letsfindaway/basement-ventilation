# All you need - Installing the necessary libraries

The project uses a set of external libraries you have to install first in order to compile it successfully. Here I will just list them and point you to the source. For most of them just read the Adafruit documentation for the Feather and the display, then you're mostly done!

* **Adafruit GFX Library** Core graphics library
* **Adafruit HX8357 Library** Display specific library
* **Adafruit STMPE610** Touch screen library
* **Adafruit Unified Sensor** library
* **DHT sensor library** for DHT22 sensor reading
* ** WiFi101** WiFi library
* **Time** originally by Michael Margolis downloaded from the maintainer Paul Stoffregen (https://github.com/PaulStoffregen/Time)

For the Time library, you should go to the installation directory after you installed it and remove or rename the file `Time.h`, as it clashes with the built-in `time.h` - at least under Windows. This header is not necessary as it only forwards you to `TimeLib.h`.
