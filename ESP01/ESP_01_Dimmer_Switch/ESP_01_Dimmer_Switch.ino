#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> 

ESP8266WebServer server(80);

const int dimmerPin = 3; // RX Pin
int brightness = 512;    // Slider 0-512
int currentPhysicalPWM = 0; 
bool isOn = true;

// Hardware calibration (0-64 physical range for saturation limit)
int scaleValue(int sliderVal) {
  return map(sliderVal, 0, 512, 0, 64);
}

void fadeTo(int targetPhysical) {
  if (currentPhysicalPWM == targetPhysical) return;
  int step = (targetPhysical > currentPhysicalPWM) ? 1 : -1;
  while (currentPhysicalPWM != targetPhysical) {
    currentPhysicalPWM += step;
    analogWrite(dimmerPin, currentPhysicalPWM);
    delay(2); 
    server.handleClient(); 
  }
}

String getHTML() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body { font-family: sans-serif; text-align: center; background: #1a1a1a; color: white; padding-top: 30px; }";
  html += ".slider { width: 85%; max-width: 400px; height: 25px; border-radius: 15px; background: #333; outline: none; margin: 30px 0; -webkit-appearance: none; }";
  html += ".slider::-webkit-slider-thumb { -webkit-appearance: none; width: 35px; height: 35px; border-radius: 50%; background: #007bff; cursor: pointer; border: 3px solid #fff; }";
  html += ".btn { background: #007bff; border: none; color: white; padding: 15px 32px; font-size: 18px; border-radius: 8px; cursor: pointer; transition: 0.3s; font-weight: bold; min-width: 150px; }";
  html += ".btn.off { background: #444; }";
  html += ".footer { margin-top: 50px; font-size: 14px; color: #888; }";
  html += ".footer a { color: #007bff; text-decoration: none; font-weight: bold; }</style></head><body>";
  
  html += "<h1>Dimmer Control</h1>";
  
  String btnClass = (isOn && brightness > 0) ? "btn" : "btn off";
  String btnText = (isOn && brightness > 0) ? "Turn OFF" : "Turn ON";
  html += "<button id='pwrBtn' class='" + btnClass + "' onclick='togglePower()'>" + btnText + "</button><br><br>";
  
  html += "<h3>Brightness: <span id='val'>" + String(map(brightness, 0, 512, 0, 100)) + "</span>%</h3>";
  html += "<input type='range' min='0' max='512' value='" + String(brightness) + "' class='slider' id='dimSlider' oninput='debouncedUpdate(this.value)'>";
  
  html += "<div class='footer'><p>Made by <b>Abhyuday Shukla</b></p>";
  html += "<p><a href='https://www.youtube.com/@AmanOnScreen' target='_blank'>Watch how it was made</a></p></div>";
  
  html += "<script>";
  html += "let timeout = null;";
  html += "function debouncedUpdate(val) {";
  html += "  const bVal = parseInt(val);";
  html += "  document.getElementById('val').innerHTML = Math.round((bVal/512)*100);";
  html += "  const btn = document.getElementById('pwrBtn');";
  html += "  if (bVal === 0) { btn.innerHTML = 'Turn ON'; btn.className = 'btn off'; }";
  html += "  else { btn.innerHTML = 'Turn OFF'; btn.className = 'btn'; }";
  html += "  clearTimeout(timeout);";
  html += "  timeout = setTimeout(() => { fetch('/setDim?v=' + bVal); }, 250);";
  html += "}";
  html += "function togglePower() { fetch('/toggle').then(() => location.reload()); }";
  html += "</script></body></html>";
  return html;
}

void handleRoot() { server.send(200, "text/html", getHTML()); }

void handleDim() {
  if (server.hasArg("v")) {
    brightness = server.arg("v").toInt();
    isOn = (brightness > 0);
    fadeTo(scaleValue(brightness));
    server.send(200, "text/plain", "OK");
  }
}

void handleToggle() {
  if (isOn && brightness > 0) { isOn = false; fadeTo(0); }
  else { isOn = true; if (brightness == 0) brightness = 256; fadeTo(scaleValue(brightness)); }
  server.send(200, "text/plain", "OK");
}

void setup() {
  // Set WiFi mode and Hostname immediately
  WiFi.mode(WIFI_STA);
  WiFi.hostname("AppSlash Devices - 100");
  
  Serial.begin(115200);

  WiFiManager wm;
  // Setting the Access Point name for the on-the-fly configuration portal
  if (!wm.autoConnect("AppSlash SmartHome - LED DIMMER")) {
    ESP.restart();
  }

  // Release Pin 3 (RX) after WiFi connection is confirmed
  delay(1000);
  Serial.end(); 

  pinMode(dimmerPin, FUNCTION_3);
  pinMode(dimmerPin, OUTPUT);
  
  if (isOn) {
    currentPhysicalPWM = scaleValue(brightness);
    analogWrite(dimmerPin, currentPhysicalPWM);
  } else {
    currentPhysicalPWM = 0;
    analogWrite(dimmerPin, 0);
  }

  server.on("/", handleRoot);
  server.on("/setDim", handleDim);
  server.on("/toggle", handleToggle);
  server.begin();
}

void loop() {
  server.handleClient();
}