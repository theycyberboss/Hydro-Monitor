/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include <VernierLib.h>
#include <math.h>
//Beginning of Auto generated function prototypes by Atmel Studio
float readTemp();
float readConductivity();
float thermistor(int raw);
void writeData(char c);
void writeData(String input);
//End of Auto generated function prototypes by Atmel Studio




VernierLib vLib;
float sensorReading1;
void setup() {
  
  Serial.begin(9600);
   vLib.autoID();
}

void loop() 
{  
  delay(800);
  Serial.flush();
  writeData("$0:" + String(readTemp()));
  writeData("$1:" + String(readConductivity()));
  //$0:27.03# - This is the basic example of the way that the messages are sent and recieved. 
  
 
  }

//Create the actual tempeture readings
float readTemp()
{

  float rawCount=analogRead(A2); //read one data value (0-1023)
  float voltage=rawCount/1023*5; //convert raw count to voltage (0-5V)
  float temp = thermistor(rawCount);
  
  return temp;

}

//Get the conductivity readings from cond. sensor
float readConductivity()
{
  sensorReading1 = vLib.readSensor();
   //return cond;

   return sensorReading1;
}

//Convert tempeture voltage into a float
float thermistor(int raw) {
  float resistor=15000; //initialize value of fixed resistor in Vernier shield
  float resistance; //create local variable for resistance
  float temp; //create local variable for temperature
 
  resistance=log(resistor*raw/(1024-raw)); //calculate resistance
  temp = 1 / (0.00102119 + (0.000222468 * resistance) + (0.000000133342 * resistance * resistance * resistance)); //calculate temperature using the Steinhart-Hart equation
  temp = temp - 273.15; //Convert Kelvin to Celsius                      
  return temp; //return the temperature
}

void writeData(char c)
{
  Serial.write(c); //Write the serial data
  
}

void writeData(String input)
{
  String data = input + "#";
  
  for(int i = 0; i < data.length(); i++)
  {
    Serial.write(data[i]); //Write the serial data
  } 
}
