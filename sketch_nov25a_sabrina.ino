#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>


///########### configuração do wifi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
// Defining the WiFi channel speeds up the connection:
#define WIFI_CHANNEL 1
int status = WL_IDLE_STATUS;    


//########## configuração do MQTT
const char* mqtt_server = "test.mosquitto.org";// MQTT broker
char* my_topic_SUB = "FIT/SUB";// the chosen topic
char* my_topic_PUB = "FIT/PUB";// the chosen topic


/////##### configuração do wifi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);


/////###configuração do tempo de leitura
int contador = 1;
char mensagem[30];
 unsigned long lastMillis = 0;






int PIR_SENSOR_PIN = 35; // Pino do sensor PIR (análogo)
int PIR_SENSOR_STATUS = 0; // Variável para armazenar o status do sensor
int previousStatus = -1; // Variável para armazenar o status anterior do sensor
unsigned long lastChangeTime = 0; // Variável para armazenar o tempo da última alteração


int led = 12;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(led , OUTPUT);
  conectawifi();


  client.setServer(mqtt_server, 1883); // Set the MQTT broker server and port
  client.setCallback(callback); // Set the callback function for incoming MQTT messages


}


void loop() {


 int movimento = Sensor_movimento();
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation


  // Check  MQTT connection and process incoming messages
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 delay(100); // Add a delay to avoid flooding the MQTT broker with messages
 
 //Aguarda 15 segundos para enviar uma nova mensagem
   if (millis() - lastMillis > 15000) {
    lastMillis = millis();
             
              sprintf(mensagem, "%d", movimento);
                //sprintf(mensagem, "MQTT ESP32 - Mensagem no. %d", contador);  
              Serial.print("Mensagem enviada: ");
              Serial.println(mensagem);
            //Envia a mensagem ao broker
              client.publish(my_topic_PUB, mensagem);
              Serial.print(contador);
              Serial.println("-Mensagem enviada com sucesso...");
             
              //Incrementa o contador
              contador++;
   }
}




int Sensor_movimento()
{


 PIR_SENSOR_STATUS = digitalRead(PIR_SENSOR_PIN); //ler o sinal da porta 35
 
    // Verifica se houve uma alteração no valor do sensor
    if (PIR_SENSOR_STATUS != previousStatus) {
      // Atualiza o valor anterior
      previousStatus = PIR_SENSOR_STATUS;




              if(PIR_SENSOR_STATUS != 0 )
              {
                Serial.println("Movimento detectado");
                digitalWrite(led, HIGH);
              }
              else{
                Serial.println("Sem movimento");
                digitalWrite(led, LOW);
              }
    }


  return previousStatus;
}






void conectawifi()
  {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Conectando ao WiFi ");
  Serial.print(WIFI_SSID);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
     //WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
     //delay(1000);
  }
  Serial.println(" Conectado!");


  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  }






 // Process incoming MQTT message and control the servo motor
void callback(char* topic, byte* payload, unsigned int length) {
  String string;
  Serial.print("Chegou a mensagem [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
     string+=((char)payload[i]);  
  }
  Serial.print(string);
 
}




// Attempt to reconnect to the MQTT broker if the connection is lost
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT ...");
    if (client.connect("ESPClient")) {
      Serial.println("Conectado");
      client.subscribe(my_topic_SUB);
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tente novamente em 5 segundos");
     
     
  }
  }
}
