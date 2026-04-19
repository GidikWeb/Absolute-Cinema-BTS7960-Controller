#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>

// ========== BTS7960 PINS ==========
int R_IS = A0;
int L_IS = A1;
int R_EN = 2;
int R_PWM = 3;
int L_EN = 4;
int L_PWM = 5;

// ========== BUTTON PINS ==========
#define BTN_UP    8
#define BTN_OK    9
#define BTN_DOWN  10

// ========== LCD I2C ==========
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ========== CUSTOM CHARACTERS (PROGMEM) ==========
const byte char_arrowLeft[8] PROGMEM      = {0b00000,0b00100,0b01000,0b11111,0b01000,0b00100,0b00000,0b00000};
const byte char_timerIcon[8] PROGMEM      = {0b00000,0b01110,0b10101,0b10111,0b10001,0b01110,0b00000,0b00000};
const byte char_pauseIcon[8] PROGMEM      = {0b00000,0b11011,0b11011,0b11011,0b11011,0b11011,0b11011,0b00000};
const byte char_playIcon[8] PROGMEM       = {0b00000,0b01100,0b01110,0b01111,0b01111,0b01110,0b01100,0b00000};
const byte char_speedIcon7[8] PROGMEM     = {0b00000,0b01110,0b10001,0b10101,0b10011,0b11111,0b00000,0b00000};
const byte char_powerOn[8] PROGMEM        = {0b11111,0b11111,0b11011,0b11011,0b11011,0b01110,0b10001,0b11111};
const byte char_powerOff[8] PROGMEM       = {0b00000,0b00000,0b00100,0b00100,0b10101,0b10001,0b01110,0b00000};
const byte char_menuIcon[8] PROGMEM       = {0b00000,0b00000,0b11111,0b00000,0b11111,0b00000,0b11111,0b00000};
const byte char_arrowRight[8] PROGMEM     = {0b00000,0b00100,0b00010,0b11111,0b00010,0b00100,0b00000,0b00000};
const byte char_speedIcon6[8] PROGMEM     = {0b00000,0b01110,0b10001,0b10111,0b10001,0b11111,0b00000,0b00000};
const byte char_speedIcon5[8] PROGMEM     = {0b00000,0b01110,0b10011,0b10101,0b10001,0b11111,0b00000,0b00000};
const byte char_speedIcon4[8] PROGMEM     = {0b00000,0b01110,0b10101,0b10101,0b10001,0b11111,0b00000,0b00000};
const byte char_speedIcon3[8] PROGMEM     = {0b00000,0b01110,0b11001,0b10101,0b10001,0b11111,0b00000,0b00000};
const byte char_speedIcon2[8] PROGMEM     = {0b00000,0b01110,0b10001,0b11101,0b10001,0b11111,0b00000,0b00000};
const byte char_speedIcon1[8] PROGMEM     = {0b00000,0b01110,0b10001,0b10101,0b11001,0b11111,0b00000,0b00000};
const byte char_newIcon[8] PROGMEM        = {0b00000,0b00000,0b00100,0b00100,0b11111,0b00100,0b00100,0b00000};
const byte char_deleteIcon[8] PROGMEM     = {0b00000,0b00000,0b00000,0b00000,0b11111,0b00000,0b00000,0b00000};
const byte char_powerMenu[8] PROGMEM      = {0b00010,0b00110,0b01100,0b11111,0b00011,0b00110,0b00100,0b00000};
const byte char_directionMenu[8] PROGMEM  = {0b00100,0b01111,0b00101,0b00001,0b10001,0b10001,0b11111,0b00000};
const byte char_speedMenu[8] PROGMEM      = {0b00000,0b01110,0b10001,0b10101,0b10011,0b11111,0b00000,0b00000};
const byte char_timersMenu[8] PROGMEM     = {0b00000,0b01110,0b10101,0b10111,0b10001,0b01110,0b00000,0b00000};
const byte char_safetyMenu[8] PROGMEM     = {0b01110,0b10001,0b10001,0b11111,0b11111,0b11011,0b11111,0b00000};
const byte char_massageMenu[8] PROGMEM    = {0b00000,0b00000,0b01010,0b11011,0b00000,0b11011,0b01010,0b00000};
const byte char_editorMenu[8] PROGMEM     = {0b00001,0b00000,0b01010,0b11011,0b00000,0b11011,0b01010,0b00000};
const byte char_autotestMenu[8] PROGMEM   = {0b00000,0b00000,0b00100,0b01110,0b11011,0b01110,0b00100,0b00000};
// Песочные часы (степень заполнения)
const byte timeLeft1[8] PROGMEM           = {0b00000,0b11111,0b10001,0b01010,0b00100,0b01010,0b10001,0b11111};
const byte timeLeft2[8] PROGMEM           = {0b00000,0b11111,0b10001,0b01010,0b00100,0b01010,0b11111,0b11111};
const byte timeLeft3[8] PROGMEM           = {0b00000,0b11111,0b10001,0b01010,0b00100,0b01110,0b11111,0b11111};
const byte timeLeft4[8] PROGMEM           = {0b00000,0b11111,0b10001,0b01110,0b00100,0b01110,0b11111,0b11111};
const byte timeLeft5[8] PROGMEM           = {0b00000,0b11111,0b11111,0b01110,0b00100,0b01110,0b11111,0b11111};

void loadCharFromProgmem(uint8_t slot, const byte* data) {
  byte buffer[8];
  for (int i = 0; i < 8; i++) buffer[i] = pgm_read_byte(&data[i]);
  lcd.createChar(slot, buffer);
}

// ========== EEPROM ADDRESSES ==========
const int EEPROM_SIGNATURE = 0xAA;
const int ADDR_SIGNATURE = 0;
const int ADDR_SPEED     = 1;
const int ADDR_DIRECTION = 2;
const int ADDR_TIMER_MIN = 3;
const int ADDR_SAFETY_TIMEOUT = 4;

#define USER_PROG_START  100
#define USER_PROG_MAX    5
#define USER_PROG_STEPS_MAX 10
#define USER_NAME_LEN    12

// ========== SAFETY SETTINGS ==========
int MAX_CONTINUOUS_MINUTES = 30;
const int CURRENT_THRESHOLD = 600;
unsigned long motorStartTime = 0;
bool safetyTimerActive = false;

// ========== SMOOTH RAMPING ==========
const unsigned long RAMP_TIME_MS = 400;
int currentSpeed = 0;
int targetSpeed = 0;
bool targetEnabled = false;
unsigned long rampStartTime = 0;
bool ramping = false;

// ========== GLOBAL VARIABLES ==========
enum MenuState {
  MAIN_MENU,
  SPEED_SETTING,
  DIRECTION_SETTING,
  TIMER_SETTING,
  SAFETY_SETTING,
  TIMER_RUNNING,
  PROGRAM_MENU,
  PROGRAM_RUNNING,
  AUTO_TEST,
  EDITOR_INPUT,
  EDITOR_NAME,
  EDITOR_SELECT,
  EDITOR_EDIT_STEPS
};

MenuState currentState = MAIN_MENU;
int menuIndex = 0;
bool motorEnabled = false;
int motorSpeed = 150;
bool motorDirection = true;
int timerMinutes = 5;
int safetyMinutes = 30;

bool backgroundTimerActive = false;
unsigned long backgroundTimerEndTime = 0;
bool timerPaused = false;
unsigned long pauseAccumulated = 0;

int currentProgram = -1;
int currentStep = 0;
unsigned long stepStartTime = 0;
unsigned long stepPauseAccum = 0;
bool programPaused = false;

// ========== FACTORY PROGRAMS (PROGMEM) ==========
struct ProgramStep {
  uint8_t speed;
  bool direction;
  uint16_t durationSec;
};

#define readStepSpeed(ptr)   pgm_read_byte(&(ptr)->speed)
#define readStepDir(ptr)     pgm_read_byte(&(ptr)->direction)
#define readStepDur(ptr)     pgm_read_word(&(ptr)->durationSec)

