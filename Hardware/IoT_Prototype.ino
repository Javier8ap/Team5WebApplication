#include "ThingSpeak.h"
#include "ESP8266WiFi.h"
#include <DFRobot_EC.h>
#include <EEPROM.h>

//GENERAL CONFIG
const unsigned long SECOND = 1000;
const unsigned long HOUR = 3600 * SECOND;
const int analogInPin = A0;

//WIFI CONFIG
WiFiClient client;
//char ssid[] = "INFINITUM6303_2.4";
//char pass[] = "UjV6s3ycyF";
char ssid[] = "AXTEL XTREMO-2187";
char pass[] = "037E2187";
int status = WL_IDLE_STATUS;

//unsigned long myChannelNumber = 1018851; //thingspeak Karim
//const char * myWriteAPIKey = "VXZQ9UM67S520JCF";

unsigned long myChannelNumber = 1040901; //thingspeak Javier
const char * myWriteAPIKey = "IZ7YEPJVW4GN6XE5";

//PH CONFIG
float calibration = 0; //0.46; //change this value to calibrate
int sensorValue = 0;
unsigned long int avgValue, avgValue_temp, avgValue_ph;
float b;
int temp, temp_temp, temp_ph, buf[10];

//SALINITY CONFIG
float voltage, ecValue, temperature = 25;
float salValue = 0;
DFRobot_EC ec;

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode( 16, OUTPUT ); //TEMPERATURE
  pinMode( 5, OUTPUT ); //PH
  pinMode( 4, OUTPUT ); //CONDUCTIVITY
  pinMode( 0, OUTPUT ); //CONDUCTIVITY OR OTHERS SELECTOR

  EEPROM.begin(256);
  Serial.begin( 115200 );
  WiFi.begin( ssid, pass );
  ThingSpeak.begin( client );
  ec.begin();
}

// the loop function runs over and over again forever
void loop() {

  while ( WiFi.status() != WL_CONNECTED )
  {
    delay( 200 );
    Serial.print( "." );
  }

  // TEMPERATURE AND PH SELECTOR ON
  digitalWrite( 0, HIGH );

  // TEMPERATURE LECTURE
  digitalWrite( 16, HIGH );
  delay( 1000 );
  for ( int i = 0; i < 50; i++ )
  {
    temp_temp = analogRead( analogInPin );
    avgValue_temp += temp_temp;
    delay( 30 );
  }
  float tempValue = ( float )avgValue_temp * 3.3 / 1024 / 5;
  avgValue_temp = 0;
  digitalWrite( 16, LOW );

  delay( 1000 );

  // PH LECTURE
  digitalWrite( 5, HIGH );
  delay( 1000 );
  for ( int i = 0; i < 10; i++ )
  {
    buf[i] = analogRead( analogInPin );
    delay( 30 );
  }
  for ( int i = 0; i < 9; i++ )
  {
    for ( int j = i + 1; j < 10; j++ )
    {
      if ( buf[i] > buf[j] )
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for ( int i = 0; i < 10; i++ )
    avgValue += buf[i];
  float pHVol = ( float )avgValue * 3.3 / 1024 / 10;
  float phValue = 4.2424 * pHVol - calibration;
  digitalWrite( 5, LOW );

  delay( 1000 );


  // SALINITY SELECTOR ON
  digitalWrite( 0, LOW );

  // SALINITY LECTURE
  digitalWrite( 4, HIGH );
  delay( 1000 );
  static unsigned long timepoint = millis();
  if (millis() - timepoint > 1000U) //time interval: 1s
  {
    timepoint = millis();
    voltage = analogRead( analogInPin ) / 1024.0 * 5000; // read the voltage
    Serial.println(voltage);
    ecValue =  ec.readEC( voltage, temperature );  // convert voltage to EC with temperature compensation
    Serial.println(ecValue);
    salValue = ( ( ecValue * 1000 ) / 20000 );
  }
  ec.calibration(voltage, temperature); // calibration process by Serail CMD

  digitalWrite( 4, LOW );

  Serial.print( "PH_sensor: " );
  Serial.print( phValue, 1 );
  Serial.print( " ph  TEMPERATURE_sensor: " );
  Serial.print( tempValue, 2 );
  Serial.print( " °C  SALINITY_sensor: " );
  Serial.print( salValue, 3 );
  Serial.println( " %" );

  ThingSpeak.setField( 1, phValue );
  ThingSpeak.setField( 2, salValue );
  ThingSpeak.setField( 3, tempValue );
  ThingSpeak.writeFields( myChannelNumber, myWriteAPIKey );

  delay( 1000 );
  //delay( 5 * HOUR ); // 5 horas de espera para nueva medición
}
