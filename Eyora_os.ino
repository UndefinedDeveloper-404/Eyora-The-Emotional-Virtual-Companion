#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_WIFI_PASS";

const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define TOUCH_PIN 4

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

const char* apps[] =
{
  "Pet",
  "Clock",
  "Weather",
  "Game"
};

int selectedApp = 0;

enum ScreenState
{
  MENU,
  PET_SCREEN,
  CLOCK_SCREEN,
  WEATHER_SCREEN,
  GAME_SCREEN
};

ScreenState currentScreen = MENU;

bool lastTouch = false;
unsigned long touchStart = 0;
int tapCount = 0;
unsigned long lastTapTime = 0;

// ======================
// WEATHER GLOBALS
// ======================

String city = "Chennai";
String apiKey = "YOUR_API";

float temperature = 0;
float feelsLike = 0;
int humidity = 0;

String weatherMain = "--";

unsigned long lastWeatherUpdate = 0;
const unsigned long updateInterval = 600000;

int cursorPos = 0;
int starPos = 0;

int level = 1;
int score = 0;

int lives = 3;
int highScore = 0;

bool gameOver = false;
bool gameWin = false;

bool starBlink = true;

unsigned long gameTouchStart = 0;
unsigned long gameLastTapTime = 0;
unsigned long blinkTimer = 0;

int gameTapCount = 0;

// ======================
// FUNCTION PROTOTYPES
// ======================

void drawEyeFrame(int h);
void smoothBlink();
void wakeAnimation();
void sleepAnimation();

void happyMode();
void curiousMode();
void excitedMode();
void sleepyMode();
void winkMode();
void angryMode();
void shockedMode();
void thinkingMode();
void deepSleepMode();

void handlepetTouch();
void handlePetLogic();
void drawPet();

// ======================
// MENU
// ======================

void drawMenu()
{
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(20,0);
  display.println("EYORA");

  for(int i=0;i<4;i++)
  {
    display.setCursor(0,16 + (i * 12));

    if(i == selectedApp)
      display.print("> ");
    else
      display.print("  ");

    display.println(apps[i]);
  }

  display.display();
}

// ======================
// PET PLACEHOLDER
// ======================
// ======================
// EYORA PET VARIABLES
// ======================

int eyeStyle = 0;
bool petSleeping = false;

bool petlastTouch = false;
unsigned long pettouchStartTime = 0;
unsigned long petlastTapTime = 0;
int pettapCount = 0;

unsigned long petblinkTimer = 0;
unsigned long petmoveTimer = 0;
unsigned long petsquishTimer = 0;

float pupilPos = 0;
float pupilTarget = 0;

int eyeHeight = 32;

// ======================
// PET INIT
// ======================

void initPet()
{
  randomSeed(micros());

  petblinkTimer = millis();
  petmoveTimer = millis();
  petsquishTimer = millis();

  eyeStyle = 0;
  petSleeping = false;
}

// ======================
// DRAW EYE FRAME
// ======================

void drawEyeFrame(int h)
{
  display.clearDisplay();

  display.fillRoundRect(
    18,
    32 - h / 2,
    32,
    h,
    8,
    WHITE
  );

  display.fillRoundRect(
    78,
    32 - h / 2,
    32,
    h,
    8,
    WHITE
  );

  if(h > 8)
  {
    display.fillCircle(
      34 + (int)pupilPos,
      32,
      5,
      BLACK
    );

    display.fillCircle(
      94 + (int)pupilPos,
      32,
      5,
      BLACK
    );

    display.fillCircle(
      36 + (int)pupilPos,
      29,
      2,
      WHITE
    );

    display.fillCircle(
      96 + (int)pupilPos,
      29,
      2,
      WHITE
    );
  }

  display.display();
}

// ======================
// BLINK
// ======================

void smoothBlink()
{
  drawEyeFrame(28); delay(20);
  drawEyeFrame(20); delay(20);
  drawEyeFrame(12); delay(20);
  drawEyeFrame(4);  delay(70);
  drawEyeFrame(12); delay(20);
  drawEyeFrame(20); delay(20);
  drawEyeFrame(32);
}

// ======================
// WAKE
// ======================

