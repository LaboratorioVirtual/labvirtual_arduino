#include <SDISerial.h>

#include <ThreeWire.h> //INCLUSÃO DA BIBLIOTECA
#include <RtcDS1302.h> //INCLUSÃO DA BIBLIOTECA

ThreeWire wire(4, 5, 7);
RtcDS1302<ThreeWire> Rtc(wire);

#define DATA_PIN 2
#define BAUDRATE 9600

SDISerial connection(DATA_PIN);

#define countof(a) (sizeof(a) / sizeof(a[0]))
char output_buffer[125];
char* resp;
double n_umidade;

void printDate(const RtcDateTime& dt) {
  char dateString[20];
  snprintf_P(dateString,
            countof(dateString),
            PSTR("%02u/%02u/%04u"),
            dt.Day(),
            dt.Month(),
            dt.Year());
  Serial.print(dateString);            
}

void printHour(const RtcDateTime& dt) {
  char hourString[20];
  snprintf_P(hourString,
            countof(hourString),
            PSTR("%02u:%02u"),
            dt.Hour(),
            dt.Minute());
  Serial.print(hourString);            
}

void setup() {
  connection.begin();
  Serial.begin(BAUDRATE);
  delay(3000);
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if(Rtc.GetIsWriteProtected()){
    Rtc.SetIsWriteProtected(false);
  }
  if(!Rtc.GetIsRunning()){
    Rtc.SetIsRunning(true);
  }
  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) {
    Rtc.SetDateTime(compiled);
  }   
}

double calibration(char *leitura_inicial) {
  char *token;
  double new_umidade;
  const char separador[2] = "+";
  int i = 0, id = 15;
  token = strtok(leitura_inicial, separador);
  while (token != NULL) {
    if (i == 0) {
      id = atoi(token);
    } else if (i == 1) {
      new_umidade = atof(token);
      if (id == 0) {
        new_umidade = 24.944*log(new_umidade) - 31.205;
      } else if (id == 9) {
        new_umidade = 24.944*log(new_umidade) - 30.205;
      }
    }
    token = strtok(NULL, separador);
    i++;
  }
  return new_umidade;
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime();
  if (now.Minute() % 5 == 0) {
      printDate(now);
      Serial.print('+');
      printHour(now);
      // Inicio Sensor A
      connection.sdi_query("0M!", 1000);
      delay(1000);
      resp = connection.sdi_query("0D0!",1000);
      if (resp) {
        Serial.print('+');
        sprintf(output_buffer, "%s", resp);
        Serial.print(output_buffer);
        Serial.print('+');
        n_umidade = calibration(output_buffer);
        Serial.print(n_umidade);
      }
      // Fim Sensor A
      // Inicio Sensor B
      connection.sdi_query("9M!", 1000);
      delay(1000);
      resp = connection.sdi_query("9D0!",1000);
      if (resp) {
        Serial.print('+');
        sprintf(output_buffer, "%s", resp);
        Serial.print(output_buffer);
        Serial.print('+');
        n_umidade = calibration(output_buffer);
        Serial.print(n_umidade);
      }
      // Fim Sensor B
      
      Serial.println();
      delay(3000);
    } else {
      delay(30000);
    }
}
