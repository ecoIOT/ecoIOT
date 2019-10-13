
/* 
 *  V1.18 - Versão ecoIOT prototipo produção - Marina Buranhem
  
 *
 * V0.6 - Sistema de Alimentação 5v
 * Placa Solar 5v > Conversor DC-DC 5v > Controlador de Carga > Bateria LiOn 3,7v 

 * >>>> NodeMCU + Shield NodeMCU >  
 * >>>> ADS1115 - expansão de porta analógica
 * >>>> DS1820b - Leitura de temperatura (temp1)agua (temp2) ambiente
 * >>>> Leitura nivel de bateria (bat1)
 * >>>> Leitura de PH (ph)
 * >>>> BMP180 - Leitura de pressão ATM (hPa)
 * >>>> BH1750 - Leitura de Irradiação SOLAR (lux)
 * >>>> AM3201 - Leitura de umidade do AR
 * >>>> deepsleep - enviando dados a cada 300 segundos
 */

 
//configuração pressao
#include <Wire.h>
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

//configuração luximetro
#include <BH1750.h> //INCLUSÃO DE BIBLIOTECA
BH1750 lightMeter(0x23); //CRIA UMA INSTÂNCIA

//Configuração umidade
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT21   // DHT 21 

DHT dht(DHTPIN, DHTTYPE);

float hum = 0.0;
float tempDHT = 0.0;

//Configuração PH
const int analogInPin = A0; 
int sensorValue = 0; 
unsigned long int avgValue; 
float b;
int buf[10],temp;


//config ADS1115 - expansor de porta analógica
//configurado para leitura do diferencial de tensão entre as portas 2 e 3, medindo a tensão da bateria
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads(0x48);
float Voltage = 0.0;
int counter;

int16_t rawADCvalue;  // The is where we store the value we receive from the ADS1115
float scalefactor = 0.1891F; // This is the scale factor for the default +/- 6.144 Volt Range we will use


//config bluemix
#include <PubSubClient.h>
#include <SPI.h>

//config wifi
#include <ESP8266WiFi.h>

//config sensor temperatura
#include <OneWire.h>
#include <DallasTemperature.h>


//pinos na placa arduino ou esp para sensor de temperatura
#define ONE_WIRE_BUS_1 14
#define ONE_WIRE_BUS_2 12
#define ONE_WIRE_BUS_3 13


OneWire oneWire_1(ONE_WIRE_BUS_1);
OneWire oneWire_2(ONE_WIRE_BUS_2);
OneWire oneWire_3(ONE_WIRE_BUS_3);


DallasTemperature sensor1(&oneWire_1);
DallasTemperature sensor2(&oneWire_2);
DallasTemperature sensor3(&oneWire_3);


//Include thingspeak
#include "ThingSpeak.h"

//#############Informações do WIFI###############
//#### ALTERAR COM PARAMETROS DA SUA CONEXÃO ####




const char* ssid =     "########";
const char* password = "########";


//instaciando a função do thingspeak
WiFiClient  client;


//######## Conexão servidor Thingspeak ######## 
//#### ALTERAR COM PARAMETROS DO SEU CANAL ####


//CANAL PRIMARIO - API


unsigned long myChannelNumber = #####;
const char * myWriteAPIKey = "#######";



//variáveis globais
uint16_t lux = 0;
//float scm = 0;
float temp1;                     //variáveis que armazenam as temperaturas de cada sensor
float temp2;
float temp3;

//variaveis BMP180
float bmpTEMP;
float bmpPa;



void setup()   
{                
  
  Wire.begin();

   //__ Inicializa a serial
  Serial.begin(115200);
  Serial.println("ecoIOT v.6");
  Serial.println();
  Serial.print("Conectando-se na rede "); 
  Serial.print(ssid);

  //__ Conecta-se na rede WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  } 
  Serial.println("");

  Serial.print("Conectado, endereço de IP: ");
  Serial.println(WiFi.localIP());

//ativando ADS
ads.begin();

//ativando o DHT
dht.begin();

//ativando sensor de pressão
if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }

//Ativando sensores TEMPERATURA DS1820b
sensor1.begin();
sensor2.begin();
sensor3.begin();
delay(500);


//Ativando Thingspeak API
ThingSpeak.begin(client);



//ATIVANDO LUXIMETRO
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  }
  else {
    Serial.println(F("Error initialising BH1750"));
  }

} //FIM DAS CONFIGURAÇOES