const ProgramStep programHard[] PROGMEM = {
  {200, true, 600}, {180, false, 300}, {220, true, 450}, {150, false, 600}, {0, false, 0}
};
const ProgramStep programRelax[] PROGMEM = {
  {120, true, 600}, {100, false, 600}, {80, true, 300}, {0, false, 0}
};
const ProgramStep programBody[] PROGMEM = {
  {180, true, 600}, {200, false, 600}, {160, true, 600}, {0, false, 0}
};
const ProgramStep programArms[] PROGMEM = {
  {150, true, 300}, {130, false, 300}, {170, true, 300}, {140, false, 300}, {0, false, 0}
};
const ProgramStep programLegs[] PROGMEM = {
  {220, true, 600}, {200, false, 600}, {180, true, 300}, {0, false, 0}
};
const ProgramStep programKneading[] PROGMEM = {
  {180, true, 600}, {160, false, 600}, {200, true, 600}, {140, false, 300}, {170, true, 300}, {0, false, 0}
};
const ProgramStep programVibration[] PROGMEM = {
  {120, true, 2}, {120, false, 2}, {120, true, 2}, {120, false, 2},
  {130, true, 2}, {130, false, 2}, {130, true, 2}, {130, false, 2},
  {140, true, 2}, {140, false, 2}, {140, true, 2}, {140, false, 2},
  {150, true, 2}, {150, false, 2}, {150, true, 2}, {150, false, 2},
  {160, true, 2}, {160, false, 2}, {160, true, 2}, {160, false, 2},
  {170, true, 2}, {170, false, 2}, {170, true, 2}, {170, false, 2},
  {180, true, 2}, {180, false, 2}, {180, true, 2}, {180, false, 2},
  {0, false, 0}
};
const ProgramStep programTapping[] PROGMEM = {
  {220, true, 2}, {0, true, 1}, {220, true, 2}, {0, true, 1},
  {200, true, 2}, {0, true, 1}, {200, true, 2}, {0, true, 1},
  {180, true, 2}, {0, true, 1}, {180, true, 2}, {0, true, 1},
  {160, true, 2}, {0, true, 1}, {160, true, 2}, {0, true, 1},
  {140, true, 2}, {0, true, 1}, {140, true, 2}, {0, true, 1},
  {120, true, 2}, {0, true, 1}, {120, true, 2}, {0, true, 1},
  {0, false, 0}
};
const ProgramStep programLymphatic[] PROGMEM = {
  {100, true, 3600}, {0, false, 0}
};

PGM_P const factoryPrograms[] PROGMEM = {
  (PGM_P)programHard, (PGM_P)programRelax, (PGM_P)programBody,
  (PGM_P)programArms, (PGM_P)programLegs, (PGM_P)programKneading,
  (PGM_P)programVibration, (PGM_P)programTapping, (PGM_P)programLymphatic
};

const char factoryProgramNames[9][8] PROGMEM = {
  "Hard", "Relax", "Body", "Arms", "Legs",
  "Kneading", "Vibration", "Tapping", "Lymphatic"
};
const int factoryProgramCount = 9;
int totalProgramCount = factoryProgramCount;

// ========== USER PROGRAMS ==========
struct UserProgram {
  char name[USER_NAME_LEN+1];
  uint8_t stepCount;
  struct Step {
    uint8_t speed;
    bool direction;
    uint16_t durationSec;
  } steps[USER_PROG_STEPS_MAX];
};

UserProgram userPrograms[USER_PROG_MAX];
int userProgramCount = 0;

UserProgram editingProg;
int editingStepIndex = 0;
int editingParam = 0;
bool isEditing = false;
int editingProgIndex = -1;

unsigned long lastBlink = 0;
bool blinkState = false;
unsigned long lastButtonPress = 0;
const int DEBOUNCE_DELAY = 150;

// ========== PROTOTYPES ==========
void loadSettings();
void saveSettings();
void updateMotorRamp();
void setMotorTarget(bool enable, int speed, bool direction);
void applyPWM(int speed);
void updateMotor();
void checkCurrentProtection();
void safetyMotorOff();
void startTimer();
void stopBackgroundTimer();
void handleBackgroundTimer();
void startProgram(int progIndex);
void stopProgram();
void applyStepFromProgmem(PGM_P stepPtr);
void applyUserStep(const UserProgram::Step& step);
void handleProgramStep();
void runAutoTest();
void updateDisplay();
void showMainMenu();
void showSpeedSetting();
void showDirectionSetting();
void showTimerSetting();
void showSafetySetting();
void showTimerRunning();
void showProgramMenu();
void showProgramRunning();
void showAutoTest();
void showEditorInput();
void showEditorName();
void showEditorSelect();
void showEditorEditSteps();
void showSplashScreen();
void showCenteredMessage(const char* msg, int ms);
bool buttonPressed();
void checkButtons();
void handleButtonUp();
void handleButtonDown();
void handleButtonOk();
void loadIconForMenuItem(int idx);
void setSpeedIcon(int speed);
void setHourglassIcon(unsigned long remaining, unsigned long total);
void loadUserPrograms();
void saveUserProgram(int idx, const UserProgram& prog);
bool deleteUserProgram(int idx);
int findFreeUserSlot();
void startNewProgram();
void startEditProgram(int progIdx);
void commitEditingProgram();
void deleteUserProgramFromList(int progIdx);
void processSerialCommands();

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  Serial.println(F("=== BTS7960 Pro Massage Controller ==="));

  pinMode(R_IS, INPUT);
  pinMode(L_IS, INPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);
  pinMode(R_PWM, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);
  analogWrite(R_PWM, 0);
  analogWrite(L_PWM, 0);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Загружаем часто используемые символы из PROGMEM
  loadCharFromProgmem(0, char_arrowRight);
  loadCharFromProgmem(1, char_arrowLeft);
  loadCharFromProgmem(2, char_playIcon);
  loadCharFromProgmem(3, char_pauseIcon);
  loadCharFromProgmem(4, timeLeft1); // начальные песочные часы
  loadCharFromProgmem(6, char_powerOn);
  loadCharFromProgmem(7, char_menuIcon);
  // Слот 5 остаётся для динамической иконки скорости

  loadSettings();
  loadUserPrograms();
  totalProgramCount = factoryProgramCount + userProgramCount;

  showSplashScreen();
  updateDisplay();

  currentSpeed = 0;
  targetSpeed = 0;
  targetEnabled = false;
  updateMotorRamp();
}

// ========== MAIN LOOP ==========
void loop() {
  processSerialCommands();
  checkButtons();
  updateMotorRamp();
  checkCurrentProtection();

  if (motorEnabled && !safetyTimerActive) {
    safetyTimerActive = true;
    motorStartTime = millis();
  }
  if (!motorEnabled && safetyTimerActive) safetyTimerActive = false;
  if (safetyTimerActive && (millis() - motorStartTime > (MAX_CONTINUOUS_MINUTES * 60000UL))) {
    safetyMotorOff();
    showCenteredMessage("Safety Timeout!", 2000);
  }

  if (backgroundTimerActive) handleBackgroundTimer();
  if (currentState == PROGRAM_RUNNING && !programPaused) handleProgramStep();

  if (currentState == SPEED_SETTING || currentState == DIRECTION_SETTING ||
      currentState == TIMER_SETTING || currentState == SAFETY_SETTING ||
      currentState == TIMER_RUNNING || currentState == PROGRAM_MENU || 
      currentState == PROGRAM_RUNNING || currentState == EDITOR_INPUT ||
      currentState == EDITOR_NAME || currentState == EDITOR_SELECT ||
      currentState == EDITOR_EDIT_STEPS) {
    if (millis() - lastBlink > 500) {
      blinkState = !blinkState;
      lastBlink = millis();
      updateDisplay();
    }
  }
  delay(50);
}

// ========== SMOOTH RAMPING ==========
void setMotorTarget(bool enable, int speed, bool direction) {
  targetEnabled = enable;
  targetSpeed = speed;
  motorDirection = direction;
  if (!enable) targetSpeed = 0;
  rampStartTime = millis();
  ramping = true;
}

void updateMotorRamp() {
  if (!ramping) {
    if (currentSpeed != targetSpeed) {
      rampStartTime = millis();
      ramping = true;
    } else {
      applyPWM(currentSpeed);
      return;
    }
  }

  unsigned long elapsed = millis() - rampStartTime;
  float t = (elapsed >= RAMP_TIME_MS) ? 1.0 : (float)elapsed / RAMP_TIME_MS;
  int newSpeed = currentSpeed + (int)((targetSpeed - currentSpeed) * t);
  if (newSpeed != currentSpeed) {
    currentSpeed = newSpeed;
    applyPWM(currentSpeed);
  }

  if (elapsed >= RAMP_TIME_MS || currentSpeed == targetSpeed) {
    currentSpeed = targetSpeed;
    applyPWM(currentSpeed);
    ramping = false;
  }
}

void applyPWM(int speed) {
  if (!targetEnabled && speed == 0) {
    analogWrite(R_PWM, 0);
    analogWrite(L_PWM, 0);
    return;
  }
  if (motorDirection) {
    analogWrite(R_PWM, speed);
    analogWrite(L_PWM, 0);
  } else {
    analogWrite(R_PWM, 0);
    analogWrite(L_PWM, speed);
  }
}

