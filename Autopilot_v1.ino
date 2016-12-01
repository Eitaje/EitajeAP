/*  Eitan's AutoPilot - Telemetry Collection

  The circuit:
   GPS attached to Serial1 (9600 baud-rate)
   SD card attached to SPI bus ** CS - digital pin 4

  created  19 Mar 2016
  by Eitan Menahem
*/
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include <TM1637Display.h>
#include "sensors.h"
#include "System.h"

int loopCounter = 0;

//Servo objects to control a servos
Servo throttle_servo;
Servo aileron_servo;
Servo elevator_servo;
Servo rudder_servo;
Servo flaps_servo;
byte rx_buffer[16];
byte rx_incomingByte;
unsigned int rx_counter = 0;

//

System::State state; // holds the system state (as precieved from the sensors)
System::Consts consts;
System::Globals globals;
char fileName[sizeof(consts.filename)];
File dataFile;
Sd2Card card;
SdVolume volume;
SdFile root;


TM1637Display display(consts.TM1637_CLK, consts.TM1637_DIO);

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};

const uint8_t SEG_SD[] = {
  SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,           // 5
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_G , // -
  SEG_G   // -
};

const uint8_t SEG_No_Satellites[] = {SEG_G, SEG_G, SEG_G, SEG_G};
boolean firstLoop = true;

void setup() {

  if (globals.use_TM1637Display)
    display.setBrightness(0x0f);

  Serial1.begin(115200); // Spektrum AR7000 Reciever
  Serial2.begin(9600);  // GPS port
  
  Wire.begin(); //MPU9250
  
  globals.debug_led_mode = 0;

  pinMode(consts.debug_led_pin, OUTPUT);
  pinMode(consts.current_sensor_pin, INPUT);
  digitalWrite(consts.debug_led_pin, LOW);

  // attached servos
  throttle_servo.attach(consts.throttle_pin);
  aileron_servo. attach(consts.ailerons_pin);
  elevator_servo.attach(consts.elevator_pin);
  rudder_servo.  attach(consts.rudder_pin);
  
  //initialize the SD card reader
  initSD();
  if (state.status_SD == 99) {
    if (globals.use_TM1637Display)
      display.setSegments(SEG_SD);
    delay(4000);
  }

  //initialize the sensors
  initSensors();

  //DEBUG
  SerialUSB.println("finished initializing sensors");

  //Print "Done" on the 7-Seg
  if (globals.use_TM1637Display) {
    display.setBrightness(0x0f);
    // Done!
    display.setSegments(SEG_DONE);
  }

  delay(500);
}

void loop() {

  //DEBUG
  //SerialUSB.println("Loop: before gps readout");

  //read GPS data
  if (globals.use_GPS && loopCounter % consts.readGPSEvery == 0) {
    get_GPS_data(&state);
  }

  //DEBUG
  //SerialUSB.println("Loop: before BMP readout");

  // read BMP data
  if ((globals.use_BMP180 || globals.use_BMP280) && loopCounter % consts.readPressureEvery == 0) {

    // Read BMP180 data
    if (globals.use_BMP180)
      get_BMP180_data(&state);

    // Read BMP280 data
    if (globals.use_BMP280)
      get_BMP280_data(&state);
  }

  // Read MPU sensors
  if (globals.use_MPU && loopCounter % consts.readMPUEvery == 0) {

    // Read Spatial sensors
    get_MPU9250_data(&state);
  }

  // Read AirSpeed
  if (globals.use_AIRSpeed && loopCounter % consts.readAirspeedEvery == 0) {

    // Read Spatial sensors
    get_airspeed_data(&state, &globals);
  }
  
  // Place holder for current measuring
//  if (loopCounter % consts.readCurrentEvery == 0) {
//    //todo
//  }

  //Autopilot mode0 ----------------------------------------------------------------------------------
  if(globals.use_stabilizer && loopCounter % consts.stabilizeProcEvery  == 0)
  {
    stabilizer(&state);
  }
  //Autopilot mode0 ----------------------------------------------------------------------------------
  
  // write state to SD card and/or Serial
  if ((globals.use_SD_card|globals.write_state_to_serial) && loopCounter % consts.writeEvery == 0) {

    //serialize current state
    serializeState(&state, &globals);


    if (firstLoop)
    {
      String header = getDataHeader(&state, &globals);

      if (globals.write_state_to_SD_card && state.status_SD == 1) {
        // if the file is available, write to it:
        if (dataFile)
          dataFile.close();

        // open the file. note that only one file can be open at a time,
        // so you have to close this one before opening another.
        dataFile = SD.open(fileName, FILE_WRITE);//output_filename //consts.output_filename.c_str()

        // if the file isn't open, pop up an error:
        if (!dataFile) {
          //SerialUSB.print("error opening: ");  //SerialUSB.println(fileName);
        }
        else
          dataFile.println(header);

      }
      firstLoop = false;
    }

    //write data to SD card?
    if (globals.use_SD_card && globals.write_state_to_SD_card && state.status_SD == 1) {

      // if the file is available, write to it:
      if (dataFile)
        dataFile.close();

      // open the file. note that only one file can be open at a time,
      // so you have to close this one before opening another.
      dataFile = SD.open(fileName, FILE_WRITE);//output_filename //consts.output_filename.c_str() //consts.output_filename

      // if the file isn't open, pop up an error:
      if (!dataFile) {
        //SerialUSB.print("error opening: ");  //SerialUSB.println(fileName);
      }
      else
        dataFile.println(globals.state_str);
    }

    // print state_str to serial
    if (globals.write_state_to_serial)
    {
      SerialUSB.println(globals.state_str);
    }
  }

  // ----------------------------------------------------------------------------

  //Toggle display content
  if (globals.use_TM1637Display && loopCounter % consts.toggleDisplayEvery == 0) {
    //Display the selected content
    flipDisplayContent();
  }


  //Debug Led - to show that the device is alive
  flipDebugLedState();

  //advance loop counter
  loopCounter++;

  //  delay(0);
}
//---------------- End of Main Loop -------------------------------------------------------------------------------------------------

