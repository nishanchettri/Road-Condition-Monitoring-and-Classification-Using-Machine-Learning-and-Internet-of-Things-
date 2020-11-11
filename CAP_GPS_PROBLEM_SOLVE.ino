#include "lib.h"

void setup()
{
  pinMode(g,OUTPUT);
  pinMode(b,OUTPUT);
  Serial.begin(115200);
  Wire.begin(sda, scl);
  MPU6050_Init();
  WiFi.begin(ssid, password);
  delay(500);
  ss.begin(9600);
  button.attachLongPressStop(longclick);
  digitalWrite(g,0);
  digitalWrite(b,0);
}

void loop()
{
  button.tick(); 
  WiFiClient client;
  client.connect(host, httpPort);
  GPS();
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  Ax = (double)AccelX;
  Ay = (double)AccelY;
  Az = (double)AccelZ;
  aax = String(Ax);
  aay = String(Ay);
  aaz = String(Az);
  String send_str = String("GET ") + "/api/fridge/insert.php?lat=" + lat_str + "&lon=" + lng_str + "&time=" + time_str + "&date=" + date_str + "&x=" + aax + "&y=" + aay + "&z=" + aaz + "&class=" + "A" + " HTTP/1.1\r\n" +
                    "Host: " + host + "\r\n" +
                    "Connection: close\r\n\r\n" ;
  client.print(send_str);
  Serial.println(send_str);
  delay(500);
}
