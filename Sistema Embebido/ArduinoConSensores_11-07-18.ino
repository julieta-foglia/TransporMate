#include <SoftwareSerial.h>

int IN3 = 3;  //MOTOR A Rueda Derecha
int IN4 = 4;  //MOTOR A Rueda Derecha
int ENA = 5;  //PWM un motor DERECHO

int IN1 = 2;  //MOTOR B Rueda izquierda
int IN2 = 7;  //MOTOR B Rueda izquierda
int ENB = 6;  //PWM el otro motor IZQUIERDO

int TX = 1; //Tx arduino
int RX = 0; //Rx arduino
int LED = 8; //Luz LED

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
  
  Serial.begin(9600);
  
  pinMode(12, OUTPUT); /*activación del pin 9 como salida: para el pulso ultrasónico*/
  pinMode(13, INPUT); /*activación del pin 8 como entrada: tiempo del rebote del ultrasonido*/

  pinMode(TX, OUTPUT); //Tx arduino envia datos
  pinMode(RX, INPUT); //Rx arduino recibe datos

  pinMode(pinSensor, INPUT_PULLUP); /* Activacion switch */

  pinMode(ENA, OUTPUT); /* Activación del pin ENA como salida. */ 
  pinMode(ENB, OUTPUT);  /* Activación del pin ENB como entrada. */
  pinMode(LED, OUTPUT); /*Activacion del pin LED */

  // digitalWrite(TX,LOW); //Activar bluetooth

  BT1.begin(9600);

  Serial.begin(9600);         //Iniciamos la comunicacion serial
  pinMode(analogPin, INPUT);  //Configuramos el pin analogico A5 como entrada

}

