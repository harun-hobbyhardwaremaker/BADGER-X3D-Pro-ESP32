/*
============================================================
 X3D PRO -> ESP32 WROOM : FINAL led light EDITION v1.0
------------------------------------------------------------
 PROJE: RAMS-FIRE // HARDWAREMAKER (hobby)
 WEB: https://rams-fire.com
 LİSANS: AGPL-3.0 (To strictly respect the underlying library creators)
 
 ■ HARDWAREMAKER FREEDOM MANIFESTO (Developer Note):
 I have absolute respect for the legends behind the base code (Oleg, Dmitry, Tobozo). 
 Their license remains intact. As for my own RAMS-FIRE bridge implementation: 
 I want this code to serve humanity, open-source robotics, and fire suppression R&D. 
 It is 100% OPEN for everyone worldwide. Take it, modify it, compile it, and deploy it 
 into your projects without any hesitation. Let the knowledge burn bright!
------------------------------------------------------------
 12 BUTTONS = 12 SEPARATE GPIO PINS (each drives its own LED)
 4 HAT DIRECTIONS = flash pattern on GPIO 2 (on-board blue LED)
 SERIAL   = BUTTONS + 4 AXES + 4 POV + HAT + HACKADAY LOVE
 DEADZONE = ADJUSTABLE INDEPENDENTLY FOR EACH AXIS
------------------------------------------------------------
 Board : ESP32 Dev Module
 Core  : esp32 by Espressif 2.0.17 (3.x will NOT work!)
 Lib   : ESP32-USB-Soft-Host (tobozo)
 USB   : D+ = GPIO16 | D- = GPIO17
 Serial: 115200
------------------------------------------------------------
 AUTHOR: Harun Er (HobbyHardwareMaker) - https://rams-fire.com
 NOTE: No debounce on purpose. LEDs mirror the raw button state.
 NOTE: "DEADZONE" values are change thresholds for Serial printing.
------------------------------------------------------------
 CREDITS / TEŞEKKÜR ZİNCİRİ (CHAIN CLOSED // 6 LINKS)
  - Oleg Mazurov (felis): documented the Logitech Extreme 3D Pro report
    layout (2012, Arduino + USB Host Shield) and wrote the "le3dp" example.
    The X3DReport structure below follows his layout.
    https://nerpa.tech
    https://github.com
  - Dmitry Samsonov (sdima1357): esp32_usb_soft_host, pure-software USB host
    https://github.com
  - tobozo: ESP32-USB-Soft-Host, Arduino wrapper of Dmitry's work
    https://github.com
  - Developed with AI assistance (Qwen); hardware testing by Harun
 Please keep this credit block if you reuse this code.
------------------------------------------------------------
 Board : ESP32 Dev Module
 Core  : esp32 by Espressif 2.0.17 (3.x OLMAZ!)
 Lib   : ESP32-USB-Soft-Host (tobozo)
 USB   : D+ = GPIO16 | D- = GPIO17
 Serial: 115200
============================================================
*/
#define DP_P0 16
#define DM_P0 17
#define LED_STATUS 2  // dahili mavi LED (sapka pattern icin)

// 12 BUTON ICIN LED PINLERI
const uint8_t BTN_LED[12] = { 4, 13, 14, 18, 19, 21, 22, 23, 25, 26, 27, 32 };
const char*   BTN_NAME[12] = {
  "TETIK", "FREN", "CRUISE", "YON", "FAR", "KORNA",
  "PAN-ORTA", "TILT-ORTA", "RAMP", "TELEMETRI", "DIREKSIYON-TERS", "SIFIRLA"
};
const char*   POV_NAME[9] = {
  "YUKARI (V4)", "SAG-YUKARI", "SAG (V3)", "SAG-ASAGI",
  "ASAGI (V2)", "SOL-ASAGI", "SOL (V1)", "SOL-YUKARI", "NOTR"
};

// --- BAĞIMSIZ ÖLÜ BÖLGE (DEADZONE) AYARLARI ---
// Joystick titremelerine göre bu sayıları değiştir!
#define DEADZONE_X      40    // X ekseni (Sağ/Sol) için ±40
#define DEADZONE_Y      40    // Y ekseni (İleri/Geri) için ±40
#define DEADZONE_TWIST  8     // Twist/Burma (Gaz) için ±8
#define DEADZONE_SLIDER 8     // Arka Slider (Gaz kolu) için ±8