void updateMotor() {
  if (motorEnabled) {
    setMotorTarget(true, motorSpeed, motorDirection);
  } else {
    setMotorTarget(false, motorSpeed, motorDirection);
  }
}

// ========== CURRENT PROTECTION ==========
void checkCurrentProtection() {
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck < 100) return;
  lastCheck = millis();

  if (!motorEnabled) return;
  int currentR = analogRead(R_IS);
  int currentL = analogRead(L_IS);
  if (currentR > CURRENT_THRESHOLD || currentL > CURRENT_THRESHOLD) {
    safetyMotorOff();
    showCenteredMessage("Overcurrent!", 2000);
  }
}

void safetyMotorOff() {
  motorEnabled = false;
  targetEnabled = false;
  targetSpeed = 0;
  ramping = true;
  rampStartTime = millis();
  backgroundTimerActive = false;
  stopProgram();
  currentState = MAIN_MENU;
  updateDisplay();
}

// ========== TIMER FUNCTIONS ==========
void startTimer() {
  backgroundTimerActive = true;
  timerPaused = false;
  backgroundTimerEndTime = millis() + (timerMinutes * 60000UL);
  motorEnabled = true;
  updateMotor();
  currentState = TIMER_RUNNING;
  updateDisplay();
  showCenteredMessage("Timer Started", 600);
}

void stopBackgroundTimer() {
  backgroundTimerActive = false;
  timerPaused = false;
  motorEnabled = false;
  updateMotor();
}

void handleBackgroundTimer() {
  if (!backgroundTimerActive || timerPaused) return;
  if (millis() >= backgroundTimerEndTime) {
    backgroundTimerActive = false;
    motorEnabled = false;
    updateMotor();
    if (currentState == TIMER_RUNNING) currentState = MAIN_MENU;
    showCenteredMessage("Time's Up!", 2000);
    updateDisplay();
  }
}

// ========== PROGRAM EXECUTION ==========
void applyStepFromProgmem(PGM_P stepPtr) {
  uint8_t sp = pgm_read_byte(stepPtr);
  if (sp == 0) {
    motorEnabled = false;
  } else {
    motorSpeed = sp;
    motorDirection = pgm_read_byte(stepPtr + 1);
    motorEnabled = true;
  }
  updateMotor();
}

void applyUserStep(const UserProgram::Step& step) {
  if (step.speed == 0) {
    motorEnabled = false;
  } else {
    motorSpeed = step.speed;
    motorDirection = step.direction;
    motorEnabled = true;
  }
  updateMotor();
}

void startProgram(int progIndex) {
  currentProgram = progIndex;
  currentStep = 0;
  programPaused = false;
  if (progIndex < factoryProgramCount) {
    PGM_P steps = (PGM_P)pgm_read_word(&factoryPrograms[progIndex]);
    applyStepFromProgmem(steps);
  } else {
    int userIdx = progIndex - factoryProgramCount;
    if (userIdx < userProgramCount) {
      applyUserStep(userPrograms[userIdx].steps[0]);
    } else {
      currentState = MAIN_MENU;
      return;
    }
  }
  stepStartTime = millis();
  currentState = PROGRAM_RUNNING;
  updateDisplay();
}

void stopProgram() {
  programPaused = false;
  motorEnabled = false;
  updateMotor();
  currentProgram = -1;
}

void handleProgramStep() {
  if (currentProgram < factoryProgramCount) {
    PGM_P steps = (PGM_P)pgm_read_word(&factoryPrograms[currentProgram]);
    PGM_P stepPtr = steps + currentStep * sizeof(ProgramStep);
    uint8_t sp = pgm_read_byte(stepPtr);
    if (sp == 0) {
      motorEnabled = false;
      updateMotor();
      currentState = MAIN_MENU;
      showCenteredMessage("Program Done!", 1500);
      updateDisplay();
      return;
    }
    unsigned long elapsed = millis() - stepStartTime;
    uint16_t dur = pgm_read_word(stepPtr + 2);
    if (elapsed >= dur * 1000UL) {
      currentStep++;
      stepPtr += sizeof(ProgramStep);
      sp = pgm_read_byte(stepPtr);
      if (sp == 0) {
        motorEnabled = false;
        updateMotor();
        currentState = MAIN_MENU;
        showCenteredMessage("Program Done!", 1500);
        updateDisplay();
        return;
      }
      applyStepFromProgmem(stepPtr);
      stepStartTime = millis();
      updateDisplay();
    }
  } else {
    int userIdx = currentProgram - factoryProgramCount;
    if (userIdx < userProgramCount) {
      UserProgram& prog = userPrograms[userIdx];
      if (currentStep >= prog.stepCount || prog.steps[currentStep].speed == 0) {
        motorEnabled = false;
        updateMotor();
        currentState = MAIN_MENU;
        showCenteredMessage("Program Done!", 1500);
        updateDisplay();
        return;
      }
      unsigned long elapsed = millis() - stepStartTime;
      unsigned long stepDuration = prog.steps[currentStep].durationSec * 1000UL;
      if (elapsed >= stepDuration) {
        currentStep++;
        if (currentStep >= prog.stepCount || prog.steps[currentStep].speed == 0) {
          motorEnabled = false;
          updateMotor();
          currentState = MAIN_MENU;
          showCenteredMessage("Program Done!", 1500);
          updateDisplay();
          return;
        }
        applyUserStep(prog.steps[currentStep]);
        stepStartTime = millis();
        updateDisplay();
      }
    }
  }
}

// ========== AUTO TEST ==========
void runAutoTest() {
  currentState = AUTO_TEST; updateDisplay();
  showCenteredMessage("Auto Test", 800);
  for(int i=0; i<=200; i+=40){
    lcd.clear(); lcd.setCursor(0,0); lcd.print(F("Test:Forward")); lcd.setCursor(0,1); lcd.print(F("Speed:")); lcd.print(i);
    motorEnabled = true; motorSpeed = i; motorDirection = true; updateMotor();
    delay(500); if(buttonPressed()) break;
  }
  delay(1000);
  for(int i=0; i<=200; i+=40){
    lcd.clear(); lcd.setCursor(0,0); lcd.print(F("Test:Reverse")); lcd.setCursor(0,1); lcd.print(F("Speed:")); lcd.print(i);
    motorEnabled = true; motorSpeed = i; motorDirection = false; updateMotor();
    delay(500); if(buttonPressed()) break;
  }
  motorEnabled = false; updateMotor();
  showCenteredMessage("Test Complete",1000);
  currentState = MAIN_MENU; updateDisplay();
}

// ========== DISPLAY ==========
int getMenuCount() {
  return 9 + 1;
}

const char* getMenuItem(int index) {
  static const char items[10][12] PROGMEM = {
    "Power", "Speed", "Direction", "Timer Set", "Timer Run",
    "Safety", "Programs", "Create Prog", "Edit/Del", "Auto Test"
  };
  static char buffer[12];
  if (index == 9) {
    return backgroundTimerActive ? "Stop Timer" : "Auto Test";
  }
  strcpy_P(buffer, items[index]);
  return buffer;
}

void loadIconForMenuItem(int idx) {
  if (idx == 9) {
    loadCharFromProgmem(7, char_autotestMenu);
    return;
  }
  switch(idx) {
    case 0: loadCharFromProgmem(7, char_powerMenu); break;
    case 1: loadCharFromProgmem(7, char_speedMenu); break;
    case 2: loadCharFromProgmem(7, char_directionMenu); break;
    case 3: loadCharFromProgmem(7, char_timersMenu); break;
    case 4: loadCharFromProgmem(7, char_timersMenu); break;
    case 5: loadCharFromProgmem(7, char_safetyMenu); break;
    case 6: loadCharFromProgmem(7, char_massageMenu); break;
    case 7: loadCharFromProgmem(7, char_editorMenu); break;
    case 8: loadCharFromProgmem(7, char_deleteIcon); break;
    default: loadCharFromProgmem(7, char_autotestMenu); break;
  }
}

void setSpeedIcon(int speed) {
  int level = constrain(speed, 0, 255);
  level = map(level, 0, 255, 1, 7);
  switch(level) {
    case 1: loadCharFromProgmem(5, char_speedIcon1); break;
    case 2: loadCharFromProgmem(5, char_speedIcon2); break;
    case 3: loadCharFromProgmem(5, char_speedIcon3); break;
    case 4: loadCharFromProgmem(5, char_speedIcon4); break;
    case 5: loadCharFromProgmem(5, char_speedIcon5); break;
    case 6: loadCharFromProgmem(5, char_speedIcon6); break;
    case 7: loadCharFromProgmem(5, char_speedIcon7); break;
    default: loadCharFromProgmem(5, char_speedIcon4); break;
  }
}

