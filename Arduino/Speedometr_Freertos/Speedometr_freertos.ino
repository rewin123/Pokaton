#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#include <TFT.h>                 // Подключаем библиотеку TFT  
#include <SPI.h>                 // Подключаем библиотеку SPI
 
#define cs   10                  // Указываем пины cs
#define dc   9                   // Указываем пины dc (A0)
#define rst  8                   // Указываем пины reset

TFT tft = TFT(cs, dc, rst);


#define W 160
#define H 128

#define LED_PIN 12

#define CHAR_BUF_SIZE 32
char ch_buf[CHAR_BUF_SIZE];

#define CYCLES_MODE 0
#define TAHO_MODE 1
#define GRAPH_MODE 2
#define SPEED_MODE 3
#define SPEED_GRAPH_MODE 4
#define END_MODE 5

#define LED_PIN 12

#define ON_TIME 61

int mode = CYCLES_MODE;

bool updateLastEvent = false;
int lastEventTime = 0;
int currentSecTime = 0;
unsigned long lastButtonPress = 0;

unsigned long cycles = 0;

#define CYCLE_TIME_WIDTH 61
int last_sec = 0;
int time_cycles[CYCLE_TIME_WIDTH]; //cycles per sec

void taskDisplay( void *pvParameters );
void taskButton( void *pvParameters );
void taskHall( void *pvParameters );
void taskSecond( void *pvParameters );

#include <avr/sleep.h>

SemaphoreHandle_t buttonSemaphore;
SemaphoreHandle_t hallSemaphore;
void setup() {
  // put your setup code here, to run once:

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  tft.begin();
  tft.background(255, 0, 0); // Очистим экран дисплея
  
  xTaskCreate(taskDisplay,
    "Display",
    128,
    NULL,
    0,
    NULL);

  xTaskCreate(taskButton,
    "Button event",
    128,
    NULL,
    1,
    NULL);

  xTaskCreate(taskHall,
    "Hell event",
    128,
    NULL,
    3,
    NULL);

  xTaskCreate(taskSecond,
    "Second update",
    128,
    NULL,
    2,
    NULL);

  buttonSemaphore = xSemaphoreCreateBinary();
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), interruptButton, RISING);

  
  hallSemaphore = xSemaphoreCreateBinary();
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), interruptHall, RISING);
}

void interruptButton() {
  xSemaphoreGiveFromISR(buttonSemaphore, NULL);
}

void interruptHall() {
  xSemaphoreGiveFromISR(hallSemaphore, NULL);
}

void loop() {}

int index = 0;

void taskButton( void *pvParameters ) {
  (void) pvParameters;

  for(;;) {
    if(xSemaphoreTake(buttonSemaphore, portMAX_DELAY) == pdPASS) {
      if(millis() > lastButtonPress + 500) {
        if(currentSecTime < lastEventTime + ON_TIME) {
          mode++;
          if(mode >= END_MODE)
            mode = 0;
        }
        digitalWrite(LED_PIN, HIGH);
        lastButtonPress = millis();
        updateLastEvent = true;
      }
    }
  }
  
}


void taskHall( void *pvParameters ) {
  (void) pvParameters;
  for(;;) {
    if(xSemaphoreTake(hallSemaphore, portMAX_DELAY) == pdPASS) {
      cycles++;
      time_cycles[CYCLE_TIME_WIDTH - 1]++;
      updateLastEvent = true;
    }
  }
}

