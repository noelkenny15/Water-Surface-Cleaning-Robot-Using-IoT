// RC Water Cleaning robot


/* 
 *  Connection:  
 *  PPM
 *  Channel 1-6 : Pins 8-13
*/


// output pins
#define bucket_dc_D1 2
#define bucket_dc_D2 7
#define bucket_dc_pwm 6
#define R_PUMP_POWER_PIN 1
#define R_PUMP_PWM_PIN 3
#define L_PUMP_POWER_PIN 4
#define L_PUMP_PWM_PIN 5

#define volt A0
#define buzzer A1
#define MIN_VOLT 500 // yet to be tested

#define protocol_pin 0 // if High, then PPM is used, else the nRf is used



/***********************************************************************************************************/
/******************************************     PPM Configuration ******************************************/
#define channels_used 3
#define BUCKET_CHANNEL 0
#define PUMP_CHANNEL_X 1
#define PUMP_CHANNEL_Y 2
/***********************************************************************************************************/
/***********************************************************************************************************/



/***********************************************************************************************************/
/******************************************     nRF Configuration ******************************************/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE 8
#define CSN 9
RF24 radio(CE, CSN);
const byte address[6] = "00100";

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct data_pack {
  uint16_t j1PotX;
  uint16_t j1PotY;
  uint16_t j2PotX;
  uint16_t j2PotY;
  uint16_t pot1;
  uint16_t pot2;
  byte tSwitch1;
  byte tSwitch2;
  byte button1;
  byte button2;
  byte button3;
  byte button4;
  byte j1Button;
  byte j2Button;
};

data_pack data;
/***********************************************************************************************************/
/***********************************************************************************************************/




uint16_t pump_pwm_R, pump_pwm_L;


/*** PPM receiver pins ***/
const byte PPM_pins[] = {8, 9, 10, 11, 12, 13};
volatile uint16_t channels_pack[channels_used];
int16_t pwm_signal;


void setup() {

  // choosing the communication protocol
  if (digitalRead(protocol_pin)) {
    // If the signal is High, then PPM is used
    configure_PPM();
  }
  else {
    configure_nRF();
  }
  
  DDRD = 0b11111111;  // Set pins 0-7 to OUTPUT
  PORTD = 0b00000000; // Set pins 0-7 to LOW
  
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  pinMode(volt, INPUT);
}

void loop() {
  if (protocol_pin == LOW) {  // nRf is used
    get_nRF_data();
  }
  control_pump();
  move_bucket();
  check_volt();
}

void configure_PPM(){
  DDRB = 0b000000;   // set all port B pins to input
  PCICR = 0b000001;  // enable pin change interrupt 0
  PCMSK0 = 0b001111; // mask pins to trigger interrupt
  sei();             // enable interrupt globally
}


// Interrupt service routine
ISR(PCINT0_vect) {
  /*** code that runs when pin change interrupt 0 is triggered ***/
  // update the corresponding element in channels array with the time (1000 - 2000 microsecond)
  static long old_value[channels_used];
  static uint16_t previous_state;
  for (int i = 0; i < channels_used; i++) {
    if (!bitRead(previous_state, i) && bitRead(PINB, i)) // resembling rising edge
    {
      old_value[i] = micros();
    }
    if (bitRead(previous_state, i) && !bitRead(PINB, i)) // resembling falling edge
    {
      channels_pack[i] = micros() - old_value[i];
    }
    
  }
  previous_state = PINB;
}

void move_bucket() {
  if (channels_pack[BUCKET_CHANNEL] >= 1600) // resembling forward motion 
  {
    // set speed
    pwm_signal = map(channels_pack[BUCKET_CHANNEL], 1600, 2000, 0, 255);
    analogWrite(bucket_dc_pwm, pwm_signal);
    // set direction
    digitalWrite(bucket_dc_D1, HIGH);
    digitalWrite(bucket_dc_D2, LOW);
  }
  if (channels_pack[BUCKET_CHANNEL] <= 1400) // resembling backward motion 
  {
    // set speed
    pwm_signal = map(channels_pack[BUCKET_CHANNEL], 1000, 1400, 255, 0);
    analogWrite(bucket_dc_pwm, pwm_signal);
    // set direction
    digitalWrite(bucket_dc_D1, LOW);
    digitalWrite(bucket_dc_D2, HIGH);
  }
  if (channels_pack[BUCKET_CHANNEL] > 1400 && channels_pack[BUCKET_CHANNEL] < 1600) // resembling no motion 
  {
    digitalWrite(bucket_dc_D1, LOW);
    digitalWrite(bucket_dc_D2, LOW);
  }
}

void control_pump() {
  int x = channels_pack[PUMP_CHANNEL_X];
  int y = channels_pack[PUMP_CHANNEL_Y];
  pump_pwm_R = constrain((max((y - 1500), 0) + (1500 - x)), 0, 500);
  pump_pwm_R = map(pump_pwm_R, 0, 500, 0, 255);
  pump_pwm_L = constrain((max((y - 1500), 0) - (1500 - x)), 0, 500);
  pump_pwm_L = map(pump_pwm_L, 0, 500, 0, 255);
  if (pump_pwm_R > 10) {
    digitalWrite(R_PUMP_POWER_PIN, HIGH);
    analogWrite(R_PUMP_PWM_PIN, pump_pwm_R);
  }
  else
  {
    digitalWrite(R_PUMP_POWER_PIN, LOW);
  }

  if (pump_pwm_L > 10) {
    digitalWrite(L_PUMP_POWER_PIN, HIGH);
    analogWrite(L_PUMP_PWM_PIN, pump_pwm_L);
  }
  else
  {
    digitalWrite(L_PUMP_POWER_PIN, LOW);
  }
}


void configure_nRF() {
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.startListening(); //  Set the module as receiver
}

void get_nRF_data() {
  /*
   * Get data from the nRF data struct.
   * Mapping the data to the same range in which the PPM signals are and
   * saving the data to the same block in memoryto generalize the control 
   * code.
   */
  channels_pack[BUCKET_CHANNEL] = map(data.j2PotY, 0, 1023, 1000, 2000);
  channels_pack[PUMP_CHANNEL_X] = map(data.j1PotX, 0, 1023, 1000, 2000);
  channels_pack[PUMP_CHANNEL_Y] = map(data.j1PotY, 0, 1023, 1000, 2000);
}

void check_volt() {
  if (analogRead(volt) <= MIN_VOLT) {
    digitalWrite(buzzer, HIGH);
  }
  else
  {
    digitalWrite(buzzer, LOW);  
  }
}
