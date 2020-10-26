/*
  Fetch number of clicks from a REST endpoint hosted on heroku

  https://ep-0-click-tracker-display.herokuapp.com/counters

  output

  [
    {
      "id":1,
      "name":"default",
      "count":1
     }
  ]
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* WIFI_SSID = "ssid";           // replace with SSID
const char* WIFI_PASSWORD = "password";   // repalce with password

const char* heroku_clicks_url = "https://ep-0-click-tracker-display.herokuapp.com/counters";

// Heroku certified by DigiCert
// DigiCert High Assurance EV Root CA
// https://www.digicert.com/kb/digicert-root-certificates.htm
// Serial #: 02:AC:5C:26:6A:0B:40:9B:8F:0B:79:F2:AE:46:25:77
static const char DIGICERT_CA_CERTIFICATE[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j
ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL
MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3
LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug
RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm
+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW
PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM
xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB
Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3
hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg
EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF
MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA
FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec
nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z
eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF
hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2
Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe
vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep
+OkuE6N36B9K
-----END CERTIFICATE-----
)EOF";

#include <LedControl.h>
const int pinMOSI = 23; // UNUSED    ESP32 MOSI pin 23
const int pinMISO = 19; // for DIN - ESP32 MISO pin 19
const int pinSCK = 18;  // for CLK - ESP32 SCK pin 18
const int pinSS = 5;    // for CS  - ESP32 SS pin 5
LedControl ledControl = LedControl(pinMISO, pinSCK, pinSS, 1); // 1 for single MAZ72XX

void showNumber(long number) {
  // either perform this programatically or dig into the lib to
  // work out how to pass an 8 digit number and display it
  // https://github.com/noah1510/LedController
  ledControl.setDigit(0, 7, (number / 10000000) % 10, false);
  ledControl.setDigit(0, 6, (number / 1000000) % 10, false);
  ledControl.setDigit(0, 5, (number / 100000) % 10, false);
  ledControl.setDigit(0, 4, (number / 10000) % 10, false);
  ledControl.setDigit(0, 3, (number / 1000) % 10, false);
  ledControl.setDigit(0, 2, (number / 100) % 10, false);
  ledControl.setDigit(0, 1, (number / 10) % 10, false);
  ledControl.setDigit(0, 0, (number / 1) % 10, false);
}

void fetchStats() {
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client -> setCACert(DIGICERT_CA_CERTIFICATE);

    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
      HTTPClient https;

      https.begin(*client, heroku_clicks_url);
      int httpsCode = https.GET();

      if (httpsCode > 0) { //Check for the returning code
        String payload = https.getString();

#ifdef DEBUG
        Serial.println(httpsCode);
        Serial.println(payload);
#endif

        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        if(error) {
          Serial.print("deserializeJson() failed: ");
          Serial.print(error.c_str());
          Serial.println(payload);
          return;
        }
        long countStat = doc[0]["count"];
        Serial.println(countStat);
            showNumber(countStat);
      } 
      else {
        Serial.println("Error on HTTP request");
      }
      https.end(); //Free the resources
    }
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Waiting for WiFi... ");

  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /*
    The MAX72XX is in power-saving mode on startup,
    we have to do a wakeup call
  */
  ledControl.shutdown(0, false);
  /* Set the brightness to a medium values */
  ledControl.setIntensity(0, 8);
  /* and clear the display */
  ledControl.clearDisplay(0);
}

void loop() {
  fetchStats();
  delay(1000); // 1 second between stats update
}
