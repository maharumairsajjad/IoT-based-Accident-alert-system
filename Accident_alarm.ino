#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <MPU6050_tockn.h>
#include <TinyGPS++.h>

#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1   

#define SW420_PIN 12   // Vibration sensor pin
#define BUZZER_PIN 32  // Buzzer pin
#define GPS_RX_PIN 16  // GPS module RX (ESP32 TX)
#define GPS_TX_PIN 17  // GPS module TX (ESP32 RX)
#define DEBOUNCE_DELAY 50 // Debounce delay in milliseconds

const char* ssid = "Almadinah Top Floor";  
const char* password = "666work000";  
const char* firebaseUrl = ""; // Add your own firebase url  

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MPU6050 mpu(Wire);
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

void beep(int duration) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(duration);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
}

int readVibrationSensor() {
    int firstRead = digitalRead(SW420_PIN);
    delay(DEBOUNCE_DELAY);
    int secondRead = digitalRead(SW420_PIN);
    return (firstRead == secondRead) ? secondRead : -1;
}

void readMPU6050(float &ax, float &ay, float &az, float &gx, float &gy, float &gz) {
    mpu.update();
    ax = mpu.getAccX();
    ay = mpu.getAccY();
    az = mpu.getAccZ();
    gx = mpu.getGyroX();
    gy = mpu.getGyroY();
    gz = mpu.getGyroZ();
}

void readGPS(float &latitude, float &longitude) {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }
    if (gps.location.isValid()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
    } else {
        latitude = 0.0;
        longitude = 0.0;
    }
}

void showLoadingAnimation() {
    for (int i = 0; i < 24; i++) {  
        oled.clearDisplay();
        oled.setTextSize(1);
        oled.setTextColor(WHITE);
        oled.setCursor(47, 5);  
        oled.println(F("SaFic"));
        oled.drawCircle(64, 32, 15, WHITE);
        int angle = (i * 15) % 360;  
        int dotX = 64 + 15 * cos(angle * PI / 180);
        int dotY = 32 + 15 * sin(angle * PI / 180);
        oled.fillCircle(dotX, dotY, 3, WHITE);
        oled.display();
        delay(100);
    }
}

void showWelcomeMessage() {
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(20, 25);
    oled.println(F("Welcome"));
    oled.display();
    delay(2000);
}

void showNames() {
    const char* names[] = {"Anas Zia", "M.Rashid", "Munawar", "Supv By", "Khan", "Bahadar"};
    for (int i = 0; i < 6; i++) {
        oled.clearDisplay();
        oled.setCursor(20, 25);
        oled.println(names[i]);
        oled.display();
        delay(1000);
    }
}

void showEnterInSensors() {
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(30, 20);
    oled.println(F("Enter in"));
    oled.setCursor(35, 40);
    oled.println(F("sensors"));
    oled.display();
    delay(2000);
    beep(300);
    delay(500);
    beep(300);
    
    // Calculate MPU6050 offsets after beep
    mpu.calcGyroOffsets(true);
    Serial.println("MPU6050 offsets calculated!");
}

void sendDataToFirebase(int vibrationState, float ax, float ay, float az, float gx, float gy, float gz, float lat, float lon) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(firebaseUrl);
        http.addHeader("Content-Type", "application/json");

        String jsonData = "{";
        jsonData += "\"MPU6050\": {";
        jsonData += "\"AccX\": " + String(ax) + ",";
        jsonData += "\"AccY\": " + String(ay) + ",";
        jsonData += "\"AccZ\": " + String(az) + ",";
        jsonData += "\"GyroX\": " + String(gx) + ",";
        jsonData += "\"GyroY\": " + String(gy) + ",";
        jsonData += "\"GyroZ\": " + String(gz) + "},";
        jsonData += "\"SW420\": " + String(vibrationState) + ",";
        jsonData += "\"GPS\": {";
        jsonData += "\"Latitude\": " + String(lat, 6) + ",";
        jsonData += "\"Longitude\": " + String(lon, 6) + "}";
        jsonData += "}";

        int httpResponseCode = http.PUT(jsonData);
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        http.end();
    } else {
        Serial.println("WiFi Disconnected! Trying to reconnect...");
        WiFi.begin(ssid, password);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(SW420_PIN, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
    beep(500);
    
    if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 initialization failed!");
        while (1);
    }
    
    oled.clearDisplay();
    showLoadingAnimation();

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");

    Wire.begin();
    mpu.begin();
    Serial.println("MPU6050 connected!");

    gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Serial.println("GPS initialized!");

    showWelcomeMessage();
    showNames();
    showEnterInSensors();
}

void loop() {
    int vibrationState = readVibrationSensor();
    float ax, ay, az, gx, gy, gz;
    float latitude = 0.0, longitude = 0.0;

    readMPU6050(ax, ay, az, gx, gy, gz);
    readGPS(latitude, longitude);

    Serial.print("SW420: "); Serial.print(vibrationState);
    Serial.print(" | AccX: "); Serial.print(ax);
    Serial.print(" AccY: "); Serial.print(ay);
    Serial.print(" AccZ: "); Serial.print(az);
    Serial.print(" | GyroX: "); Serial.print(gx);
    Serial.print(" GyroY: "); Serial.print(gy);
    Serial.print(" GyroZ: "); Serial.print(gz);
    Serial.print(" | GPS: Lat: "); Serial.print(latitude, 6);
    Serial.print(" Lon: "); Serial.println(longitude, 6);

    sendDataToFirebase(vibrationState, ax, ay, az, gx, gy, gz, latitude, longitude);
    
    delay(1000);
}