#include <AsyncUDP.h>
#include <M5Stack.h>
#include <stdlib.h>
#include <WiFi.h>

char ssid[] = "M5StickC-Plus-Controller";
char pass[] = "controller"; 
// use a ring buffer to increase speed and reduce memory allocation
char buf[1]; 
AsyncUDP udp; // generate udp instance
unsigned int port = 8888;  // local port to listen on

HardwareSerial Roomba(2); // ESP to Roomba, rx:16, tx:17
int v = 100; // velocity

// devide integer to 8 bits
unsigned int hex_convert_to16(int a, int b){
    return (unsigned int)(a << 8)|(int)(b);
}


unsigned int hex_convert_to8_high(int a){
    return (unsigned int)(a >> 8)&0x00FF;
}


unsigned int hex_convert_to8_low(int a){
    return a^(hex_convert_to8_high(a) << 8);
}


void roomba_send_num(int num) { // devide into two 8-bit commands 
    Roomba.write(hex_convert_to8_high(num));
    Roomba.write(hex_convert_to8_low(num));
}


void roomba_drive(int right,int left) { // go advance
    Roomba.write(byte(145));
    roomba_send_num(right);  //Velocity right 
    roomba_send_num(left);  //Velocity left 
    delay(100);
}


void roomba_moter_stop(){  //モーターを止める
    Roomba.write(137);
    roomba_send_num(0);  //Velocity 0mm/s
    roomba_send_num(0);  //Radius
    delay(100);
};


void roomba_drive_turn_counterclockwise(int num) {
    Roomba.write(137);
    roomba_send_num(num);  //Velocity 100mm/s
    roomba_send_num(1);  //Radius 1
    delay(100);
};


void roomba_drive_turn_clockwise(int num) { 
    Roomba.write(137);
    roomba_send_num(num);  //Velocity 
    roomba_send_num(-1);  //Radius 
    delay(100);
};


void setup() {
    Roomba.begin(115200); // default 115200bps
    Serial.begin(9600);

    M5.begin();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    delay(100);

    Serial.print("Connecting...");
    while (WiFi.waitForConnectResult() != WL_CONNECTED) { 
        Serial.print(".");    
        delay(500);
    }
    Serial.println("");
 
    if (udp.listen(port)) {
        udp.onPacket([](AsyncUDPPacket packet) {
            buf[0]= (char)*(packet.data());
            Serial.println(buf[0]);
        });
    }
}


void loop() {
    M5.update();
    M5.Lcd.setTextSize(5);
    M5.Lcd.fillScreen(BLACK);

    if (M5.BtnA.wasPressed() && 0 < v) { // min: 0
        v -= 50;
    } else if (M5.BtnC.wasPressed() && v < 250) { // max: 250
        v += 50;
    } 

    M5.Lcd.setCursor(10, 10);
    M5.Lcd.printf("v = %d", v);
    M5.Lcd.setCursor(10, 70);

    switch (buf[0]) {
        case 'A':
            M5.Lcd.println("Forward");
            roomba_drive(v, v); //go advance
            break;
        case 'B':
            M5.Lcd.println("Back");
            roomba_drive(-v, -v); //go back
            break;
        case 'C':
            //left_shift(0,200,0,200);
            break;
        case 'D':
            //right_shift(200,0,200,0);
            break;
        case 'E':
            //left_shift(200,0,200,0);
            break;
        case 'F':
            //right_shift(0,200,0, 200);
            break;
        case 'G':
            M5.Lcd.println("Turn left");
            roomba_drive(v, v/2); // left shift
            break;
        case 'H':
            M5.Lcd.println("Turn right");
            roomba_drive(v/2, v);
            break;
        case 'I':
            M5.Lcd.println("Clockwise");
            roomba_drive_turn_clockwise(v); // turn clockwise
            break;
        case 'J':
            M5.Lcd.println("Counterclockwise");
            roomba_drive_turn_counterclockwise(v); // turn counterclockwise
            break;
        case 'K':
            M5.Lcd.println("Stop");
            roomba_drive(0, 0);
            break;
        default:
            break;
    }
    
    delay(100);
}
