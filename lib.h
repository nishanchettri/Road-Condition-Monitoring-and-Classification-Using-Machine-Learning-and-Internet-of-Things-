#ifndef __NISSAN_H__
#define __NISSAN_H__

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include "I2Cdev.h"
#include "Wire.h" 
#include "OneButton.h"                              
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>



OneButton button(D3, true);   
const String FirmwareVer={"1.0"}; 
#define g D8
#define b D5

#define URL_fw_Version "http://iotprojectnis.000webhostapp.com/version.txt"
#define URL_fw_Bin "http://iotprojectnis.000webhostapp.com/firmware.bin"
HTTPClient http; 

void FirmwareUpdate()
{
  http.begin(URL_fw_Version);     // check version URL
  delay(100);
  int httpCode = http.GET();            // get data from version file
  delay(100);
  String payload;
  if (httpCode == HTTP_CODE_OK)         // if version received
  {
    payload = http.getString();  // save received version
    Serial.println(payload );
  }
  else
  {
    Serial.print("error in downloading version file:");
    Serial.println(httpCode);

  }
  
  http.end();
  if (httpCode == HTTP_CODE_OK)         // if version received
  {
  if(payload.equals(FirmwareVer) )
  {   
     Serial.println("Device already on latest firmware version"); 
  }
  else
  {
     Serial.println("New firmware detected");
     WiFiClient client;

    // The line below is optional. It can be used to blink the LED on the board during flashing
    // The LED will be on during download of one buffer of data from the network. The LED will
    // be off during writing that buffer to flash
    // On a good connection the LED should flash regularly. On a bad connection the LED will be
    // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
    // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);


    t_httpUpdate_return ret = ESPhttpUpdate.update(URL_fw_Bin);
    
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");  
              for(int i=0;i<5;i++)
      {
        digitalWrite(g,HIGH);
        delay(500);
        digitalWrite(g,LOW);
        delay(500);
      }
        break;
    } 
  }
 }  
}


/**************ACCELEROMETER********************************/

const uint8_t MPU6050SlaveAddress = 0x68;
const uint8_t scl = D6;
const uint8_t sda = D7;
double Ax, Ay, Az;
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;
int16_t AccelX, AccelY, AccelZ;

/****************ACCELEROMETER_END*******************/

/****************GPS_START***************************/

TinyGPSPlus gps;  // The TinyGPS++ object
SoftwareSerial ss(4, 5);// rx- d1,tx=d2
float latitude , longitude;
int year , month , date, hour , minute , second,pm;
String lat_str , lng_str,date_str , time_str,aax,aay,aaz;
void GPS();

/*****************GPS_END*****************************/

/********************WIFI_SETTINGS******************************/

const char* ssid     = "nissan";
const char* password = "password";
const char* host = "iotprojectnis.000webhostapp.com";
const int httpPort = 80;
String send_str;

/*********************END_WIFI*********************************/


void GPS()
{
   while (ss.available() > 0)
    if (gps.encode(ss.read()))
    {
      if (gps.location.isValid())
      {
        latitude = gps.location.lat();
        lat_str = String(latitude , 6);
        longitude = gps.location.lng();
        lng_str = String(longitude , 6);
      }
      if (gps.date.isValid())
      {
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
          date_str = '0';
        date_str += String(date);

        date_str += " / ";

        if (month < 10)
          date_str += '0';
        date_str += String(month);

        date_str += " / ";

        if (year < 10)
          date_str += '0';
        date_str += String(year);
      }

      if (gps.time.isValid())
      {
        time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();

        minute = (minute + 30);
        if (minute > 59)
        {
          minute = minute - 60;
          hour = hour + 1;
        }
        hour = (hour + 5) ;
        if (hour > 23)
          hour = hour - 24;

        if (hour >= 12)
          pm = 1;
        else
          pm = 0;

        hour = hour % 12;

        if (hour < 10)
          time_str = '0';
        time_str += String(hour);

        time_str += " : ";

        if (minute < 10)
          time_str += '0';
        time_str += String(minute);

        time_str += " : ";

        if (second < 10)
          time_str += '0';
        time_str += String(second);

        if (pm == 1)
          time_str += " PM ";
        else
          time_str += " AM ";

      }
    }
}

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data)
{
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
  
}

// read all 14 register
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress)
{
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  
}

//configure MPU6050
void MPU6050_Init()
{
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);//set +/-250 degree/second full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);// set +/- 2g full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
  
}
void longclick()
 {
      for(int i=0;i<5;i++)
      {
        digitalWrite(g,HIGH);
        delay(500);
        digitalWrite(g,LOW);
        delay(500);
      }
       FirmwareUpdate();
       
    
 }
#endif