void setHourglassIcon(unsigned long remaining, unsigned long total) {
  if (total == 0) return;
  int percent = (remaining * 100) / total;
  if (percent > 80) {
    loadCharFromProgmem(4, timeLeft1);
  } else if (percent > 60) {
    loadCharFromProgmem(4, timeLeft2);
  } else if (percent > 40) {
    loadCharFromProgmem(4, timeLeft3);
  } else if (percent > 20) {
    loadCharFromProgmem(4, timeLeft4);
  } else {
    loadCharFromProgmem(4, timeLeft5);
  }
}

void updateDisplay() {
  lcd.clear();
  switch(currentState) {
    case MAIN_MENU:        showMainMenu(); break;
    case SPEED_SETTING:    showSpeedSetting(); break;
    case DIRECTION_SETTING:showDirectionSetting(); break;
    case TIMER_SETTING:    showTimerSetting(); break;
    case SAFETY_SETTING:   showSafetySetting(); break;
    case TIMER_RUNNING:    showTimerRunning(); break;
    case PROGRAM_MENU:     showProgramMenu(); break;
    case PROGRAM_RUNNING:  showProgramRunning(); break;
    case AUTO_TEST:        showAutoTest(); break;
    case EDITOR_INPUT:     showEditorInput(); break;
    case EDITOR_NAME:      showEditorName(); break;
    case EDITOR_SELECT:    showEditorSelect(); break;
    case EDITOR_EDIT_STEPS:showEditorEditSteps(); break;
  }
}

void showMainMenu() {
  lcd.setCursor(0,0); 
  if (motorEnabled) {
    lcd.write(6);
    lcd.print(F("ON "));
  } else {
    lcd.write(6);
    lcd.print(F("OFF"));
  }
  setSpeedIcon(motorSpeed);
  lcd.setCursor(5,0); lcd.write(5);
  if(motorSpeed<100) lcd.print(' ');
  if(motorSpeed<10) lcd.print(' ');
  lcd.print(motorSpeed);
  lcd.setCursor(10,0); lcd.write(motorDirection?0:1);
  if(backgroundTimerActive){
    unsigned long total = timerMinutes * 60000UL;
    unsigned long rem = timerPaused ? (backgroundTimerEndTime - pauseAccumulated) : 
                        (backgroundTimerEndTime > millis() ? backgroundTimerEndTime - millis() : 0);
    setHourglassIcon(rem, total);
    lcd.setCursor(12,0); lcd.write(4); // песочные часы
    int m = rem / 60000, s = (rem % 60000) / 1000;
    lcd.setCursor(13,0);
    if(m<10) lcd.print('0');
    lcd.print(m); lcd.print(':');
    if(s<10) lcd.print('0');
    lcd.print(s);
  } else {
    lcd.setCursor(12,0); lcd.print(F("    "));
  }
  loadIconForMenuItem(menuIndex);
  lcd.setCursor(0,1); lcd.write(7);
  String txt = getMenuItem(menuIndex);
  lcd.setCursor(2,1); lcd.print(txt.substring(0,13));
  if(txt.length()>13){ lcd.setCursor(15,1); lcd.write(0); }
}

void showSpeedSetting() {
  lcd.setCursor(0,0); lcd.write(5); lcd.print(F(" SET SPEED ")); lcd.write(blinkState?0:1);
  lcd.setCursor(0,1); lcd.print(' '); if(motorSpeed<100)lcd.print(' '); if(motorSpeed<10)lcd.print(' ');
  lcd.print(motorSpeed); lcd.print(F("/255"));
  lcd.setCursor(9,1); int bars = map(motorSpeed,0,255,0,7);
  for(int i=0;i<7;i++) lcd.write(i<bars?255:(blinkState && i==bars-1?255:'-'));
}

void showDirectionSetting() {
  lcd.setCursor(0,0); lcd.print(F("SET DIRECTION")); lcd.setCursor(14,0); lcd.write(blinkState?0:' ');
  lcd.setCursor(0,1);
  if(motorDirection){ lcd.print(F("FORWARD  ")); lcd.write(0); for(int i=0;i<4;i++) lcd.write(blinkState?'>':'-'); }
  else { lcd.print(F("REVERSE  ")); for(int i=0;i<4;i++) lcd.write(blinkState?'<':'-'); lcd.write(1); }
}

void showTimerSetting() {
  lcd.setCursor(0,0); lcd.write(4); lcd.print(F(" SET TIMER ")); lcd.write(blinkState?0:1);
  lcd.setCursor(0,1); lcd.print(F("Minutes: "));
  if(blinkState){ if(timerMinutes<10)lcd.print('0'); lcd.print(timerMinutes); } else lcd.print(F("  "));
  lcd.setCursor(13,1); lcd.print(F("MIN"));
}

void showSafetySetting() {
  lcd.setCursor(0,0); lcd.print(F("SAFETY TIMEOUT"));
  lcd.setCursor(0,1); lcd.print(F("Minutes: "));
  if(blinkState){ if(safetyMinutes<10)lcd.print('0'); lcd.print(safetyMinutes); } else lcd.print(F("  "));
  lcd.setCursor(13,1); lcd.print(F("MIN"));
}

void showTimerRunning() {
  unsigned long total = timerMinutes * 60000UL;
  unsigned long elapsed = timerPaused ? (pauseAccumulated - (backgroundTimerEndTime - total)) : 
                          (millis() - (backgroundTimerEndTime - total));
  unsigned long remaining = total - elapsed;

  setHourglassIcon(remaining, total);
  
  lcd.setCursor(0,0); lcd.write(4); // песочные часы
  lcd.print(F(" TIMER "));
  lcd.write(timerPaused ? 3 : 2); // pause/play
  
  lcd.setCursor(0,1);
  lcd.print(F("Left: "));
  int m = remaining / 60000;
  int s = (remaining % 60000) / 1000;
  if(m < 10) lcd.print('0');
  lcd.print(m); lcd.print(':');
  if(s < 10) lcd.print('0');
  lcd.print(s);
  
  lcd.setCursor(12,1);
  lcd.print(F("\x01P \x00S OK"));
}

void showProgramMenu() {
  lcd.setCursor(0,0); lcd.print(F("Select Program:"));
  lcd.setCursor(0,1);
  if (menuIndex < factoryProgramCount) {
    char buf[9];
    strcpy_P(buf, factoryProgramNames[menuIndex]);
    lcd.print('>'); lcd.print(buf); lcd.print('<');
  } else {
    int userIdx = menuIndex - factoryProgramCount;
    lcd.print('>'); lcd.print(userPrograms[userIdx].name); lcd.print('<');
  }
  lcd.setCursor(0,1); lcd.write(blinkState?1:' ');
  lcd.setCursor(15,1); lcd.write(blinkState?0:' ');
}

void showProgramRunning() {
  if (currentProgram < factoryProgramCount) {
    PGM_P steps = (PGM_P)pgm_read_word(&factoryPrograms[currentProgram]);
    char buf[9];
    strcpy_P(buf, factoryProgramNames[currentProgram]);
    lcd.setCursor(0,0);
    lcd.print(buf);
    lcd.print(' ');
    lcd.print(currentStep+1); lcd.print('/');
    int totalSteps=0; 
    PGM_P stepPtr = steps;
    while(pgm_read_byte(stepPtr)!=0) { totalSteps++; stepPtr += sizeof(ProgramStep); }
    lcd.print(totalSteps);
    lcd.setCursor(10,0); lcd.write(programPaused?3:2);
    lcd.setCursor(0,1);
    stepPtr = steps + currentStep * sizeof(ProgramStep);
    lcd.print(pgm_read_byte(stepPtr+1)?F(">>"):F("<<"));
    lcd.print(pgm_read_byte(stepPtr)); lcd.print(' ');
    unsigned long elapsed = programPaused ? (stepPauseAccum - stepStartTime) : (millis() - stepStartTime);
    unsigned long stepDur = pgm_read_word(stepPtr+2) * 1000UL;
    long remaining = (stepDur - elapsed) / 1000;
    if(remaining < 0) remaining = 0;
    int min = remaining/60, sec = remaining%60;
    if(min<10) lcd.print('0'); lcd.print(min); lcd.print(':');
    if(sec<10) lcd.print('0'); lcd.print(sec);
  } else {
    int userIdx = currentProgram - factoryProgramCount;
    UserProgram& prog = userPrograms[userIdx];
    lcd.setCursor(0,0);
    lcd.print(prog.name);
    lcd.print(' ');
    lcd.print(currentStep+1); lcd.print('/');
    lcd.print(prog.stepCount);
    lcd.setCursor(10,0); lcd.write(programPaused?3:2);
    lcd.setCursor(0,1);
    lcd.print(prog.steps[currentStep].direction?F(">>"):F("<<"));
    lcd.print(prog.steps[currentStep].speed); lcd.print(' ');
    unsigned long elapsed = programPaused ? (stepPauseAccum - stepStartTime) : (millis() - stepStartTime);
    unsigned long stepDur = prog.steps[currentStep].durationSec * 1000UL;
    long remaining = (stepDur - elapsed) / 1000;
    if(remaining < 0) remaining = 0;
    int min = remaining/60, sec = remaining%60;
    if(min<10) lcd.print('0'); lcd.print(min); lcd.print(':');
    if(sec<10) lcd.print('0'); lcd.print(sec);
  }
}

