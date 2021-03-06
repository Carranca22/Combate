/************* PROGRAMAÇÃO DESENVOLVIDA PARA A WINTER CHALLENGE 2017***********
*****************PELA EQUIPE DE ROBÓTICA CARRANCA****************
* Código para controle dos robôs de combate com inversão manual e sem comunicação com sabertooth. *********
* OBS: Funções de inversão com o GIROSCÓPIO comentadas por motivos de busca por um dispositivo mais resistente a impactos. **
*******************************************
* Autores: Isabella Galvão, Marcus Vinícius, Kewin Lima******************
*Última alteração em: 13/06/17 ********************************
******************************************/
//Inclui biblioteca para acesso a funções matemáticas avançadas
#include <math.h> 

/*Define os pinos que os motores estarão conectados
essas variaveis irão se alterar de acordo com o ângulo em Z */
int MOTOR_E1 = 5;
int MOTOR_E2 = 6;
int MOTOR_D1 = 9;
int MOTOR_D2 = 10;

//Define os sinais de thr para a condição de inversão de movimento ALTERAR APÓS LEITURA DE CANAL NO CONTROLE
#define NORMAL 0
#define INVERTE 0


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
#define PORTA_CH5 A4

//Constantes para leitura do controle (Devem ser calibradas)
#define ENTRADA_MAX 1894
#define ENTRADA_MIN 1102

//Constantes para saida para os motores
#define SAIDA_MAX 255
#define SAIDA_MIN -255

//Vão guardar as leituras vindas do controle
int aile_sinal = 0;
int ele_sinal = 0;
int ele_potencia = 0;
int aile_potencia = 0;
int thr_sinal = 0;
int ch5_sinal = 0;
bool thr_bin=1;

//limiar aile ele
#define LIMIAR_MAX_AILE 70
#define LIMIAR_MIN_AILE - 70
#define LIMIAR_MAX_ELE 70
#define LIMIAR_MIN_ELE - 70

void setup()
{
  // put your setup code here, to run once:
  pinMode(PORTA_AILE, INPUT);
  pinMode(PORTA_ELE, INPUT);
  pinMode(PORTA_THR, INPUT);

  pinMode(MOTOR_E1, OUTPUT);
  pinMode(MOTOR_E2, OUTPUT);
  pinMode(MOTOR_D1, OUTPUT);
  pinMode(MOTOR_D2, OUTPUT);
  Serial.begin(9600);
 // Função que inicializa o giroscopio e suas leituras.
  Serial.flush();
}

void loop()
{
   geral();
   //virgulino();
   locomocao();
}

