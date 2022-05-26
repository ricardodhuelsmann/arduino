// Desenvolvido por Ricardo Dagnoni Huelsmann
#include <SpeedyStepper.h>                    


const int MOTOR_X_STEP_PIN = 2;
const int MOTOR_Y_STEP_PIN = 3;
const int MOTOR_Z_STEP_PIN = 4;
const int MOTOR_X_DIR_PIN = 5;
const int MOTOR_Y_DIR_PIN = 6;
const int MOTOR_Z_DIR_PIN = 7;
const int STEPPERS_ENABLE_PIN = 8;
const int LIMIT_SWITCH_X_PIN = 9;
const int LIMIT_SWITCH_Y_PIN = 10;
const int LIMIT_SWITCH_Z_PIN = 11;


SpeedyStepper stepperX;                         
SpeedyStepper stepperY;
SpeedyStepper stepperZ;

long int diferenca_tempo = 0;
unsigned long tempo_1 = 0;
unsigned long tempo_2 = 0;
int num_ext;
int num_des;
int conf = 1;
long int trajeto_volta;
long int movimento_x = 533*15;
long int movimento_y = 533*20;
long int movimento_z = 533*42;
long int movimento_y_curto = 533*12;
long int movimento_z_curto = 533*15;
long int movimento_y_restante = movimento_y - movimento_y_curto;
long int movimento_z_restante = movimento_z - movimento_z_curto;
int trajeto_x = 533;
int trajeto_y = 533;
int trajeto_z = 533;


void setup() {
  Serial.begin(9600);
  pinMode(STEPPERS_ENABLE_PIN, OUTPUT);
  pinMode(LIMIT_SWITCH_X_PIN, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH_Y_PIN, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH_Z_PIN, INPUT_PULLUP);
  stepperX.connectToPins(MOTOR_X_STEP_PIN, MOTOR_X_DIR_PIN);
  stepperY.connectToPins(MOTOR_Y_STEP_PIN, MOTOR_Y_DIR_PIN);
  stepperZ.connectToPins(MOTOR_Z_STEP_PIN, MOTOR_Z_DIR_PIN);
  digitalWrite(STEPPERS_ENABLE_PIN, HIGH);
  stepperX.setSpeedInStepsPerSecond(1350);                        
  stepperX.setAccelerationInStepsPerSecondPerSecond(400);
  stepperY.setSpeedInStepsPerSecond(500);                          
  stepperY.setAccelerationInStepsPerSecondPerSecond(100);
  stepperZ.setSpeedInStepsPerSecond(1350);
  stepperZ.setAccelerationInStepsPerSecondPerSecond(400);
}


void mover_x(long int x) {
  stepperX.moveRelativeInSteps(x);
  delay(100);
}


void mover_y(long int y) {
  stepperY.moveRelativeInSteps(y);
  delay(100);
}


void mover_z(long int z) {  
  stepperZ.moveRelativeInSteps(z);
  delay(100);
}

  
void extrair(int num_ext) {
  Serial.println("Executando extração... \n");
  for(int n = 0; n < num_ext; n++) {
     mover_x(-movimento_x); 
     mover_z(-movimento_z);
     mover_y(movimento_y_curto);
     delay(2500);
     mover_z(movimento_z_curto);
     mover_y(movimento_y_restante);
     delay(1000);
     mover_y(-movimento_y);
     mover_z(movimento_z_restante);
   }
}


void dessorver(int num_des) {
  Serial.println("Executando dessorção... \n");
  mover_x(-movimento_x);
  mover_z(-movimento_z);
  for(int n = 0; n < num_des; n++) {
     mover_y(movimento_y_curto);
     delay(2500);
     mover_z(movimento_z_curto);
     mover_y(movimento_y_restante);
     delay(1000);
     mover_y(-movimento_y);
     if (n < (num_des-1)) {
        mover_z(-movimento_z_curto);
     } else {
        mover_z(movimento_z_restante);
     }
  }
}

void inicia_tempo() {
   tempo_1 = millis()/1000;
   digitalWrite(STEPPERS_ENABLE_PIN, LOW);
   delay(50);
}