void wakeAnimation()
{
  display.clearDisplay();

  display.fillCircle(34,32,2,WHITE);
  display.fillCircle(94,32,2,WHITE);

  display.display();
  delay(150);

  drawEyeFrame(8);
  delay(120);

  drawEyeFrame(18);
  delay(120);

  drawEyeFrame(32);

  smoothBlink();
}

// ======================
// SLEEP
// ======================

void sleepAnimation()
{
  drawEyeFrame(32); delay(50);
  drawEyeFrame(20); delay(80);
  drawEyeFrame(10); delay(80);
  drawEyeFrame(4);  delay(150);
}

// ======================
// HAPPY 😊
// ======================

void happyMode()
{
  display.clearDisplay();

  display.fillRoundRect(18,16,32,32,8,WHITE);
  display.fillRoundRect(78,16,32,32,8,WHITE);

  display.fillCircle(34,32,5,BLACK);
  display.fillCircle(94,32,5,BLACK);

  display.fillCircle(36,28,2,WHITE);
  display.fillCircle(96,28,2,WHITE);

  display.display();
}

// ======================
// CURIOUS 👀
// ======================

void curiousMode()
{
  display.clearDisplay();

  display.fillRoundRect(18,18,26,28,8,WHITE);
  display.fillRoundRect(74,12,40,40,8,WHITE);

  display.fillCircle(28,32,4,BLACK);
  display.fillCircle(94,32,6,BLACK);

  display.display();
}

// ======================
// EXCITED 🤩
// ======================

void excitedMode()
{
  display.clearDisplay();

  display.fillCircle(34,32,15,WHITE);
  display.fillCircle(94,32,15,WHITE);

  display.fillCircle(34,32,4,BLACK);
  display.fillCircle(94,32,4,BLACK);

  display.fillCircle(42,24,2,WHITE);
  display.fillCircle(102,24,2,WHITE);

  display.display();
}

// ======================
// SLEEPY 😪
// ======================

void sleepyMode()
{
  display.clearDisplay();

  display.fillRoundRect(18,26,32,10,4,WHITE);
  display.fillRoundRect(78,26,32,10,4,WHITE);

  display.display();
}

// ======================
// WINK 😉
// ======================

void winkMode()
{
  display.clearDisplay();

  display.fillRoundRect(18,30,32,4,2,WHITE);

  display.fillRoundRect(78,16,32,32,8,WHITE);

  display.fillCircle(94,32,5,BLACK);

  display.display();
}

// ======================
// ANGRY 😠
// ======================

void angryMode()
{
  display.clearDisplay();

  display.fillRoundRect(18,18,32,24,6,WHITE);
  display.fillRoundRect(78,18,32,24,6,WHITE);

  display.fillCircle(34,30,4,BLACK);
  display.fillCircle(94,30,4,BLACK);

  display.drawLine(16,12,50,20,WHITE);
  display.drawLine(78,20,112,12,WHITE);

  display.display();
}

// ======================
// SHOCKED 😲
// ======================

void shockedMode()
{
  display.clearDisplay();

  display.fillCircle(34,32,16,WHITE);
  display.fillCircle(94,32,16,WHITE);

  display.fillCircle(34,32,8,BLACK);
  display.fillCircle(94,32,8,BLACK);

  display.display();
}

// ======================
// THINKING 🤔
// ======================

void thinkingMode()
{
  display.clearDisplay();

  display.fillRoundRect(18,18,32,28,8,WHITE);
  display.fillRoundRect(78,18,32,28,8,WHITE);

  display.fillCircle(28,24,4,BLACK);
  display.fillCircle(88,24,4,BLACK);

  display.drawLine(18,12,50,8,WHITE);
  display.drawLine(78,8,110,12,WHITE);

  display.display();
}

// ======================
// DEEP SLEEP 😴
// ======================

void deepSleepMode()
{
  display.clearDisplay();

  display.drawLine(18,32,50,32,WHITE);
  display.drawLine(78,32,110,32,WHITE);

  display.setTextSize(1);

  display.setCursor(90,10);
  display.print("Z");

  display.setCursor(102,4);
  display.print("Z");

  display.setCursor(114,10);
  display.print("Z");

  display.display();
}
// ======================
// TOUCH CONTROL
// ======================