void loop() {

  valPir = digitalRead(analogPin);    //Leemos el valor analogico que nos devuelve el sensor
  valSwitch = digitalRead(pinSensor); //Leemos el valor del sensor reed switch (LOW, activado)

  if(valPir == 0)
    Serial.println("T");
  
  // SI SE ENCUENTRA EL MATE APOYADO SE DEBE ENCENDER EL LED, Y DEPENDIENDO DE ESO, HABILITAR O DESHABILITAR LOS CONTROLES.
  if (valSwitch == LOW) {
    encenderLed();
    //habilitar controles de avanzar/retroceder y enviar mensaje de temperatura
    Serial.println("H");
  }
  else {
    apagarLed(); // Descomentar cuando arreglemos el tema del iman-calor //
      //bloquear controles
      Serial.println("D");
  }

  Comando = leerBluetooth(); //LEEMOS LO QUE NOS ENVIA LA APLICACION ANDROID
  if( Comando == 'z' )
  {
    Comando = ComandoAnt;
  }
  else
  {
    ComandoAnt = Comando;
  }

  //VERIFICAMOS SI NO SE ACTIVO LA FUNCION DE LOS SENSORES DEL CELULAR, EN CASO DE QUE SE TAPE EL SENSOR DE PROXIMIDAD, SE ENCENDERÁ EL LED
  if(Comando == 'x' )
  {
    encenderLed();
  }
  else
  {
    if(Comando == 'y')
    {
      apagarLed();
    }
  }

  sensorProximidad(); //Obtenemos la distancia del auto a el objeto mas cercano por delante
  if (distancia > 20 && Comando != 'y' && Comando != 'x')
  {
      switch (Comando) {
        //MOTOR IZQUIERDA
        case 'a': //a retrocede motor izq 100%
            analogWrite(ENB, 255);
            RetrocedeMotoresIzquierda();
            break;

        case 'b': //b retrocede motor izq 75%
            analogWrite(ENB, 191);
            RetrocedeMotoresIzquierda();
            break;
          
        case 'c': //c retrocede motor izq 50%
            analogWrite(ENB, 126);
            RetrocedeMotoresIzquierda();
            break;
          
        case 'e' : //e detener motor izq
            analogWrite(ENB, 0);
            DetenerMotoresIzquierda();
            break;
          
        case 'g': //g avanza motor izq 50%
            analogWrite(ENB, 126);
            AvanzaMotoresIzquierda();
            break;
          
        case 'h' : //h avanza motor izq 75%
            analogWrite(ENB, 191);
            AvanzaMotoresIzquierda();
            break;
          
        case 'i' : //i avanza motor izq 100%
            analogWrite(ENB, 255);
            AvanzaMotoresIzquierda();
            break;

         //MOTOR DERECHA
        case 'j' : //j retrocede motor derecha 100%
            analogWrite(ENA, 255);
            RetrocedeMotoresDerecha();
            break;
          
        case 'k' : //k retrocede motor derecha 75%
            analogWrite(ENA, 191);
            RetrocedeMotoresDerecha();
            break;
          
        case 'l' : //l retrocede motor derecha 50%
            analogWrite(ENA, 126);
            RetrocedeMotoresDerecha();
            break;
          
        case 'n' : //n detener motor derecho
            analogWrite(ENA, 0);
            DetenerMotoresDerecha();
            break;
          
        case 'p' : //p avanza motor derecha 50%
            analogWrite(ENA, 126);
            AvanzaMotoresDerecha();
            break;
          
        case 'q' : //q avanza motor derecha 75%
            analogWrite(ENA, 191);
            AvanzaMotoresDerecha();
            break;
          
        case 'r' : //r avanza motor derecha 100%
            analogWrite(ENA, 255);
            AvanzaMotoresDerecha();
            break;

        //SENSORES DEL CELULAR
        case 'u' : //u para los dos motores. Lo hace cuando le tapamos el sensor de luz al celular.
          pararMotores();
          break;

        case 'v' : //v pone a ambos motores en sentido retroceso y a la máxima velocidad.
          RetrocederMotores();
          break;

        case 'w' : //w pone a ambos motores en sentido avance y a la máxima velocidad.
          AvanzaMotores();
          break;     
      }

  }
  else //Permitimos que en caso de que el carro se pare por detectar algo delante, se pueda retroceder o realizar las funciones con los sensores del celular.
  {
    if( Comando == 'a' || Comando == 'b' || Comando == 'c' || Comando == 'j' || Comando == 'k' || Comando == 'l' || Comando == 'u' || Comando == 'v')
    {
      switch(Comando)
      {
         case 'a': 
            analogWrite(ENB, 255);
            RetrocedeMotoresIzquierda();
            break;
          
        case 'b': 
            analogWrite(ENB, 191);
            RetrocedeMotoresIzquierda();
            break;
          
        case 'c': 
            analogWrite(ENB, 126);
            RetrocedeMotoresIzquierda();
            break;

        case 'j' :
            analogWrite(ENA, 255);
            RetrocedeMotoresDerecha();
            break;
          
        case 'k' :
            analogWrite(ENA, 191);
            RetrocedeMotoresDerecha();
            break;
          
        case 'l' :
            analogWrite(ENA, 126);
            RetrocedeMotoresDerecha();
            break;

        case 'u' :
          pararMotores();
          break;

        case 'v' :
          RetrocederMotores();
          break;
      }
    }
    else //Si detecta algo al frente, frena los motores salvo que quiera retroceder.
    {
      pararMotores(); 
    }
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
  return 'z'; //Valor por si no se lee nada.
}

void encenderLed() {
  digitalWrite(LED, HIGH);
}

void apagarLed() {
  digitalWrite(LED, LOW);
}

void sensorProximidad() { //REVISAR LOS DELAY QUE HAY ACA
  digitalWrite(12, LOW); /* Por cuestión de estabilización del sensor*/
  delayMicroseconds(5);
  digitalWrite(12, HIGH); /* envío del pulso ultrasónico*/
  delayMicroseconds(10);
  tiempo = pulseIn(13, HIGH); /* Función para medir la longitud del pulso entrante. Mide el tiempo que transcurrido entre el envío
                              del pulso ultrasónico y cuando el sensor recibe el rebote, es decir: desde que el pin 12 empieza a 
                              recibir el rebote, HIGH, hasta que deja de hacerlo, LOW, la longitud del pulso entrante*/
  distancia = int(0.017 * tiempo); /*fórmula para calcular la distancia obteniendo un valor entero*/
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
  analogWrite(ENB, 255);
  analogWrite(ENA, 255);
  digitalWrite (IN2, LOW);
  digitalWrite (IN4, LOW);
  digitalWrite (IN3, HIGH);
  digitalWrite (IN1, HIGH);
}

void RetrocederMotores() {
  analogWrite(ENB, 255);
  analogWrite(ENA, 255);
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
