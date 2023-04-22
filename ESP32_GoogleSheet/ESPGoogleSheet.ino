/*******************************************************************************/
//----------------------------
#include <WiFi.h>
#include <HTTPClient.h>
#define RXD2 16
#define TXD2 17
// NRF: 8     6    7    5
//    : Rx   TX   CTS   RTS

//ESP: tx->rx->cts->rts
//   :          d2   d4
//
using namespace std;
//---------------------------------------------------------------------
//
//
//ENTER_GOOGLE_DEPLOYMENT_ID
const char* ssid = "RedmiK20";
const char* password = "Ankit2515";
String GOOGLE_SCRIPT_ID = "AKfycbzUEwPLqIdm4OFxJjBQ2AhBJ2sG8nkvdKpMjle2sMH2YUihvO3mRY5QYeLiQz3K0Ggt";
//---------------------------------------------------------------------

const int sendInterval = 2000;

/************************************************************************************
void my_split(char* array, char* strings[], char* ptr = NULL) {
  //  Serial.begin(9600);
  //Serial.print(array);
  byte index = 0;
  ptr = strtok(array, ":");  // delimiters is colon
  while (ptr != NULL) {
    strings[index] = ptr;
    index++;
    ptr = strtok(NULL, ",");
  }
  //Serial.println(index);
  // print all the parts
  Serial.println("The Pieces separated by strtok()");
  for (int n = 0; n < index; n++) {
    Serial.print("piece ");
    Serial.print(n);
    Serial.print(" = ");
    Serial.println(strings[n]);
  }
}
************************************************************************************/
void another_split(String data, vector<String>& splited_data) {
  int start_index = data.indexOf("1234", 0) + 4;  //-1;
  int end_index = 0;
  String value;
  while (data.indexOf(":", start_index + 1) >= 0) {
    end_index = data.indexOf(":", start_index + 1);
    value = data.substring(start_index + 1, end_index);
    splited_data.push_back(value);
    start_index = end_index;
  }
  value = data.substring(start_index + 1);
  splited_data.push_back(value);
  // for (auto i : splited_data) {
  //   Serial.println(i);
  // }
}

/************************************************************************************
 *  setup function starts
 **********************************************************************************/
void setup() {
  //--------------------------------------------
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(10);
  Serial.println("Loopback program started");
  Serial2.read();
  //start serial communication with Serial Monitor
  //--------------------------------------------
  //start serial communication with Neo6mGPS
  //  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(10);
  //--------------------------------------------
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("OK");
  //--------------------------------------------
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM



/************************************************************************************
 *  loop function starts
 **********************************************************************************/
void loop() {
  int serial_data;
  String content = "";
  char character;
  vector<String> splited_data;
  while (Serial2.available() > 0) {  // Collecting data from serial port
    character = Serial2.read();
    content.concat(character);
  }

  if (content != "") {
    Serial.println(content);
    another_split(content, splited_data);
    // for(auto i: splited_data){
    //   Serial.println(i);
    // }
  }
  delay(10);

  boolean newData = false;
  if (!splited_data.empty()) {
    // Serial.println("newData is true");
    Serial.println(splited_data[0]);
    newData = true;
  }


  //If newData is true
  if (newData == true) {
    newData = false;
    print_speed(splited_data);
  } else {
  }
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM




/************************************************************************************
 *  loop function starts
 **********************************************************************************/
void print_speed(vector<String> splited_data) {
  if (!splited_data.empty()) {
    String param;
    param = "temp=" + splited_data[0];
    param += "&humi=" + splited_data[1];
    param += "&name=" + splited_data[2].substring(0, 2);

    Serial.println(param);
    write_to_google_sheet(param);
  } else {
  }
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM




/************************************************************************************
 *  loop function starts
 **********************************************************************************/
void write_to_google_sheet(String params) {
  HTTPClient http;
  String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + params;
  // https://script.google.com/macros/s/AKfycbzUEwPLqIdm4OFxJjBQ2AhBJ2sG8nkvdKpMjle2sMH2YUihvO3mRY5QYeLiQz3K0Ggt/exec?value=100&name=n1   // for testing
  //Serial.print(url);
  Serial.println("Postring data to Google Sheet");
  //---------------------------------------------------------------------
  //starts posting data to google sheet
  http.begin(url.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  Serial.print("HTTP Status Code: ");
  Serial.println(httpCode);
  //---------------------------------------------------------------------
  //getting response from google sheet
  String payload;
  if (httpCode > 0) {
    payload = http.getString();
    Serial.println("Payload: " + payload);
  }
  //---------------------------------------------------------------------
  http.end();
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM