#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <WiFi.h>

// SENİN MEVCUT PİNLERİN
#define TFT_CS    17
#define TFT_DC    16
#define TFT_RST   5

#define BTN_UP    26
#define BTN_DOWN  25
#define BTN_LEFT  33
#define BTN_RIGHT 13
#define BTN_OK    27

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int menuIndex = 0;
int currentMode = -1;
int totalMenuItems = 5;
unsigned long lastBtnTime = 0;

const char* menuItems[] = {
  "1. Wi-Fi Scanner",
  "2. Guvenlik Analizi",
  "3. Kanal Caksima",
  "4. Ag Istatistigi",
  "5. Mesafe Tahmini"
};

void setup() {
  // 1. Önce Butonlar
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);

  // 2. Ekran Reset Zamanlaması
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, LOW);
  delay(100);
  digitalWrite(TFT_RST, HIGH);
  delay(100);

  // 3. Ekran Sürücüsü Başlatma
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(ST7735_BLACK);

  // 4. Üst Başlığı Çiz
  tft.fillRect(0, 0, 128, 18, ST7735_BLUE);
  tft.setTextColor(ST7735_WHITE, ST7735_BLUE);
  tft.setTextSize(1);
  tft.setCursor(12, 5);
  tft.print("METE_LAB v4.0");
  tft.drawFastHLine(0, 18, 128, ST7735_CYAN);

  drawMenu();

  // 5. Wi-Fi En Son Başlatılıyor
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}

void loop() {
  handleButtons();

  if (currentMode != -1) {
    switch (currentMode) {
      case 0: runWifiScanner(); break;
      case 1: runSecurityAudit(); break;
      case 2: runChannelAnalyzer(); break;
      case 3: runNetworkStats(); break;
      case 4: runDistanceMeter(); break;
    }
  }
}

void handleButtons() {
  if (millis() - lastBtnTime < 180) return;

  if (currentMode == -1) {
    if (digitalRead(BTN_DOWN) == LOW) {
      menuIndex = (menuIndex + 1) % totalMenuItems;
      drawMenu();
      lastBtnTime = millis();
    } 
    else if (digitalRead(BTN_UP) == LOW) {
      menuIndex--;
      if (menuIndex < 0) menuIndex = totalMenuItems - 1;
      drawMenu();
      lastBtnTime = millis();
    } 
    else if (digitalRead(BTN_OK) == LOW || digitalRead(BTN_RIGHT) == LOW) {
      currentMode = menuIndex;
      tft.fillRect(0, 20, 128, 140, ST7735_BLACK);
      lastBtnTime = millis();
    }
  } else {
    if (digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_OK) == LOW) {
      currentMode = -1;
      tft.fillRect(0, 20, 128, 140, ST7735_BLACK);
      drawMenu();
      lastBtnTime = millis();
    }
  }
}

void drawMenu() {
  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);
  tft.setCursor(4, 22);
  tft.print("== ANA MENU ==");

  for (int i = 0; i < totalMenuItems; i++) {
    int y = 38 + (i * 20);
    if (i == menuIndex) {
      tft.fillRect(2, y - 3, 124, 16, 0x000F);
      tft.drawRect(2, y - 3, 124, 16, ST7735_CYAN);
      tft.setTextColor(ST7735_GREEN, 0x000F);
      tft.setCursor(6, y);
      tft.print("> ");
      tft.print(menuItems[i]);
    } else {
      tft.fillRect(2, y - 3, 124, 16, ST7735_BLACK);
      tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
      tft.setCursor(6, y);
      tft.print("  ");
      tft.print(menuItems[i]);
    }
  }
}

void runWifiScanner() {
  tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
  tft.setCursor(4, 22);
  tft.print("1. WI-FI SCANNER ");

  int n = WiFi.scanNetworks();
  if (n > 0) {
    int maxDisplay = min(n, 5);
    for (int i = 0; i < maxDisplay; i++) {
      int y = 36 + (i * 21);
      String ssid = WiFi.SSID(i);
      if (ssid.length() > 8) ssid = ssid.substring(0, 8);

      tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
      tft.setCursor(4, y);
      tft.print(ssid);

      int dbm = WiFi.RSSI(i);
      int barW = map(dbm, -90, -30, 5, 45);
      barW = constrain(barW, 5, 45);

      tft.fillRect(58, y, 45, 8, ST7735_BLACK);
      tft.fillRect(58, y, barW, 8, ST7735_GREEN);
      tft.drawRect(58, y, 45, 8, ST7735_WHITE);
    }
  }
  delay(400);
}

void runSecurityAudit() {
  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);
  tft.setCursor(4, 22);
  tft.print("2. GUVENLIK ANALIZI");

  int n = WiFi.scanNetworks();
  if (n > 0) {
    int maxDisplay = min(n, 5);
    for (int i = 0; i < maxDisplay; i++) {
      int y = 38 + (i * 20);
      String ssid = WiFi.SSID(i);
      if (ssid.length() > 7) ssid = ssid.substring(0, 7);

      tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
      tft.setCursor(4, y);
      tft.print(ssid);

      if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) {
        tft.setTextColor(ST7735_RED, ST7735_BLACK);
        tft.setCursor(65, y);
        tft.print("[ACIK AG]");
      } else {
        tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
        tft.setCursor(65, y);
        tft.print("[WPA2/3]");
      }
    }
  }
  delay(400);
}

void runChannelAnalyzer() {
  tft.setTextColor(ST7735_CYAN, ST7735_BLACK);
  tft.setCursor(4, 22);
  tft.print("3. KANAL CAKISMA  ");

  int n = WiFi.scanNetworks();
  int chCount[14] = {0};

  for (int i = 0; i < n; i++) {
    int ch = WiFi.channel(i);
    if (ch >= 1 && ch <= 13) chCount[ch]++;
  }

  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.setCursor(4, 42); tft.print("CH 1 : "); tft.print(chCount[1]);
  tft.setCursor(4, 60); tft.print("CH 6 : "); tft.print(chCount[6]);
  tft.setCursor(4, 78); tft.print("CH 11: "); tft.print(chCount[11]);

  delay(500);
}

void runNetworkStats() {
  tft.setTextColor(ST7735_ORANGE, ST7735_BLACK);
  tft.setCursor(4, 22);
  tft.print("4. AG ISTATISTIGI ");

  int n = WiFi.scanNetworks();
  int openN = 0, securedN = 0;

  for (int i = 0; i < n; i++) {
    if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) openN++;
    else securedN++;
  }

  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.setCursor(6, 45); tft.print("TOPLAM AG : "); tft.print(n);
  tft.setCursor(6, 68); tft.print("SIFRELI   : "); tft.print(securedN);
  tft.setCursor(6, 91); tft.print("ACIK AG   : "); tft.print(openN);

  delay(400);
}

void runDistanceMeter() {
  tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
  tft.setCursor(4, 22);
  tft.print("5. MESAFE TAHMINI ");

  int n = WiFi.scanNetworks();
  if (n > 0) {
    String bestSSID = WiFi.SSID(0);
    if (bestSSID.length() > 9) bestSSID = bestSSID.substring(0, 9);
    int rssi = WiFi.RSSI(0);

    double expVal = (-69 - (rssi)) / (10 * 2.0);
    double dist = pow(10, expVal);

    tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
    tft.setCursor(4, 45); tft.print("EN YAKIN AG:");
    tft.setCursor(4, 60); tft.print(bestSSID);
    tft.setCursor(4, 85); tft.print("MESAFE: ~"); tft.print(dist, 1); tft.print(" M");
  }
  delay(400);
}
