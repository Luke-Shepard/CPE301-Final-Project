/*
Name: Ben Miller, Luke Shepard
UNR CPE 301 Final Project
Date: 12/15/2023
*/

//library decleration and pin definitions
#include <LiquidCrystal.h>
#include <DHT.h>
#include <Stepper.h>

#define FAN_PIN 9
#define DHT_PIN 7
#define DHT_TYPE DHT11
#define STEPPER_PIN 46
#define DRE 0x10
#define TBE 0x20  
#define RDA 0x80
#define WATER_SENSOR_PIN A5
#define STEPPER_ENABLE_PIN 53
#define STEPPER_DIRECTION_PIN 52

//pointer defenitions
volatile unsigned int currentTicks = 65535;
volatile unsigned char* pin_a = (unsigned char*) 0x20;
volatile unsigned char* ddr_a  = (unsigned char*) 0x21; 
volatile unsigned char* port_a = (unsigned char*) 0x22;
volatile unsigned char* pin_b = (unsigned char*) 0x23;
volatile unsigned char* ddr_b  = (unsigned char*) 0x24; 
volatile unsigned char* port_b = (unsigned char*) 0x25;
volatile unsigned char* pin_d = (unsigned char*) 0x29;
volatile unsigned char* ddr_d  = (unsigned char*) 0x2A; 
volatile unsigned char* port_d = (unsigned char*) 0x2B;
volatile unsigned char* pin_h = (unsigned char*) 0x100;
volatile unsigned char* ddr_h  = (unsigned char*) 0x101; 
volatile unsigned char* port_h = (unsigned char*) 0x102;
volatile unsigned char* pin_c  = (unsigned char*) 0x26;
volatile unsigned char* ddr_c  = (unsigned char*) 0x27; 
volatile unsigned char* port_c = (unsigned char*) 0x28;
volatile unsigned char* pin_e = (unsigned char*) 0x2c;
volatile unsigned char* ddr_e  = (unsigned char*) 0x2d; 
volatile unsigned char* port_e = (unsigned char*) 0x2e;
volatile unsigned char* pin_f = (unsigned char*) 0x2F;
volatile unsigned char* ddr_f  = (unsigned char*) 0x30; 
volatile unsigned char* port_f = (unsigned char*) 0x31;
volatile unsigned char* pin_g = (unsigned char*) 0x32;
volatile unsigned char* ddr_g  = (unsigned char*) 0x33; 
volatile unsigned char* port_g = (unsigned char*) 0x34;
volatile unsigned char* pin_L = (unsigned char*) 0x3B;
volatile unsigned char* ddr_L  = (unsigned char*) 0x3C; 
volatile unsigned char* port_L = (unsigned char*) 0x3D;
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;
volatile unsigned char *myTCCR1A  = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B  = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C  = (unsigned char *) 0x82;
volatile unsigned char *myTIMSK1  = (unsigned char *) 0x6F;
volatile unsigned char *myTIFR1   = (unsigned char *) 0x36;
volatile unsigned int  *myTCNT1   = (unsigned int *) 0x84;
volatile unsigned char* analogPort_A5 = (unsigned char*) 0x67;
volatile unsigned char* analogPin_A5 = (unsigned char*) 0x68;
volatile unsigned char* analogDdr_A5  = (unsigned char*) 0x66;

const int rs = 12, en = 11, d4 = 22, d5 = 24, d6 = 26, d7 = 28;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

DHT dht(DHT_PIN, DHT_TYPE);

float temp = 0;
float hum = 0;
int waterLevel = 0;

// Stepper motor configuration
const int stepsPerRevolution = 2038;
Stepper myStepper(stepsPerRevolution, 31, 33, 35, 37);

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  pinMode(FAN_PIN, OUTPUT);
  pinMode(STEPPER_PIN, INPUT);
  pinMode(STEPPER_ENABLE_PIN, INPUT);
  pinMode(STEPPER_DIRECTION_PIN, OUTPUT);

  digitalWrite(FAN_PIN, LOW);

  lcd.begin(16, 2);
  
  myStepper.setSpeed(5);
}

