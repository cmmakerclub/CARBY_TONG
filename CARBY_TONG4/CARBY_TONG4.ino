#include <ESP8266WiFi.h>
#include <WiFiUdp.h>




#define buffer_size 10

#define period_udp 10
const char *ssid = "CARBY_TONG";
const char *password = "12345678";

unsigned int localPort = 12345;
unsigned long time_now, time_prev_udp, time_prev_led;
int cb = 1;



char packetBuffer[buffer_size] = {0}; //buffer to hold incoming and outgoing packets
WiFiUDP udp;
int count = 0;

IPAddress local_ip(192, 168, 5, 1);
IPAddress gateway(192, 168, 5, 1);
IPAddress subnet(255, 255, 255, 255);


void Read_UDP();


void setup()
{
  //Serial.begin(115200);
  pinMode(16, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(3, OUTPUT);
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  delay(500);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(500);
  IPAddress myIP = WiFi.softAPIP();

  if ( udp.begin(localPort) > 0) {

  } else {

  }


  time_now = millis();
  time_prev_udp = time_now;
  time_prev_led = time_now;

}

void loop()
{
  time_now = millis();

  if (time_now - time_prev_udp >= period_udp)
  {
    time_prev_udp = time_now;
    Read_UDP();
  }



  delay(2);
}

void Read_UDP()
{
  cb = udp.parsePacket();
  if (!cb) {

  }
  else {
    memset(packetBuffer, 0, buffer_size); // clear mem
    udp.read(packetBuffer, cb); // read the packet into the buffer
    digitalWrite(16, !digitalRead(16));
    int LR = (int8_t)packetBuffer[1];
    int FB = (int8_t)packetBuffer[2];
    LR = LR*9 ;
    FB = FB*9 ;
    if (FB > 1) {
      digitalWrite(15, 0);
      digitalWrite(13, 1);
      analogWrite(1, FB);
      //Write_PWM_Pin(2, 0);
    } else if (FB < -1) {
      analogWrite(1, -FB);
      digitalWrite(15, 1);
      digitalWrite(13, 0);
      //Write_PWM_Pin(2, 0);
    } else {
      analogWrite(1, 0);
      digitalWrite(15, 0);
      digitalWrite(13, 0);
    }
    if (LR > 700) {
      digitalWrite(14, 0);
      digitalWrite(12, 1);
      analogWrite(3, 1000);
    } else if (LR < -700) {
      digitalWrite(14, 1);
      digitalWrite(12, 0);
      analogWrite(3, 1000);
    } else {
      digitalWrite(14, 0);
      digitalWrite(12, 0);
      analogWrite(3, 0);
    }



  }
}


