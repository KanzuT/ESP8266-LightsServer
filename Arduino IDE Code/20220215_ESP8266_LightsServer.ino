// ESP8266 WebServer with NeoPixel LED by Kevin Kailing


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <Adafruit_NeoPixel.h>

#ifndef STASSID
#define STASSID "ssid"
#define STAPSK  "pw"
#endif

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        D1 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 12 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 100 // Time (in milliseconds) to pause between pixels

int R = 255;
int G = 255;
int B = 255;
int BRIGHTNESS = 100;

int R_ref = R;
int G_ref = G;
int B_ref = B;

boolean ledon = false;


const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);


void handleRoot() {
  server.send(200, "text/plain", "Server ist online");
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

void lighton(){

  double Rval = (double)R*((double)BRIGHTNESS/100);
  double Gval = (double)G*((double)BRIGHTNESS/100);
  double Bval = (double)B*((double)BRIGHTNESS/100);

  int rv = (int)Rval;
  int gv = (int)Gval;
  int bv = (int)Bval;
  
  if(!ledon){
    pixels.clear();

    for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
      pixels.setPixelColor(i, pixels.Color(rv,gv,bv));
      pixels.show();   // Send the updated pixel colors to the hardware.
  
      delay(DELAYVAL); // Pause before next pass through loop
    }
    ledon = true;
  }
  
  server.send(200, "text/plain", "Licht ist an\nR : " + (String)rv + "\nG : " + (String)gv + "\nB : " + (String)bv);
  
}

void lightoff(){

  if(ledon){
    for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      pixels.show();   // Send the updated pixel colors to the hardware.

      delay(DELAYVAL); // Pause before next pass through loop

      pixels.clear();
    }  
  }

  ledon = false;
  server.send(200, "text/plain", "Licht ist aus");
  
}

void setbrightness(){
  
  String m = "";
  int minval;
  int maxval;

  int oldbrightness = BRIGHTNESS;
  
  //URL?brightness=value
  BRIGHTNESS = server.arg(0).toInt();
  
  m = ("Neue Helligkeit: " + (String)BRIGHTNESS);

  server.send(200, "text/plain", m); 

  if(ledon){
    lightoff();
    lighton();
  }
  else{
    
  } 
}
void setcolor(){

  //URL?r=value&g=value&b=value
  R = server.arg(0).toInt();
  G = server.arg(1).toInt();
  B = server.arg(2).toInt();

  R_ref = R;
  G_ref = G;
  B_ref = B;

  String m = "R = " + (String)R + "\nG = " + (String)G + "\nB = " + (String)B;
  server.send(200, "text/plain", m);

  if(ledon){
      lightoff();
      lighton();    
    }
    else{
      lightoff();
    }
}

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", lighton);
  server.on("/off", lightoff);
  server.on("/setbrightness", setbrightness); //GEHT NICHT RICHTIG, NICHT NUTZEN
  server.on("/setcolor", setcolor);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");


  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  pixels.begin();

  R_ref = R;
  G_ref = G;
  B_ref = B;

  lightoff();
  
}

void loop(void) {
  server.handleClient();  
}
