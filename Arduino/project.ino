//==============================
//Set motor port
//============================== 
int Left_motor_back = 9; 
int Left_motor_go = 5; 
int Right_motor_go = 6; 
int Right_motor_back = 10; 
int Right_motor_en = 8; 
int Left_motor_en = 7;

/*Infrared obstacle avoidance*/
const int SensorRight_2 = A4;     // Right  Infrared sensor
const int SensorLeft_2 = A5;     // Left  Infrared sensor
int SL_2;    // State of Left  Infrared sensor
int SR_2;    // State of Right  Infrared sensor

enum State
{
  INIT,
  OBSTACLE_REACHED,
  FIRST_TURN,
  FIRST_TURN_DONE,
  SECOND_TURN,
  BORDER_REACHED, 
  NO_STATE
};

State state = INIT;
bool shouldSpinLeft = true;
const int DEGREES_90 = 6;
const int DEGREES_LESS_THAN_90 = 5;
const int DEGREES_15 = 1;

double x = 0;
double y = 0;
const int STEP = 1;
void setup()
{
  //Initialize motor drive for output mode
  pinMode(Left_motor_go,OUTPUT); 
  pinMode(Left_motor_back,OUTPUT); 
  pinMode(Right_motor_go,OUTPUT);
  pinMode(Right_motor_back,OUTPUT); 

  pinMode(SensorRight_2, INPUT); //Set Right  Infrared sensor as input
  pinMode(SensorLeft_2, INPUT); //Set left Infrared sensor as input
  
  Serial.begin(9600);     // Set Serial baud rate 9600
}

//=======================Motor=========================

void run()
{
  digitalWrite(Right_motor_go,HIGH);// right motor go ahead
  digitalWrite(Right_motor_back,LOW);     
  analogWrite(Right_motor_go,100);//PWM--Pulse Width Modulation(0~255). It can be adjusted to control speed.
  analogWrite(Right_motor_back,0);
  digitalWrite(Left_motor_go,HIGH);// set left motor go ahead
  digitalWrite(Left_motor_back,LOW);
  analogWrite(Left_motor_go,100);//PWM--Pulse Width Modulation(0~255). It can be adjusted to control speed.
  analogWrite(Left_motor_back,18);
}

void brake() //stop
{
  digitalWrite(Right_motor_go,LOW);
  digitalWrite(Right_motor_back,LOW);
  digitalWrite(Left_motor_go,LOW);
  digitalWrite(Left_motor_back,LOW);
}

void left()//turn left
{
  digitalWrite(Right_motor_go,HIGH);  // right motor go ahead
  digitalWrite(Right_motor_back,LOW);
  analogWrite(Right_motor_go,50); 
  analogWrite(Right_motor_back,0);// PWM--Pulse Width Modulation(0~255) control speed 
  digitalWrite(Left_motor_go,LOW);    // left motor stop
  digitalWrite(Left_motor_back,LOW);
  analogWrite(Left_motor_go,0); 
  analogWrite(Left_motor_back,0);// PWM--Pulse Width Modulation(0~255) control speed  
}

void spin_left(int time)         //Left rotation
{
  digitalWrite(Right_motor_go,HIGH);   // right motor go ahead
  digitalWrite(Right_motor_back,LOW);
  analogWrite(Right_motor_go,50); 
  analogWrite(Right_motor_back,0);// PWM--Pulse Width Modulation(0~255) control speed
  digitalWrite(Left_motor_go,LOW);// left motor back off
  digitalWrite(Left_motor_back,HIGH);
  analogWrite(Left_motor_go,0); 
  analogWrite(Left_motor_back,100);// PWM--Pulse Width Modulation(0~255) control speed
  delay(time * 100);
  brake(); 
}

void right() //turn right
{
  digitalWrite(Right_motor_go,LOW);    // right motor stop
  digitalWrite(Right_motor_back,LOW);
  analogWrite(Right_motor_go,0); 
  analogWrite(Right_motor_back,0);
  digitalWrite(Left_motor_go,HIGH);// left motor go ahead
  digitalWrite(Left_motor_back,LOW);
  analogWrite(Left_motor_go,50); 
  analogWrite(Left_motor_back,0);// PWM--Pulse Width Modulation(0~255) control speed 
}

void spin_right(int time)        //Right rotation
{
  digitalWrite(Right_motor_go,LOW);   // right motor back off
  digitalWrite(Right_motor_back,HIGH);
  analogWrite(Right_motor_go,0); 
  analogWrite(Right_motor_back,50);// PWM--Pulse Width Modulation(0~255) control speed
  digitalWrite(Left_motor_go,HIGH);// left motor go ahead
  digitalWrite(Left_motor_back,LOW);
  analogWrite(Left_motor_go,100); 
  analogWrite(Left_motor_back,0);// PWM--Pulse Width Modulation(0~255) control speed
  delay(time * 100);
  brake();  
}

