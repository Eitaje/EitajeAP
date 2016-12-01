const int throttle_max = 1024, throttle_min = 0;
const int aileron_max = 2048, aileron_min = 1024;
const int elevator_max = 3072, elevator_min = 2048;
const int rudder_max = 4096, rudder_min = 3072;
const int gear_max = 5120, gear_min = 4096;
const int flaps_max = 6144, flaps_min = 5120;
const int aux2_max = 7168, aux2_min = 6144;

const int servo_range = 70;
const int middle_position =100;

//define control surfaces position variable
int aileron, prev_aileron,
    flaps, prev_flaps,
    gear, prev_gear,
    elevator, prev_elevator,
    aux2, prev_aux2,
    throttle, prev_throttle,
    rudder, prev_rudder;
    
// Invoked when there is a Serial event
void serialEvent1() {
  
  if (!globals.use_receiver)
    return;
  
  //DEBUG
  //  SerialUSB.println("serial event");//DEBUG

  //read current byte
  if (Serial1.available()) {
    rx_buffer[rx_counter] = Serial1.read();

    if (rx_counter == 15)
    {
      aileron   = (rx_buffer[3] | rx_buffer[2] << 8) - aileron_min;
      flaps     = (rx_buffer[5] | rx_buffer[4] << 8) - flaps_min;
      gear      = (rx_buffer[7] | rx_buffer[6] << 8) - gear_min;
      elevator  = (rx_buffer[9] | rx_buffer[8] << 8) - elevator_min;
      aux2      = (rx_buffer[11] | rx_buffer[10] << 8) - aux2_min;
      throttle  = (rx_buffer[13] | rx_buffer[12] << 8) - throttle_min;
      rudder    = (rx_buffer[15] | rx_buffer[14] << 8) - rudder_min;

      //DEBUG
      String aileron_str  = intToStr(aileron ,-1,4);
      String flaps_str    = intToStr (flaps   ,-1,4);
      String gear_str     = intToStr (gear    ,-1,4);
      String elevator_str = intToStr (elevator,-1,4);
      String aux2_str     = intToStr (aux2    ,-1,4);
      String throttle_str = intToStr (throttle,-1,4);
      String rudder_str   = intToStr (rudder  ,-1,4);

      // calculate Servo Position
      int aileron_position  = ((float)(aileron  - 512)/512)*servo_range+middle_position;
      int flaps_position    = ((float)(flaps    - 512)/512)*servo_range+middle_position;
      int gear_position     = ((float)(gear - 512)/512)*servo_range+middle_position;
      int elevator_position = ((float)(elevator - 512)/512)*servo_range+middle_position;
      int aux2_position     = ((float)(aux2     - 512)/512)*servo_range+middle_position;
      int throttle_position = ((float)(throttle - 512)/512)*servo_range+middle_position;
      int rudder_position   = ((float)(rudder   - 512)/512)*servo_range+middle_position;

      //DEBUG
      SerialUSB.println(
                            "Aileron: " +  String(aileron_position) 
                        + ", elevator: " + String(elevator_position) 
                        + ", rudder: "   + String (rudder_position)
                        + ", throttle: " + String (throttle_position)
                        + ", Flaps: "    + String (flaps_position) 
                        + ", Gear: "     + String (gear_position) 
                        + ", aux2: "     + String (aux2_position)
                       );

        // Write values to the servos
//            throttle_servo.write(elevator_position);
      if(aileron_position !=prev_aileron){
        aileron_servo.write(aileron_position);
        prev_aileron = aileron_position;
      }
      if(elevator_position !=prev_elevator){
        elevator_servo.write(elevator_position);
        prev_elevator = aileron_position;
      }
      //      rudder_servo.write(rudder_position);
      //      flaps_servo.write(flaps_position);

    }
      //increase counter modulu 16
      rx_counter = (rx_counter + 1) % 16;
  }

}
//*/
