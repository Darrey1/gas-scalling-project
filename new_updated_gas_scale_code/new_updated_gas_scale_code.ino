
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include "HX711.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);


#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 6

unsigned long previousMillis = 0;       
unsigned long interval = 60000; //start off

char replybuffer[255];
SoftwareSerial sim800l(FONA_TX, FONA_RX);
Adafruit_FONA gsm(FONA_RST);
HX711 scale;
 
int rbutton = 8; // this button will be used to reset the scale to 0.
float weight; 
// float scale_va = 141.006076 * 1000;
float calibration_factor = 741112.836;
float scale_va = 123.518806 * 1000;
long offset_value = 4294555816;

void setup() 
{
  Serial.begin(9600);
  sim800l.begin(9600);
  pinMode(rbutton, INPUT); 
  scale.begin(4,5);
  
  scale.set_offset(offset_value);
  scale.set_scale(scale_va);

  scale.tare(); //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
  
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(6,0);
  lcd.print("Cylinder");
  lcd.setCursor(1,1);
  lcd.print("Weighing Scale");
  delay(3000);
  lcd.clear();
}



void loading(int row, int max){
  int timecount = 0;
  while (timecount <= max){
  lcd.setCursor(row,0);   //Set cursor to character 2 on line 0
  lcd.print(".");
  lcd.setCursor(row,0); 
  delay(500);
  lcd.print("..");
  lcd.setCursor(row,0);
  delay(500);
  lcd.print("...");
  delay(500);
  lcd.setCursor(row,0);
  lcd.print("   ");
  delay(300);
  timecount++;
  }
}



void sendMessage(char message) {
  if (!gsm.begin(sim800l)) {            
    Serial.println(F("Couldn't find SIM800L!"));
    while (1);
  }
  
  char sendto[21] = "09074842728";
  bool status = gsm.sendSMS(sendto, message);
  if (status){
      Serial.println("Message Sent!");
  }
  else{
    Serial.println("Error!");
  }
  
  
  delay(1000);
}


 
void loop() 
 
{
  scale.set_offset(offset_value);
  scale.set_scale(scale_va); 
 
  weight = abs(scale.get_units(5)); 
  Serial.print("the weight is:");
  Serial.print(weight);
  int state = digitalRead(rbutton);
  Serial.print("the switch state:");
  Serial.println(state);
  lcd.setCursor(0, 0);
  lcd.print("Measured Weight");
  lcd.setCursor(0, 1);
  lcd.print(weight);
  lcd.print(" KG  ");
  delay(2000);
  lcd.clear();
  
  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.println(" KG");
  Serial.println();
  weight_monitor();
  



 
 
}


void weight_monitor() {
  unsigned long currentMillis = millis();
  Serial.print("current milliseconds: ");
  Serial.println(currentMillis);
  float epsilon = 0.05;
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (abs(weight - 4.5) < epsilon) {
      notify("The gas level is low, prepare for the filling!", "09074842728");
    } else if (weight >= 3.0 && weight <= 3.6) {
      notify("The gas has finished, need to be refill!", "09074842728");
    } else {
      previousMillis = 0; // Reset previousMillis if weight doesn't meet conditions
    }
  }
}



void notify(const char* message, const char* phoneNumber) {
  lcd.print("Initializing");
  loading(12, 5);
  scale.set_scale();
  scale.tare(); // Reset the scale to 0
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sending Sms");
  loading(11, 3);
  lcd.setCursor(0, 1);
  lcd.print("To:");
  lcd.print(phoneNumber);
  
  sendMessage(message);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Message Sent To!");
  lcd.setCursor(2, 1);
  lcd.print(phoneNumber);
  delay(5000);  // Delay to show the message sent status
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Monitoring");
  loading(11, 3);
}



