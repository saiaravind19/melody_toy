#ifndef PINOUT_H    // Include guard to prevent multiple inclusions
#define PINOUT_H


#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>



DFRobotDFPlayerMini myDFPlayer;

// Button inputs
#define play_pause_button 4
#define next_track_button 3
#define prev_track_button 2


// RFID pins
#define rfid_rest_pin  9

#define Volume A0

// mp3 pinout
#define software_tx A4
#define software_rx A5


#define ANALOG_THRESHOLD 40   // Minimum change required to consider a new value
#define VOLUME_UPDATE_DELAY 300 // milliseconds
#define power_mills 2000


// Define the enum
enum OperationMode {
    POWER_OFF,
    BOOT_TRIGGERED,
    POWER_ON

};

enum PlayerMode{
  WELCOME,
  WAITING_FOR_PLAYCARD,
  WAITING_FOR_PLAY,
  PLAY,
  WAITING_FOR_PAUSE,
  PAUSE,
  PLAY_NXT,
  PLAY_PREV,
  SHUTDOWN_TRIGGERED
};




unsigned long prev_power = 0;
unsigned long lastVolumeUpdate = 0;


int file_index = 0;
int player_index = 1;
int player_index_limit = 0;

bool player_prev_state = 1;
OperationMode device_state = POWER_OFF;

PlayerMode current_state = -1;

unsigned int current_volume = 30;
unsigned int lastAnalogValue = 0;

bool power_button_flag = false;

uint8_t keyb[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t unlock1[16] = { 'u', 'n', 'l', 'o', 'c', 'k', '1','0','0','0','0','0','0','0','0','0'}; //unlock1
uint8_t unlock2[16] = { 'u', 'n', 'l', 'o', 'c', 'k', '2','0','0','0','0','0','0','0','0','0'};//unlock2
uint8_t unlock3[16] = { 'u', 'n', 'l', 'o', 'c', 'k', '3','0','0','0','0','0','0','0','0','0'};//unlock3
uint8_t unlock4[16] = { 'u', 'n', 'l', 'o', 'c', 'k', '4','0','0','0','0','0','0','0','0','0'};//unlock4


#endif // PINOUTS_H