void showAutoTest() {
  lcd.setCursor(0,0); lcd.print(F("  AUTO TEST")); lcd.setCursor(12,0); lcd.write(blinkState?2:3);
  lcd.setCursor(0,1); lcd.print(F("Press any key"));
}

// ========== USER PROGRAM EDITOR ==========
void startNewProgram() {
  memset(&editingProg, 0, sizeof(editingProg));
  editingStepIndex = 0;
  editingParam = 0;
  editingProgIndex = -1;
  isEditing = true;
  currentState = EDITOR_INPUT;
  updateDisplay();
}

void startEditProgram(int progIdx) {
  int userIdx = progIdx - factoryProgramCount;
  if (userIdx >= 0 && userIdx < userProgramCount) {
    editingProg = userPrograms[userIdx];
    editingStepIndex = 0;
    editingParam = 0;
    editingProgIndex = userIdx;
    currentState = EDITOR_EDIT_STEPS;
    updateDisplay();
  } else {
    showCenteredMessage("Invalid prog", 1000);
    currentState = MAIN_MENU;
    updateDisplay();
  }
}

void commitEditingProgram() {
  if (editingProgIndex == -1) {
    int slot = findFreeUserSlot();
    if (slot != -1) {
      saveUserProgram(slot, editingProg);
      loadUserPrograms();
      totalProgramCount = factoryProgramCount + userProgramCount;
    } else {
      showCenteredMessage("No free slots", 1500);
    }
  } else {
    saveUserProgram(editingProgIndex, editingProg);
    loadUserPrograms();
  }
  isEditing = false;
  currentState = MAIN_MENU;
  updateDisplay();
}

void deleteUserProgramFromList(int progIdx) {
  int userIdx = progIdx - factoryProgramCount;
  if (userIdx >= 0 && deleteUserProgram(userIdx)) {
    loadUserPrograms();
    totalProgramCount = factoryProgramCount + userProgramCount;
    showCenteredMessage("Deleted", 800);
  } else {
    showCenteredMessage("Delete fail", 800);
  }
  currentState = MAIN_MENU;
  updateDisplay();
}

void showEditorInput() {
  lcd.setCursor(0,0);
  if (editingParam == 0) {
    lcd.print(F("Step ")); lcd.print(editingStepIndex+1);
    lcd.print(F(" Speed:"));
    lcd.setCursor(0,1);
    lcd.print(F("Value: "));
    if (blinkState) {
      lcd.print(editingProg.steps[editingStepIndex].speed);
    } else {
      lcd.print(F("   "));
    }
    lcd.print(F("/255"));
  } else if (editingParam == 1) {
    lcd.print(F("Step ")); lcd.print(editingStepIndex+1);
    lcd.print(F(" Direction"));
    lcd.setCursor(0,1);
    if (editingProg.steps[editingStepIndex].direction) {
      lcd.print(F("FORWARD  "));
      lcd.write(0);
    } else {
      lcd.print(F("REVERSE  "));
      lcd.write(1);
    }
  } else if (editingParam == 2) {
    lcd.print(F("Step ")); lcd.print(editingStepIndex+1);
    lcd.print(F(" Time(sec)"));
    lcd.setCursor(0,1);
    lcd.print(F("Value: "));
    if (blinkState) {
      lcd.print(editingProg.steps[editingStepIndex].durationSec);
    } else {
      lcd.print(F("     "));
    }
  }
}

void showEditorName() {
  lcd.setCursor(0,0);
  lcd.print(F("Enter name:"));
  lcd.setCursor(0,1);
  lcd.print(editingProg.name);
  if (blinkState) {
    int len = strlen(editingProg.name);
    lcd.setCursor(len,1);
    lcd.write('_');
  }
}

void showEditorSelect() {
  lcd.setCursor(0,0);
  lcd.print(F("Select user prog:"));
  lcd.setCursor(0,1);
  if (userProgramCount == 0) {
    lcd.print(F("No user programs"));
    return;
  }
  int idx = menuIndex % userProgramCount;
  lcd.print('>');
  lcd.print(userPrograms[idx].name);
  lcd.print('<');
  lcd.setCursor(0,1); lcd.write(blinkState?1:' ');
  lcd.setCursor(15,1); lcd.write(blinkState?0:' ');
}

void showEditorEditSteps() {
  lcd.setCursor(0,0);
  lcd.print(F("Editing: "));
  lcd.print(editingProg.name);
  lcd.setCursor(0,1);
  lcd.print(F("Step ")); lcd.print(editingStepIndex+1);
  lcd.print('/'); lcd.print(editingProg.stepCount);
  lcd.print(F(" UP/DOWN"));
}

// ========== EEPROM USER PROGRAMS ==========
void loadUserPrograms() {
  userProgramCount = 0;
  int addr = USER_PROG_START;
  for (int i=0; i<USER_PROG_MAX; i++) {
    uint8_t signature = EEPROM.read(addr);
    if (signature != 0x55) { addr += 1 + USER_NAME_LEN + 1 + USER_PROG_STEPS_MAX*4; continue; }
    addr++;
    for (int j=0; j<USER_NAME_LEN; j++) {
      userPrograms[userProgramCount].name[j] = EEPROM.read(addr++);
    }
    userPrograms[userProgramCount].name[USER_NAME_LEN] = '\0';
    userPrograms[userProgramCount].stepCount = EEPROM.read(addr++);
    int stepCount = userPrograms[userProgramCount].stepCount;
    if (stepCount > USER_PROG_STEPS_MAX) stepCount = USER_PROG_STEPS_MAX;
    for (int s=0; s<stepCount; s++) {
      userPrograms[userProgramCount].steps[s].speed = EEPROM.read(addr++);
      userPrograms[userProgramCount].steps[s].direction = EEPROM.read(addr++);
      uint8_t low = EEPROM.read(addr++);
      uint8_t high = EEPROM.read(addr++);
      userPrograms[userProgramCount].steps[s].durationSec = (high << 8) | low;
    }
    userProgramCount++;
  }
}

void saveUserProgram(int idx, const UserProgram& prog) {
  int addr = USER_PROG_START + idx * (1 + USER_NAME_LEN + 1 + USER_PROG_STEPS_MAX*4);
  EEPROM.write(addr++, 0x55);
  for (int j=0; j<USER_NAME_LEN; j++) {
    EEPROM.write(addr++, prog.name[j]);
  }
  EEPROM.write(addr++, prog.stepCount);
  for (int s=0; s<prog.stepCount; s++) {
    EEPROM.write(addr++, prog.steps[s].speed);
    EEPROM.write(addr++, prog.steps[s].direction ? 1 : 0);
    EEPROM.write(addr++, prog.steps[s].durationSec & 0xFF);
    EEPROM.write(addr++, (prog.steps[s].durationSec >> 8) & 0xFF);
  }
}

bool deleteUserProgram(int idx) {
  int addr = USER_PROG_START + idx * (1 + USER_NAME_LEN + 1 + USER_PROG_STEPS_MAX*4);
  EEPROM.write(addr, 0x00);
  return true;
}

int findFreeUserSlot() {
  for (int i=0; i<USER_PROG_MAX; i++) {
    int addr = USER_PROG_START + i * (1 + USER_NAME_LEN + 1 + USER_PROG_STEPS_MAX*4);
    if (EEPROM.read(addr) != 0x55) return i;
  }
  return -1;
}