void locomocao(){
  //leitura dos canais do controle
  aile_sinal = filtro(PORTA_AILE);
  ele_sinal = filtro(PORTA_ELE);
  //constrain(aile_sinal, SAIDA_MIN, SAIDA_MAX);
  //constrain(ele_sinal, SAIDA_MIN, SAIDA_MAX);
  //Condicao para que o controle esteja em comunicacao com o receptor
  if (aile_sinal != 0 && ele_sinal != 0)
  {
    ele_potencia = potenciaPwm(ele_sinal);
    aile_potencia = potenciaPwm(aile_sinal);

    //codicao parado
    if (((aile_potencia >= LIMIAR_MIN_AILE) && (aile_potencia <= LIMIAR_MAX_AILE)) && ((ele_potencia >= LIMIAR_MIN_ELE) && (ele_potencia <= LIMIAR_MAX_ELE)))
    {
      movimentacao(0, 0);
      digitalWrite(13,LOW);
      imprimirDebug(0, 0, "PARADO");
    }
    //condição para frente
    else if (((aile_potencia > LIMIAR_MIN_AILE && aile_potencia < LIMIAR_MAX_AILE) && ele_potencia > LIMIAR_MAX_ELE))
    {
      movimentacao(ele_potencia, ele_potencia);
      imprimirDebug(ele_potencia, ele_potencia, "FRENTE");
    }
    //condição para trás
    else if (((aile_potencia > LIMIAR_MIN_AILE && aile_potencia < LIMIAR_MAX_AILE) && ele_potencia < LIMIAR_MIN_ELE))
    {
      movimentacao(ele_potencia, ele_potencia);
      imprimirDebug(ele_potencia, ele_potencia, "TRAS");
    }
    //condição para direita
    else if ((ele_potencia > LIMIAR_MIN_ELE && ele_potencia < LIMIAR_MAX_ELE) && (aile_potencia > LIMIAR_MAX_AILE))
    {
      movimentacao(aile_potencia, -aile_potencia);
      imprimirDebug(aile_potencia, -aile_potencia, "DIREITA");
    }
    //condição para esquerda
    else if (((ele_potencia > LIMIAR_MIN_ELE && ele_potencia < LIMIAR_MAX_ELE) && aile_potencia < LIMIAR_MIN_AILE))
    {
      movimentacao(aile_potencia, -aile_potencia);
      imprimirDebug(aile_potencia, -aile_potencia, "ESQUERDA");
    }
    //diagonal frente direita
    else if (ele_potencia > LIMIAR_MAX_ELE && aile_potencia > LIMIAR_MAX_ELE)
    {
      movimentacao(aile_potencia, (ele_potencia - aile_potencia));
      imprimirDebug(aile_potencia, ele_potencia - aile_potencia, "FRENTE DIREITA");
    }
    //diagonal frente esquerda
    else if (ele_potencia > LIMIAR_MAX_ELE && aile_potencia < LIMIAR_MIN_ELE)
    {
      movimentacao(aile_potencia + ele_potencia, abs(aile_potencia));
      imprimirDebug(aile_potencia + ele_potencia, abs(aile_potencia), "FRENTE ESQUERDA");
    }
    //diagonal tras direita
    else if ((ele_potencia < LIMIAR_MIN_ELE && aile_potencia > LIMIAR_MAX_ELE))
    {
      movimentacao(-aile_potencia, aile_potencia + ele_potencia);
      imprimirDebug(-aile_potencia, aile_potencia + ele_potencia, "TRAS DIREITA");
    }
    //diagonal tras esquerda
    else if ((ele_potencia < LIMIAR_MIN_ELE && aile_potencia < LIMIAR_MIN_ELE))
    {
      movimentacao(abs(aile_potencia) - abs(ele_potencia), aile_potencia);
      imprimirDebug(abs(aile_potencia) - abs(ele_potencia), aile_potencia, "TRAS ESQUERDA");
    }
  }
  //CONDIÇÃO PARADO PARA CONTROLE DESLIGADO
  else if ((aile_sinal == 0) || (ele_sinal == 0))
  {
    movimentacao(0, 0);
    imprimirDebug(0, 0, "TRAVADO");
  }
}

//Função que cuida da movimentação dos motores
void movimentacao(int potenciaEsquerda, int potenciaDireita)
{
  if (potenciaEsquerda == 0 && potenciaDireita == 0)
  {
    potenciaEsquerda = 0;
    potenciaDireita = 0;
  }
  else
  {
       potenciaEsquerda = limitadorDePotencia(potenciaEsquerda);
       potenciaDireita = limitadorDePotencia(potenciaDireita);
  }

  if (potenciaEsquerda >= ((SAIDA_MAX + SAIDA_MIN) / 2) && potenciaDireita >= ((SAIDA_MAX + SAIDA_MIN) / 2)) // codição para os dois motores girarem para frente
  {
    analogWrite(MOTOR_E1, abs(potenciaEsquerda));
    analogWrite(MOTOR_E2, 0);
    analogWrite(MOTOR_D1, abs(potenciaDireita));
    analogWrite(MOTOR_D2, 0);
  }
  else if (potenciaEsquerda <= ((SAIDA_MAX + SAIDA_MIN) / 2) && potenciaDireita <= ((SAIDA_MAX + SAIDA_MIN) / 2)) // condição para os dois motores girarem para tras 
  {
    analogWrite(MOTOR_E1, 0);
    analogWrite(MOTOR_E2, abs(potenciaEsquerda));
    analogWrite(MOTOR_D1, 0);
    analogWrite(MOTOR_D2, abs(potenciaDireita));
  }
  else if (potenciaEsquerda <= ((SAIDA_MAX + SAIDA_MIN) / 2) && potenciaDireita >= ((SAIDA_MAX + SAIDA_MIN) / 2)) // condição motor direita frente e esquerda tras
  {
    analogWrite(MOTOR_E1, 0);
    analogWrite(MOTOR_E2, abs(potenciaEsquerda));
    analogWrite(MOTOR_D1, abs(potenciaDireita));
    analogWrite(MOTOR_D2, 0);
  }
  else if (potenciaEsquerda >= ((SAIDA_MAX + SAIDA_MIN) / 2) && potenciaDireita <= ((SAIDA_MAX + SAIDA_MIN) / 2)) // condição motor direita tras e esquerda frente
  {
    analogWrite(MOTOR_E1, abs(potenciaEsquerda));
    analogWrite(MOTOR_E2, 0);
    analogWrite(MOTOR_D1, 0);
    analogWrite(MOTOR_D2, abs(potenciaDireita));
  }
  else if (potenciaEsquerda == 0 && potenciaDireita == 0)
  {
    analogWrite(MOTOR_E1, 0);
    analogWrite(MOTOR_E2, 0);
    analogWrite(MOTOR_D1, 0);
    analogWrite(MOTOR_D2, 0);
  }
}

