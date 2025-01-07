#include "Pinout.h"


SoftwareSerial mySoftwareSerial(software_rx, software_tx);
PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);


uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;


void prev_track();
void next_track();
void power_callback();
void power_state_callback();

void init_buttons() {
  pinMode(play_pause_button, INPUT);
  pinMode(next_track_button, INPUT);
  pinMode(prev_track_button, INPUT);
  pinMode(Volume, INPUT);

  player_prev_state = digitalRead(play_pause_button);

  attachInterrupt(digitalPinToInterrupt(prev_track_button), prev_track, FALLING);
  attachInterrupt(digitalPinToInterrupt(next_track_button), next_track, FALLING);

}

void init_player() {
  mySoftwareSerial.begin(9600);

  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini"));
  Serial.println(F("Initializing DFPlayer module ... Wait!"));

  if (!myDFPlayer.begin(mySoftwareSerial))
  {
    Serial.println(F("Not initialized:"));
    Serial.println(F("1. Check the DFPlayer Mini connections"));
    Serial.println(F("2. Insert an SD card"));
    while (true);
  }

}

void setup()
{
  // initialize mp3 player
  init_player();

  Serial.println(F("DFPlayer Mini module initialized!"));
  nfc.begin();

  // check for RFID connection
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); 
  }
  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for an ISO14443A Card ...");

  // configure player
  myDFPlayer.setTimeOut(500); //Timeout serial 500ms
  myDFPlayer.volume(30); //Volume 5
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);

  // initialize buttons and interrupts 
  init_buttons();


}

void check_for_change() {
  if (player_prev_state != digitalRead(play_pause_button))
  { player_prev_state = !player_prev_state;
    power_callback();
  }
}

int read_rfid()
{
  uint8_t auth[16];
  uint8_t success;

  success = nfc.mifareclassic_ReadDataBlock(4, auth);
  if (memcmp(auth, unlock1, sizeof(auth) *  sizeof(auth[0])) == 0) return 1;
  else if (memcmp(auth, unlock2, sizeof(auth) *  sizeof(auth[0])) == 0) return 2;
  else if (memcmp(auth, unlock3, sizeof(auth) *  sizeof(auth[0])) == 0) return 3;
  else if (memcmp(auth, unlock4, sizeof(auth) *  sizeof(auth[0])) == 0) return 4;
  else return 0;


}

bool check_for_rfid()
{
  uint8_t success;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success)return true;
  else return false;

}

void check_for_volumechange()
{

  unsigned int rawAnalogValue = analogRead(Volume);

  // Check if the change exceeds the threshold
  if (abs((int)rawAnalogValue - lastAnalogValue) > ANALOG_THRESHOLD) {

    unsigned int newVolume = rawAnalogValue / 34;

    // Ensure the volume is within valid bounds
    newVolume = constrain(newVolume, 0, 30);

    // Update volume if it's different and enough time has passed
    if (newVolume != current_volume )
    {
      current_volume = newVolume;
      myDFPlayer.volume(current_volume);
      Serial.print("Volume changed to: ");
      Serial.println(current_volume);
      lastVolumeUpdate = millis();
    }
  }
  // Update the last analog value for filtering in the next loop
  lastAnalogValue = rawAnalogValue;
}