#define FORCE_TEMPLATED_NOPS
#include <ESP32-USB-Soft-Host.h>

static const uint16_t X3D_VID = 0x046D, X3D_PID = 0xC215;

struct __attribute__((packed)) X3DReport {
  union {
    uint32_t axes;
    struct { uint32_t x:10; uint32_t y:10; uint32_t hat:4; uint32_t twist:8; };
  };
  uint8_t buttonsA, slider, buttonsB;
};

static volatile bool     x3d    = false;
static volatile uint32_t lastRx = 0;
static volatile uint16_t btnNow = 0;
static volatile int8_t   hatNow = 8;

// EKSEN VE POV DEĞİŞKENLERİ
static volatile uint8_t  povNow = 0;
static volatile uint16_t xNow = 512;   
static volatile uint16_t yNow = 512;   
static volatile uint8_t  twistNow = 128; 
static volatile uint8_t  sliderNow = 128; 

// SAPKA FLASH STATE MACHINE
static volatile uint8_t povFlashCount = 0;
static uint32_t povFlashLastToggle = 0;
static bool     povFlashLedState = false;
static bool     povLongFlash = false;

usb_pins_config_t USB_Pins_Config = { DP_P0, DM_P0, -1,-1,-1,-1,-1,-1 };

// ---------- USB Cihaz Tespiti ----------
static void myDetectCB(uint8_t u, void *dev){
  sDevDesc *d = (sDevDesc*)dev;
  Serial.printf("[USB] device: VID=%04x PID=%04x\n", d->idVendor, d->idProduct);
  if(d->idVendor == X3D_VID && d->idProduct == X3D_PID){
    x3d = true;
    Serial.println(">>> X3D PRO CONNECTED - HACKADAY DEMO MODE ACTIVE <<<");
  }
}

// ---------- Joystick Verisi ----------
static void myDataCB(uint8_t u, uint8_t bd, uint8_t *data, uint8_t len){
  if(!x3d || len < sizeof(X3DReport)) return;
  X3DReport r; memcpy(&r, data, sizeof(r));

  uint16_t btn = r.buttonsA | ((uint16_t)(r.buttonsB & 0x0F) << 8);
  uint16_t pressed  = btn & ~btnNow;
  uint16_t released = ~btn & btnNow;
  btnNow = btn;

  // 12 BUTON -> AYRI LED'LERI YAK/SÖNDÜR
  for(uint8_t i=1; i<=12; i++){
    if(pressed & (1<<(i-1))){
      digitalWrite(BTN_LED[i-1], HIGH);
      Serial.printf("[BTN%02d] %s PRESSED -> GPIO%02d ON\n", i, BTN_NAME[i-1], BTN_LED[i-1]);
      Serial.println("  ❤️ I love Hackaday & GitHub! 💀");
    }
    if(released & (1<<(i-1))){
      digitalWrite(BTN_LED[i-1], LOW);
      Serial.printf("[BTN%02d] %s RELEASED -> GPIO%02d OFF\n", i, BTN_NAME[i-1], BTN_LED[i-1]);
    }
  }

  // --- EKSENLER - BAĞIMSIZ DEADZONE'LU ---
  
  // X ekseni (Sağa-Sola)
  int16_t xDiff = abs((int16_t)r.x - (int16_t)xNow);
  if(xDiff > DEADZONE_X){
    Serial.printf("[AXIS] X (LEFT/RIGHT): %d\n", r.x);
    xNow = r.x;
  }
  
  // Y ekseni (İleri-Geri)
  int16_t yDiff = abs((int16_t)r.y - (int16_t)yNow);
  if(yDiff > DEADZONE_Y){
    Serial.printf("[AXIS] Y (FORWARD/BACK): %d\n", r.y);
    yNow = r.y;
  }
  
  // Twist/Burma (Gaz)
  int16_t twistDiff = abs((int16_t)r.twist - (int16_t)twistNow);
  if(twistDiff > DEADZONE_TWIST){
    Serial.printf("[AXIS] THROTTLE (TWIST): %d\n", r.twist);
    twistNow = r.twist;
  }

  // Arka Slider (Gaz kolu)
  int16_t sliderDiff = abs((int16_t)r.slider - (int16_t)sliderNow);
  if(sliderDiff > DEADZONE_SLIDER){
    Serial.printf("[AXIS] REAR SLIDER (GAS): %d\n", r.slider);
    sliderNow = r.slider;
  }

  // SAPKA (HAT) -> GPIO 2 FLASH PATTERN
  static int8_t prevHat = 8;
  if(r.hat != prevHat){
    if(r.hat != 8){
      uint8_t gear = 255;
      if(r.hat == 0) gear = 4;       
      else if(r.hat == 2) gear = 3;  
      else if(r.hat == 4) gear = 2;  
      else if(r.hat == 6) gear = 1;  
      
      if(gear != 255){
        Serial.printf("[HAT] %s -> %d flash\n", POV_NAME[r.hat], gear);
        povFlashCount = gear;
        povLongFlash = false;
      }
    } else {
      Serial.println("[HAT] CENTER (released) -> 1 long flash");
      povFlashCount = 1;
      povLongFlash = true;
    }
    prevHat = r.hat;
  }

  // 4 POV BUTONU OKUMA
  uint8_t currentPov = r.buttonsB & 0xF0; 
  if(currentPov != povNow){
    uint8_t p1 = (r.buttonsB >> 4) & 0x01;
    uint8_t p2 = (r.buttonsB >> 5) & 0x01;
    uint8_t p3 = (r.buttonsB >> 6) & 0x01;
    uint8_t p4 = (r.buttonsB >> 7) & 0x01;
    
    Serial.printf("[POV] POV1: %s | POV2: %s | POV3: %s | POV4: %s\n", 
                  p1 ? "PRESSED" : "IDLE", 
                  p2 ? "PRESSED" : "IDLE",
                  p3 ? "PRESSED" : "IDLE",
                  p4 ? "PRESSED" : "IDLE");
                  
    povNow = currentPov;
  }

  hatNow = r.hat;
  lastRx = millis();
}

