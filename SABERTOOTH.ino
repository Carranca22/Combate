/* Programacao Combate V2.2.4.ino
 * Código para controle de robô beetleweight
 * Autores: Isabella Galvão, Marcus Vinícius, Kewin Lima e Marina Collier.
 * 16/03/2015
 */
//Inclui biblioteca para acesso a funções matemáticas avançadas
#include <math.h> 
#include <Servo.h>

/*Define os pinos que os motores estarão conectados
essas variaveis irão se alterar de acordo com o ângulo em Z */
/*int MOTOR_E1 = 5;
int MOTOR_E2 = 6;
int MOTOR_D1 = 9;
int MOTOR_D2 = 10;*/
#define num_motor 2

//Caso 1 a saida será pela serial caso 0 será a resposta para os motores
#define DEBUG 0

// [ 1 ]- Imprime a saida aile e ele, [ 2 ] - imprime a resposta para os motores, [ 3 ] - imprime ambos
#define TIPO_DE_DEBUG 3

//Define o numero de interaões do filtro das entradas do controle
#define NUMERO_DE_INTERACOES 1

//Define o delay de espera para a vizualização na porta serial
#define DELAY 0

//Define as entradas de sinal do controle
#define PORTA_AILE A1
#define PORTA_ELE A0
#define PORTA_THR A4

//Constantes para leitura do controle (Devem ser calibradas)
int ENTRADA_MAX = 1894;
int ENTRADA_MIN = 1102;

//Constantes para saida para os motores
#define SAIDA_MAX 180
#define SAIDA_MIN 0
#define BASE_DE_MOVIMENTO 90

//Vão guardar as leituras vindas do controle
int aile_sinal = 0;
int ele_sinal = 0;
int ele_potencia = 0;
int aile_potencia = 0;
int thr_sinal=0;
int power_ele, power_aile;


Servo motores[num_motor];

void setup()
{
  motores[0].attach(5, ENTRADA_MIN, ENTRADA_MAX);
  motores[1].attach(9, ENTRADA_MIN, ENTRADA_MAX);
  // put your setup code here, to run once:
  pinMode(PORTA_AILE, INPUT);
  pinMode(PORTA_ELE, INPUT);
  pinMode(PORTA_THR, INPUT);
  Serial.begin(9600);
 // Função que inicializa o giroscopio e suas leituras.
  Serial.flush();
}
void loop(){
  thr_sinal = pulseIn(PORTA_THR, HIGH);
  if (thr_sinal >= 1800){
    ENTRADA_MAX = 1894;
    ENTRADA_MIN = 1102;
 }
 else if( thr_sinal <= 1320){
    ENTRADA_MIN = 1894;
    ENTRADA_MAX = 1102;
}
  locomocao();
  
}

int limitadorDePotencia(int potencia)
{
  if (abs(potencia) < SAIDA_MAX && abs(potencia) < SAIDA_MIN) return (potencia);
  else if (abs(potencia) > SAIDA_MAX) return (potencia / abs(potencia)) * SAIDA_MAX; // Limitando potencia maxima
  else if (abs(potencia) < SAIDA_MIN) return (potencia / abs(potencia)) * SAIDA_MIN; //Limitando potencia minima          
}
int filtro(int porta)
{
  unsigned long somador = 0;
  for (int n = 0; n < NUMERO_DE_INTERACOES; n++)
  somador += pulseIn(porta, HIGH);
  return (somador / NUMERO_DE_INTERACOES);
}
int potenciaPwm(int sinal)
{
  int potencia = constrain(map(sinal, ENTRADA_MIN, ENTRADA_MAX, SAIDA_MIN, SAIDA_MAX), SAIDA_MIN, SAIDA_MAX); //mapeando o sinal ( RESPOSTA LINEAR )
  return potencia;
}
void locomocao(){
aile_sinal = filtro(PORTA_AILE);
ele_sinal = filtro(PORTA_ELE);
ele_potencia = potenciaPwm(ele_sinal);
aile_potencia = potenciaPwm(aile_sinal);
power_ele = limitadorDePotencia(ele_potencia);
power_aile = limitadorDePotencia(aile_potencia);

motores[0].write(power_ele);
motores[1].write(power_aile);
}




