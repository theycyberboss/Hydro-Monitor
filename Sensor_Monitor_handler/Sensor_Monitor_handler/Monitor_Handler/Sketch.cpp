#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>


String readDataBytes();
void printError(int errorCode);
void printToDisplay();
void parseData(String data);
void checkReadings();
void initDisplay();

LiquidCrystal_I2C lcd(0x27, 16, 2);

//Version 1.2

//ERROR CODES
const int PH_ERROR = 0;
const int TEMP_ERROR = 1;
const int CONDUCT_ERROR = 2;
const int DATA_ERROR = 3;

//Thresh Holds 
const float TEMP_THREASHOLD = 1.5f;
const float PH_THREASHOLD = 0.2f;
const float CONDUCT_THREASHOLD = 150.0f;

const float TEMP_OPTIMAL = 20.0f;
const float PH_OPTIMAL = 7.0f;
const float CONDUCT_OPTIMAL = 1200.0f;

const int SYNCH_PIN = 8;

//Actual sensor readings
float temp,ph,conductivity;

//boolean for each sensor
bool ph_high, ph_normal, ph_low; //May be removed in the final program

int temp_state = 0; //0 - high 1 - normal 2 - low
String temp_state_string = "BOOT";

int ph_state = 0; //0 - high 1 - normal 2 - low
String ph_state_string = "BOOT";

int cond_state = 0; //0 - high 1 - normal 2 - low
String cond_state_string = "BOOT";

bool readingData = true;
bool synchError = false;

String currentError;
String data;



void setup() {

  pinMode(SYNCH_PIN,OUTPUT);
  
  initDisplay();
  
  Serial.begin(9600);
}


//Loop function
void loop() {
  

   
    
   if(Serial.available() > 0){ //Make sure we dont have a synch error before we output to the display
     readingData = true; 
     data = readDataBytes();
     parseData(data);
     checkReadings();
     printToDisplay();
     
     
   }else {
    readingData = false;
    printError(DATA_ERROR);
   }
   delay(800);
   digitalWrite(SYNCH_PIN,LOW);
}

void initDisplay()
{
	lcd.begin();
	lcd.backlight();
	lcd.setCursor(0,0);
	lcd.print("Starting Monitor");
	
}

//Read in the incoming data as charecters and convert them into a string
String readDataBytes()
{
    String readData = "";

	while(readingData){
      char charecter =  Serial.read();
      readData += charecter;
	  
      if(charecter == '#') //The # is used to tell that it is the end of the 'packet' of data
	  {
		readingData = false;
        synchError = false;
          
      }
  	
	  digitalWrite(SYNCH_PIN,HIGH);
		
	} 
	   
    digitalWrite(SYNCH_PIN,LOW);
	
    Serial.flush();
    return readData.substring(0,readData.length()-1);
    
    
}

//Print out error codes, Codes are viewed at the top
void printError(int errorCode)
{
  if(errorCode == DATA_ERROR)
  {
    lcd.clear();
    lcd.print("DATA SYNCH ERROR");
    
  }else if(errorCode == TEMP_ERROR)
  {
    lcd.clear();
    lcd.print("TEMP SENSOR ERROR");
    
  }else if(errorCode == CONDUCT_ERROR)
  {
    lcd.clear();
    lcd.print("COND. SENSOR ERROR");
    
  }else if(errorCode == PH_ERROR)
  {
    lcd.clear();
    lcd.print("PH SENSOR ERROR");
    
  }
}

void printToDisplay()
{
  //Convert all of readings into a printable string
  String sTemp = "TEMP: " + String(temp) + " " + temp_state_string;
  String sPh = "PH: " + String(ph) +  " " + ph_state_string;
  String sConductivity = "COND: " + String((int)(conductivity + 0.5));

   
  lcd.clear();


  //Print out the temperature and the conductivity 
  lcd.setCursor(0,0);
  lcd.print(sTemp);

  lcd.setCursor(0,1);
  lcd.print(sConductivity);
  lcd.setCursor(12,1);
  lcd.print(cond_state_string);

}

//Get the data contained in the Serial String
void parseData(String data)
{
  if(data.startsWith("$0:"))
  {
    temp = data.substring(3).toFloat();
  }else if(data.startsWith("$2:"))
  {
    ph = data.substring(3).toFloat();
  }else if(data.startsWith("$1:"))
  {
    conductivity = data.substring(3).toFloat();
  }

  //Used to check if their was a reading error sent from the slave arduino. 
  if(data.startsWith("$E:"))
  {
    currentError = data.substring(3);
  }

  
}

//Check the sensor readings to make sure they are in the optimal place
void checkReadings()
{
  //Tempeture Readings
  if(temp <= (TEMP_OPTIMAL + TEMP_THREASHOLD) && temp >= (TEMP_OPTIMAL - TEMP_THREASHOLD)) // Check for normal temp
  {
    temp_state = 1;
    temp_state_string = "NORM";
  }else if(temp < TEMP_OPTIMAL - TEMP_THREASHOLD) // Check for low temp
  {
    temp_state = 2;
    temp_state_string = "LOW";
  }else if(temp > TEMP_OPTIMAL + TEMP_THREASHOLD) // Check for high temp
  {
    temp_state = 0;
    temp_state_string = "HIGH";
  }
  
  /////////////
  //Ph Readings
  
  /*
  if(ph <= (PH_OPTIMAL + PH_THREASHOLD) && ph >= (PH_OPTIMAL - PH_THREASHOLD)) // Check for normal temp
  {
    ph_state = 1;
    ph_state_string = "NORM";
  }else if(ph < PH_OPTIMAL - PH_THREASHOLD) // Check for low temp
  {
    ph_state = 2;
    ph_state_string = "LOW";
  }else if(ph > PH_OPTIMAL + PH_THREASHOLD) // Check for high temp
  {
    ph_state = 0;
    ph_state_string = "HIGH";
  }
  */
 

  ///////////////// 
  //Conductivity Readings

  if(conductivity <= (CONDUCT_OPTIMAL + CONDUCT_THREASHOLD) && conductivity >= (CONDUCT_OPTIMAL - CONDUCT_THREASHOLD)) // Check for normal temp
  {
    cond_state = 1;
    cond_state_string = "NORM";
  }else if(conductivity < CONDUCT_OPTIMAL - CONDUCT_THREASHOLD) // Check for low temp
  {
    cond_state = 2;
    cond_state_string = "LOW";
  }else if(conductivity > CONDUCT_OPTIMAL + CONDUCT_THREASHOLD) // Check for high temp
  {
    cond_state = 0;
    cond_state_string = "HIGH";
  }
}
