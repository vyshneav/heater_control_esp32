#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"

#define DHTPIN 2     
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);

#define RELAY_PIN  18
#define BUTTON_UP  4
#define BUTTON_DOWN  5

 LiquidCrystal_I2C lcd(0x27, 16, 2);


enum State { IDLE, HEATING, STABILIZING, TARGET_REACHED, OVERHEAT };
State systemState = IDLE;

float currentTemp = 0.0;
float targetTemp = 25.0;

SemaphoreHandle_t lcdMutex;

// Task to read temperature
void TaskTemperature(void *pvParameters) {
  for (;;) {
    
    currentTemp = dht.readTemperature();
    Serial.println(currentTemp);
    // Update state machine
    if (currentTemp < targetTemp - 1) {
      systemState = HEATING;
      digitalWrite(RELAY_PIN, HIGH);
    } else if (abs(currentTemp - targetTemp) <= 0.5) {
      systemState = TARGET_REACHED;
      digitalWrite(RELAY_PIN, LOW);
    } else if (currentTemp > targetTemp + 2) {
      systemState = OVERHEAT;
      digitalWrite(RELAY_PIN, LOW);
    } else {
      systemState = STABILIZING;
      digitalWrite(RELAY_PIN, LOW);
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// Task to handle buttons
void TaskButton(void *pvParameters) {
  for (;;) {
    if (digitalRead(BUTTON_UP) == HIGH) {
      Serial.println("up button pressed");
      targetTemp += 0.5;
      vTaskDelay(300); // debounce
    }
    if (digitalRead(BUTTON_DOWN) == HIGH) {
      Serial.println("down button pressed");
      targetTemp -= 0.5;
      vTaskDelay(300); // debounce
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Task to update LCD
void TaskLCD(void *pvParameters) {
  float prevTemp = -1000;
  float prevTarget = -1000;
  State prevState = IDLE;

  // Initial layout (draw static text once)
  xSemaphoreTake(lcdMutex, portMAX_DELAY);
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.setCursor(0, 1);
  lcd.print("Set: ");
  xSemaphoreGive(lcdMutex);

  for (;;) {
    if (xSemaphoreTake(lcdMutex, portMAX_DELAY) == pdTRUE) {
      // Update temperature only if changed
      if (abs(currentTemp - prevTemp) > 0.1) {
        lcd.setCursor(6, 0); // position after "Temp:"
        lcd.print("     ");  // clear old value
        lcd.setCursor(6, 0);
        lcd.print(currentTemp, 1);
        lcd.print((char)223);
        lcd.print("C");
        prevTemp = currentTemp;
      }

      // Update target temperature only if changed
      if (abs(targetTemp - prevTarget) > 0.1) {
        lcd.setCursor(5, 1); // position after "Set: "
        lcd.print("     ");  // clear old value
        lcd.setCursor(5, 1);
        lcd.print(targetTemp, 1);
        prevTarget = targetTemp;
      }

      // Update state only if changed
      if (systemState != prevState) {
        lcd.setCursor(11, 1);  // right-aligned state
        lcd.print("     ");    // clear previous state
        lcd.setCursor(11, 1);
        switch (systemState) {
          case IDLE: lcd.print("IDLE"); break;
          case HEATING: lcd.print("HEAT"); break;
          case STABILIZING: lcd.print("STBL"); break;
          case TARGET_REACHED: lcd.print(" OK "); break;
          case OVERHEAT: lcd.print("HOT!"); break;
        }
        prevState = systemState;
      }

      xSemaphoreGive(lcdMutex);
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.println(F("DHT22 example!"));

  dht.begin();

  lcd.init();
  lcd.backlight();

 

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_DOWN, INPUT);

  lcdMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(TaskTemperature, "TempTask", 1000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskButton, "ButtonTask", 1000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskLCD, "LCDTask", 2000, NULL, 1, NULL, 1);
}

void loop() {
 
}



