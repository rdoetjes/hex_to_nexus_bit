
#define TX_PIN 2

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
  static String cmd="9b 0f 68 f6 8";
  
  if (Serial.available()>0) {
    cmd = Serial.readString();  
  }
  
  delay(1000);
  
  for (int i=0; i<10;i++) {
    send_ook_tlow(&cmd, 500, 4000, 1000, 2000);
    delay(1);
  }
}