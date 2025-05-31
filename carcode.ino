const int HORMOTOR_SPD = 100; //255 is max speed, 0 is no speed
const int PIN_HORMOTOR = A1; //Control pin for the horizontal motion motor. goint to pwm it.

const int USONIC_N_AV_FINE = 100;
const int USONIC_N_AV_COARSE = 5;
const double SSOUND = 330; //Speed of sound

const double HORSTEP = 0.2;
const double HOR_POS_END = 3;

const int PIN_IND_LEDP = 13;
const int PIN_STARTBTN = 10;

const struct u_sens{
  int trigpin;
  int echopin;
}
struct vert_sensor = {1,2};
struct hor_sensor = (3,4);
struct scan_sensor = 

struct hor_pos_strct{
  double current_pos;
  double step;
  double end;
} horpos = {0,HORSTEP,HOR_POS_END};

void go_to_horpos(double des_pos);
double measure_horpos(int n_av);
void do_vert_scan();
void record(int type, double value);
void indicate(int indicode);

void setup() {
  pinMode(PIN_IND_LEDP, OUTPUT);
  pinMode(PIN_STARTBTN, INPUT);
}

void loop(){

  //Waiting for the button to be pressed
  indicate(0)
  while (digitalRead(PIN_STARTBTN) == LOW){
    delay(100);
  }

  indicate(1);
  while (horpos.current_pos < horpos.end) {
    go_to_horpos(horpos.current_pos + horpos.step);
    horpos.current_pos = measure_horpos(HORPOS_N_AV);
    record(1,horpos.current_pos);
    do_vert_scan();
    indicate(2);
  }
  indicate(3);
}

void go_to_horpos(double des_pos){
  analogWrite(PIN_HORMOTOR,HORMOTOR_SPD);
}

double measure_horpos(int n_av){
  int a = 1;
}

void indicate(int indicode){
  //indicodes
  //0: ready to start
  //1: entering the main loop
  //2: finished a vertical scan
  //3 finished
  if (indicode == 1){
    for (int i = 1; i<=3;i ++){
      digitalWrite(IND_LEDPIN,HIGH);
      delay(500);
      digitalWrite(IND_LEDPIN,LOW);
    }
  } else if (indicode == 2){
    for (int i = 1; i<=4; i++){
      digitalWrite(IND_LEDPIN,HIGH);
      delay(1000);
      digitalWrite(IND_LEDPIN,LOW);
    }
  } else if (indicode ==3){
    for (int i = 1; i<=5; i++){
      digitalWrite(IND_LEDPIN,HIGH);
      delay(2000);
      digitalWrite(IND_LEDPIN,LOW);
    }
  } else if (indicode == 0){
    digitalWrite(IND_LEDPIN,HIGH);
  }
}

double measure_ultrasonic_sensor(int trigpin, int echopin, int n_av){
  double duration, distance, distance_av;
  distance_av = 0;
  for (int i = 0, i<n_av, i++){
    digitalWrite(trigpin,LOW);
    delayMicroseconds(2);
    digitalWrite(trigpin,HIGH);
    delayMicroseconds(10);
    digitalWrite(triPin,LOW);
    // https://projecthub.arduino.cc/Isaac100/getting-started-with-the-hc-sr04-ultrasonic-sensor-7cabe1
    //Suppose to send out a pulse, and then echo pin will be high for the microseoncds it took for the sound
    //to travel back and forth.
    duration = pulseIn(echiPin, HIGH);
    distance = duration/1000000*SSOUND/2;
    distance_av += distance;
  }
  distance_out = distance_av/n_av;
}