// ========== SERIAL COMMANDS ==========
void processSerialCommands() {
  if (!Serial.available()) return;
  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toUpperCase();

  // === ОСНОВНЫЕ КОМАНДЫ ===
  if (cmd == F("ON")) {
    motorEnabled = true;
    updateMotor();
    currentState = MAIN_MENU;
    updateDisplay();
    Serial.println(F("Motor ON"));
  }
  else if (cmd == F("OFF")) {
    motorEnabled = false;
    updateMotor();
    currentState = MAIN_MENU;
    updateDisplay();
    Serial.println(F("Motor OFF"));
  }
  else if (cmd.startsWith(F("SPEED "))) {
    int sp = cmd.substring(6).toInt();
    sp = constrain(sp, 0, 255);
    motorSpeed = sp;
    if (motorEnabled) updateMotor();
    saveSettings();
    currentState = MAIN_MENU;
    updateDisplay();
    Serial.print(F("Speed set to ")); Serial.println(motorSpeed);
  }
  else if (cmd.startsWith(F("DIR "))) {
    String dir = cmd.substring(4);
    dir.trim();
    if (dir == F("F") || dir == F("FORWARD")) {
      motorDirection = true;
      Serial.println(F("Direction: FORWARD"));
    } else if (dir == F("R") || dir == F("REVERSE")) {
      motorDirection = false;
      Serial.println(F("Direction: REVERSE"));
    } else {
      Serial.println(F("Invalid direction. Use F or R"));
      return;
    }
    if (motorEnabled) updateMotor();
    saveSettings();
    currentState = MAIN_MENU;
    updateDisplay();
  }
  else if (cmd.startsWith(F("TIMER "))) {
    int t = cmd.substring(6).toInt();
    timerMinutes = constrain(t, 0, 99);
    saveSettings();
    currentState = MAIN_MENU;
    updateDisplay();
    Serial.print(F("Timer set to ")); Serial.print(timerMinutes); Serial.println(F(" min"));
  }
  else if (cmd == F("START_TIMER")) {
    if (timerMinutes > 0) {
      startTimer();
      Serial.println(F("Timer started"));
    } else {
      Serial.println(F("Set timer minutes first (>0)"));
    }
  }
  else if (cmd == F("STOP_TIMER")) {
    stopBackgroundTimer();
    currentState = MAIN_MENU;
    updateDisplay();
    Serial.println(F("Timer stopped"));
  }
  else if (cmd.startsWith(F("SAFETY "))) {
    int s = cmd.substring(7).toInt();
    safetyMinutes = constrain(s, 1, 60);
    MAX_CONTINUOUS_MINUTES = safetyMinutes;
    saveSettings();
    currentState = MAIN_MENU;
    updateDisplay();
    Serial.print(F("Safety timeout set to ")); Serial.print(safetyMinutes); Serial.println(F(" min"));
  }
  else if (cmd == F("STATUS")) {
    Serial.println(F("=== STATUS ==="));
    Serial.print(F("Motor: ")); Serial.println(motorEnabled ? F("ON") : F("OFF"));
    Serial.print(F("Speed: ")); Serial.print(motorSpeed); Serial.print(F(" / 255  Direction: "));
    Serial.println(motorDirection ? F("FORWARD") : F("REVERSE"));
    if (backgroundTimerActive) {
      unsigned long rem = timerPaused ? (backgroundTimerEndTime - pauseAccumulated) : 
                         (backgroundTimerEndTime > millis() ? backgroundTimerEndTime - millis() : 0);
      int m = rem / 60000;
      int s = (rem % 60000) / 1000;
      Serial.print(F("Timer active, remaining: "));
      if (m < 10) Serial.print('0');
      Serial.print(m); Serial.print(':');
      if (s < 10) Serial.print('0');
      Serial.println(s);
    } else {
      Serial.println(F("Timer: inactive"));
    }
    if (currentState == PROGRAM_RUNNING) {
      Serial.print(F("Program running: "));
      if (currentProgram < factoryProgramCount) {
        char buf[9];
        strcpy_P(buf, factoryProgramNames[currentProgram]);
        Serial.print(buf);
      } else {
        int userIdx = currentProgram - factoryProgramCount;
        Serial.print(userPrograms[userIdx].name);
      }
      Serial.print(F(" step ")); Serial.print(currentStep+1);
      Serial.print('/');
      if (currentProgram < factoryProgramCount) {
        PGM_P steps = (PGM_P)pgm_read_word(&factoryPrograms[currentProgram]);
        int cnt = 0;
        PGM_P ptr = steps;
        while (pgm_read_byte(ptr) != 0) { cnt++; ptr += sizeof(ProgramStep); }
        Serial.println(cnt);
      } else {
        int userIdx = currentProgram - factoryProgramCount;
        Serial.println(userPrograms[userIdx].stepCount);
      }
    } else {
      Serial.println(F("No program running"));
    }
    Serial.print(F("Safety: ")); Serial.print(safetyMinutes); Serial.println(F(" min"));
    Serial.println(F("=============="));
  }

  // === ПРОГРАММЫ ===
  else if (cmd == F("LIST")) {
    Serial.println(F("Factory programs:"));
    for (int i = 0; i < factoryProgramCount; i++) {
      Serial.print(i); Serial.print(F(": "));
      char buf[9];
      strcpy_P(buf, factoryProgramNames[i]);
      Serial.println(buf);
    }
    Serial.println(F("User programs:"));
    for (int i = 0; i < userProgramCount; i++) {
      Serial.print(i + factoryProgramCount); Serial.print(F(": "));
      Serial.println(userPrograms[i].name);
    }
  }
  else if (cmd.startsWith(F("RUN "))) {
    int idx = cmd.substring(4).toInt();
    if (idx >= 0 && idx < totalProgramCount) {
      if (backgroundTimerActive) stopBackgroundTimer();
      startProgram(idx);
      Serial.print(F("Started program ")); Serial.println(idx);
    } else {
      Serial.println(F("Invalid program index"));
    }
  }
  else if (cmd == F("STOP")) {
    if (currentState == PROGRAM_RUNNING) {
      stopProgram();
      currentState = MAIN_MENU;
      updateDisplay();
      Serial.println(F("Program stopped"));
    } else {
      Serial.println(F("No program running"));
    }
  }
  else if (cmd == F("PAUSE")) {
    if (currentState == PROGRAM_RUNNING && !programPaused) {
      programPaused = true;
      stepPauseAccum = millis();
      motorEnabled = false;
      updateMotor();
      updateDisplay();
      Serial.println(F("Program paused"));
    } else {
      Serial.println(F("Cannot pause"));
    }
  }
  else if (cmd == F("RESUME")) {
    if (currentState == PROGRAM_RUNNING && programPaused) {
      programPaused = false;
      stepStartTime += (millis() - stepPauseAccum);
      motorEnabled = true;
      updateMotor();
      updateDisplay();
      Serial.println(F("Program resumed"));
    } else {
      Serial.println(F("Cannot resume"));
    }
  }
  else if (cmd.startsWith(F("PROG_INFO "))) {
    int idx = cmd.substring(10).toInt();
    if (idx >= 0 && idx < totalProgramCount) {
      if (idx < factoryProgramCount) {
        char buf[9];
        strcpy_P(buf, factoryProgramNames[idx]);
        Serial.print(F("Program: ")); Serial.println(buf);
        PGM_P steps = (PGM_P)pgm_read_word(&factoryPrograms[idx]);
        int stepNum = 1;
        PGM_P ptr = steps;
        while (pgm_read_byte(ptr) != 0) {
          uint8_t sp = pgm_read_byte(ptr);
          bool dir = pgm_read_byte(ptr + 1);
          uint16_t dur = pgm_read_word(ptr + 2);
          Serial.print(F("Step ")); Serial.print(stepNum); Serial.print(F(": Speed="));
          Serial.print(sp); Serial.print(F(" Dir=")); Serial.print(dir ? F("FWD") : F("REV"));
          Serial.print(F(" Time=")); Serial.print(dur); Serial.println(F("s"));
          ptr += sizeof(ProgramStep);
          stepNum++;
        }
      } else {
        int uIdx = idx - factoryProgramCount;
        UserProgram& prog = userPrograms[uIdx];
        Serial.print(F("Program: ")); Serial.println(prog.name);
        for (int s = 0; s < prog.stepCount; s++) {
          Serial.print(F("Step ")); Serial.print(s+1); Serial.print(F(": Speed="));
          Serial.print(prog.steps[s].speed); Serial.print(F(" Dir="));
          Serial.print(prog.steps[s].direction ? F("FWD") : F("REV"));
          Serial.print(F(" Time=")); Serial.print(prog.steps[s].durationSec); Serial.println(F("s"));
        }
      }
    } else {
      Serial.println(F("Invalid program index"));
    }
  }

  // === РЕДАКТОР ===
  else if (cmd == F("EDITOR_NEW")) {
    startNewProgram();
    Serial.println(F("New program started. Use EDITOR_ADD_STEP, EDITOR_SET_NAME, EDITOR_SAVE."));
  }
  else if (cmd.startsWith(F("EDITOR_ADD_STEP "))) {
    if (!isEditing) {
      Serial.println(F("Not in editor mode. Use EDITOR_NEW or EDITOR_EDIT first."));
      return;
    }
    int sp, dir, sec;
    if (sscanf(cmd.c_str(), "EDITOR_ADD_STEP %d %d %d", &sp, &dir, &sec) == 3) {
      if (editingStepIndex >= USER_PROG_STEPS_MAX) {
        Serial.println(F("Maximum steps reached!"));
        return;
      }
      editingProg.steps[editingStepIndex].speed = constrain(sp, 0, 255);
      editingProg.steps[editingStepIndex].direction = (dir == 1);
      editingProg.steps[editingStepIndex].durationSec = constrain(sec, 1, 65535);
      editingStepIndex++;
      editingProg.stepCount = editingStepIndex;
      Serial.print(F("Step added. Total steps: ")); Serial.println(editingStepIndex);
    } else {
      Serial.println(F("Usage: EDITOR_ADD_STEP speed direction(1/0) seconds"));
    }
  }
  else if (cmd.startsWith(F("EDITOR_DEL_STEP "))) {
    if (!isEditing) {
      Serial.println(F("Not in editor mode."));
      return;
    }
    int stepNum = cmd.substring(16).toInt();
    if (stepNum < 1 || stepNum > editingProg.stepCount) {
      Serial.println(F("Invalid step number"));
      return;
    }
    int idx = stepNum - 1;
    for (int i = idx; i < editingProg.stepCount - 1; i++) {
      editingProg.steps[i] = editingProg.steps[i+1];
    }
    editingProg.stepCount--;
    if (editingStepIndex > editingProg.stepCount) editingStepIndex = editingProg.stepCount;
    Serial.print(F("Step deleted. Total steps: ")); Serial.println(editingProg.stepCount);
  }
  else if (cmd.startsWith(F("EDITOR_SET_NAME "))) {
    if (!isEditing) {
      Serial.println(F("Not in editor mode."));
      return;
    }
    String name = cmd.substring(17);
    name.trim();
    if (name.length() > USER_NAME_LEN) name = name.substring(0, USER_NAME_LEN);
    strcpy(editingProg.name, name.c_str());
    Serial.print(F("Name set to: ")); Serial.println(editingProg.name);
  }
  else if (cmd == F("EDITOR_SAVE")) {
    if (!isEditing) {
      Serial.println(F("Not in editor mode."));
      return;
    }
    if (editingProg.stepCount == 0) {
      Serial.println(F("Cannot save empty program"));
      return;
    }
    if (strlen(editingProg.name) == 0) {
      strcpy(editingProg.name, "Unnamed");
    }
    commitEditingProgram();
    Serial.println(F("Program saved."));
  }
  else if (cmd == F("EDITOR_CANCEL")) {
    if (isEditing) {
      isEditing = false;
      currentState = MAIN_MENU;
      updateDisplay();
      Serial.println(F("Editing cancelled."));
    } else {
      Serial.println(F("No active editing."));
    }
  }
  else if (cmd.startsWith(F("EDITOR_EDIT "))) {
    int idx = cmd.substring(12).toInt();
    if (idx >= factoryProgramCount && idx < totalProgramCount) {
      startEditProgram(idx);
      Serial.print(F("Editing program: ")); Serial.println(editingProg.name);
      Serial.println(F("Use EDITOR_ADD_STEP, EDITOR_DEL_STEP, EDITOR_SET_NAME, EDITOR_SAVE."));
    } else {
      Serial.println(F("Invalid user program index"));
    }
  }
  else if (cmd.startsWith(F("EDITOR_DEL_PROG "))) {
    int idx = cmd.substring(16).toInt();
    if (idx >= factoryProgramCount && idx < totalProgramCount) {
      deleteUserProgramFromList(idx);
      Serial.println(F("Program deleted."));
    } else {
      Serial.println(F("Invalid user program index"));
    }
  }

  // === СПЕЦИАЛЬНЫЕ ===
  else if (cmd == F("AUTO_TEST")) {
    runAutoTest();
    Serial.println(F("Auto test completed."));
  }
  else if (cmd == F("HELP")) {
    Serial.println(F("=== AVAILABLE COMMANDS ==="));
    Serial.println(F("ON, OFF, SPEED <0-255>, DIR <F/R>, TIMER <min>, START_TIMER, STOP_TIMER"));
    Serial.println(F("SAFETY <min>, STATUS, LIST, RUN <idx>, STOP, PAUSE, RESUME"));
    Serial.println(F("PROG_INFO <idx>"));
    Serial.println(F("EDITOR_NEW, EDITOR_ADD_STEP <sp> <dir> <sec>, EDITOR_DEL_STEP <num>"));
    Serial.println(F("EDITOR_SET_NAME <name>, EDITOR_SAVE, EDITOR_CANCEL, EDITOR_EDIT <idx>, EDITOR_DEL_PROG <idx>"));
    Serial.println(F("AUTO_TEST, HELP"));
    Serial.println(F("=========================="));
  }
  else {
    Serial.print(F("Unknown command: ")); Serial.println(cmd);
    Serial.println(F("Type HELP for list."));
  }
}

