/* Programacao Combate V2.2.4.ino
 * Código para controle de robô beetleweight
 * Autores: Isabella Galvão, Marcus Vinícius e Kewin Lima.
 * 16/03/2015
 */
//Inclui biblioteca para acesso a funções matemáticas avançadas
#include <math.h> 
/*Define os pinos que os motores estarão conectados
essas variaveis irão se alterar de acordo com o ângulo em Z */
int MOTOR_E1 = 5;
int MOTOR_E2 = 6;
int MOTOR_D1 = 9;
int MOTOR_D2 = 10;

//Caso 1 a saida será pela serial caso 0 será a resposta para os motores
#define DEBUG 1

// [ 1 ]- Imprime a saida aile e ele, [ 2 ] - imprime a resposta para os motores, [ 3 ] - imprime ambos
#define TIPO_DE_DEBUG 2

//Define o numero de interaões do filtro das entradas do controle
#define NUMERO_DE_INTERACOES 1

//Define o delay de espera para a vizualização na porta serial
#define DELAY 0

//Define as entradas de sinal do controle
#define PORTA_AILE A0
#define PORTA_ELE A1

//Constantes para leitura do controle (Devem ser calibradas)
#define ENTRADA_MAX 1894
#define ENTRADA_MIN 1102

//Constantes para saida para os motores
#define SAIDA_MAX 255
#define SAIDA_MIN -255

//Vão guardar as leituras vindas do cont
void setup()
{
  // put your setup code here, to run once:
  pinMode(PORTA_AILE, INPUT);
  pinMode(PORTA_ELE, INPUT);

  pinMode(MOTOR_E1, OUTPUT);
  pinMode(MOTOR_E2, OUTPUT);
  pinMode(MOTOR_D1, OUTPUT);
  pinMode(MOTOR_D2, OUTPUT);
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
 
}
