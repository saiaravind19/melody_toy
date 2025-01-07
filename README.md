# Melody Toy

A customizable RFID-based music player. Organize your SD card to play specific playlists when RFID tags are scanned.

---

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

[Sample for SD card](references/sample.zip).