// ========== MISC ==========
void showSplashScreen() {
  lcd.clear();
  for(int i=0;i<16;i++){ lcd.setCursor(i,0); lcd.print('='); lcd.setCursor(15-i,1); lcd.print('='); delay(50); }
  lcd.clear(); lcd.setCursor(1,0); lcd.print(F("D-LEEK Massage")); lcd.setCursor(0,1); lcd.print(F("Pro v5.0")); delay(3000);
  lcd.clear();
}

void showCenteredMessage(const char* msg, int ms) {
  lcd.clear(); lcd.setCursor((16-strlen(msg))/2,0); lcd.print(msg); lcd.setCursor(6,1); lcd.print(F("...")); delay(ms);
}

bool buttonPressed() {
  return (digitalRead(BTN_OK)==LOW || digitalRead(BTN_UP)==LOW || digitalRead(BTN_DOWN)==LOW);
}

// ========== EEPROM SETTINGS ==========
void loadSettings() {
  if (EEPROM.read(ADDR_SIGNATURE) == EEPROM_SIGNATURE) {
    motorSpeed = EEPROM.read(ADDR_SPEED);
    motorDirection = EEPROM.read(ADDR_DIRECTION);
    timerMinutes = EEPROM.read(ADDR_TIMER_MIN);
    safetyMinutes = EEPROM.read(ADDR_SAFETY_TIMEOUT);
    if (motorSpeed < 0 || motorSpeed > 255) motorSpeed = 150;
    if (timerMinutes < 0 || timerMinutes > 99) timerMinutes = 5;
    if (safetyMinutes < 1 || safetyMinutes > 60) safetyMinutes = 30;
    MAX_CONTINUOUS_MINUTES = safetyMinutes;
  } else {
    saveSettings();
  }
}

void saveSettings() {
  EEPROM.write(ADDR_SIGNATURE, EEPROM_SIGNATURE);
  EEPROM.write(ADDR_SPEED, motorSpeed);
  EEPROM.write(ADDR_DIRECTION, motorDirection ? 1 : 0);
  EEPROM.write(ADDR_TIMER_MIN, timerMinutes);
  EEPROM.write(ADDR_SAFETY_TIMEOUT, safetyMinutes);
}

// ========== BUTTON HANDLING ==========
void checkButtons() {
  static bool lastUp = HIGH, lastOk = HIGH, lastDown = HIGH;
  bool up = digitalRead(BTN_UP);
  bool ok = digitalRead(BTN_OK);
  bool down = digitalRead(BTN_DOWN);

  if (lastUp == HIGH && up == LOW) { lastButtonPress = millis(); handleButtonUp(); }
  if (lastOk == HIGH && ok == LOW) { lastButtonPress = millis(); handleButtonOk(); }
  if (lastDown == HIGH && down == LOW) { lastButtonPress = millis(); handleButtonDown(); }

  lastUp = up; lastOk = ok; lastDown = down;
}

