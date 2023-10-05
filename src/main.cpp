#include <Arduino.h>
#include <LibRobus.h>
#include "util.h"


void foward(float acceleration);
void stopMotors();
void motorsAccelerate();
void readPulse();
void printState();
Speed *initSpeed();
State *initState();
Pulse *initPulse();

//detecteur de sifflet
float getAmbient() { return analogRead(PINA0); }
float getFrequency() { return analogRead(PINA1); }

int detectFrequency() { return (-(getAmbient() - 45) + getFrequency() > 50); }




BasicSettings BaseSet;
Pin pin;

//initialisation des variable de base
Speed *speed = initSpeed();
State *state = initState();
Pulse *pulse = initPulse();


void setup() {
  BoardInit();

  
//   Serial.begin(9600);

 

  //detecteur de proximité
  pinMode(pin.capGauche, INPUT); //Pin.capDroite
  pinMode(pin.capGauche, INPUT); //capGauche
  pinMode(pin.led_capDroite, OUTPUT); //ledPin.capDroite
  pinMode(pin.led_capGauche, OUTPUT); //ledcapGauche
  
  //detecteur de sifflet
  pinMode(PINA0, INPUT);
  pinMode(PINA1, INPUT);
  
}

void printState(){

  
  Serial.print("| pulse droit = ");
  Serial.print(pulse->right);
  Serial.print(" |\n");
  Serial.print("| pulse gauche = ");
  Serial.print(pulse->left);
  Serial.print(" |\n");
  Serial.print("| vitesse droite = ");
  Serial.print(speed->motorRight);
  Serial.print(" |\n");
  Serial.print("| vitesse gauche = ");
  Serial.print(speed->motorLeft);
  Serial.print(" |\n");
  Serial.print("| detect droit = ");
  Serial.print(state->detectRight);
  Serial.print(" |\n");
  Serial.print("| detect gauche = ");
  Serial.print(state->detectLeft);
  Serial.print(" |\n");
  Serial.print("| moving = ");
  Serial.print(state->moving);
  Serial.print(" |\n");

}
void readPulse(){

  pulse->leftPast=pulse->left;
  pulse->rightPast=pulse->right;
  pulse->left=ENCODER_Read(BaseSet.ENCODER_LEFT);
  pulse->right=ENCODER_Read(BaseSet.ENCODER_RIGHT);
}

void motorsAccelerate(){
  uint8_t delayMs = 150;
  foward(0.10);
  delay(delayMs);
  foward(0.20);
  delay(delayMs);
  foward(0.30);
  delay(delayMs);
  foward(0.40);
  delay(delayMs);
  foward(0.50);
  delay(delayMs);
  foward(0.60);
  delay(delayMs);
  foward(0.70);
  delay(delayMs);
  foward(0.80);
  delay(delayMs);
  foward(0.90);
  delay(delayMs);
  foward(1);
}


void foward(float acceleration){
  if(pulse->right < pulse->left) {
    speed->motorRight =(speed->motorRight+((pulse->left-pulse->right)*BaseSet.KP));
    speed->motorLeft =(speed->motorLeft-((pulse->left-pulse->right)*BaseSet.KP));
    MOTOR_SetSpeed(BaseSet.MOTOR_RIGHT,speed->motorRight*acceleration);
    MOTOR_SetSpeed(BaseSet.MOTOR_LEFT,speed->motorLeft*acceleration);
  }
  if(pulse->right > pulse->left) {
    speed->motorLeft = (speed->motorLeft+((pulse->right-pulse->right)*BaseSet.KP))*acceleration;
    speed->motorRight = (speed->motorRight-((pulse->right-pulse->right)*BaseSet.KP))*acceleration;
    MOTOR_SetSpeed(BaseSet.MOTOR_LEFT,speed->motorLeft*acceleration);
    MOTOR_SetSpeed(BaseSet.MOTOR_RIGHT,speed->motorRight*acceleration);
  }
  if(pulse->right == pulse->left) {
    MOTOR_SetSpeed(BaseSet.MOTOR_LEFT,speed->motorLeft*acceleration*acceleration);
    MOTOR_SetSpeed(BaseSet.MOTOR_RIGHT,speed->motorRight*acceleration*acceleration);
  }
  if(pulse->leftPast != pulse->left) {
    Serial.print("LEFT ");
    Serial.println(pulse->left);
  }
  if(pulse->rightPast != pulse->right) {
    Serial.print("RIGHT ");
    Serial.println(pulse->right);
  }
}


void stopMotors(){
  state->moving = 0;
  MOTOR_SetSpeed(BaseSet.MOTOR_LEFT,0);
  MOTOR_SetSpeed(BaseSet.MOTOR_RIGHT,0);
}

void detecteurProximite(){


  
	if(digitalRead(pin.capDroite)==HIGH){ //Détection à droite
      digitalWrite(pin.led_capDroite, HIGH); //Allumage du led droit
      state->detectRight = 1;
    } 
  else {
		digitalWrite(pin.led_capDroite, LOW);
    state->detectRight = 0;
	}

  
	if(digitalRead(pin.capGauche)==HIGH){ //Détection à gauche
    	digitalWrite(pin.led_capGauche, HIGH);//Allumage du led gauche
      state->detectLeft = 1;
    }
  else {
		digitalWrite(pin.led_capGauche, LOW);
    state->detectLeft = 0;
	}

}

Speed *initSpeed(){

  Speed *vitesse = (Speed*)malloc(sizeof(Speed));

  vitesse->motorLeft = BaseSet.speed_ini;
  vitesse->motorRight = BaseSet.speed_ini;

  return vitesse;
}

State *initState(){

  State *etat = (State*)malloc(sizeof(State));

  etat->coordinates[0] = 0;
  etat->coordinates[1] = 0;

  etat->detectLeft = 0;
  etat->detectRight = 0;

  etat->moving = 0;


  return etat;
}

Pulse *initPulse(){

  Pulse * pul = (Pulse*)malloc(sizeof(Pulse));

  pul->left = 0;
  pul->right = 0;
  


  return pul;

}

void loop() {
 
  
  Serial.println("XXX\n");
  readPulse();
  Serial.println("@@@\n");
  printState();
  
  
  while(state->moving == 0){
  
    /*if(detectFrequency()){
      motorsAccelerate();
      state->moving=1;
      break;
    }
    */
    
    
    if(ROBUS_IsBumper(REAR)){
      motorsAccelerate();
      Serial.print("Speed droit = ");
      Serial.println(speed->motorLeft);
      Serial.print("Speed droit = ");
      Serial.println(speed->motorRight);
      Serial.println("WWW\n");
      state->moving=1;
      break;
    }
    
  }
  
  detecteurProximite();
  
  
  if(state->detectLeft == 0 || state->detectRight == 0){
    stopMotors();
    state->moving=0;
  }
  
  else{
    Serial.println("AAA\n");
    foward(1);
  }
  
  
  
  delay(100);
  
  
  
   
}