void loop() {
  my_delay(1000);
  
  temp = dht.readTemperature();
  hum = dht.readHumidity();

  waterLevel = analogRead(WATER_SENSOR_PIN);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print(" F");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(hum);
  lcd.print("%");

  if (digitalRead(STEPPER_ENABLE_PIN) == HIGH) {
    myStepper.step(stepsPerRevolution);
    my_delay(100);
  }

  if (waterLevel >= 100) {
    displayHighWaterLevel();
  } else {
    displayLowWaterLevel();
  }
}

void displayHighWaterLevel() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print("F");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(hum);
  lcd.print("%");

  // Displaying water high indication
  digitalWrite(STEPPER_DIRECTION_PIN, HIGH);
  digitalWrite(FAN_PIN, HIGH);
  lcd.print("Water High");
}

void displayLowWaterLevel() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print("F");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(hum);
  lcd.print("%");

  // Displaying water low indication
  digitalWrite(STEPPER_DIRECTION_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);
  lcd.print("Water Low");
}

void U0init(int U0baud){
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}
unsigned char U0kbhit(){
  return *myUCSR0A & RDA;
}
unsigned char U0getchar(){
  return *myUDR0;
}
void U0putchar(unsigned char U0pdata){
  while((*myUCSR0A & TBE) == 0);
  *myUDR0 = U0pdata;
}

void adc_init(){
  // setup the A register
  *my_ADCSRA |= 0b10000000; // set bit   7 to 1 to enable the ADC
  *my_ADCSRA &= 0b11011111; // clear bit 6 to 0 to disable the ADC trigger mode
  *my_ADCSRA &= 0b11110111; // clear bit 5 to 0 to disable the ADC interrupt
  *my_ADCSRA &= 0b11111000; // clear bit 0-2 to 0 to set prescaler selection to slow reading
  // setup the B register
  *my_ADCSRB &= 0b11110111; // clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &= 0b11111000; // clear bit 2-0 to 0 to set free running mode
  // setup the MUX Register
  *my_ADMUX  &= 0b01111111; // clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX  |= 0b01000000; // set bit   6 to 1 for AVCC analog reference
  *my_ADMUX  &= 0b11011111; // clear bit 5 to 0 for right adjust result
  *my_ADMUX  &= 0b11100000; // clear bit 4-0 to 0 to reset the channel and gain bits
}

unsigned int adc_read(unsigned char adc_channel_num){
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX  &= 0b11100000;
  // clear the channel selection bits (MUX 5)
  *my_ADCSRB &= 0b11110111;
  // set the channel number
  if(adc_channel_num > 7)
  {
    // set the channel selection bits, but remove the most significant bit (bit 3)
    adc_channel_num -= 8;
    // set MUX bit 5
    *my_ADCSRB |= 0b00001000;
  }
  // set the channel selection bits
  *my_ADMUX  += adc_channel_num;
  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;
  // wait for the conversion to complete
  while((*my_ADCSRA & 0x40) != 0);
  // return the result in the ADC data register
  return *my_ADC_DATA;
}

void setup_timer_regs(){
  // setup the timer control registers
  *myTCCR1A= 0x00;
  *myTCCR1B= 0X00;
  *myTCCR1C= 0x00;
  // reset the TOV flag
  *myTIFR1 |= 0x01;
  // enable the TOV interrupt
  *myTIMSK1 |= 0x01;
}

ISR(TIMER1_OVF_vect){
  // Stop the Timer
  *myTCCR1B &=0xF8;
  // Load the Count
  currentTicks = 65535;
  *myTCNT1 =  (unsigned int) (65535 -  (unsigned long) (currentTicks));
  // Start the Timer
  *myTCCR1B |=   0b00000001;
  // if it's not the STOP amount
  if(currentTicks != 65535)
  {
    // XOR to toggle PB6
    *port_b ^= 0x40;
  }
}

void my_delay(unsigned int freq){
  double period = 1.0/double(freq);
  double half_period = period/ 2.0f;
  double clk_period = 0.0000000625;
  unsigned int ticks = half_period / clk_period;
  *myTCCR1B &= 0xF8;
  *myTCNT1 = (unsigned int) (65536 - ticks);
  * myTCCR1B |= 0b00000001;
  while((*myTIFR1 & 0x01)==0);
  *myTCCR1B &= 0xF8;
  *myTIFR1 |= 0x01;
}
