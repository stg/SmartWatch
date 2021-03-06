#include <stdio.h>
#include <stm32f2xx_rtc.h>

static const char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void setup() {
  digitalWrite(BUZZER, HIGH);
  delay(250);
  digitalWrite(BUZZER, LOW);
  while(digitalRead(BUTTON));


  OLED.begin();
  OLED.fillScreen(0x5555);

  OLED.setTextColor(0xFFFF);
  OLED.setTextSize(1);
  
  //OLED.setPower(100);

  //delay(1000);

  OLED.setCursor(0, 0);
  OLED.print((char*)"PMU init...   ");
  if(Battery.begin()) OLED.println((char*)"OK"); else OLED.println((char*)"FAIL");
  OLED.print((char*)"PMU enable... ");
  if(Battery.enableCharging()) OLED.println((char*)"OK"); else OLED.println((char*)"FAIL");
  OLED.print((char*)"Touch init... ");
  if(touch_init()) OLED.println((char*)"OK"); else OLED.println((char*)"FAIL");
  
  DateTime.begin();
  if(!DateTime.isRunning()) {
    // RTC has lost power, we need to set a new time
    DateTime.setDateTime(13, 05, 03, 6, 20, 41, 00);
  }
  
  
}

void loop() {
  char text[32];

  OLED.fillRect(0, 32, 128, 72, 0x5555);
  OLED.setCursor(0, 32);
  
  OLED.print("USB connected: ");
  OLED.println((char*)(Battery.canCharge() ? "YES" : "NO"));
    
  OLED.print("PMU charging:  ");
  OLED.println((char*)(Battery.isCharging() ? "YES" : "NO"));
  
  OLED.setCursor(0, 56);

  sprintf(text, "Battery: %.2fV", ((float)Battery.readMilliVolts()) / 1000.0);
  OLED.println(text);  
  sprintf(text, "Sensor:  %u%%", (LightSensor.readRaw()*100)/4095);
  OLED.println(text);
  
  DateTime.update();
  sprintf(text, "Time: %02u:%02u:%02u", DateTime.hour(), DateTime.minute(), DateTime
.second());
  OLED.println(text);

  OLED.setCursor(0, 88);
  uint8_t x, y;
  bool t;
  OLED.print("Touch: ");
  if(touch_read(&t, &x, &y)) {
    if(t) {
      sprintf(text, "%u, %u", 127 - x, 127 - y);
    } else {
      sprintf(text, "NO");
    }
  } else {
    sprintf(text, "FAIL");
  }
  OLED.println(text);

  // Delay in low speed mode
  CPU.setSpeed(CPU_LS);
  delay(150);
  for(uint8_t n = 40; n <= 90; n++) {
    OLED.setPower(n);
    delay(2);
  }
  for(uint8_t n = 90; n >= 40; n--) {
    OLED.setPower(n);
    delay(2);
  }
  delay(150);
  CPU.setSpeed(CPU_HS);

  if(digitalRead(BUTTON)) {
    OLED.shutdown();
    digitalWrite(BUZZER, HIGH);
    delay(250);
    digitalWrite(BUZZER, LOW);
    while(digitalRead(BUTTON));
    digitalWrite(POWER, LOW);
    delay(500);
    while(!digitalRead(BUTTON));
    digitalWrite(POWER, HIGH);
    setup();
    return;
  }
  
}


