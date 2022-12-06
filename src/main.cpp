#include <AsyncUDP.h>
#include <M5Stack.h>
#include <stdlib.h>
#include <WiFi.h>


// use a ring buffer to increase speed and reduce memory allocation
char buf[1];
int brush = 0; // brush flag
int music = 0; // music flag
char ssid[] = "M5StickC-Plus-Controller";
char pass[] = "controller";
AsyncUDP udp; // generate udp instance
unsigned int port = 8888;  // local port to listen on

HardwareSerial roomba(2); // m5stack to roomba, rx:16, tx:17
int v = 100; // velocity

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
    roomba_send_num(right);  //Velocity right 
    roomba_send_num(left);  //Velocity left 
    delay(100);
}


void roomba_moter_stop() {
    roomba.write(137);
    roomba_send_num(0);  //Velocity 0mm/s
    roomba_send_num(0);  //Radius
    delay(100);
};


void roomba_drive_turn_counterclockwise(int num) {
    roomba.write(137);
    roomba_send_num(num);  //Velocity 100mm/s
    roomba_send_num(1);  //Radius 1
    delay(100);
};


void roomba_drive_turn_clockwise(int num) {
    roomba.write(137);
    roomba_send_num(num);  //Velocity 
    roomba_send_num(-1);  //Radius 
    delay(100);
};


void setup() {
    roomba.begin(115200); // default 115200bps
    Serial.begin(9600);

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
    }
}


