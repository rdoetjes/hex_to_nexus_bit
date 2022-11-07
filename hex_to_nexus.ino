
#define TX_PIN 2
/*
 you connect an RF433 module to VCC (preferably it's own power supply with 12V to increase range)
 you connect the ground to the ground
 you connect the data of the module to TX_PIN (in this case 2)

 Now you need to hope you have a xmit module that has coils or better a trimmer pot to move to the
 433.920MHz mine did not and then it becomes hit or miss to spoof the Nexus Weather Station

 Now send a hex data string into the serial console such as: 9b 80 68 f4 80

    # 9b is the id (155) of your station -- you'll need to find that out or just brute force generate 255 of then :D
    # 8 is batter okay 00 is battery empty
    # 0 68 is the temperature (12 bits the result is divided by 10 by the weather station to get the decimal)
    # f is a control nibble
    # 4 8 is the himidity (48%)
*/
void setup(){
  pinMode(TX_PIN, OUTPUT);
  digitalWrite(TX_PIN, LOW);
  Serial.begin(9600);
}

unsigned char get_number(unsigned char t){
  unsigned char b = 0;
  if (t>96 && t<103) b = t - 87;
  if (t>47 && t<58) b = t ^ 48;
  return b;
}

void pulse_and_wait(int pulse_us, int wait_us){
  digitalWrite(TX_PIN, HIGH);
  delayMicroseconds(pulse_us);
  digitalWrite(TX_PIN, LOW);
  
  delayMicroseconds(wait_us); 
}

void send_ook_tlow(String *cmd, int pulse_us, int pre_send_us, int low_bit_us, int high_bit_us){
  pulse_and_wait(pulse_us, pre_send_us); 
  for (int i=0; i < cmd->length(); i++){
    unsigned char t = cmd->charAt(i);
    if (t == 32) continue;
        t = get_number(t);
    for(int j=0; j<4; j++){
      unsigned char b = (t & 0x8);
      t = t << 1;
      if (b == 0) 
        pulse_and_wait(pulse_us, low_bit_us); 
      else
        pulse_and_wait(pulse_us, high_bit_us);
    }
  }
}


void loop(){
  static String cmd="";
  
  if (Serial.available()>0) {
    cmd = Serial.readString();  
  }
  
  delay(10000);
  
  for (int i=0; i<10;i++) send_ook_tlow(&cmd, 500, 4000, 1000, 2000);
}