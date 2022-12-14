#include <AsyncUDP.h>
#include <M5Stack.h>
#include <stdlib.h>
#include <vector>
#include <WiFi.h>

#define sensorF 21
#define sensorB 22
#define sensorFR 3
#define sensorFL 19
#define sensorBR 35
#define sensorBL 36


char buf[1]; // direction command
bool brush = false; // brush flag
bool music; // music flag
HardwareSerial roomba(2); // esp32 to roomba, rx:16, tx:17
int v = 100; // velocity
// super mario
std::vector<int> oneUp = {140, 2, 6, 84, 5, 91, 5, 100, 5, 96, 5, 98, 5, 103, 5, 141, 2};

char ssid[] = "M5StickC-Plus-Controller";
char pass[] = "controller";
AsyncUDP udp; // udp instance
unsigned int port = 8888;  // local port to listen on

// devide integer to 8 bits
unsigned int hex_convert_to16(int a, int b) {
    return (unsigned int)(a << 8)|(int)(b);
}


unsigned int hex_convert_to8_high(int a) {
    return (unsigned int)(a >> 8) & 0x00FF;
}


unsigned int hex_convert_to8_low(int a) {
    return a^(hex_convert_to8_high(a) << 8);
}


void roomba_send_num(int num) { // devide into two 8-bit commands 
    roomba.write(hex_convert_to8_high(num));
    roomba.write(hex_convert_to8_low(num));
}


void roomba_drive(int right,int left) { // go advance
    roomba.write(byte(145));
    roomba_send_num(right);  // velocity right 
    roomba_send_num(left);  // velocity left 
    delay(100);
}


void stop() {
    roomba.write(137);
    roomba_send_num(0);  // velocity 0mm/s
    roomba_send_num(0);  // radius
    delay(100);
};


void roomba_drive_turn_counterclockwise(int num) {
    roomba.write(137);
    roomba_send_num(num);  // velocity 100mm/s
    roomba_send_num(1);  // radius
    delay(100);
};


void roomba_drive_turn_clockwise(int num) {
    roomba.write(137);
    roomba_send_num(num);  // velocity 
    roomba_send_num(-1);  // radius 
    delay(100);
};


void send_data(std::vector<int>& arr) {
    for (int i = 0; i < arr.size(); i++) {
        roomba.write(arr[i]);
    }
}


void yobikomi() {
    std::vector<std::vector<int>> arr2 = {
        {140, 1, 6, 81, 14, 81, 28, 83, 14, 81, 14, 78, 14, 81, 28, 141, 1},
        {141, 1},
        {140, 1, 6, 74, 14, 74, 14, 74, 14, 76, 14, 78, 42, 74, 14, 141, 1},
        {140, 1, 3, 78, 42, 81, 14, 81, 56, 141, 1},
        {140, 1, 5, 74, 14, 74, 14, 74, 14, 76, 14, 78, 56, 141, 1},
        {141, 1},
        {140, 1, 6, 76, 14, 76, 14, 76, 14, 74, 14, 76, 28, 78, 28, 141, 1},
        {140, 1, 4, 81, 28, 79, 28, 78, 28, 76, 28, 141, 1},
        {140, 1, 6, 81, 14, 81, 28, 83, 14, 81, 14, 78, 14, 81, 28, 141, 1},
        {140, 1, 6, 81, 14, 81, 28, 83, 14, 81, 14, 78, 14, 76, 28, 141, 1},
        {140, 1, 1, 74, 60, 141, 1}
    };
    for (int i = 0; i < arr2.size(); i++) {
        std::vector<int> arr1 = arr2[i];
        send_data(arr1);
        delay(1800);
    }
}


void setup() {
    pinMode(sensorF, INPUT);
    pinMode(sensorB, INPUT);
    pinMode(sensorFR, INPUT);
    pinMode(sensorFL, INPUT);
    pinMode(sensorBR, INPUT);
    pinMode(sensorBL, INPUT);

    roomba.begin(115200);
    Serial.begin(115200);

    M5.begin();
    M5.Power.begin();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    delay(100);

    M5.Lcd.setTextSize(5);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.println("Connecting...");

    Serial.print("Connecting...");
    while (WiFi.waitForConnectResult() != WL_CONNECTED) { 
        Serial.print(".");    
        delay(500);
    }
    Serial.println("");
    
    roomba.write(128);
    roomba.write(132);

    if (udp.listen(port)) {
        udp.onPacket([](AsyncUDPPacket packet) {
            buf[0]= (char)*(packet.data());
            Serial.println(buf[0]);
        });
    } else {
        stop();
        Serial.println("No Packet");
    }
}