void loop() {
    M5.update();
    M5.Lcd.fillScreen(BLACK);

    if (M5.BtnA.wasPressed() && 0 < v) { // min: 0
        v -= 50;
        music = 0;
    } else if (M5.BtnB.wasPressed()) {
        roomba.write(128);
        roomba.write(133);
        roomba.write(128);
        roomba.write(173);
        delay(200);
        M5.Power.powerOFF();
        music = 0;
    } else if (M5.BtnC.wasPressed() && v < 250) { // max: 250
        v += 50;
        music = 0;
    } else if (v == 0 && music == 0) {
        M5.Lcd.setCursor(10, 70);
        M5.Lcd.println("Yobikomi");

        roomba.write(140);
        roomba.write(1);
        roomba.write(6);
        roomba.write(81);
        roomba.write(14);
        roomba.write(81);
        roomba.write(28);
        roomba.write(83);
        roomba.write(14);
        roomba.write(81);
        roomba.write(14);
        roomba.write(78);
        roomba.write(14);
        roomba.write(81);
        roomba.write(28);
        roomba.write(141);
        roomba.write(1);
        delay(1800);
        roomba.write(141);
        roomba.write(1);
        delay(1800);
        roomba.write(140);
        roomba.write(1);
        roomba.write(6);
        roomba.write(74);
        roomba.write(14);
        roomba.write(74);
        roomba.write(14);
        roomba.write(74);
        roomba.write(14);
        roomba.write(76);
        roomba.write(14);
        roomba.write(78);
        roomba.write(42);
        roomba.write(74);
        roomba.write(14);
        roomba.write(141);
        roomba.write(1);
        delay(1800);
        roomba.write(140);
        roomba.write(1);
        roomba.write(3);
        roomba.write(78);
        roomba.write(42);
        roomba.write(81);
        roomba.write(14);
        roomba.write(81);
        roomba.write(56);
        roomba.write(141);
        roomba.write(1);
        delay(1800);
        roomba.write(140);
        roomba.write(1);
        roomba.write(5);
        roomba.write(74);
        roomba.write(14);
        roomba.write(74);
        roomba.write(14);
        roomba.write(74);
        roomba.write(14);
        roomba.write(76);
        roomba.write(14);
        roomba.write(78);
        roomba.write(56);
        roomba.write(141);
        roomba.write(1);
        delay(1800);
        roomba.write(141);
        roomba.write(1);
        delay(1800);
        roomba.write(140);
        roomba.write(1);
        roomba.write(6);
        roomba.write(76);
        roomba.write(14);
        roomba.write(76);
        roomba.write(14);
        roomba.write(76);
        roomba.write(14);
        roomba.write(74);
        roomba.write(14);
        roomba.write(76);
        roomba.write(28);
        roomba.write(78);
        roomba.write(28);
        roomba.write(141);
        roomba.write(1);
        delay(1800);
        roomba.write(140);
        roomba.write(1);
        roomba.write(4);
        roomba.write(81);
        roomba.write(28);
        roomba.write(79);
        roomba.write(28);
        roomba.write(78);
        roomba.write(28);
        roomba.write(76);
        roomba.write(28);
        roomba.write(141);
        roomba.write(1);
        delay(1800);
        roomba.write(140);
        roomba.write(1);
        roomba.write(6);
        roomba.write(81);
        roomba.write(14);
        roomba.write(81);
        roomba.write(28);
        roomba.write(83);
        roomba.write(14);
        roomba.write(81);
        roomba.write(14);
        roomba.write(78);
        roomba.write(14);
        roomba.write(81);
        roomba.write(28);
        roomba.write(141);
        roomba.write(1);
        delay(1800);
        roomba.write(140);
        roomba.write(1);
        roomba.write(6);
        roomba.write(81);
        roomba.write(14);
        roomba.write(81);
        roomba.write(28);
        roomba.write(83);
        roomba.write(14);
        roomba.write(81);
        roomba.write(14);
        roomba.write(78);
        roomba.write(14);
        roomba.write(76);
        roomba.write(28);
        roomba.write(141);
        roomba.write(1);
        delay(1800);
        roomba.write(140);
        roomba.write(1);
        roomba.write(1);
        roomba.write(74);
        roomba.write(60);
        roomba.write(141);
        roomba.write(1);
        delay(1800);
        music = 1;
    } 

    M5.Lcd.setCursor(10, 10);
    M5.Lcd.printf("v = %d", v);
    M5.Lcd.setCursor(10, 70);

    switch (buf[0]) {
        case 'a':
            M5.Lcd.println("Forward");
            roomba_drive(v, v); //go advance
            break;
        case 'A':
            M5.Lcd.println("Forward");
            roomba_drive(2*v, 2*v); //go advance
            break;
        case 'b':
            M5.Lcd.println("Back");
            roomba_drive(-v, -v); //go back
            break;
        case 'B':
            M5.Lcd.println("Back");
            roomba_drive(-2*v, -2*v); //go back
            break;
        case 'c':
            M5.Lcd.println("Left Forward");
            roomba_drive(v, v/5);
            break;
        case 'C':
            M5.Lcd.println("Left Forward");
            roomba_drive(2*v, 2*v/5);
            break;
        case 'd':
            M5.Lcd.println("Right Forward");
            roomba_drive(v/5, v);
            break;
        case 'D':
            M5.Lcd.println("Right Forward");
            roomba_drive(2*v/5, 2*v);
            break;
        case 'e':
            M5.Lcd.println("Left Back");
            roomba_drive(-v, -v/5);
            break;
        case 'E':
            M5.Lcd.println("Left Back");
            roomba_drive(-2*v, -2*v/5);
            break;
        case 'f':
            M5.Lcd.println("Right Back");
            roomba_drive(-v/5, -v);
            break;
        case 'F':
            M5.Lcd.println("Right Back");
            roomba_drive(-2*v/5, -2*v);
            break;
        case 'g':
            M5.Lcd.println("Counterclockwise");
            roomba_drive_turn_counterclockwise(v); // turn counterclockwise
            break;
        case 'G':
            M5.Lcd.println("Counterclockwise");
            roomba_drive_turn_counterclockwise(2*v); // turn counterclockwise
            break;
        case 'h':
            M5.Lcd.println("Clockwise");
            roomba_drive_turn_clockwise(v); // turn clockwise
            break;
        case 'H':
            M5.Lcd.println("Clockwise");
            roomba_drive_turn_clockwise(2*v); // turn clockwise
            break;
        case 'I':
            roomba.write(140);
            roomba.write(2);
            roomba.write(6);
            roomba.write(84);
            roomba.write(5);
            roomba.write(91);
            roomba.write(5);
            roomba.write(100);
            roomba.write(5);
            roomba.write(96);
            roomba.write(5);
            roomba.write(98);
            roomba.write(5);
            roomba.write(103);
            roomba.write(5);
            roomba.write(141);
            roomba.write(2);
            break;
        case 'J':
            if (brush == 0) {
                roomba.write(138);
                roomba.write(7);
                brush = 1;
                delay(1000);
            } else if (brush == 1) {
                roomba.write(138);
                roomba.write(0);
                brush = 0;
                delay(1000);
            }
            break;
        case 'K':
            M5.Lcd.println("Stop");
            roomba_moter_stop();
            break;
        default:
            M5.Lcd.println("Stop");
            roomba_moter_stop();
            break;
    }
   
    delay(100);
}
