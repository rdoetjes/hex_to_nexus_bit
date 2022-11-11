
#define TX_PIN 2

/*
sets the pin (TX_PIN) that is connected to the 433 xmit data in to output.
And serial to 9600 baud. Serial will only be used to send a different command string than my defaul 9b 0f 68 f6 8
*/
void setup(){
  pinMode(TX_PIN, OUTPUT);
  digitalWrite(TX_PIN, LOW);
  Serial.begin(9600);
}

/*
converts the char (0123456789abcdef) into the actual unsigned char value 0123456789-10-11-12-13-14-15
*/
unsigned char get_number(unsigned char t){
  unsigned char b = 0;
  if (t>96 && t<103) b = t - 87;
  if (t>47 && t<58) b = t ^ 48;
  return b;
}

/*
sends a pulse (marker for a bit) that is pulse_us long and then wait_us long
our protocol has a 500uS pulse and then 1000uS low for a 0 and a 2000uS low for a 1
*/
void pulse_and_wait(int pulse_us, int wait_us){
  digitalWrite(TX_PIN, HIGH);
  delayMicroseconds(pulse_us);
  digitalWrite(TX_PIN, LOW);
  
  delayMicroseconds(wait_us); 
}

/*
send the on off keying sequence in cmd string.
every bit is preceded by the starter oulse that is pulse_us long
a low bit is low_bit_us long
a high bit is high_bit_us long
This relies on the pulse_and_wait function (which can be overriden when the on off needs to be handled differently
*/
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
  //this is the command I send for device id 155, battery is low temperature is -15.5 and the hunidity 105% :D
  static String cmd="9b 0f 68 f6 8";
  
  //if there's serial data in the buffer read it and put it into the command string
  //There's no heap overflow here because the serial RX buffer is at default only 64 bytes
  if (Serial.available()>0) {
    cmd = Serial.readString();  
  }
  
  //send 10 requests with the cmd every 1 second
  delay(1000);
  
  for (int i=0; i<10;i++) {
    //send the cmd 
    send_ook_tlow(&cmd, 500, 4000, 1000, 2000);
    //wait 1 ms for the next request (this is protocol dependent
    delay(1);
  }
}
