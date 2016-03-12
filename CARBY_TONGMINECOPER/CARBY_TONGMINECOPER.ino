#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>

#define ARM_Address 111
void Write_RGB_Pin(uint8_t color, uint8_t value)
{
  uint8_t command = 0xc1;
  twi_writeTo(ARM_Address, &value, 1, 1);
  twi_writeTo(ARM_Address, &color, 1, 1);
  twi_writeTo(ARM_Address, &command, 1, 1);

}

void Write_PWM_Pin(uint8_t pin, uint8_t value)
{
  uint8_t command = 0xc2;
  twi_writeTo(ARM_Address, &value, 1, 1);
  twi_writeTo(ARM_Address, &pin, 1, 1);
  twi_writeTo(ARM_Address, &command, 1, 1);
}

void Write_GPIO_Pin(uint8_t pin, uint8_t value)
{
  uint8_t command = 0xc4;
  twi_writeTo(ARM_Address, &value, 1, 1);
  twi_writeTo(ARM_Address, &pin, 1, 1);
  twi_writeTo(ARM_Address, &command, 1, 1);
}

void Set_PWM_Pin(uint8_t pin)
{
  uint8_t command = 0xc6;
  twi_writeTo(ARM_Address, &pin, 1, 1);
  twi_writeTo(ARM_Address, &command, 1, 1);
}


/**
    _mode ,0 input, 1 output, 2 output_open drain
    _pull ,0 on pull, 1 pull up, 2 pull down


*/
void Set_GPIO_Pin(uint8_t pin, uint8_t _mode, uint8_t _pull)
{
  uint8_t buf = (_pull << 6) | (_mode << 4) | (pin);
  uint8_t command = 0xc5;
  twi_writeTo(ARM_Address, &buf, 1, 1);
  twi_writeTo(ARM_Address, &command, 1, 1);
}


uint8_t Read_GPIO_Pin(uint8_t pin)
{
  uint16_t temp ;
  uint8_t command = 0xc3;
  twi_writeTo(ARM_Address, &pin, 1, 1);
  twi_writeTo(ARM_Address, &command, 1, 1);

  Wire.requestFrom(ARM_Address, 2);
  temp = Wire.read() << 8 | Wire.read();
  return (uint8_t)temp;

}

uint16_t Read_ADC_Pin(uint8_t pin)
{
  uint16_t temp ;
  uint8_t command = 0xc7;
  twi_writeTo(ARM_Address, &pin, 1, 1);
  twi_writeTo(ARM_Address, &command, 1, 1);

  Wire.requestFrom(ARM_Address, 2);
  temp = Wire.read() << 8 | Wire.read();
  return temp;
}

/**********************************************************/

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
  Wire.begin();
  Wire.setClock(400000);
  Set_GPIO_Pin(5, 1, 0);
  Set_GPIO_Pin(6, 1, 0);
  Set_GPIO_Pin(7, 1, 0);
  Set_GPIO_Pin(8, 1, 0);
  Set_PWM_Pin(1);
  Set_PWM_Pin(2);
  Write_PWM_Pin(1, 0);
  Write_PWM_Pin(2, 0);
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
    LR = LR * 2;
    FB = FB * 1.8;
    if (FB > 1) {
      Write_GPIO_Pin(7, 1);
      Write_GPIO_Pin(8, 0);
      Write_PWM_Pin(2, FB);
      //Write_PWM_Pin(2, 0);
    } else if (FB < -1) {
      Write_PWM_Pin(2, -FB);
      Write_GPIO_Pin(7, 0);
      Write_GPIO_Pin(8, 1);
      //Write_PWM_Pin(2, 0);
    } else {
      Write_PWM_Pin(1, 0);
      Write_GPIO_Pin(7, 0);
      Write_GPIO_Pin(8, 0);
    }
    if (LR > 80) {
      Write_GPIO_Pin(5, 0);
      Write_GPIO_Pin(6, 1);
      Write_PWM_Pin(1, 180);
    } else if (LR < -80) {
      Write_GPIO_Pin(5, 1);
      Write_GPIO_Pin(6, 0);
      Write_PWM_Pin(1, 180);
    } else {
      Write_GPIO_Pin(5, 0);
      Write_GPIO_Pin(6, 0);
      Write_PWM_Pin(1, 0);
    }



  }
}