// ---------- GPIO 2 FLASH YONETICISI ----------
void updateStatusLED(){
  uint32_t now = millis();
  bool linked = x3d && (now - lastRx < 500);

  if(povFlashCount == 0){
    if(linked){
      digitalWrite(LED_STATUS, (now % 1000) < 80);
    } else {
      digitalWrite(LED_STATUS, LOW);
    }
    return;
  }

  uint16_t onMs, offMs;
  if(povLongFlash){
    onMs = 400; offMs = 200;
  } else {
    onMs = 100; offMs = 100;
  }

  if(!povFlashLedState){
    if(now - povFlashLastToggle >= offMs){
      povFlashLedState = true;
      povFlashLastToggle = now;
      digitalWrite(LED_STATUS, HIGH);
    }
  } else {
    if(now - povFlashLastToggle >= onMs){
      povFlashLedState = false;
      povFlashLastToggle = now;
      povFlashCount--;
      digitalWrite(LED_STATUS, LOW);
    }
  }
}

void setup(){
  Serial.begin(115200);
  delay(800);

  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(LED_STATUS, LOW);

  for(uint8_t i=0; i<12; i++){
    pinMode(BTN_LED[i], OUTPUT);
    digitalWrite(BTN_LED[i], LOW);
  }

  Serial.println("\n========================================");
  Serial.println("  X3D PRO / ESP32-WROOM");
  Serial.println("  FINAL led light EDITION v1.0");
  Serial.println("========================================");
  Serial.println("12 BTN -> 12 LED (GPIO 4,13,14,18,19,21,22,23,25,26,27,32)");
  Serial.println("4 HAT  -> GPIO 2 (internal blue LED) flash pattern");
  Serial.println("AXIS   -> X, Y, TWIST, REAR SLIDER (Independent Deadzones)");
  Serial.println("POV    -> POV1, POV2, POV3, POV4 buttons");
  Serial.println("\nPlug joystick (D+=16, D-=17)");
  Serial.println("💀 Made with love for Hackaday & GitHub 💀\n");

  USH.setOnConfigDescCB(Default_USB_ConfigDescCB);
  USH.setOnIfaceDescCb (Default_USB_IfaceDescCb); // Küçük 'b'
  USH.setOnHIDDevDescCb(Default_USB_HIDDevDescCb);
  USH.setOnEPDescCb    (Default_USB_EPDescCb);
  USH.init(USB_Pins_Config, myDetectCB, myDataCB);
}

void loop(){
  updateStatusLED();
  delay(2);
}