void handlepetTouch()
{
  bool touch = digitalRead(TOUCH_PIN);

  if(touch && !petlastTouch)
  {
    pettouchStartTime = millis();
  }

  if(!touch && petlastTouch)
  {
    unsigned long pressTime =
      millis() - pettouchStartTime;

    if(pressTime > 2000)
    {
      if(!petSleeping)
      {
        sleepAnimation();
        petSleeping = true;
      }
      else
      {
        petSleeping = false;
        wakeAnimation();
      }

      pettapCount = 0;
    }
    else
    {
      pettapCount++;
      petlastTapTime = millis();
    }
  }

  if(pettapCount > 0 &&
     millis() - petlastTapTime > 350)
  {
    if(pettapCount >= 1)
    {
      eyeStyle++;

      if(eyeStyle > 8)
      {
        eyeStyle = 0;
      }
    }

    pettapCount = 0;
  }

  petlastTouch = touch;
}

// ======================
// MOVEMENT
// ======================

void updatePupil()
{
  pupilPos +=
  (pupilTarget - pupilPos) * 0.08;
}

void updateSquish()
{
  if(millis() - petsquishTimer > 8000)
  {
    eyeHeight = 28;

    delay(120);

    eyeHeight = 32;

    petsquishTimer = millis();
  }
}

// ======================
// PET LOGIC
// ======================

void handlePetLogic()
{
  handlepetTouch();

  if(millis() - petblinkTimer > 5000)
  {
    smoothBlink();
    petblinkTimer = millis();
  }

  if(millis() - petmoveTimer > 2500)
  {
    pupilTarget = random(-4,5);
    petmoveTimer = millis();
  }

  updatePupil();
  updateSquish();
}

// ======================
// MAIN PET DRAW
// ======================
void drawPet()
{
  Serial.print("EyeStyle = ");
  Serial.println(eyeStyle);

  handlePetLogic();

  if(petSleeping)
  {
    drawEyeFrame(8);
    return;
  }

  switch(eyeStyle)
  {
    case 0: happyMode(); break;
    case 1: curiousMode(); break;
    case 2: excitedMode(); break;
    case 3: sleepyMode(); break;
    case 4: winkMode(); break;
    case 5: angryMode(); break;
    case 6: shockedMode(); break;
    case 7: thinkingMode(); break;
    case 8: deepSleepMode(); break;
  }
}
// ======================
// CLOCK PLACEHOLDER
// ======================

