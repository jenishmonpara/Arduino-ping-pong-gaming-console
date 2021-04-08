#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define POTPIN 0   // For player's input

const unsigned long cpulag = 100;     // make it high to slow down cpu's response
const unsigned long PADDLE_RATE = 20;// keeping this high because there was so much noise in  resistaance if pot pin during actual testing
const unsigned long BALL_SIZE = 1;
const unsigned long BALL_RATE = 1; // higher ball rate means slower 
const uint8_t WIN_SCORE = 3; // keep it below 10 only!
const uint8_t PADDLE_HEIGHT = 20;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint8_t ball_x = 64, ball_y = 32;
uint8_t ball_dir_x = 1, ball_dir_y = 1;
const uint8_t CPU_X = 12, PLAYER_X = 115;
uint8_t cpu_y = 16, player_y = 16;
unsigned long ball_update, paddle_update, cpu_update = 0;
uint8_t playerscore,cpuscore;

void setup() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.display();
    display.clearDisplay();
    drawCourt();
    display.setTextSize(2);     
    display.setTextColor(WHITE);
    display.cp437(true);         // enables all unicode characters
    display.display();

    ball_update = millis();
    paddle_update = ball_update;
    playerscore = cpuscore = 0;
}

void loop() {
    unsigned long time = millis();
    
    if(time > ball_update){
        uint8_t new_x = ball_x + ball_dir_x;
        uint8_t new_y = ball_y + ball_dir_y;

        //Check if we hit the vertical walls
        if(new_x == 0 || new_x == 127) {  
          
          if(new_x == 0)playerscore++;
          else cpuscore++;
          
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        // Check if we hit the horizontal walls.
        if(new_y == 0 || new_y == 63) {
            ball_dir_y = -ball_dir_y;
            new_y += ball_dir_y + ball_dir_y;
        }

        // Check if we hit the CPU paddle
        if(new_x == CPU_X && new_y >= cpu_y && new_y <= cpu_y + PADDLE_HEIGHT) {
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        // Check if we hit the player paddle
        if(new_x == PLAYER_X && new_y >= player_y && new_y <= player_y + PADDLE_HEIGHT)
        {
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        eraseBall(ball_x,ball_y);
        drawBall(new_x,new_y);
        drawCourt();
        ball_x = new_x;
        ball_y = new_y;

        ball_update += BALL_RATE;
    
    }

    if(time > paddle_update) {
        paddle_update += PADDLE_RATE;
        // CPU paddle
        display.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, BLACK);

        //calculating cpu's response and updating it based on speed
        const uint8_t half_paddle = PADDLE_HEIGHT >> 1;
        if(cpu_y + half_paddle > ball_y and time > cpu_update + cpulag) {
            cpu_update = time;
            cpu_y -= 1;
        }
        if(cpu_y + half_paddle < ball_y and time > cpu_update + cpulag) {
            cpu_update = time;
            cpu_y += 1;
        }
        if(cpu_y < 1) cpu_y = 1;
        if(cpu_y + PADDLE_HEIGHT > 63) cpu_y = 63 - PADDLE_HEIGHT;
        display.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, WHITE);

        // Player paddle
        display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, BLACK);
       
        player_y = map(analogRead(POTPIN), 0, 500, 1, 63 - PADDLE_HEIGHT);
        if(player_y < 1) player_y = 1;
        if(player_y + PADDLE_HEIGHT > 63) player_y = 63 - PADDLE_HEIGHT;
        display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, WHITE);
       
    }
    check_winner();
    displayscores();
    
    display.display();
}

void check_winner(){
    if(cpuscore >= WIN_SCORE ){
      display.clearDisplay();
      erasescores();
      //display.setTextSize(1);
      display.setCursor(16,24);
      display.print("CPU Wins. LOL xD");
      delay(30000);
      
    }
    if(playerscore >= WIN_SCORE){
      display.clearDisplay();
      erasescores();
      //display.setTextSize(1);
      display.setCursor(0,24);
      display.print("You Won :)");
      delay(30000);
    }
}

void displayscores(){
    erasescores();
    display.setCursor(50, 4);
    display.write('0'+cpuscore);
    display.setCursor(70, 4);
    display.write('0'+playerscore);
}

void erasescores(){
      //erasing cpu score
      for(int i=50;i<64;i++){
        for(int j=1;j<20;j++){
          display.drawPixel(i,j, BLACK);
        }
      }

      //now of player
      for(int i=70;i<85;i++){
        for(int j=1;j<20;j++){
          display.drawPixel(i,j, BLACK);
        }
      }
}

void drawCourt() {
    display.drawRect(0, 0, 128, 64, WHITE);

    // Drawnig one vertical net
    for(int y = 0;y<64;y += 6){
      for(int k=y;k<y+3 and k<64;k++)
              display.drawPixel(64,k, WHITE);
    }
}

void drawBall(int x, int y) {
  display.drawCircle(x, y, BALL_SIZE, WHITE);
}
void eraseBall(int x, int y) {
  display.drawCircle(x, y, BALL_SIZE, BLACK);
}
    