void termina_tempo() {
  digitalWrite(STEPPERS_ENABLE_PIN, HIGH);
  delay(50);
  tempo_2 = millis()/1000;
  diferenca_tempo = tempo_2 - tempo_1;
  int minutos = (diferenca_tempo/60)%60;
  int segundos = diferenca_tempo%60 + 1;
  Serial.print("O tempo total aproximado foi: ");
  Serial.print(minutos); Serial.print(" min e ");
  Serial.print(segundos); Serial.println(" segundos.");
}

  
void executa (int num_ext, int num_des) {
  extrair(num_ext);
  dessorver(num_des);
  trajeto_volta = (num_ext+1)*movimento_x;
  Serial.println("Voltando à posição de origem \n");
  mover_x(trajeto_volta);
}
                                            
  
void pergunta1() {
  Serial.println("\nPor favor, indique o número de ciclos de extração desejado:");
  while (Serial.available() == 0) {}
  num_ext = Serial.parseInt();
  Serial.print("Você selecionou: ");
  Serial.println(num_ext);
  delay(500);
}


void pergunta2() {
  Serial.println("\nAgora, indique o número de ciclos de dessorção desejado:");
  while (Serial.available() == 0) {}
  num_des = Serial.parseInt();
  Serial.print("Você selecionou: ");
  Serial.println(num_des); 
  delay(500);
}


void confirmacao() {
  if (num_ext>9 || num_ext<1) {
    Serial.println("Você escolheu um número não permitido de ciclos de extração. Escolha um valor de 1 a 9. \n");
    principal();
  } 
  if (num_des>9 || num_des<1) {
    Serial.println("Você escolheu um número não permitido de ciclos de dessorção. Escolha um valor de 1 a 9. \n");
    principal();
  } 
  else {
    Serial.print("Você selecionou ");
    Serial.print(num_ext); Serial.print(" ciclo(s) de extração(ões) e ");
    Serial.print(num_des); 
    Serial.println(" ciclo(s) de dessorção(ões). \n\nPara corrigir digite 0. Para iniciar digite 1 \n");
    while (Serial.available() == 0) {}
    conf = Serial.parseInt();
    if (conf>1 || conf<0) {
      Serial.println("Você escolheu um número não permitido. Favor inserir 1 para confirmar e 0 para cancelar: \n");
      while (Serial.available() == 0) {}
      conf = Serial.parseInt();
      confirmacao();
    }
  }
}


void zerar_posicao(){
  const long directionTowardHome = 1;
  const float speedInStepsPerSecond = 100;
  const long maxDistanceToMoveInSteps = 9999999;
  stepperZ.moveToHomeInSteps(directionTowardHome, 15*speedInStepsPerSecond, maxDistanceToMoveInSteps, LIMIT_SWITCH_Z_PIN);
  Serial.println("Posição Z verificada.\n");
  delay(100);
  stepperY.moveToHomeInSteps(-directionTowardHome, 8*speedInStepsPerSecond, maxDistanceToMoveInSteps, LIMIT_SWITCH_Y_PIN);
  Serial.println("Posição Y verificada.\n");
  delay(100);
  stepperX.moveToHomeInSteps(directionTowardHome, 20*speedInStepsPerSecond, maxDistanceToMoveInSteps, LIMIT_SWITCH_X_PIN);
  Serial.println("Posição X verificada.\n");
  delay(100);
  mover_x(-500);
  mover_y(3*533);
  mover_z(-13*533);
} 

    
void principal() {
  Serial.println("Olá, bem-vindo!");
  delay(1000);
  if (conf == 1) {
    pergunta1();
    delay(250);
    pergunta2();
    delay(250);
    confirmacao();
    if (conf == 0) {
      Serial.println("\nOK! Vamos começar novamente. \n");
      delay(2500);
      conf = 1;
      principal();
    } 
    if (conf == 1) {
      Serial.println("Iniciando o procedimento... \n");       
      delay(500);
      Serial.println("Fazendo verificação do aparelho...\n");
      inicia_tempo();
      zerar_posicao();
      executa(num_ext, num_des);
      termina_tempo(); 
      Serial.println("\nObrigado por utilizar este programa!");
      Serial.end();
    }
  }
}

  
void loop(){
  principal();}
