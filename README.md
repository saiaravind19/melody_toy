# Melody Toy

A customizable RFID-based music player. Organize your SD card to play specific playlists when RFID tags are scanned.

## Repo overview

- Codes
    - [RFID Formatter](codes/format_rfid_ndef/format_rfid_ndef.ino) is used to format RFID.
    - [RFID writer](codes/rfid_write/rfid_write.ino) is used to format RFID.
    - [Melody toy](codes/melody/melody.ino) is the code for toy.
- references
    - [Sample](references/sample.zip) is the sample SD card setup used during developing and testing the stack.

## SD Card Setup

### Step 1: Format the SD Card
- Use a compatible file system, such as **FAT32**.
- Ensure the SD card is empty before setting up.

### Step 2: Directory Structure
The SD card should follow this structure:

    .
    ├── 01
    ├── 02
    ├── 03
    └── mp3

- The **`mp3`** folder contains the welcome and shutdown audio notes. 
  - **Important:** Do not delete the `mp3` folder or its contents.
- Directories are named from `01` to `99`. Each corresponds to a specific RFID tag.
   - For example:
     - `01` → Playlist for RFID tag 1
     - `02` → Playlist for RFID tag 2
     - ... up to `99`.
- Each directory contains multiple MP3 files/playlist tracks.


### File Naming Rules

    .
    ├── 01
    │   ├── 001.mp3
    │   ├── 002.mp3
    │   └── 003.mp3
    ├── 02
    │   ├── 001.mp3
    │   ├── 002.mp3
    │   ├── 003.mp3
    │   └── 004.mp3
    ├── 03
    │   ├── 001.mp3
    │   ├── 002.mp3
    │   ├── 003.mp3
    │   └── 004.mp3
    └── mp3
        ├── 0001.mp3
        └── 0002.mp3


- Files/playlist names within the directories must follow these rules:
  - File names must be integers starting from `001` up to `255`.
  - File names must be **sequential without gaps**.
  - **Custom naming conventions are not allowed.**
- Each folder can hold a maximum of **255 MP3 files**.

Refer to [Sample for SD card](references/sample.zip) which is used for the demo for more insights.


## System setup

### Minimum Requirement
- [Arduino IDE](https://www.arduino.cc/en/software).

### Library used & Installtion Guide

```
- SoftwareSerial
- DFRobotDFPlayerMini
- PN532
```
### Installation Steps:

1. **SoftwareSerial** and **DFRobotDFPlayerMini**:  
   Install these libraries using the **Arduino Library Manager**.  
   - Go to **Tools** → **Manage Libraries** → Search and install.

2. **PN532**:  
   To install and configure the PN532 library, refer to the [PN532 setup guide](https://dangerousthings.com/wp-content/uploads/PN532_Manual_V3.pdf#:~:text=Here%20we%20show%20how%20to%20read%20and%20write,open%20Serial%20monitor.%20Put%20a%20card%20on%20it%3A).


## Adding a New Story

To add new stories, you need:
- An **RFID card**.
- The story files stored on the SD card in their respective directories.


### Step 1: Formatting the RFID Card

1. Upload the [RFID Formatter](codes/format_rfid_ndef/format_rfid_ndef.ino) sketch to your Arduino.
2. Open the **Serial Monitor** and set the baud rate to **115200**.
3. Follow the on-screen instructions:
   - Once the RFID module is detected, the code will prompt you to **press any key** to trigger the formatting process.

### Step 2: Configuring the RFID Card

1. Update the data block with the necessary key for the RFID card.  
   Replace the contents of `dataBlock` in the [RFID Writer](codes/rfid_write/rfid_write.ino) sketch:

    ```c
    uint8_t dataBlock[16] = { 'u', 'n', 'l', 'o', 'c', 'k', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0'};
    ```
2. Upload the updated sketch to your Arduino.
3. Open the **Serial Monitor** and set the baud rate to **115200**.
4. Follow the on-screen instructions:
   - Once the RFID module is detected, the code will prompt you to **press any key** to write data to the RFID card.


### Alternative: Using a Mobile NFC Tool

If your mobile supports NFC, you can use the [MIFARE Classic Tool](https://play.google.com/store/apps/details?id=de.syss.MifareClassicTool&hl=en-US) app to:
- Format the RFID card.
- Write data to the RFID card.

# References 
- [PN532 user manaual](https://dangerousthings.com/wp-content/uploads/PN532_Manual_V3.pdf#:~:text=Here%20we%20show%20how%20to%20read%20and%20write,open%20Serial%20monitor.%20Put%20a%20card%20on%20it%3A).
- [DFmini player](https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299#target_6).
- [PN532 Library](https://github.com/elechouse/PN532).