void loop() 
{

//Leitura de pressão ATM
    
    Serial.print("Temperature = ");
    bmpTEMP = bmp.readTemperature();
    Serial.print(bmpTEMP);
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    bmpPa = (bmp.readPressure() /100); 
    Serial.print(bmpPa);
    Serial.println(" hPa");
    
  


//Leitura de PH

for(int i=0;i<10;i++) 
 { 
  buf[i]=analogRead(analogInPin);
  delay(10);
 }
 for(int i=0;i<9;i++)
 {
  for(int j=i+1;j<10;j++)
  {
   if(buf[i]>buf[j])
   {
    temp=buf[i];
    buf[i]=buf[j];
    buf[j]=temp;
   }
  }
 }
 avgValue=0;
 for(int i=2;i<8;i++)
 avgValue+=buf[i];
 float pHVol=(float)avgValue*3.1/1023/6;
 float phValue = -19.707 * pHVol + 54.952;
 Serial.print("PHsensor = ");
 Serial.println(phValue);
 Serial.println(pHVol);
//Fim da leitura de PH

delay(2000);

//Leitura de umidade  
hum = dht.readHumidity();
Serial.print("Umidade: ");
Serial.println(hum);

temp3 = dht.readTemperature();
Serial.print("Temperatura: ");
Serial.println(tempDHT);
delay(1000);


// lendo Temperatura DS1820b  
Serial.println("Lendo temperaturas...\n\r");
 sensor1.requestTemperatures();
 sensor2.requestTemperatures();
 sensor3.requestTemperatures();

 delay(1000);



//Mostrando TEMPERATURA
//Envio a Nuvem validando LEITURA CORRETA
 
temp1 = (((sensor1.getTempCByIndex(0) - 0.5625) * 100) / 98.90) + 0;
// temp1 = sensor1.getTempCByIndex(0);


 
  if(temp1!= -127 && temp1!= 85){
    Serial.print("Temperatura Sensor 1: ");
    Serial.println(temp1);

    ThingSpeak.setField(1,temp1); //envia dado ao thingspeak em caso de leitura

  
  }else{
    Serial.println("Erro ao ler temperatura do Sensor 1");
    temp1 = 0.00;
    ThingSpeak.setField(1,temp1); // Envia dado ao thingspeak no caso de leitura
 
    }
temp2 = (((sensor2.getTempCByIndex(0) + 0.3125) * 100) / 99.3875) + 0;
// temp2 = sensor2.getTempCByIndex(0);
 if(temp2!= -127 && temp2!= 85){
   Serial.print("Temperatura Sensor 2: ");
   Serial.println(temp2);

   ThingSpeak.setField(4,temp2); // Envia dado ao thingspeak no caso de leitura
 
  }else{
    Serial.println("Erro ao ler temperatura do Sensor 2");
    temp2 = 0.00;
    ThingSpeak.setField(4,temp2); // Envia dado ao thingspeak no caso de leitura
 
}

  temp3 = sensor3.getTempCByIndex(0);

  if(temp3!= -127 && temp1!= 85){
    Serial.print("Temperatura Sensor 3: ");
    Serial.println(temp3);

    ThingSpeak.setField(3,temp3); //envia dado ao thingspeak em caso de leitura

  
  }else{
    Serial.println("Erro ao ler temperatura do Sensor 3");
    temp3 = 0.00;
    ThingSpeak.setField(3,temp3); // Envia dado ao thingspeak no caso de leitura
 
    }

 delay(1000);
 
//ADS
//leitura diferencial entre portas (tensão da bateria)

  rawADCvalue = ads.readADC_Differential_2_3(); 
  Voltage = (rawADCvalue * scalefactor)/1000.0;

  Serial.print("Raw ADC Value = "); 
  Serial.print(rawADCvalue); 
  Serial.print("\tVoltage Measured = ");
  Serial.println(Voltage,7);
  Serial.println();


//LEITURA DO LUXIMETRO

  lux = lightMeter.readLightLevel();// Get Lux value //VARIÁVEL RECEBE O VALOR DE LUMINOSIDADE MEDIDO
  Serial.print("Luminosidade: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(lux); //IMPRIME NA SERIAL O VALOR DE LUMINOSIDADE MEDIDO
  Serial.println(" lux"); //IMPRIME O TEXTO NA SERIAL

//CONTADOR DE INTERAÇÕES - CONTROLE E TESTES 
 delay(1000);
  
  //ENVIO DE DADOS PARA THINGSPEAK
 
    ThingSpeak.setField(2,phValue);

//    ThingSpeak.setField(3,bmpTEMP);
    
    ThingSpeak.setField(5,lux);
    
    ThingSpeak.setField(6,hum);
    
    ThingSpeak.setField(7,Voltage);
     
    ThingSpeak.setField(8,bmpPa);



ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); 


//envio canal secundário (CASO ATIVADO)
String sbmpTEMP = String(bmpTEMP);
String stempDHT = String(tempDHT);
//String stempDHT = String(counter);

  String payload="field1=";
  payload+=sbmpTEMP;
  payload+="&field2=";
  payload+=stempDHT;
  payload+="&status=MQTTPUBLISH";
  
  if (client.connected()){
    Serial.print("Sending payload: ");
    Serial.println(payload);
    
    if (client.publish(topic, (char*) payload.c_str())) {
      Serial.println("Publish ok");
    }
    else {
      Serial.println("Publish failed");
    }
  }    

++counter;
  Serial.print("Contador de interações: ");
  Serial.println(counter);

delay(10000);  
ESP.deepSleep(5 * 60000000);//Dorme por 5 Minutos (Deep-Sleep em Micro segundos).

   
} //end loop