void handleButtonUp() {
  switch(currentState) {
    case MAIN_MENU:
      menuIndex = (menuIndex - 1 + getMenuCount()) % getMenuCount();
      updateDisplay(); break;
    case SPEED_SETTING:
      motorSpeed = constrain(motorSpeed + 5, 0, 255);
      updateMotor();
      saveSettings();
      updateDisplay(); break;
    case DIRECTION_SETTING:
      motorDirection = !motorDirection;
      updateMotor();
      saveSettings();
      updateDisplay(); break;
    case TIMER_SETTING:
      timerMinutes = constrain(timerMinutes + 1, 0, 99);
      saveSettings();
      updateDisplay(); break;
    case SAFETY_SETTING:
      safetyMinutes = constrain(safetyMinutes + 1, 1, 60);
      MAX_CONTINUOUS_MINUTES = safetyMinutes;
      saveSettings();
      updateDisplay(); break;
    case TIMER_RUNNING:
      if (backgroundTimerActive) {
        timerPaused = !timerPaused;
        if (timerPaused) { pauseAccumulated = millis(); motorEnabled = false; }
        else { backgroundTimerEndTime += (millis() - pauseAccumulated); motorEnabled = true; }
        updateMotor();
        updateDisplay();
      }
      break;
    case PROGRAM_MENU:
      menuIndex = (menuIndex - 1 + totalProgramCount) % totalProgramCount;
      updateDisplay(); break;
    case PROGRAM_RUNNING:
      programPaused = !programPaused;
      if (programPaused) { stepPauseAccum = millis(); motorEnabled = false; }
      else { stepStartTime += (millis() - stepPauseAccum); motorEnabled = true; }
      updateMotor();
      updateDisplay(); break;
    case AUTO_TEST:
      currentState = MAIN_MENU; updateDisplay(); break;
    case EDITOR_INPUT:
      if (editingParam == 0) {
        editingProg.steps[editingStepIndex].speed = constrain(editingProg.steps[editingStepIndex].speed + 5, 0, 255);
      } else if (editingParam == 1) {
        editingProg.steps[editingStepIndex].direction = !editingProg.steps[editingStepIndex].direction;
      } else if (editingParam == 2) {
        editingProg.steps[editingStepIndex].durationSec = constrain(editingProg.steps[editingStepIndex].durationSec + 5, 1, 65535);
      }
      updateDisplay(); break;
    case EDITOR_NAME: {
      int len = strlen(editingProg.name);
      if (len < USER_NAME_LEN) {
        char c = editingProg.name[len];
        if (c == 0) c = 'A';
        else c++;
        if (c > 'Z') c = 'A';
        editingProg.name[len] = c;
        editingProg.name[len+1] = '\0';
      }
      updateDisplay(); break;
    }
    case EDITOR_SELECT:
      if (userProgramCount > 0) {
        menuIndex = (menuIndex - 1 + userProgramCount) % userProgramCount;
      }
      updateDisplay(); break;
    case EDITOR_EDIT_STEPS:
      break;
  }
}

void handleButtonDown() {
  switch(currentState) {
    case MAIN_MENU:
      menuIndex = (menuIndex + 1) % getMenuCount();
      updateDisplay(); break;
    case SPEED_SETTING:
      motorSpeed = constrain(motorSpeed - 5, 0, 255);
      updateMotor();
      saveSettings();
      updateDisplay(); break;
    case DIRECTION_SETTING:
      motorDirection = !motorDirection;
      updateMotor();
      saveSettings();
      updateDisplay(); break;
    case TIMER_SETTING:
      timerMinutes = constrain(timerMinutes - 1, 0, 99);
      saveSettings();
      updateDisplay(); break;
    case SAFETY_SETTING:
      safetyMinutes = constrain(safetyMinutes - 1, 1, 60);
      MAX_CONTINUOUS_MINUTES = safetyMinutes;
      saveSettings();
      updateDisplay(); break;
    case TIMER_RUNNING:
      stopBackgroundTimer();
      showCenteredMessage("Timer Stopped", 800);
      currentState = MAIN_MENU; updateDisplay(); break;
    case PROGRAM_MENU:
      currentState = MAIN_MENU; updateDisplay(); break;
    case PROGRAM_RUNNING:
      stopProgram();
      showCenteredMessage("Stopped", 800);
      currentState = MAIN_MENU; updateDisplay(); break;
    case AUTO_TEST:
      currentState = MAIN_MENU; updateDisplay(); break;
    case EDITOR_INPUT:
      if (editingParam == 0) {
        editingProg.steps[editingStepIndex].speed = constrain(editingProg.steps[editingStepIndex].speed - 5, 0, 255);
      } else if (editingParam == 1) {
        editingProg.steps[editingStepIndex].direction = !editingProg.steps[editingStepIndex].direction;
      } else if (editingParam == 2) {
        editingProg.steps[editingStepIndex].durationSec = constrain(editingProg.steps[editingStepIndex].durationSec - 5, 1, 65535);
      }
      updateDisplay(); break;
    case EDITOR_NAME: {
      int len = strlen(editingProg.name);
      if (len > 0) {
        editingProg.name[len-1] = '\0';
      }
      updateDisplay(); break;
    }
    case EDITOR_SELECT:
      if (userProgramCount > 0) {
        int idx = menuIndex % userProgramCount;
        deleteUserProgramFromList(factoryProgramCount + idx);
      }
      break;
    case EDITOR_EDIT_STEPS:
      if (editingProg.stepCount > 0 && editingStepIndex < editingProg.stepCount) {
        for (int i=editingStepIndex; i<editingProg.stepCount-1; i++) {
          editingProg.steps[i] = editingProg.steps[i+1];
        }
        editingProg.stepCount--;
        if (editingStepIndex >= editingProg.stepCount && editingStepIndex>0) editingStepIndex--;
        updateDisplay();
      }
      break;
  }
}

void handleButtonOk() {
  switch(currentState) {
    case MAIN_MENU: {
      int total = getMenuCount();
      if (menuIndex == total - 1) {
        if (backgroundTimerActive) {
          stopBackgroundTimer();
          showCenteredMessage("Timer Stopped", 800);
        } else {
          runAutoTest();
        }
        updateDisplay();
      } else {
        switch(menuIndex) {
          case 0: motorEnabled = !motorEnabled; updateMotor(); updateDisplay(); break;
          case 1: currentState = SPEED_SETTING; updateDisplay(); break;
          case 2: currentState = DIRECTION_SETTING; updateDisplay(); break;
          case 3: currentState = TIMER_SETTING; updateDisplay(); break;
          case 4: if (timerMinutes > 0) startTimer(); else showCenteredMessage("Set Time First", 1000); break;
          case 5: currentState = SAFETY_SETTING; updateDisplay(); break;
          case 6: currentState = PROGRAM_MENU; menuIndex = 0; updateDisplay(); break;
          case 7: startNewProgram(); break;
          case 8: currentState = EDITOR_SELECT; menuIndex = 0; updateDisplay(); break;
        }
      }
      break;
    }
    case SPEED_SETTING:
    case DIRECTION_SETTING:
    case TIMER_SETTING:
    case SAFETY_SETTING:
      currentState = MAIN_MENU; updateDisplay(); break;
    case TIMER_RUNNING:
      currentState = MAIN_MENU; updateDisplay(); showCenteredMessage("Timer in BG", 800); break;
    case PROGRAM_MENU:
      startProgram(menuIndex);
      break;
    case PROGRAM_RUNNING:
      programPaused = !programPaused;
      if (programPaused) { stepPauseAccum = millis(); motorEnabled = false; }
      else { stepStartTime += (millis() - stepPauseAccum); motorEnabled = true; }
      updateMotor();
      updateDisplay(); break;
    case AUTO_TEST:
      currentState = MAIN_MENU; updateDisplay(); break;
    case EDITOR_INPUT:
      if (editingParam < 2) {
        editingParam++;
        updateDisplay();
      } else {
        editingParam = 0;
        editingStepIndex++;
        editingProg.stepCount = editingStepIndex;
        if (editingStepIndex >= USER_PROG_STEPS_MAX) {
          currentState = EDITOR_NAME;
          memset(editingProg.name, 0, sizeof(editingProg.name));
          strcpy(editingProg.name, "UserProg");
        }
        updateDisplay();
      }
      break;
    case EDITOR_NAME: {
      int len = strlen(editingProg.name);
      if (len < USER_NAME_LEN) {
        editingProg.name[len] = 'A';
        editingProg.name[len+1] = '\0';
      } else {
        commitEditingProgram();
        currentState = MAIN_MENU;
      }
      updateDisplay();
      break;
    }
    case EDITOR_SELECT:
      if (userProgramCount > 0) {
        int idx = menuIndex % userProgramCount;
        startEditProgram(factoryProgramCount + idx);
      }
      break;
    case EDITOR_EDIT_STEPS:
      if (editingProg.stepCount > 0) {
        editingParam = 0;
        currentState = EDITOR_INPUT;
        updateDisplay();
      }
      break;
  }
}
