
# ESP32 Wireless Microphone


There are two microphone boards type: analog and digital.
the analog is MAX9814 and is using the internal ADC on the ESP32
the digital is INMP441 is outputting directly to the ESP32

## Video on using internal DAC for audio input:
https://www.youtube.com/watch?v=pPh3_ciEmzs

## Video on using MEMS Mic for audio input:
https://www.youtube.com/watch?v=3g7l5bm7fZ8

## Software:
https://github.com/atomic14/esp32_wireless_microphone/tree/main/firmware

## Connections:
```
INMP441         ESP32 (DevKitV1-30Pins)
GND             GND
VDD             3.3v
SD              D21
SCK             D26
WS              D22
L/R             GND
```

## ESP32 I2S Audio - both DAC and MEMS
https://github.com/atomic14/esp32_audio

https://projects-raspberry.com/esp32-voice-streamer/


I bought these from Aliexpress:

ESP32-DevKitC Core Board ESP32 Development Board ESP32-WROOM-32D ESP32-WROOM-32U For Arduino+ Free Shipping
https://www.aliexpress.com/item/4000296658456.html


## How to play MP3 files without a codec chip on the ESP32:
https://www.youtube.com/watch?v=kdKnddqCJbY

## PlatformIO - dev
https://www.instructables.com/Develop-ESP32-With-PlatformIO-IDE/



### Intro
You can watch a video of this working here: https://youtu.be/0jR-QNTfydA at around 4:23
[![Demo Video](https://img.youtube.com/vi/0jR-QNTfydA/0.jpg)](https://www.youtube.com/watch?v=0jR-QNTfydA)

### Getting going

Open this folder using Platform IO and upload the firmware. You'll also need to upload the filesystem image.
Once it's running, you can either go to http://microphone.local to get the audio over websockets. Or you can use the Python script in the `player` folder.
