//Sample using LiquidCrystal library
#include <LiquidCrystal.h>

/*******************************************************

This program will test the LCD panel and the buttons

********************************************************/

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
const int BACKLIGHT = 10;
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// define thermometer values
const int THERM = A1;
int temperature = 0;
const int SAMPLE_NUMBER = 10;
const double BALANCE_RESISTOR = 10000.0;
const double MAX_ADC = 1023.0;
const double BETA = 3950.0;
const double ROOM_TEMP = 298.15;
const double RESISTOR_ROOM_TEMP = 10000.0;
double currentTemperature = 0;
bool unit = false;
bool backlight = false;

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these values: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  

 // For V1.0 comment the other threshold and use the one below:
/*
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   
*/


 return btnNONE;  // when all others fail, return this...
}

void setup()
{
 pinMode(BACKLIGHT, OUTPUT);
 //pinMode(THERM, INPUT);
 lcd.begin(16, 2);              // start the library
 lcd.setCursor(0,0);
 lcd.print("Good Day"); // print a simple message
 Serial.begin(9600);
}
void loop()
{
 currentTemperature = readThermistor();
 int roundTemp = (int)currentTemperature;
 delay(500);
 //temperature = analogRead(THERM);
 lcd.setCursor(0,1);
 lcd.print("Temperature: ");
 lcd.setCursor(13,1);            // move cursor to second line "1" and 9 spaces over
 if(!unit) {
  lcd.print(roundTemp);      // display seconds elapsed since power-up
  lcd.print("C");
 }
 else {
  roundTemp = (int)(currentTemperature*1.8 + 32);
  lcd.print(roundTemp);
  lcd.print("F");
 }
 if(backlight) {
   digitalWrite(BACKLIGHT, HIGH);
 }
 else {
   digitalWrite(BACKLIGHT, LOW);
 }
 Serial.println(currentTemperature);
 Serial.flush();
 lcd.setCursor(0,1);            // move to the begining of the second line
 lcd_key = read_LCD_buttons();  // read the buttons
 
 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
      if(unit) {
        unit = false;
      }
      else {
        unit = true;
      }
      //lcd.print(currentTemperature*1.8 + 32);
     //lcd.print("RIGHT ");
     break;
     }
     
   case btnLEFT:
    {
      if(backlight) {
        backlight = false;
      }
      else {
        backlight = true;
      }
     //lcd.print(adc_key_in);
    // lcd.print(" v");
     break;
     }
     /*
   case btnUP:
     {
     lcd.print("UP    ");
     break;
     }
   case btnDOWN:
     {
     lcd.print("DOWN  ");
     break;
     }
   case btnSELECT:
     {
     lcd.print("SELECT");
     break;
     }
     case btnNONE:
     {
     lcd.print("TEST  ");
     break;
     }
     */
 }
 
}

double readThermistor() 
{
  // variables that live in this function
  double rThermistor = 0;            // Holds thermistor resistance value
  double tKelvin     = 0;            // Holds calculated temperature
  double tCelsius    = 0;            // Hold temperature in celsius
  double adcAverage  = 0;            // Holds the average voltage measurement
  int    adcSamples[SAMPLE_NUMBER];  // Array to hold each voltage measurement

  /* Calculate thermistor's average resistance:
     As mentioned in the top of the code, we will sample the ADC pin a few times
     to get a bunch of samples. A slight delay is added to properly have the
     analogRead function sample properly */
  
  for (int i = 0; i < SAMPLE_NUMBER; i++) 
  {
    adcSamples[i] = analogRead(THERM);  // read from pin and store
    delay(10);        // wait 10 milliseconds
  }

  /* Then, we will simply average all of those samples up for a "stiffer"
     measurement. */
  for (int i = 0; i < SAMPLE_NUMBER; i++) 
  {
    adcAverage += adcSamples[i];      // add all samples up . . .
  }
  adcAverage /= (double)SAMPLE_NUMBER;        // . . . average it w/ divide

  /* Here we calculate the thermistor’s resistance using the equation 
     discussed in the article. */
  rThermistor = BALANCE_RESISTOR / ( (MAX_ADC / adcAverage) - 1.0);

  /* Here is where the Beta equation is used, but it is different
     from what the article describes. Don't worry! It has been rearranged
     algebraically to give a "better" looking formula. I encourage you
     to try to manipulate the equation from the article yourself to get
     better at algebra. And if not, just use what is shown here and take it
     for granted or input the formula directly from the article, exactly
     as it is shown. Either way will work! */
  tKelvin = (BETA * ROOM_TEMP) / 
            (BETA + (ROOM_TEMP * log(rThermistor / RESISTOR_ROOM_TEMP)));

  /* I will use the units of Celsius to indicate temperature. I did this
     just so I can see the typical room temperature, which is 25 degrees
     Celsius, when I first try the program out. I prefer Fahrenheit, but
     I leave it up to you to either change this function, or create
     another function which converts between the two units. */
  tCelsius = tKelvin - 273.15;  // convert kelvin to celsius 

  return tCelsius;    // Return the temperature in Celsius
}