void back(int time)  //back off 
{
  digitalWrite(Right_motor_go,LOW);  //right motor back off
  digitalWrite(Right_motor_back,HIGH);
  analogWrite(Right_motor_go,0);
  analogWrite(Right_motor_back,50);// PWM--Pulse Width Modulation(0~255) control speed
  digitalWrite(Left_motor_go,LOW);  //left motor back off
  digitalWrite(Left_motor_back,HIGH);
  analogWrite(Left_motor_go,0);
  analogWrite(Left_motor_back,50);// PWM--Pulse Width Modulation(0~255) control speed
  delay(time * 100); 
}

void printCurrCoords()
{
  Serial.println((String)"x:"+x+" y:"+y);
}
//==========================================================

/*main loop*/
void loop()
{
  SR_2 = digitalRead(SensorRight_2);//Right infrared sensor detects the obstacle,then LED[L5] light illuminates and otherwise it goes off.
  SL_2 = digitalRead(SensorLeft_2);//Left infrared sensor detects the obstacle,then LED[L4] light illuminates and otherwise it goes off.
  
  switch(state)
  {
    case INIT:
    {
      Serial.println("INIT");
      // There is no obstacle
      if (SL_2 == HIGH && SR_2==HIGH)
      {
        run();
        //delay(100);
        shouldSpinLeft ? x += (double)((double)STEP / 5) : x -= (double)((double)STEP / 5);
        printCurrCoords();
      }
      // There is an obstacle
      else if ((SL_2 == HIGH & SR_2 == LOW) || (SL_2 == LOW & SR_2 == HIGH))
      {
        brake();
        state = OBSTACLE_REACHED;
        printCurrCoords();
      }
      else
      {
        brake();
      }
      
      delay(500);
     break;
    }
    case OBSTACLE_REACHED:
    {
      Serial.println("OBSTACLE_REACHED");
      spin_left(DEGREES_90);
      delay(500);
      
      run();
      shouldSpinLeft ? y += ((double)STEP * 0.8) : y -= ((double)STEP * 0.8);
      delay(800);
      brake();
      delay(500);
        
      spin_right(DEGREES_90);
      printCurrCoords();
      state = FIRST_TURN;
      
      delay(500);
      break;
    }
    case FIRST_TURN:
    {
      Serial.println("FIRST_TURN");
      // There is no obstacle
      if (SL_2 == HIGH && SR_2==HIGH)
      {
        shouldSpinLeft ? x += ((double)STEP + 0.2) : x -= ((double)STEP + 0.2);
        run();
        delay(1200);
        brake();
        delay(500);
        
        spin_right(DEGREES_90);
        
        printCurrCoords();
        state = FIRST_TURN_DONE;
      }
      else
      {
        // border reached
        state = BORDER_REACHED;
      }
      break;
    }
    case FIRST_TURN_DONE:
    {
      Serial.println("FIRST_TURN_DONE");
      if (SL_2 == HIGH && SR_2==HIGH)
      {
        state = SECOND_TURN;
      }
      else
      {
        spin_left(DEGREES_15);  
      }
      
      delay(500);
      break;
    }
    case SECOND_TURN:
    {
      Serial.println("SECOND_TURN");
      if (SL_2 == HIGH && SR_2==HIGH) // There is no obstacle
      {
        shouldSpinLeft ? y -= (double)((double)STEP / 2) : y += (double)((double)STEP / 2);
        run();
        delay(500);
        brake();
        delay(500);
      
        spin_left(DEGREES_90);
        delay(500);

        //state = NO_STATE;
        printCurrCoords();
        state = INIT;
      }
      else
      {
        brake();
        state = NO_STATE;
      }
      break;
    }
    case BORDER_REACHED:
    {
      Serial.println("BORDER_REACHED");
      if (shouldSpinLeft)
      {
        spin_left(DEGREES_90);
        delay(500);
        spin_left(DEGREES_90);
        delay(500);
        shouldSpinLeft = false;
      }
      else
      {
        spin_right(DEGREES_90);
        delay(500);
        run();
        delay(800);
        brake();
        delay(500);
        run();
        delay(800);
        brake();
        delay(500);
        spin_right(DEGREES_90);
        delay(500);
        y += (double)((double)STEP + 1.6);
        shouldSpinLeft = true;
      }
      printCurrCoords();
      state = INIT;
      break;
    }
  }
}
