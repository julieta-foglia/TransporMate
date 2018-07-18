#include <SoftwareSerial.h>
#include <variables.h>


int IN3 = 3;  //MOTOR A Rueda Derecha
int IN4 = 4;  //MOTOR A Rueda Derecha
int ENA = 5;  //PWM un motor DERECHO

int IN1 = 2;  //MOTOR B Rueda izquierda
int IN2 = 7;  //MOTOR B Rueda izquierda
int ENB = 6;  //PWM el otro motor IZQUIERDO

int TX = 1; //Tx arduino
int RX = 0; //Rx arduino
int LED = 8; //Luz LED
int PIN12 = 12;
int PIN13 = 13;
int DELAY_5 = 5;
int DELAY_10 = 10;

char Comando = 'z';
char ComandoAnt = 'z';

//Declaramos los pines que vamos a usar para el sensor pir
int analogPin = A5;   //Analogico A5 para el sensor

//Pin para Switch Reed
const int pinSensor = 11;

//Declaramos una variable para almacenar el valor que nos devuelve el sensor pir
int valPir = 0;

//Declaro variable para almacenar valor que devuelve el switch reed
int valSwitch;

//Declaramos variables a utilizar con el sensor de proximidad
long distancia;
long tiempo;

SoftwareSerial BT1(RX, TX); // RX, TX

void setup() {
  pinMode (IN4, OUTPUT);    // Input4 conectada al pin 7
  pinMode (IN3, OUTPUT);    // Input3 conectada al pin 2

  pinMode (IN1, OUTPUT);    // Input1 conectada al pin 4
  pinMode (IN2, OUTPUT);    // Input2 conectada al pin 3
  
  Serial.begin(BT_VELOCIDAD);
  
  pinMode(PIN12, OUTPUT); /*activación del pin 9 como salida: para el pulso ultrasónico*/
  pinMode(PIN13, INPUT); /*activación del pin 8 como entrada: tiempo del rebote del ultrasonido*/

  pinMode(TX, OUTPUT); //Tx arduino envia datos
  pinMode(RX, INPUT); //Rx arduino recibe datos

  pinMode(pinSensor, INPUT_PULLUP); /* Activacion switch */

  pinMode(ENA, OUTPUT); /* Activación del pin ENA como salida. */ 
  pinMode(ENB, OUTPUT);  /* Activación del pin ENB como entrada. */
  pinMode(LED, OUTPUT); /*Activacion del pin LED */

  // digitalWrite(TX,LOW); //Activar bluetooth

  BT1.begin(BT_VELOCIDAD);

  Serial.begin(BT_VELOCIDAD);         //Iniciamos la comunicacion serial
  pinMode(analogPin, INPUT);  //Configuramos el pin analogico A5 como entrada

}

