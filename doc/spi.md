# Get on the SPI bus - headaches because of an SD card

Everything quite seemed to be working properly, but from time to time something suddenly stopped working:

* no access to the SD card possible
* UDP logging stopped working
* or even the complete sketch hang somewhere

Such things are difficult to trace. I just found out that they occur more often when I try to fetch something from the embedded web server, which is also using SD card content. So I assumed that these two could be interrelated somehow.

After a lot of browsing on the Internet I came over several threads discussing problems accessing an SD card together with some other module on the SPI bus. Everything works well separately, but as soon as you use several functions in parallel, problems occur.

What finally brought me to the right track - I think - was a small remark on this thread: https://arduino.stackexchange.com/questions/44175/switching-between-spi-devices-when-one-is-an-sd-card. It mentioned that some SD cards play not well with other devices on the bus. Somewhere else I also found people mentioning that SD cards fail to release the MISO line early enough, so the next transfer from another device can be scrambled. Just to exclude this problem I added a `delay(10)` each time when I switch SPI devices. And voila - no problems since then.

Just today, while writing this information I found another thread which also addresses this problem: https://forum.arduino.cc/index.php?topic=360718.msg2863201#msg2863201. The SD card seems not properly to release the MISO line after a transfer even if chip select goes high. The proposed solution just to write some 0 data when no device is selected seems to be very promising to me and would avoid the delay, which I do not really like. But perhaps this is also just a (very small) delay sufficient in some cases? However, does it really help?

As the 10 millisecond delays do not hurt and do not affect the performance of the sketch in any way, I will stick with them as long as this works.