void drawClock()
{
  struct tm timeinfo;

  if(!getLocalTime(&timeinfo))
  {
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(25,28);
    display.println("TIME ERROR");

    display.display();
    return;
  }

  char dayBuffer[20];
  char dateBuffer[20];
  char timeBuffer[10];

  strftime(
    dayBuffer,
    sizeof(dayBuffer),
    "%A",
    &timeinfo
  );

  strftime(
    dateBuffer,
    sizeof(dateBuffer),
    "%d %b %Y",
    &timeinfo
  );

  bool showColon =
    (millis() / 1000) % 2;

  if(showColon)
  {
    sprintf(
      timeBuffer,
      "%02d:%02d",
      timeinfo.tm_hour,
      timeinfo.tm_min
    );
  }
  else
  {
    sprintf(
      timeBuffer,
      "%02d %02d",
      timeinfo.tm_hour,
      timeinfo.tm_min
    );
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(20,0);
  display.println("EYORA");

  display.setTextSize(3);
  display.setCursor(15,14);
  display.println(timeBuffer);

  display.setTextSize(1);
  display.setCursor(25,45);
  display.println(dayBuffer);

  display.setCursor(15,55);
  display.println(dateBuffer);

  display.display();
}
// ======================
// WEATHER PLACEHOLDER
// ======================

void drawWeatherIcon()
{
  if(weatherMain == "Clear")
  {
    display.drawCircle(16,18,8,WHITE);

    display.drawLine(16,2,16,8,WHITE);
    display.drawLine(16,28,16,34,WHITE);

    display.drawLine(0,18,6,18,WHITE);
    display.drawLine(26,18,32,18,WHITE);

    display.drawLine(6,8,10,12,WHITE);
    display.drawLine(22,24,26,28,WHITE);

    display.drawLine(22,8,26,4,WHITE);
    display.drawLine(6,24,10,20,WHITE);
  }
  else if(weatherMain == "Clouds")
  {
    display.fillCircle(10,18,6,WHITE);
    display.fillCircle(18,18,8,WHITE);
    display.fillRect(8,18,18,8,WHITE);
  }
  else if(weatherMain == "Rain")
  {
    display.fillCircle(10,16,6,WHITE);
    display.fillCircle(18,16,8,WHITE);
    display.fillRect(8,16,18,8,WHITE);

    display.drawLine(10,28,8,32,WHITE);
    display.drawLine(16,28,14,32,WHITE);
    display.drawLine(22,28,20,32,WHITE);
  }
  else
  {
    display.drawCircle(16,18,8,WHITE);
  }
}

void drawWeather()
{
  display.clearDisplay();

  drawWeatherIcon();
  

  display.setTextSize(1);
  display.setCursor(42,0);
  display.println(city);

  display.setTextSize(2);
  display.setCursor(45,12);

  display.print((int)temperature);
  display.print("C");

  display.setTextSize(1);

  display.setCursor(35,38);
  display.println(weatherMain);

  display.setCursor(0,50);
  display.print("H:");
  display.print(humidity);
  display.print("%");

  display.setCursor(60,50);
  display.print("F:");
  display.print((int)feelsLike);
  display.print("C");

  display.display();
}

void getWeather()
{
  if(WiFi.status() != WL_CONNECTED)
    return;

  HTTPClient http;

  String url =
    "http://api.openweathermap.org/data/2.5/weather?q=" +
    city +
    "&appid=" +
    apiKey +
    "&units=metric";

  http.begin(url);

  int httpCode = http.GET();

  if(httpCode == 200)
  {
    String payload = http.getString();

    DynamicJsonDocument doc(4096);

    deserializeJson(doc, payload);

    temperature =
      doc["main"]["temp"];

    feelsLike =
      doc["main"]["feels_like"];

    humidity =
      doc["main"]["humidity"];

    weatherMain =
      doc["weather"][0]["main"].as<String>();
  }

  http.end();
}

// ======================
// GAME PLACEHOLDER
// ======================

// ======================
// DRAW GAME
// ======================

void drawGame()
{
  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(0,0);
  display.print("L:");
  display.print(level);

  display.setCursor(45,0);
  display.print("S:");
  display.print(score);

  display.setCursor(90,0);
  display.print("HP:");
  display.print(lives);

  // Targets

  display.setCursor(15,25);
  display.print("+");

  display.setCursor(60,25);
  display.print("+");

  display.setCursor(105,25);
  display.print("+");

  // Star

  if(starPos == 0)
  {
    display.drawCircle(18,28,8,WHITE);
  }
  else if(starPos == 1)
  {
    display.drawCircle(63,28,8,WHITE);
  }
  else
  {
    display.drawCircle(108,28,8,WHITE);
  }

  // Cursor

  if(cursorPos == 0)
  {
    display.setCursor(15,50);
  }
  else if(cursorPos == 1)
  {
    display.setCursor(60,50);
  }
  else
  {
    display.setCursor(105,50);
  }

  display.print("^");

  display.display();
}

// ======================
// NEXT LEVEL
// ======================

void nextLevel()
{
  level++;

  if(level > 10)
  {
    gameWin = true;
    return;
  }

  starPos = random(0,3);
}

// ======================
// GAME OVER
// ======================

void drawGameOver()
{
  display.clearDisplay();

  display.setTextSize(2);

  display.setCursor(5,10);
  display.println("GAME");

  display.setCursor(10,30);
  display.println("OVER");

  display.setTextSize(1);

  display.setCursor(20,55);
  display.print("Score:");
  display.print(score);

  display.display();
}

// ======================
// WIN
// ======================

void drawWin()
{
  display.clearDisplay();

  display.setTextSize(2);

  display.setCursor(15,10);
  display.println("YOU");

  display.setCursor(20,30);
  display.println("WIN");

  display.setTextSize(1);

  display.setCursor(20,55);
  display.print("Score:");
  display.print(score);

  display.display();
}

// ======================
// RESTART
// ======================

void restartGame()
{
  level = 1;
  score = 0;
  lives = 3;

  cursorPos = 0;

  gameOver = false;
  gameWin = false;

  starPos = random(0,3);
}
// ======================
// TOUCH ENGINE
// ======================

void handleTouch()
{
  bool touch = digitalRead(TOUCH_PIN);

  if(touch && !lastTouch)
  {
    touchStart = millis();
  }

  if(!touch && lastTouch)
  {
    unsigned long pressTime =
      millis() - touchStart;

    // Long Press
    if(pressTime > 1500)
    {
      currentScreen = MENU;
      drawMenu();

      tapCount = 0;
    }
    else
    {
      tapCount++;
      lastTapTime = millis();
    }
  }

  if(tapCount > 0 &&
     millis() - lastTapTime > 300)
  {
    // ======================
    // PET
    // ======================

    if(currentScreen == PET_SCREEN)
    {
        if(tapCount >= 1)
        {
            eyeStyle++;

            if(eyeStyle > 8)
            {
                eyeStyle = 0;
            }
        }
    }
    
    // ======================
    // MENU
    // ======================

    if(currentScreen == MENU)
    {
      if(tapCount == 1)
      {
        selectedApp++;

        if(selectedApp > 3)
          selectedApp = 0;

        drawMenu();
      }

      else if(tapCount >= 2)
      {
        switch(selectedApp)
        {
          case 0:
            currentScreen = PET_SCREEN;
            drawPet();
            break;

          case 1:
            currentScreen = CLOCK_SCREEN;
            drawClock();
            break;

          case 2:
            currentScreen = WEATHER_SCREEN;
            drawWeather();
            break;

          case 3:
            currentScreen = GAME_SCREEN;

            restartGame();

            drawGame();
            break;
        }
      }
    }

    // ======================
    // GAME
    // ======================

    else if(currentScreen == GAME_SCREEN)
    {
      if(tapCount == 1)
      {
        if(!gameOver && !gameWin)
        {
          cursorPos++;

          if(cursorPos > 2)
            cursorPos = 0;

          drawGame();
        }
      }

      else if(tapCount >= 2)
      {
        if(!gameOver && !gameWin)
        {
          if(cursorPos == starPos)
          {
            score++;

            nextLevel();

            if(!gameWin)
            {
              drawGame();
            }
          }
          else
          {
            lives--;

            if(lives <= 0)
            {
              gameOver = true;
              drawGameOver();
            }
            else
            {
              starPos = random(0,3);
              drawGame();
            }
          }
        }
        else
        {
          restartGame();
          drawGame();
        }
      }
    }

    tapCount = 0;
  }

  lastTouch = touch;
}
// ======================
// SETUP
// ======================
void setup()
{
  Serial.begin(115200);

  pinMode(TOUCH_PIN, INPUT);

  Wire.begin(21,22);

  if(!display.begin(
      SSD1306_SWITCHCAPVCC,
      0x3C))
  {
    while(true);
  }

  display.clearDisplay();
  display.display();

  // ======================
  // WIFI
  // ======================

  WiFi.begin(ssid, password);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10,28);
  display.println("Connecting WiFi");
  display.display();

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  // ======================
  // CLOCK (NTP)
  // ======================

  configTime(
    gmtOffset_sec,
    daylightOffset_sec,
    "pool.ntp.org",
    "time.nist.gov"
  );

  // ======================
  // WEATHER
  // ======================

  getWeather();

  lastWeatherUpdate = millis();

  // ======================
  // MENU
  // ======================

  drawMenu();

  // ======================
  // PET INIT
  // ======================

  initPet();    
}

// ======================
// LOOP
// ======================

void loop()
{
  handleTouch();

  if(currentScreen == PET_SCREEN)
  {
    drawPet();
  }

  else if(currentScreen == CLOCK_SCREEN)
  {
    drawClock();
  }

  else if(currentScreen == WEATHER_SCREEN)
  {
    drawWeather();
  }

  else if(currentScreen == GAME_SCREEN)
  {
    if(gameOver)
    {
      drawGameOver();
    }
    else if(gameWin)
    {
      drawWin();
    }
    else
    {
      drawGame();
    }
  }
  delay(20);
}