void loop() {

  valPir = digitalRead(analogPin);    //Leemos el valor analogico que nos devuelve el sensor
  valSwitch = digitalRead(pinSensor); //Leemos el valor del sensor reed switch (LOW, activado)

  if(valPir == VALOR_0)
    Serial.println(CARACTER_T);
  
  // SI SE ENCUENTRA EL MATE APOYADO SE DEBE ENCENDER EL LED, Y DEPENDIENDO DE ESO, HABILITAR O DESHABILITAR LOS CONTROLES.
  if (valSwitch == LOW) {
    encenderLed();
    //habilitar controles de avanzar/retroceder y enviar mensaje de temperatura
    Serial.println(CARACTER_HM);
  }
  else {
    apagarLed(); // Descomentar cuando arreglemos el tema del iman-calor //
      //bloquear controles
      Serial.println(CARACTER_D);
  }

  Comando = leerBluetooth(); //LEEMOS LO QUE NOS ENVIA LA APLICACION ANDROID
  if( Comando == CARACTER_Z )
  {
    Comando = ComandoAnt;
  }
  else
  {
    ComandoAnt = Comando;
  }

  //VERIFICAMOS SI NO SE ACTIVO LA FUNCION DE LOS SENSORES DEL CELULAR, EN CASO DE QUE SE TAPE EL SENSOR DE PROXIMIDAD, SE ENCENDERÁ EL LED
  if(Comando == CARACTER_X )
  {
    encenderLed();
  }
  else
  {
    if(Comando == CARACTER_Y)
    {
      apagarLed();
    }
  }

  sensorProximidad(); //Obtenemos la distancia del auto a el objeto mas cercano por delante
  if (distancia > VALOR_DISTANCIA && Comando != CARACTER_Y && Comando != CARACTER_X)
  {
      switch (Comando) {
        //MOTOR IZQUIERDA
        case CARACTER_A: //a retrocede motor izq 100%
            analogWrite(ENB, VALOR_255);
            RetrocedeMotoresIzquierda();
            break;

        case CARACTER_B: //b retrocede motor izq 75%
            analogWrite(ENB, VALOR_191);
            RetrocedeMotoresIzquierda();
            break;
          
        case CARACTER_C: //c retrocede motor izq 50%
            analogWrite(ENB, VALOR_126);
            RetrocedeMotoresIzquierda();
            break;
          
        case CARACTER_E: //e detener motor izq
            analogWrite(ENB, VALOR_0);
            DetenerMotoresIzquierda();
            break;
          
        case CARACTER_G: //g avanza motor izq 50%
            analogWrite(ENB, VALOR_126);
            AvanzaMotoresIzquierda();
            break;
          
        case CARACTER_H: //h avanza motor izq 75%
            analogWrite(ENB, VALOR_191);
            AvanzaMotoresIzquierda();
            break;
          
        case CARACTER_I: //i avanza motor izq 100%
            analogWrite(ENB, VALOR_255);
            AvanzaMotoresIzquierda();
            break;

         //MOTOR DERECHA
        case CARACTER_J: //j retrocede motor derecha 100%
            analogWrite(ENA, VALOR_255);
            RetrocedeMotoresDerecha();
            break;
          
        case CARACTER_K: //k retrocede motor derecha 75%
            analogWrite(ENA, VALOR_191);
            RetrocedeMotoresDerecha();
            break;
          
        case CARACTER_L: //l retrocede motor derecha 50%
            analogWrite(ENA, VALOR_126);
            RetrocedeMotoresDerecha();
            break;
          
        case CARACTER_N: //n detener motor derecho
            analogWrite(ENA, VALOR_0);
            DetenerMotoresDerecha();
            break;
          
        case CARACTER_P: //p avanza motor derecha 50%
            analogWrite(ENA, VALOR_126);
            AvanzaMotoresDerecha();
            break;
          
        case CARACTER_Q: //q avanza motor derecha 75%
            analogWrite(ENA, VALOR_191);
            AvanzaMotoresDerecha();
            break;
          
        case CARACTER_R: //r avanza motor derecha 100%
            analogWrite(ENA, VALOR_255);
            AvanzaMotoresDerecha();
            break;

        //SENSORES DEL CELULAR
        case CARACTER_U: //u para los dos motores. Lo hace cuando le tapamos el sensor de luz al celular.
          pararMotores();
          break;

        case CARACTER_V: //v pone a ambos motores en sentido retroceso y a la máxima velocidad.
          RetrocederMotores();
          break;

        case CARACTER_W: //w pone a ambos motores en sentido avance y a la máxima velocidad.
          AvanzaMotores();
          break;     
      }

  }
  else //Permitimos que en caso de que el carro se pare por detectar algo delante, se pueda retroceder o realizar las funciones con los sensores del celular.
  {
     switch(Comando)
      {
         case 'a': 
            analogWrite(ENB, VALOR_255);
            RetrocedeMotoresIzquierda();
            break;
          
        case 'b': 
            analogWrite(ENB, VALOR_191);
            RetrocedeMotoresIzquierda();
            break;
          
        case 'c': 
            analogWrite(ENB, VALOR_126);
            RetrocedeMotoresIzquierda();
            break;

        case 'j' :
            analogWrite(ENA, VALOR_255);
            RetrocedeMotoresDerecha();
            break;
          
        case 'k' :
            analogWrite(ENA, VALOR_191);
            RetrocedeMotoresDerecha();
            break;
          
        case 'l' :
            analogWrite(ENA, VALOR_126);
            RetrocedeMotoresDerecha();
            break;

        case 'u' :
          pararMotores();
          break;

        case 'v' :
          RetrocederMotores();
          break;
      }
      
      pararMotores(); 
  }
}

//FUNCIONES A UTILIZAR

char leerBluetooth() {
  if (BT1.available())
  { 
    char letra = BT1.read();
    BT1.flush();
    return letra;
  }
  return CARACTER_Z; //Valor por si no se lee nada.
}

void encenderLed() {
  digitalWrite(LED, HIGH);
}

void apagarLed() {
  digitalWrite(LED, LOW);
}

void sensorProximidad() { //REVISAR LOS DELAY QUE HAY ACA
  digitalWrite(PIN12, LOW); /* Por cuestión de estabilización del sensor*/
  delayMicroseconds(DELAY_5);
  digitalWrite(PIN12, HIGH); /* envío del pulso ultrasónico*/
  delayMicroseconds(DELAY_10);
  tiempo = pulseIn(PIN13, HIGH); /* Función para medir la longitud del pulso entrante. Mide el tiempo que transcurrido entre el envío
                              del pulso ultrasónico y cuando el sensor recibe el rebote, es decir: desde que el pin 12 empieza a 
                              recibir el rebote, HIGH, hasta que deja de hacerlo, LOW, la longitud del pulso entrante*/
  distancia = int(VELOCIDAD_PROXIMIDAD * tiempo); /*fórmula para calcular la distancia obteniendo un valor entero*/
}

void AvanzaMotoresDerecha() {
  digitalWrite (IN4, LOW);
  digitalWrite (IN3, HIGH);
}

void AvanzaMotoresIzquierda() {
  digitalWrite (IN1, HIGH);
  digitalWrite (IN2, LOW);
}

void RetrocedeMotoresDerecha() {
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
}

void RetrocedeMotoresIzquierda() {
  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
}

void DetenerMotoresIzquierda() {
  digitalWrite(IN2, LOW);
  digitalWrite(IN1, LOW);
}

void DetenerMotoresDerecha() {
  digitalWrite(IN4, LOW);
  digitalWrite(IN3, LOW);
}

void AvanzaMotores()
{
  analogWrite(ENB, VALOR_255);
  analogWrite(ENA, VALOR_255);
  digitalWrite (IN2, LOW);
  digitalWrite (IN4, LOW);
  digitalWrite (IN3, HIGH);
  digitalWrite (IN1, HIGH);
}

void RetrocederMotores() {
  analogWrite(ENB, VALOR_255);
  analogWrite(ENA, VALOR_255);
  digitalWrite(IN3, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN4, HIGH);
}

void pararMotores()
{
  digitalWrite (IN3, LOW);
  digitalWrite (IN1, LOW);
  digitalWrite (IN4, LOW);
  digitalWrite (IN2, LOW);
}
