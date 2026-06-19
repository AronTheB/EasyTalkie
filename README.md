<img width="1500" height="2300" alt="EasyTalkie" src="https://github.com/user-attachments/assets/1c430fa0-9a65-4b68-8a5b-504f0c4c2dba" />

# A ESP32/ESPNOW based walkie talkie

## Description

This is a easy to build/use Walkie-Talkie, that work via the ESPNOW system. The project uses a ESP-32 S3, INMP441, MAX98357A, SSD1306 OLED SCREEN, and 1000mAH LIPO BATTERY.

Made for [fallout.hackclub.com](https://fallout.hackclub.com/)

### WHY I MADE THIS:

I made this project, beaceuse my mom always have to yell at me to hear that she is calling me. So i made this walkie talkie so we can communicate easier in the house. I think also anyone could use this for easy communication and also at event like fallout where there are hunderds of kids, and you need to quickly communicate something to your partner.

### HOW TO BUILD:

You can replicate this project at home pretty easily. Clone this repo and download all the neccesary files such as the gerber.zip for pcb .stp files for the 3d models to print and also use the BOM to find all the parts needed for the project. After everything arrives solder it like in the schematic. (The battery has to be manually wired as shown in the schematic), put the pcb in the case, it should fit like a glove. connect it to a computer via a cable and flash the ESP32 with the firmware and after a reboot it should start working.

### HOW TO USE/FEATURES:

After turning on the ESP-32's will find eachother if near and than use the button to trasmit audio between the walkie-talkies. the main feature is the communication, if u press the button you start speaking while holding the button down, it will start transmitting audio and if you are not holding the key down, you can receive audio.

## KiCad

### Schematic

<img width="549" height="852" alt="image" src="https://github.com/user-attachments/assets/3269845a-ba4d-45a1-9a14-d0a0b7b6aff5" />

### PCB

<img width="426" height="592" alt="image" src="https://github.com/user-attachments/assets/710c84fe-6999-4623-b0b1-458d91eb1657" />

## Fusion 360

<img width="453" height="708" alt="image" src="https://github.com/user-attachments/assets/95f2a72b-dd72-468f-8881-41cac1e44214" />



## Preview

https://github.com/user-attachments/assets/4c29f206-930c-4e9f-b8dc-11138d7f1e36

## BOM

| NAME               | LINK                                                                                                           | PRICE  | QUANTITY | Need To buy? |
| :----------------- | :------------------------------------------------------------------------------------------------------------- | :----- | -------: | :----------- |
| XIAO ESP32 S3      | [https://www.aliexpress.com/item/1005005382287176.html](https://www.aliexpress.com/item/1005005382287176.html) | $6,64  |        2 | Yes          |
| INMP441            | [https://www.aliexpress.com/item/1005007987577953.html](https://www.aliexpress.com/item/1005007987577953.html) | $1,39  |        2 | Yes          |
| MAX98357A          | [https://www.aliexpress.com/item/1005007003802663.html](https://www.aliexpress.com/item/1005007003802663.html) | $1,63  |        2 | Yes          |
| SSD1306            | [https://www.aliexpress.com/item/32896971385.html](https://www.aliexpress.com/item/32896971385.html)           | $1,85  |        2 | Yes          |
| LiPo 1000mAh       | [https://www.aliexpress.com/item/1005004375261361.html](https://www.aliexpress.com/item/1005004375261361.html) | $7,69  |        2 | Yes          |
| SS-12D00 6mm       | [https://www.aliexpress.com/item/1005005633418066.html](https://www.aliexpress.com/item/1005005633418066.html) | $1,77  |        2 | Yes          |
| 8Ω 1W 28mm Speaker | [https://www.aliexpress.com/item/1005005458778825.html](https://www.aliexpress.com/item/1005005458778825.html) | $0,42  |        2 | Yes          |
| Mx Switches        | [https://www.aliexpress.com/item/1005004285423123.html](https://www.aliexpress.com/item/1005004285423123.html) | $2,86  |        1 | Yes          |
| Keycaps            | [https://www.aliexpress.com/item/1005009364428646.html](https://www.aliexpress.com/item/1005009364428646.html) | $3,54  |        1 | Yes          |
| TOTAL:             |                                                                                                                | $49,18 |          |              |


