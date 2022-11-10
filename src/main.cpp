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
    roomba_send_num(0);  //Radius  0  速度が0なのでなんでも良い
    delay(100);
};


void roomba_drive_turn_counterclockwise(int num){ //反時計回り 引数は速さ
    Roomba.write(137);
    roomba_send_num(num);  //Velocity 100mm/s
    roomba_send_num(1);  //Radius 1
    delay(100);
};


void roomba_drive_turn_clockwise(int num){ //時計回り 引数は速さ
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
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 70);
    M5.Lcd.setTextSize(5);

    switch (buf[0]) {
        case 'A':
            roomba_drive(v, v); //go advance
            M5.Lcd.println("Forward");
            break;
        case 'B':
            roomba_drive(-v, -v);//go back
            M5.Lcd.println("Back");
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
            roomba_drive(v, v/2); // left shift
            M5.Lcd.println("Turn left");
            break;
        case 'H':
            roomba_drive(v/2, v);
            M5.Lcd.println("Turn right");
            break;
        case 'I':
            roomba_drive_turn_clockwise(v); // right turn
            M5.Lcd.println("Clockwise");
            break;
        case 'J':
            roomba_drive_turn_counterclockwise(v); // left turn
            M5.Lcd.println("Counterclockwise");
            break;
        case 'K':
            roomba_drive(0, 0);
            M5.Lcd.println("Stop");
            break;
        default:
            break;
    }
    
    delay(150);
}
