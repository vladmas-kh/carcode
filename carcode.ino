#include <Stepper.h>
#include <SPI.h>
#include <SD.h>

const double SSOUND = 330; //Speed of sound
const int SCAN_NAV  = 200; //Measurments to average for scan position
//Description of desired horizontal position range and speed
const double HORSTEP     = 0.2;
const double HOR_POS_END = 3;
const int HORPOS_N_AV    = 10;
const int HORPOS_N_AV_PR = 100;
const int HORMOTOR_SPD   = 100; //255 is max speed, 0 is no speed

//Description of desired vertical position range and speed
const double VERTSTEP     = 0.2;
const double VERT_POS_END = 3;
const int VERTPOS_N_AV    = 10;
const int VERTPOS_N_AV_PR = 100;
const long VERTMOTOR_SPD  = 20; //See stepper motor library for units

//Hardware pins assignment
const int PIN_IND_LEDP = 13; //Indicator LED
const int PIN_STARTBTN = 10; //Start cycle button
const int PIN_HORMOTOR = A1; //Control pin for the horizontal motion motor. goint to pwm it.

struct u_sens{
  int trigpin;
  int echopin;
};
const struct u_sens VERT_SENSOR = {1,2};
const struct u_sens HOR_SENSOR  = {3,4};
const struct u_sens SCAN_SENSOR = {5,6};

//Logging paramenters
File logfile;

Stepper vert_scan_stepper(1, 8, 9, 10, 11); //Vertical scan stepper motor

struct pos_strct{
  double current_pos;
  double step;
  double end;
};
struct pos_strct horpos  = {0,HORSTEP , HOR_POS_END};
struct pos_strct vertpos = {0,VERTSTEP,VERT_POS_END};



void setup() {
  pinMode(PIN_IND_LEDP, OUTPUT);
  pinMode(PIN_STARTBTN, INPUT);
}


double scan_distance = 0;
void loop(){

  //Waiting for the button to be pressed
  indicate(0);
  while (digitalRead(PIN_STARTBTN) == LOW){
    delay(100);
  }

  indicate(1);
  while (horpos.current_pos < horpos.end) {
    horpos.current_pos = increase_horpos_step(horpos);
    indicate(2);
    while(vertpos.current_pos < vertpos.end){
      vertpos.current_pos = increase_verpos_step(vertpos);
      scan_distance = measure_ultrasonic_sensor(SCAN_SENSOR.trigpin,SCAN_SENSOR.echopin,SCAN_NAV);
    }
  }
  indicate(3);
}

double increase_horpos_step(struct pos_strct horpos){
  //Increases current position by a step, described in horpos.
  //If the position is larger than the maximum, does nothing.
  //Returns actual horizontal position
  double act_position = 0;
  double desired_pos = horpos.current_pos + horpos.step;
  if (desired_pos < horpos.end){
    analogWrite(PIN_HORMOTOR,HORMOTOR_SPD);
    act_position = measure_ultrasonic_sensor(HOR_SENSOR.trigpin,HOR_SENSOR.echopin,HORPOS_N_AV);
    while (act_position < desired_pos){
      act_position = measure_ultrasonic_sensor(HOR_SENSOR.trigpin,HOR_SENSOR.echopin,HORPOS_N_AV);
    }
  }
  //Stop the motor. Record actual position, with more datapoints.
  analogWrite(PIN_HORMOTOR,0);
  act_position = measure_ultrasonic_sensor(HOR_SENSOR.trigpin,HOR_SENSOR.echopin,HORPOS_N_AV_PR);
  return act_position;
}

double increase_verpos_step(struct pos_strct vertpos ){
  //Same thing as with horizontal position, but using a stepper motor set to constant speed now.
  //Increases current position by a step, described in horpos.
  //If the position is larger than the maximum, does nothing.
  //Returns actual vertical position
  double act_position = 0;
  double desired_pos = vertpos.current_pos + vertpos.step;
  if (desired_pos < vertpos.end){
    vert_scan_stepper.setSpeed(VERTMOTOR_SPD);
    act_position = measure_ultrasonic_sensor(VERT_SENSOR.trigpin,VERT_SENSOR.echopin,VERTPOS_N_AV);
    while (act_position < desired_pos){
      act_position = measure_ultrasonic_sensor(VERT_SENSOR.trigpin,VERT_SENSOR.echopin,VERTPOS_N_AV);
    }
  }
  //Stop the motor. Record actual position, with more datapoints.
  vert_scan_stepper.setSpeed(0);
  act_position = measure_ultrasonic_sensor(VERT_SENSOR.trigpin,HOR_SENSOR.echopin,VERTPOS_N_AV_PR);
  return act_position;
}

void indicate(int indicode){
  //indicodes
  //0: ready to start
  //1: entering the main loop
  //2: finished a vertical scan
  //3 finished
  if (indicode == 1){
    for (int i = 1; i<=3;i ++){
      digitalWrite(PIN_IND_LEDP,HIGH);
      delay(500);
      digitalWrite(PIN_IND_LEDP,LOW);
    }
  } else if (indicode == 2){
    for (int i = 1; i<=4; i++){
      digitalWrite(PIN_IND_LEDP,HIGH);
      delay(1000);
      digitalWrite(PIN_IND_LEDP,LOW);
    }
  } else if (indicode ==3){
    for (int i = 1; i<=5; i++){
      digitalWrite(PIN_IND_LEDP,HIGH);
      delay(2000);
      digitalWrite(PIN_IND_LEDP,LOW);
    }
  } else if (indicode == 0){
    digitalWrite(PIN_IND_LEDP,HIGH);
  }
}

double measure_ultrasonic_sensor(int trigpin, int echopin, int n_av){
  double duration, distance, distance_av, distance_out;
  distance_av = 0;
  for (int i = 0; i<n_av; i++){
    digitalWrite(trigpin,LOW);
    delayMicroseconds(2);
    digitalWrite(trigpin,HIGH);
    delayMicroseconds(10);
    digitalWrite(trigpin,LOW);
    // https://projecthub.arduino.cc/Isaac100/getting-started-with-the-hc-sr04-ultrasonic-sensor-7cabe1
    //Suppose to send out a pulse, and then echo pin will be high for the microseoncds it took for the sound
    //to travel back and forth.
    duration = pulseIn(echopin, HIGH);
    distance = duration/1000000*SSOUND/2;
    distance_av += distance;
  }
  distance_out = distance_av/n_av;
  return distance_out;
}