void loop() {
    M5.update();
    M5.Lcd.fillScreen(BLACK);

    if (M5.BtnA.wasPressed() && 0 < v) { // min: 0
        v -= 50;
        music = false;
    } else if (M5.BtnB.wasPressed()) {
        roomba.write(128);
        roomba.write(133);
        roomba.write(128);
        roomba.write(173);
        delay(200);
        M5.Power.powerOFF();
        music = false;
    } else if (M5.BtnC.wasPressed() && v < 250) { // max: 250
        v += 50;
        music = false;
    } else if (v == 0 && music == false) {
        M5.Lcd.setCursor(10, 70);
        M5.Lcd.println("Yobikomi");
        delay(100);
        yobikomi();
        music = true;
    } 

    M5.Lcd.setCursor(10, 10);
    M5.Lcd.printf("v = %d", v);
    M5.Lcd.setCursor(10, 70);

    switch (buf[0]) {
        case 'a':
            if (!digitalRead(sensorF) + !digitalRead(sensorFR) + !digitalRead(sensorFL)) {
                M5.Lcd.println("Stop");
                stop();
            } else {
                M5.Lcd.println("Forward");
                roomba_drive(v, v); //go advance
            }
            break;
        case 'A':
            if (!digitalRead(sensorF) + !digitalRead(sensorFR) + !digitalRead(sensorFL)) {
                M5.Lcd.println("Stop");
                stop();
            } else {
                M5.Lcd.println("Forward");
                roomba_drive(2*v, 2*v); //go advance
            }
            break;
        case 'b':
            if (!digitalRead(sensorB) + !digitalRead(sensorBR) + !digitalRead(sensorBL)) {
                M5.Lcd.println("Stop");
                stop();
            } else {
                M5.Lcd.println("Back");
                roomba_drive(-v, -v);
            }
            break;
        case 'B':
            if (!digitalRead(sensorB) + !digitalRead(sensorBR) + !digitalRead(sensorBL)) {
                M5.Lcd.println("Stop");
                stop();
            } else {
                M5.Lcd.println("Back");
                roomba_drive(-2*v, -2*v);
            }
            break;
        case 'c':
            if (!digitalRead(sensorF) + !digitalRead(sensorFL)) {
                M5.Lcd.println("Stop");
                stop();
            } else {
                M5.Lcd.println("Left Forward");
                roomba_drive(v, v/5);
            }
            break;
        case 'C':
            if (!digitalRead(sensorF) + !digitalRead(sensorFL)) {
                M5.Lcd.println("Stop");
                stop();
            } else {
                M5.Lcd.println("Left Forward");
                roomba_drive(2*v, 2*v/5);
            }
            break;
        case 'd':
            if (!digitalRead(sensorF) + !digitalRead(sensorFR)) {
                M5.Lcd.println("Stop");
                stop();
            } else {
                M5.Lcd.println("Right Forward");
                roomba_drive(v/5, v);
            }
            break;
        case 'D':
            if (!digitalRead(sensorF) + !digitalRead(sensorFR)) {
                M5.Lcd.println("Stop");
                stop();
            } else {
                M5.Lcd.println("Right Forward");
                roomba_drive(2*v/5, 2*v);
            }
            break;
        case 'e':
            if (!digitalRead(sensorB) + !digitalRead(sensorBL)) {
                M5.Lcd.println("Stop");
                stop();
            } else {
                M5.Lcd.println("Left Back");
                roomba_drive(-v, -v/5);
            }
            break;
        case 'E':
            if (!digitalRead(sensorB) + !digitalRead(sensorBL)) {
                M5.Lcd.println("Stop");
                stop();
            } else {
                M5.Lcd.println("Left Back");
                roomba_drive(-2*v, -2*v/5);
            }
            break;
        case 'f':
            if (!digitalRead(sensorB) + !digitalRead(sensorBR)) {
                M5.Lcd.println("Stop");
                stop();
            } else {
                M5.Lcd.println("Right Back");
                roomba_drive(-v/5, -v);
            }
            break;
        case 'F':
            if (!digitalRead(sensorB) + !digitalRead(sensorBR)) {
                M5.Lcd.println("Stop");
                stop();
            } else {
                M5.Lcd.println("Right Back");
                roomba_drive(-2*v/5, -2*v);
            }
            break;
        case 'g':
            M5.Lcd.println("Left Turn");
            roomba_drive_turn_counterclockwise(v); // turn counterclockwise
            break;
        case 'G':
            M5.Lcd.println("Left Turn");
            roomba_drive_turn_counterclockwise(2*v); // turn counterclockwise
            break;
        case 'h':
            M5.Lcd.println("Right Turn");
            roomba_drive_turn_clockwise(v); // turn clockwise
            break;
        case 'H':
            M5.Lcd.println("Right Turn");
            roomba_drive_turn_clockwise(2*v); // turn clockwise
            break;
        case 'I':
            send_data(oneUp);
            break;
        case 'J':
            if (!brush) {
                roomba.write(138);
                roomba.write(7);
                brush = true;
                delay(1000);
            } else if (brush) {
                roomba.write(138);
                roomba.write(0);
                brush = false;
                delay(1000);
            }
            break;
        case 'K':
            M5.Lcd.println("Stop");
            stop();
            break;
        default:
            M5.Lcd.println("Stop");
            stop();
            break;
    }
    delay(100);
}