void loop()
{
  bool rfid_detected = check_for_rfid();

  check_for_change();

  if (millis() - lastVolumeUpdate > VOLUME_UPDATE_DELAY)check_for_volumechange();

  if (power_button_flag)
  {
    power_state_callback();
  }

  switch (current_state) {
    case WELCOME:

      myDFPlayer.playMp3Folder(1);  //Play next mp3
      delay(3000);
      //. fist paly the welcome note
      current_state = WAITING_FOR_PLAYCARD;
      break;

    case SHUTDOWN_TRIGGERED:
      Serial.println("Shutdown triggered");
      //play shutdown triggered music
      myDFPlayer.playMp3Folder(2);
      delay(2050);

      device_state = POWER_OFF;
      current_state = PAUSE;
      break;

    case WAITING_FOR_PLAYCARD:

      if (rfid_detected) {
        if (uidLength == 4) {
          uint8_t success;
          success = nfc.mifareclassic_AuthenticateBlock (uid, uidLength, 4, 0, keyb);
          if (success)
          {
            file_index = read_rfid();
            if (file_index != 0) {
              player_index_limit = get_file_count(file_index);
              myDFPlayer.playFolder(file_index, player_index);
              current_state = PLAY;
            }
          }
        }
      }

      break;

    case WAITING_FOR_PAUSE:

      myDFPlayer.pause();

      current_state = PAUSE;
      break;

    case WAITING_FOR_PLAY:

      myDFPlayer.start();
      current_state = PLAY;
      break;


    case PLAY_NXT:
      Serial.println("here_plynxt");
      current_state = PLAY;

      if (player_index >= player_index_limit) player_index = 1;
      else player_index = player_index + 1;
      myDFPlayer.playFolder(file_index, player_index);

      break;

    case PLAY_PREV:
      Serial.println("here_plyprev");
      current_state = PLAY;

      if (player_index == 1)player_index = player_index_limit;
      else player_index = player_index - 1;
      myDFPlayer.playFolder(file_index, player_index);
      break;

    default:
      break;
  }

  if (rfid_detected == false and device_state == POWER_ON )
  {
    // check if current state is pause tehn directly change the state to waiting for playcard
    if (current_state ==  PAUSE)current_state = WAITING_FOR_PLAYCARD;

    // if the player is already runing then set the state to pause
    else if (current_state > WAITING_FOR_PLAYCARD)current_state = WAITING_FOR_PAUSE;
    else current_state = WAITING_FOR_PLAYCARD;
  }

  Serial.print (device_state);
  Serial.print(" ");
  Serial.print (current_state);
  Serial.print(" ");
  Serial.print("FI: ");
  Serial.print(file_index);
  Serial.print("PI: ");
  Serial.print(player_index);
  Serial.print(" P_L: ");
  Serial.print(player_index_limit);
  Serial.print(" Current millis:");
  Serial.print(millis());
  Serial.print(" prev_millis:");
  Serial.print(prev_power);
  Serial.print( " Diff:");
  Serial.println(millis() - prev_power);

  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}

void power_state_callback() {
  if (millis() - prev_power >= power_mills)
  {
    if (device_state == BOOT_TRIGGERED)
    { device_state = POWER_ON;
      current_state = WELCOME;
    }

    else {
      current_state = SHUTDOWN_TRIGGERED;
    }
    prev_power = 0;
    power_button_flag = false;

  }

}
void power_callback()
{
  if (digitalRead(play_pause_button) == LOW)
  {

    prev_power =
      millis();
    if (device_state == POWER_OFF) {
      device_state = BOOT_TRIGGERED;
    }

    else if (device_state == POWER_ON && current_state == PLAY)current_state = WAITING_FOR_PAUSE;
    else if (device_state == POWER_ON && current_state == PAUSE)current_state = WAITING_FOR_PLAY;
    power_button_flag = true;

  }
  else
  {
    if (device_state == BOOT_TRIGGERED)device_state = POWER_OFF;
    power_button_flag = false;

  }
}

int get_file_count(int folder_index) {
  int count = 0;
  while (true) {
    count = myDFPlayer.readFileCountsInFolder(folder_index);
    if (count >= 1 && count != 65535)
    {
      break;
    }
    else delay(10);

  }
  return count;
}
void next_track()
{
  if (current_state > WAITING_FOR_PLAYCARD)current_state = PLAY_NXT;
}

void prev_track()
{

  if (current_state >  WAITING_FOR_PLAYCARD)current_state = PLAY_PREV;

}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }

}
