#include <LiquidCrystal.h>
#include <DHT.h>
#include <Stepper.h>

#define FAN_PIN 9
#define DHT_PIN 7
#define DHT_TYPE DHT11
#define STEPPER_PIN 46
#define WATER_SENSOR_PIN A5
#define STEPPER_ENABLE_PIN 53
#define STEPPER_DIRECTION_PIN 52

volatile unsigned int currentTicks = 65535;

volatile unsigned char* pin_a = (unsigned char*) 0x20;
volatile unsigned char* ddr_a  = (unsigned char*) 0x21; 
volatile unsigned char* port_a = (unsigned char*) 0x22;

volatile unsigned char* pin_b = (unsigned char*) 0x23;
volatile unsigned char* ddr_b  = (unsigned char*) 0x24; 
volatile unsigned char* port_b = (unsigned char*) 0x25;

volatile unsigned char* pin_c  = (unsigned char*) 0x26;
volatile unsigned char* ddr_c  = (unsigned char*) 0x27; 
volatile unsigned char* port_c = (unsigned char*) 0x28;

volatile unsigned char* pin_d = (unsigned char*) 0x29;
volatile unsigned char* ddr_d  = (unsigned char*) 0x2A; 
volatile unsigned char* port_d = (unsigned char*) 0x2B;

volatile unsigned char* pin_e = (unsigned char*) 0x2c;
volatile unsigned char* ddr_e  = (unsigned char*) 0x2d; 
volatile unsigned char* port_e = (unsigned char*) 0x2e;

volatile unsigned char* pin_f = (unsigned char*) 0x2F;
volatile unsigned char* ddr_f  = (unsigned char*) 0x30; 
volatile unsigned char* port_f = (unsigned char*) 0x31;

volatile unsigned char* pin_g = (unsigned char*) 0x32;
volatile unsigned char* ddr_g  = (unsigned char*) 0x33; 
volatile unsigned char* port_g = (unsigned char*) 0x34;

volatile unsigned char* pin_h = (unsigned char*) 0x100;
volatile unsigned char* ddr_h  = (unsigned char*) 0x101; 
volatile unsigned char* port_h = (unsigned char*) 0x102;

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

// Temperature and humidity module
DHT dht(DHT_PIN, DHT_TYPE);

float temperature = 0;
float humidity = 0;
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
  lcd.print("Booting Up...");
  
  myStepper.setSpeed(5);
}

void loop() {
  delay(1000);
  
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Reading water level using analogRead()
  waterLevel = analogRead(WATER_SENSOR_PIN);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" F");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%");

  if (digitalRead(STEPPER_PIN) == HIGH) {
    myStepper.step(stepsPerRevolution);
    delay(100);
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
  lcd.print(temperature);
  lcd.print(" F");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
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
  lcd.print(temperature);
  lcd.print(" F");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%");

  // Displaying water low indication
  digitalWrite(STEPPER_DIRECTION_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);
  lcd.print("Water Low");
}
