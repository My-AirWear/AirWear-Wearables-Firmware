#define GPS_RX 8
#define GPS_TX 7
#define GPS_BAUD 115200
/* the first coordinates found are 
----------------------------------------
  Status   : Fix acquired
  Time     : 20:33:03 UTC
  Date     : 14/04/2026
  Latitude : 43.653240
  Longitude: -79.413261
  Speed    : 0.57 km/h
  Heading  : 260.91 degrees
---------------------------------------- 
this is bellwoods office/studio window. 
*/

String nmeaLine = "";
unsigned long lastPrint = 0;
unsigned long lastStatus = 0;
#define PRINT_INTERVAL 3000
#define STATUS_INTERVAL 10000

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("========================================");
  Serial.println("  GPS Module Test — Parsed Output");
  Serial.println("========================================\n");
  Serial1.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);
}

String getField(String sentence, int index) {
  int found = 0;
  int start = 0;
  for (int i = 0; i <= sentence.length(); i++) {
    if (i == sentence.length() || sentence[i] == ',') {
      if (found == index) return sentence.substring(start, i);
      found++;
      start = i + 1;
    }
  }
  return "";
}

float nmeaToDegrees(String raw, String direction) {
  if (raw.length() == 0) return 0.0;
  int dotPos = raw.indexOf('.');
  int degreeDigits = dotPos - 2;
  float degrees = raw.substring(0, degreeDigits).toFloat();
  float minutes = raw.substring(degreeDigits).toFloat();
  float decimal = degrees + (minutes / 60.0);
  if (direction == "S" || direction == "W") decimal *= -1;
  return decimal;
}

String formatTime(String raw) {
  if (raw.length() < 6) return "Unknown";
  return raw.substring(0, 2) + ":" + raw.substring(2, 4) + ":" + raw.substring(4, 6) + " UTC";
}

String formatDate(String raw) {
  if (raw.length() < 6) return "Unknown";
  return raw.substring(0, 2) + "/" + raw.substring(2, 4) + "/20" + raw.substring(4, 6);
}

void parseAndPrint(String sentence) {
  String status  = getField(sentence, 2);
  String timeRaw = getField(sentence, 1);
  String latRaw  = getField(sentence, 3);
  String latDir  = getField(sentence, 4);
  String lonRaw  = getField(sentence, 5);
  String lonDir  = getField(sentence, 6);
  String speed   = getField(sentence, 7);
  String heading = getField(sentence, 8);
  String dateRaw = getField(sentence, 9);

  Serial.println("----------------------------------------");

  if (status == "A") {
    float lat = nmeaToDegrees(latRaw, latDir);
    float lon = nmeaToDegrees(lonRaw, lonDir);
    float speedKmh = speed.toFloat() * 1.852;

    Serial.println("  Status   : Fix acquired");
    Serial.print  ("  Time     : "); Serial.println(formatTime(timeRaw));
    Serial.print  ("  Date     : "); Serial.println(formatDate(dateRaw));
    Serial.print  ("  Latitude : "); Serial.println(lat, 6);
    Serial.print  ("  Longitude: "); Serial.println(lon, 6);
    Serial.print  ("  Speed    : "); Serial.print(speedKmh, 2); Serial.println(" km/h");
    Serial.print  ("  Heading  : "); Serial.print(heading); Serial.println(" degrees");
  } else {
    Serial.println("  Status   : Searching for satellites...");
    Serial.print  ("  Time     : "); Serial.println(formatTime(timeRaw));
    Serial.print  ("  Date     : "); Serial.println(formatDate(dateRaw));
  }

  Serial.println("----------------------------------------\n");
  lastStatus = millis();
}

void loop() {
  while (Serial1.available()) {
    char c = Serial1.read();

    if (c == '\n') {
      if (nmeaLine.startsWith("$GNRMC") || nmeaLine.startsWith("$GPRMC")) {
        if (millis() - lastPrint >= PRINT_INTERVAL) {
          parseAndPrint(nmeaLine);
          lastPrint = millis();
        }
      }
      nmeaLine = "";
    } else if (c != '\r') {
      nmeaLine += c;
    }
  }

  // Fallback — if no GNRMC has arrived in 10 seconds, say so
  if (millis() - lastStatus >= STATUS_INTERVAL) {
    Serial.println("----------------------------------------");
    Serial.println("  Waiting for $GNRMC sentence...");
    Serial.println("----------------------------------------\n");
    lastStatus = millis();
  }
}