//Função que mapeia os valores e limita a potência
int potenciaPwm(int sinal)
{
  int potencia = constrain(map(sinal, ENTRADA_MIN, ENTRADA_MAX, SAIDA_MIN, SAIDA_MAX), SAIDA_MIN, SAIDA_MAX); //mapeando o sinal ( RESPOSTA LINEAR )
  return potencia;
}

//Limitador de Potencia
int limitadorDePotencia(int potencia)
{
  if (abs(potencia) < SAIDA_MAX && abs(potencia) < SAIDA_MIN) return (potencia);
  else if (abs(potencia) > SAIDA_MAX) return (potencia / abs(potencia)) * SAIDA_MAX; // Limitando potencia maxima
  else if (abs(potencia) < SAIDA_MIN) return (potencia / abs(potencia)) * SAIDA_MIN; //Limitando potencia minima          
}

//Filtra os valores brutos recebidos do controle
int filtro(int porta)
{
  unsigned long somador = 0;
  for (int n = 0; n < NUMERO_DE_INTERACOES; n++)
  somador += pulseIn(porta, HIGH);
  return (somador / NUMERO_DE_INTERACOES);
}
void geral () {
  thr_sinal = pulseIn(PORTA_THR, HIGH); // FUNCIONA COM O CANAL RUD TAMBÉM
  if (thr_sinal >= 1800){
    MOTOR_E1 = 5;
    MOTOR_E2 = 6;
    MOTOR_D1 = 9;
    MOTOR_D2 = 10;
 }
 else if( thr_sinal <= 1320){
    MOTOR_E1 = 6;
    MOTOR_E2 = 5;
    MOTOR_D1 = 10;
    MOTOR_D2 = 9;
}
}
void virgulino ()
{
  ch5_sinal = pulseIn(PORTA_CH5, HIGH);
  //NORMAL
  if (ch5_sinal >= 1400){
    MOTOR_E1 = 5;
    MOTOR_E2 = 6;
    MOTOR_D1 = 9;
    MOTOR_D2 = 10;
 }
  //INVERTE
 else if( ch5_sinal < 1400){
    MOTOR_E1 = 6;
    MOTOR_E2 = 5;
    MOTOR_D1 = 10;
    MOTOR_D2 = 9;
 }
}  
    

//Imprime na serial caso o DEBUG seja verdadeiro
void imprimirDebug(int potenciaEsquerdo, int potenciaDireito, const char direcao[25])
{
  if (DEBUG)
  {
    if (TIPO_DE_DEBUG == 1)
    {
      Serial.print(" AILE : ");
      Serial.print(aile_sinal);
      Serial.print(" | ELE : ");
      Serial.print(ele_sinal);
      Serial.print(" | ");
      Serial.println(direcao);
      delay(DELAY);
    }
    else if (TIPO_DE_DEBUG == 2)
    {
      Serial.print(" Esquerdo : ");
      Serial.print(potenciaEsquerdo);
      Serial.print(" | Direito : ");
      Serial.print(potenciaDireito);
      Serial.print(" | ");
      Serial.println(direcao);
      delay(DELAY);
    }
    else if (TIPO_DE_DEBUG == 3)
    {
      Serial.print(" AILE : ");
      Serial.print(aile_sinal);
      Serial.print(" | ELE : ");
      Serial.print(ele_sinal);
      Serial.print(" | Esquerdo : ");
      Serial.print(potenciaEsquerdo);
      Serial.print(" | Direito : ");
      Serial.print(potenciaDireito);
      Serial.print(" | ");
      Serial.println(direcao);
      delay(DELAY);
    }
  }
}
