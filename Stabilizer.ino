void stabilizer(System::State* state){

  float roll = atan((float)state->ay / abs(state->az)) * 57.296;
  float pitch = -atan((float)state->ax / state->az) * 57.296;

  // limit the range to [-90,90] degrees
  roll = roll - ((int)roll / 90) * 90;
  pitch = pitch - ((int)pitch / 90) * 90;
  
  //control parameters
  //---------------------------
  float stbl_moderator = 0.2; //stabalizing moderator (to ensure cenvergence)
  float exponent_factor = 1.25;
  int null_region = 1; //units: degree
  int max_signal = 40;
  float target_position_roll = 0, target_position_pitch=0;
  
  //difference between the required and actual rates
  float delta_roll = target_position_roll -roll;
  float delta_pitch = target_position_pitch - pitch;

  //keep zero region
  if(abs(delta_roll) < null_region) delta_roll =0;
  if(abs(delta_pitch) < null_region) delta_pitch =0;
  
  // compute needed compensation
  int roll_signal  = - (delta_roll   * abs(pow(delta_roll  ,1)) * stbl_moderator); //(int)
  int pitch_signal = - (delta_pitch  * abs(pow(delta_pitch ,1)) * stbl_moderator); //(int)

  //bound to max input value
  roll_signal =  max(-max_signal,min(max_signal,roll_signal));
  pitch_signal = max(-max_signal,min(max_signal,pitch_signal)); //(int)

    //DEBUG
//  SerialUSB.print ("elevator servo input: ("); SerialUSB.print(delta_pitch);  SerialUSB.print(") ");  SerialUSB.print(pitch_signal); SerialUSB.print("\t");
//  SerialUSB.print ("rudder servo input: (");   SerialUSB.print(delta_roll);   SerialUSB.print(") ");  SerialUSB.print(roll_signal); SerialUSB.print("\t");

  elevator_servo.write(90+1*pitch_signal);
  rudder_servo.write(90+1*roll_signal);

  SerialUSB.println("");

}









