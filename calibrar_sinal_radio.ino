#define DELAY 0
//Define as entradas de sinal do controle
#define PORTA_AILE A0
#define PORTA_ELE A1

//Vão guardar as leituras vindas do cont
void setup()
{
  // put your setup code here, to run once:
  pinMode(PORTA_AILE, INPUT);
  pinMode(PORTA_ELE, INPUT);
  Serial.begin(9600);
  Serial.flush();
}
void loop(){
 aile_sinal=pulseIn(PORTA_AILE,HIGH);
 ele_sinal =pulseIn(PORTA_ELE,HIGH);
 Serial.print("Aile : ");
 Serial.println(aile_sinal);
 Serial.print("Ele : ");
 Serial.println(ele_sinal);
 delay(DELAY);
}
