#include <SoftwareSerial.h>

long distancia;
long tiempo;

int IN3 = 3;  //MOTOR A 
int IN4 = 4;  //MOTOR A

int IN1 = 2;  //MOTOR B
int IN2 = 7;  //MOTOR B

int TX = 1; //Tx arduino
int RX = 0; //Rx arduino

char Comando = 's';
char ComandoAnt = 's';
char estado = 's';

//Declaramos los pines que vamos a usar para el sensor pir
int analogPin = A5;   //Analogico A5 para el sensor

//Pin para switch
const int pinSensor = 11;
 
//Declaramos una variable para almacenar el valor que nos devuelve el sensor pir
int val = 0;
int valAux = 0;

//Declaro variable para almacenar valor que devuelve el switch
int valSwitch;

SoftwareSerial BT1(RX,TX); // RX, TX

void setup() {
  pinMode (IN4, OUTPUT);    // Input4 conectada al pin 7 
  pinMode (IN3, OUTPUT);    // Input3 conectada al pin 2
  
  pinMode (IN1, OUTPUT);    // Input1 conectada al pin 4 
  pinMode (IN2, OUTPUT);    // Input2 conectada al pin 3
  Serial.begin(9600);  
  pinMode(12, OUTPUT); /*activación del pin 9 como salida: para el pulso ultrasónico*/
  pinMode(13, INPUT); /*activación del pin 8 como entrada: tiempo del rebote del ultrasonido*/

  pinMode(TX,OUTPUT); //Tx arduino envia datos
  pinMode(RX,INPUT); //Rx arduino recibe datos
 
  pinMode(pinSensor, INPUT_PULLUP); /* Activacion switch */
  
  // digitalWrite(TX,LOW); //Activar bluetooth

  //Serial.begin(9600);
  
  Serial.println("Enter AT commands:");
  BT1.begin(9600);

  Serial.begin(9600);         //Iniciamos la comunicacion serial
  pinMode(analogPin, INPUT);  //Configuramos el pin analogico A5 como entrada 

}

void loop() {


  valAux = digitalRead(analogPin);    //Leemos el valor analogico que nos devuelve el sensor
  if(valAux != val)
  {
    val = valAux;
    Serial.println(val);          //Sacamos el valor del sensor por serial
  }

  valSwitch = digitalRead(pinSensor);
  if (valSwitch == LOW) {
    // Serial.print("Activado");
  } else {
    // Serial.print("Desactivado");
  }

  // ACÁ IRÍA EL CÓDIGO PIR + SWITCH = PRENDER LUZ
  
  sensorProximidad();
  if(distancia > 20 )
  {
    //Serial.print(" Entro");
    if(ComandoAnt != Comando){
       ComandoAnt = Comando;
    }
    
    Comando = leerBluetooth();
    
    if( (Comando != 'a' && Comando != 'r' && Comando != 's') )
    {
      Comando = ComandoAnt;
    }
    switch(Comando)
    {
      case 's':
            pararMotores();
            break;
      case 'a':
            AvanzaMotores();
            break;
      case 'r':
            RetrocederMotores();
            break;
    }
  }
  else
  {
    if(Comando != 'r') {
      pararMotores();
    }
  }
}


char leerBluetooth(){
     if (BT1.available())
          {  char S = BT1.read();
             BT1.print(S);
             BT1.flush();
             return S;
          }
    return 'Z';
}

void sensorProximidad(){
  digitalWrite(12,LOW); /* Por cuestión de estabilización del sensor*/
  delayMicroseconds(5);
  digitalWrite(12, HIGH); /* envío del pulso ultrasónico*/
  delayMicroseconds(10);
  tiempo = pulseIn(13, HIGH); /* Función para medir la longitud del pulso entrante. Mide el tiempo que transcurrido entre el envío
  del pulso ultrasónico y cuando el sensor recibe el rebote, es decir: desde que el pin 12 empieza a recibir el rebote, HIGH, hasta que
  deja de hacerlo, LOW, la longitud del pulso entrante*/
  distancia= int(0.017*tiempo); /*fórmula para calcular la distancia obteniendo un valor entero*/  
}

void pararMotores()
{
    // Motor no gira
  digitalWrite (IN3, LOW);
  digitalWrite (IN1, LOW);
  digitalWrite (IN4, LOW);
  digitalWrite (IN2, LOW);
}

void AvanzaMotores()
{
  digitalWrite (IN2, LOW);
  digitalWrite (IN4, LOW);
  digitalWrite (IN3, HIGH);
  digitalWrite (IN1, HIGH);
}

void RetrocederMotores(){
  digitalWrite(IN3,LOW);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN4,HIGH);
}