/*
  // Write the header of the GPS to file
  void writeGPSHeader(String gpsHeader)
  {
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(consts.output_filename, FILE_WRITE); //output_filename

  // if the file is available, write to it:

  if (dataFile) {
    dataFile.println(gpsHeader);
    dataFile.close();

    // print to the debug port too:
    //SerialUSB.println(gpsHeader);
  }
  // if the file isn't open, pop up an error:
  else {
    //SerialUSB.println("error opening: " + consts.output_filename);
  }
  }
*/

// Init the SD card reader
void initSD() {
  if (globals.use_SD_card) {

    //Create the filename for persistance
    consts.filename.toCharArray(fileName, sizeof(fileName));

    //Setup SD card -----------------------------------------
    if (!card.init(SPI_FULL_SPEED, consts.SD_reader_chipSelect)) {
      //SerialUSB.println("initialization failed. Things to check:");
      //SerialUSB.println("* is a card inserted?");
      //SerialUSB.println("* is your wiring correct?");
      //SerialUSB.println("* did you change the chipSelect pin to match your shield or module?");
      state.status_SD = 99;//update state accordingly
      return;
    } else {
      //SerialUSB.println("Wiring is correct and a card is present.");
    }

    // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
    if (!volume.init(card)) {
      //SerialUSB.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
      state.status_SD = 99;//update state accordingly
      return;
    }

    //SerialUSB.print("Initializing SD card...");

    // see if the card is present and can be initialized:
    if (!SD.begin(consts.SD_reader_chipSelect)) {
      //SerialUSB.println("Card failed, or not present");


      state.status_SD = 99;//update state accordingly
      return;
    }

    //bool result = SD.remove(fileName);// delete previous RUN data
    int counter = 0;
    //search for file name that does not currently exist

    boolean fileExist = true;
    String newFileName;
    while (fileExist) {
      newFileName = consts.filename + counter + ".csv";
      //SerialUSB.println("trying: " + newFileName);
      fileExist = SD.exists(newFileName);
      counter += 1;
    }

    consts.filename = newFileName;
    //Create the filename for persistance
    consts.filename.toCharArray(fileName, sizeof(newFileName));

    //SerialUSB.print("card initialized. filename used: "); //SerialUSB.println(fileName);
    state.status_SD = 1;

    // SerialUSB.println("\nFiles found on the card (name, date and size in bytes): ");
    root.openRoot(volume);

    // list all files in the card with date and size
    //root.ls(LS_R | LS_DATE | LS_SIZE);
    /**/
  }
  // end of setup-up the SD card -----------------------------------------

}

//initializes the sensors
void initSensors()
{
  // Setup GPS  ---------------------------
  if (globals.use_GPS) {
    //String gpsHeader = setupGPS();
    //writeGPSHeader(gpsHeader); // todo: do we need this?
  }
  // Setup GPS  ---------------------------

  //Setup BMP pressure sensors -----------------------------------------
  if (globals.use_BMP180)
    setupBMP180(&state);
  if (globals.use_BMP280)
    setupBMP280(&state);
  //Setup BMP pressure sensors -----------------------------------------

  //Setup 9-Axis Accelometer ///////////////////////////
  if (globals.use_MPU)
    setupMPU9250(&state);
  //Setup 9-Axis Accelometer ///////////////////////////

  setupAirSpeedSensor(&state);

}

void flipDebugLedState()
{
  if (globals.debug_led_mode == 0)
  {
    globals.debug_led_mode = 1;
    digitalWrite(consts.debug_led_pin, HIGH);
  }
  else
  {
    globals.debug_led_mode = 0;
    digitalWrite(consts.debug_led_pin, LOW);
  }
}

//iterates between different content to display
void flipDisplayContent() {

  switch (globals.displayContent) {

    case 0 :
      //display the number of sattelites
      if (state.satellites > 200 || state.satellites < 1)
        display.setSegments(SEG_No_Satellites);
      else
        display.showNumberDec(state.satellites , false);
      break; /* optional */
    case 1  :
      display.showNumberDec( (int) (state.temperature_BMP280 + 0.5), false);
      break; /* optional */

    case 2  :
      // Print the G-force
      display.showNumberDec(state.az, false);
      break; /* optional */
    case 3  :
      display.showNumberDec( (int) (state.temperature_BMP180 + 0.5), false);
      break; /* optional */

    case 4  :
      display.showNumberDec( (int) (state.pressure_BMP280), false);
      break; /* optional */

    /* you can have any number of case statements */
    default : /* Optional */
      display.showNumberDec(9999);
  }

  //update the content index
  globals.displayContent = (globals.displayContent + 1) % globals.numContentsToDisplay;
}










