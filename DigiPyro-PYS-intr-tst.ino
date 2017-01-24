#include <Arduino.h>

#define PYS D5
#define PYS_DELAY1 30 // Min: 25 us
#define PYS_DELAY2 1 // Min: 200 ns
#define PYS_DELAY3 5 // Min 1 us
#define PYS_CHAN_1 0
#define PYS_CHAN_0 1
#define PYS_CHAN_TEMP 2

volatile bool pys_do_read = false;
uint16_t pys_data[3];
volatile uint32_t intr_counter = 0;
uint32_t read_counter = 0;

void pys_enable_conv(bool enable) {
  if (enable) {
    digitalWrite(PYS, LOW);
    pinMode(PYS, OUTPUT);
    delayMicroseconds(PYS_DELAY2);
    pinMode(PYS, INPUT);
  } else {
    digitalWrite(PYS, HIGH);
    pinMode(PYS, OUTPUT);
    delayMicroseconds(PYS_DELAY2);
  }
}

void pys_enable_intr(bool att) {
  if (att) {
    attachInterrupt(digitalPinToInterrupt(PYS), pys_handler, RISING);
    digitalWrite(PYS, LOW);
    pinMode(PYS, OUTPUT);
    delayMicroseconds(PYS_DELAY2);
    pinMode(PYS, INPUT);
  } else {
    digitalWrite(PYS, HIGH);
    pinMode(PYS, OUTPUT);
    delayMicroseconds(PYS_DELAY2);
    detachInterrupt(digitalPinToInterrupt(PYS));
  }
}

void pys_handler() {
  pys_do_read = true;
  //pys_enable_intr(false);
  intr_counter++;
}

void pys_read_data() {
  byte i, k;

  pinMode(PYS, INPUT);
  while (digitalRead(PYS) == 0);
  delayMicroseconds(PYS_DELAY1);

  for (k = 0; k < 3; k++) {
    uint16_t tmp = 0;
    for (i = 0; i < 14; i++) {
      digitalWrite(PYS, LOW);
      pinMode(PYS, OUTPUT);
      delayMicroseconds(PYS_DELAY2);

      digitalWrite(PYS, HIGH);
      delayMicroseconds(PYS_DELAY2);

      pinMode(PYS, INPUT);
      delayMicroseconds(PYS_DELAY3);

      tmp <<= 1;
      if (digitalRead(PYS))
        tmp++;
    }
    pys_data[k] = tmp;
  }

  digitalWrite(PYS, LOW);
  pinMode(PYS, OUTPUT);
  delayMicroseconds(PYS_DELAY2);
  pinMode(PYS, INPUT);
}

void setup() {
  Serial.begin(9600);
  pys_enable_conv(true);
  //digitalWrite(PYS, LOW);
  //pinMode(PYS, OUTPUT);
  //delayMicroseconds(PYS_DELAY2);
  //pinMode(PYS, INPUT);
  pys_enable_intr(true);
}

void loop() {
  static bool do_print = 0;

  //delay(1000);

  if (pys_do_read) {
    pys_enable_intr(false);
    pys_read_data();
    pys_do_read = false;
    read_counter++;
    pys_enable_intr(true);
    //Serial.println("Reading");
    //pys_attach_intr(true);
  }

  if (millis() % 1000 == 0) {
    if (do_print) {
      //Serial.printf("%hx:%hx:%hx\r\n", pys_data[0], pys_data[1], pys_data[2]);
      //Serial.printf("Intr count: %lu, Read count: %lu\n", intr_counter, read_counter);
      //Serial.printf("Temp: %hd, Chan0: %hd, Chan1: %hd\n",
      //  pys_data[PYS_CHAN_TEMP], pys_data[PYS_CHAN_0], pys_data[PYS_CHAN_1]);
      Serial.printf("%hu %hu %hu\n",
        pys_data[PYS_CHAN_TEMP], pys_data[PYS_CHAN_0], pys_data[PYS_CHAN_1]);
      do_print = 0;
    }
  } else {
    do_print = 1;
  }
}