void taskDisplay( void *pvParameters ) {
  (void) pvParameters;

  for(;;) {
    switch(mode) {
      case CYCLES_MODE:
        drawCycles();
        break;
      case TAHO_MODE:
        drawCyclesPerMin();
        break;
      case GRAPH_MODE:
        drawMinGraph();
        break;
      case SPEED_MODE:
        drawSpeed();
        break;
      case SPEED_GRAPH_MODE:
        drawSpeedGraph();
        break;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void taskSecond( void *pvParameters ) {
  (void) pvParameters;
  for(;;) {
    currentSecTime++;
    if(updateLastEvent) {
      updateLastEvent = false;
      lastEventTime = currentSecTime;
    }

    for(int i = 1; i < CYCLE_TIME_WIDTH;i++)
      time_cycles[i - 1] = time_cycles[i];
    time_cycles[CYCLE_TIME_WIDTH - 1] = 0;

    if(currentSecTime > lastEventTime + ON_TIME) {
      digitalWrite(LED_PIN, LOW);
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      noInterrupts ();
      sleep_enable();
      interrupts ();
      sleep_cpu();
    } else {
      digitalWrite(LED_PIN, HIGH);
    }
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

}

void addTo(char *dst, char *src, int max_dst) {
  int start_index = 0;
  while(dst[start_index] != 0 && start_index < max_dst)
    start_index++;

  if(start_index >= max_dst)
    return;

  int i = 0;
  while(src[i] != 0) {
    dst[start_index + i] = src[i];
    i++;
  }

  if(i < max_dst) {
    dst[start_index + i] = 0;
  }
    
}

void drawCycles() {
    tft.background(0, 0, 0);
    tft.stroke(255,255,255);
    ltoa(cycles, ch_buf, 10);
    tft.setTextSize(4);
    tft.text(ch_buf, 10, 10);
    tft.setTextSize(3);
    tft.text("cycles", 10, 64);
  
}

void drawCyclesPerSec() {
    tft.background(0, 0, 0);
    tft.stroke(255,255,255);
    ltoa(time_cycles[CYCLE_TIME_WIDTH - 2], ch_buf, 10);
    tft.setTextSize(4);
    tft.text(ch_buf, 10, 10);
    tft.setTextSize(3);
    tft.text("cyc/sec", 10, 64);
}

void drawCyclesPerMin() {
    tft.background(0, 0, 0);
    tft.stroke(255,255,255);
    ltoa(time_cycles[CYCLE_TIME_WIDTH - 2] * 60, ch_buf, 10);
    tft.setTextSize(4);
    tft.text(ch_buf, 10, 10);
    tft.setTextSize(3);
    tft.text("cyc/min", 10, 64);
}

void drawSpeed() {
    tft.background(0, 0, 0);
    tft.stroke(255,255,255);
    ltoa(time_cycles[CYCLE_TIME_WIDTH - 2] * 60.f * 60.f / 1000.f, ch_buf, 10);
    tft.setTextSize(4);
    tft.text(ch_buf, 10, 10);
    tft.setTextSize(3);
    tft.text("km/h", 10, 64);
}

void drawSpeedGraph() {
    int max_val = 0;
    for(int i = 0; i < CYCLE_TIME_WIDTH - 1;i++)
      max_val = max(max_val, time_cycles[i]);

    tft.background(0, 0, 0);
    tft.stroke(255,255,255);
    for(int i = 1; i < CYCLE_TIME_WIDTH - 1;i++) {
      tft.line((i - 1) * W / (CYCLE_TIME_WIDTH - 1),H - time_cycles[i - 1] * H / max_val, i * W / (CYCLE_TIME_WIDTH - 1),H - time_cycles[i] * H / max_val);
    }

    ltoa(max_val * 60.f * 60.f / 1000, ch_buf, 10);
    addTo(ch_buf, " km/h", CHAR_BUF_SIZE);
    tft.setTextSize(2);
    tft.text(ch_buf, 5,5);
}

void drawMinGraph() {
    int max_val = 0;
    for(int i = 0; i < CYCLE_TIME_WIDTH - 1;i++)
      max_val = max(max_val, time_cycles[i]);

    tft.background(0, 0, 0);
    tft.stroke(255,255,255);
    for(int i = 1; i < CYCLE_TIME_WIDTH - 1;i++) {
      tft.line((i - 1) * W / (CYCLE_TIME_WIDTH - 1),H - time_cycles[i - 1] * H / max_val, i * W / (CYCLE_TIME_WIDTH - 1),H - time_cycles[i] * H / max_val);
    }

    ltoa(max_val * 60, ch_buf, 10);
    addTo(ch_buf, " cyc/min", CHAR_BUF_SIZE);
    tft.setTextSize(2);
    tft.text(ch_buf, 5,5);
}
