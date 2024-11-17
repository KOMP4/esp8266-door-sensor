#include <Arduino.h>
#include <FastBot.h>
// #include <Wire.h>
#include <MPU9250_asukiaaa.h>
// #include <floatToString.h>

#define FB_USE_LOG Serial   // выводить логи ошибок
#define WIFI_SSID "aboba"//"studentakl"
#define WIFI_PASS "12345678"//"Student-13"
#define BOT_TOKEN "7845055694:AAHIHPK_xmA6yKLtXV28dxvSckSLycyvZ0w"
#define CHAT_ID "939506835"

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 2
#define SCL_PIN 1
#endif

long long timer;

float l_AngX = 0;
float p_AvgGx =0;
float AngX = 0;
float offset_gX = 0;
float degX=0, degY=0, degZ=0;
float gX, gY, gZ;
// float last_aSqrt;

MPU9250_asukiaaa mySensor;

// FastBot bot;
FastBot bot(BOT_TOKEN); // с указанием токена
// }

String floatToString(float a){
  String res = "";
  if (a < 0){
    res += "-";
  }
  int int_p = abs(int(a * 100));
  int int_part = int_p/100;
  int hundredths_part = int_p % 100;
  res += String(int_part) + "." + String(hundredths_part);

  return res;
}


float AvgGX(int iter){
  float gX0=0;
  for(int i = 0; i<iter; i++){
    mySensor.gyroUpdate();
    gX0 += mySensor.gyroX();
    // delay(1);
  }
  float res = gX0/iter;
  
  return res;
}


void setup() {

  connectWiFi();

  bot.setChatID(CHAT_ID); // передай "" (пустую строку) чтобы отключить проверку

  // можно указать несколько ID через запятую
  //bot.setChatID("123456,7891011,12131415");

  // подключаем функцию-обработчик
  bot.attach(newMsg);

  // отправить сообщение в указанный в setChatID
  bot.sendMessage("Bot started!");

#ifdef _ESP32_HAL_I2C_H_ // For ESP32
  Wire.begin(SDA_PIN, SCL_PIN);
  mySensor.setWire(&Wire);
#endif

  mySensor.beginAccel();
  mySensor.beginGyro();
  
  bot.sendMessage("Каллибровка...");
  offset_gX = AvgGX(2000);
  String mesg = "Каллибровка завершена, коррекции по осям";
  bot.sendMessage(mesg);
  bot.sendMessage(floatToString(offset_gX));  

  timer = 0;
}
 
void loop() {

  if (millis() - timer >= 300){
    if(abs(l_AngX - AngX) >= 100){
      Serial.println("Дверь в движени!");
      bot.sendMessage("Дверь в движени!");
      l_AngX = AngX;
    }
    
    // bot.sendMessage(floatToString(AngX));
    timer = millis();
  }
  
  bot.tick();

  

  float n_AvgGX = AvgGX(100);

  if (abs(p_AvgGx - n_AvgGX) < 0.5){
    AngX +=0;
  }else{
    AngX += n_AvgGX + offset_gX;
  }
  Serial.print(millis() - timer);Serial.print(" ");Serial.print(l_AngX);Serial.print(" ");
  

  Serial.println(floatToString(AngX));
  float p_AvgGx = n_AvgGX;
  // float p_AvgGx = n_AvgGX;
}






void connectWiFi() {
  delay(2000);
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() > 15000) ESP.restart();
  }
  Serial.println("Connected");
}

void newMsg(FB_msg& msg) {
  // выводим имя юзера и текст сообщения
  // Serial.print(msg.username);
  // Serial.print(", ");
  // Serial.println(msg.text);
  
  // выводим всю информацию о сообщении
  Serial.println(msg.toString());
}

float AvgGY(int iter){
  float gY0;
  for(int i = 0; i<iter; i++){
    long long time = millis();
    mySensor.gyroUpdate();
    gY0 += mySensor.gyroY();
    delay(1);
  }
  return gY0/iter;
}
float AvgGZ(int iter){
  float gZ0=0;
  for(int i = 0; i<iter; i++){
    long long time = millis();
    mySensor.gyroUpdate();
    gZ0 += mySensor.gyroZ();
    delay(1);
  }
  return gZ0/iter;
}