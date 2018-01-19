DEFINES += __SAMD21G16A__
DEFINES += SAMG_SERIES=1

INCLUDEPATH += "C:\Users\FamilieWinter\AppData\Local\Arduino15\packages\adafruit\hardware\samd\1.0.21\cores\arduino"
INCLUDEPATH += "C:\Users\FamilieWinter\AppData\Local\Arduino15\packages\arduino\tools\CMSIS-Atmel\1.1.0\CMSIS\Device\ATMEL"
INCLUDEPATH += C:\Users\FamilieWinter\Documents\Arduino\libraries\DHT_sensor_library
INCLUDEPATH += C:\Users\FamilieWinter\Documents\Arduino\libraries\Adafruit_HX8357_Library
INCLUDEPATH += C:\Users\FamilieWinter\Documents\Arduino\libraries\Adafruit_GFX_Library
INCLUDEPATH += C:\Users\FamilieWinter\Documents\Arduino\libraries\Adafruit_STMPE610
INCLUDEPATH += C:\Users\FamilieWinter\Documents\Arduino\libraries\WiFi101\src
INCLUDEPATH += "C:\Program Files (x86)\Arduino\libraries\SD\src"

HEADERS += \
    pins.h \
    klima.h \
    anzeige.h \
    raum.h \
    motor.h \
    settings.h \
    logger.h \
    wlan.h \
    Timezone.h \
    TimeLib.h \
    Time.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/WVariant.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/WString.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/WMath.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/wiring_shift.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/wiring_private.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/wiring_digital.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/wiring_constants.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/wiring_analog.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/wiring.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/WInterrupts.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/WCharacter.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Udp.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Uart.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Tone.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Stream.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Server.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/SERCOM.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/RingBuffer.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Reset.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/pulse.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Printable.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Print.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/itoa.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/IPAddress.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/HardwareSerial.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/delay.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Client.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/binary.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Arduino.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/variants/feather_m0/variant.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/variants/feather_m0/pins_arduino.h \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/arduino/tools/CMSIS-Atmel/1.1.0/CMSIS/Device/ATMEL/samd21/include/samd21g18a.h

SOURCES += \
    basement-ventilation.ino \
    klima.cpp \
    anzeige.cpp \
    raum.cpp \
    motor.cpp \
    settings.cpp \
    logger.cpp \
    wlan.cpp \
    Timezone.cpp \
    Time.cpp \
    DateStrings.cpp \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/WString.cpp \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/WMath.cpp \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/wiring_shift.c \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/wiring_private.c \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/wiring_digital.c \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/wiring_analog.c \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/wiring.c \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/WInterrupts.c \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Uart.cpp \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Tone.cpp \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Stream.cpp \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/startup.c \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/SERCOM.cpp \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/RingBuffer.cpp \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Reset.cpp \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/pulse.c \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/Print.cpp \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/new.cpp \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/main.cpp \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/itoa.c \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/IPAddress.cpp \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/hooks.c \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/delay.c \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/cortex_handlers.c \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/cores/arduino/abi.cpp \
    C:/Users/FamilieWinter/AppData/Local/Arduino15/packages/adafruit/hardware/samd/1.0.21/variants/feather_m0/variant.cpp
