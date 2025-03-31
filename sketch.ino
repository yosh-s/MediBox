#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <DHTesp.h>

// Define OLED Display Parameters
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define hardware pins
#define BUZZER 5
#define LED_1 15
#define LED_2 4
#define PB_CANCEL 34
#define PB_OK 32
#define PB_UP 33
#define PB_DOWN 35
#define DHTPIN 12

// Time settings
#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET 19800  // 5 hours 30 minutes in seconds (5 * 3600 + 30 * 60)
#define UTC_OFFSET_DST 0  // No daylight saving time in Sri Lanka

// Declare objects
DHTesp dhtSensor;

// Variables
int hours, minutes, seconds;
int alarm_hours[] = {-1, -1};
int alarm_minutes[] = {-1, -1};
bool snooze_active = false;
unsigned long snooze_time = 0;
int LED2_blink ;

// Function to print text on OLED display at given position and size
void print_line(String text, int x, int y, int size) {
  display.setTextSize(size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(x, y);
  display.println(text);
}

// display time, temp and humidity
void print_time_now() {
  display.clearDisplay(); // Clear display once at the start
  char text[9];
  sprintf(text, "%02d:%02d:%02d", hours, minutes, seconds);
  print_line(text, 20, 0, 2);

  TempAndHumidity data = dhtSensor.getTempAndHumidity();

  print_line("Temperature: " + String(data.temperature) + " C", 0, 20, 1);
  print_line("Humidity:    " + String(data.humidity) + " %", 0, 30, 1);

  boolean temp_warning = data.temperature < 24 || data.temperature > 32;
  boolean humidity_warning = data.humidity < 65 || data.humidity > 80;

  if (temp_warning)
    print_line("Temperature Warning!", 0, 40, 1);
  if (humidity_warning)
    print_line("Humidity Warning!", 0, 50, 1);

  display.display(); // Refresh once after all updates

  if (temp_warning || humidity_warning) {
    digitalWrite(LED_1, !digitalRead(LED_1));
    if (LED2_blink + 500 < millis()) {
      digitalWrite(LED_2, !digitalRead(LED_2));
      digitalWrite(LED_1, !digitalRead(LED_1));
      LED2_blink = millis();

      tone(BUZZER, 330);
      delay(300);
      noTone(BUZZER);
      delay(50);
    }
  } else {
    digitalWrite(LED_2, LOW);
    digitalWrite(LED_1, LOW);
  }
}

// Function to play a simple melody
void play_melody() {
  int melody[] = { 262, 294, 330, 262, 330, 392, 440 }; // musical notes (C4, D4, E4, C4, E4, G4, A4)
  int noteDurations[] = { 300, 300, 300, 300, 400, 400, 500 }; // playing durations in milliseconds

  for (int i = 0; i < 7; i++) {
    tone(BUZZER, melody[i]);  // Play note
    delay(noteDurations[i]);  // Hold note duration
    noTone(BUZZER);           // Stop note before next one
    delay(50);                 // Small pause between notes
  }
}

// Function to ring the alarm
void ring_alarm() {
  display.clearDisplay();
  print_line("MEDICINE", 25, 5, 2);
  print_line("TIME!", 40, 20, 2);
  print_line("OK: Snooze (5 min)", 10, 40, 1);
  print_line("Cancel: Stop", 10, 50, 1);
  display.display();

  digitalWrite(LED_1, HIGH); // LED alert

  while (true) {
    play_melody();  // Play melody on buzzer
    digitalWrite(LED_1, !digitalRead(LED_1));

    // Snooze Alarm if PB_OK is pressed
    if (digitalRead(PB_OK) == LOW) {
      snooze_active = true;
      snooze_time = millis() + 300000; // 5 minutes = 300000 ms

      display.clearDisplay();
      print_line("Snoozed for 5 min", 10, 20, 1);
      display.display();
      delay(1000);
      break;
    }

    // Stop Alarm if PB_CANCEL is pressed
    if (digitalRead(PB_CANCEL) == LOW) {
      display.clearDisplay();
      print_line("Alarm", 10, 20, 2);
      print_line("Stopped !", 10, 40, 2);
      display.display();
      delay(1000);
      break;
    }
  }

  // Stop Buzzer & LED after alarm is stopped or snoozed
  noTone(BUZZER);
  digitalWrite(LED_1, LOW);
  display.clearDisplay();
  display.display();
}

// Function to update time and check for alarm triggers
void update_time_with_check_alarm() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;
  hours = timeinfo.tm_hour;
  minutes = timeinfo.tm_min;
  seconds = timeinfo.tm_sec;
  print_time_now();

  // Check if the current time matches any alarm time
  for (int i = 0; i < 2; i++) {
    if (hours == alarm_hours[i] && minutes == alarm_minutes[i] && seconds == 0) {
      ring_alarm();  // Trigger alarm
    }
  }

  if (snooze_active && millis() >= snooze_time) {
    snooze_active = false;
    ring_alarm();  // Trigger snoozed alarm
  }
}

// Function to set the system time
void set_time() {
  int new_hours = UTC_OFFSET / 3600; // Convert seconds to hours
  int new_minutes = UTC_OFFSET % 3600 / 60; // Convert seconds to minutes
  bool setting_minutes = false;

  // Loop until time is set
  while (true) {
    display.clearDisplay();
    print_line("Time Zone:", 0, 0, 2);
    print_line(String(new_hours) + ":" + (new_minutes < 10 ? "0" : "") + String(new_minutes), 20, 20, 2);
    print_line(setting_minutes ? "Adjusting Minutes" : "Adjusting Hours", 0, 40, 1);
    print_line("Press Cancel to Exit", 0, 50, 1);
    display.display();

    // handle "PB_UP" button presses
    if (digitalRead(PB_UP) == LOW) {
      if (!setting_minutes) {
        new_hours = (new_hours == 14) ? -12 : new_hours + 1;
      } else {
        new_minutes = (new_minutes + 1) % 60;
      }
      delay(200);
    }

    // handle "PB_DOWN" button presses
    if (digitalRead(PB_DOWN) == LOW) {
      if (!setting_minutes) {
        new_hours = (new_hours == -12) ? 14 : new_hours - 1;
      } else {
        new_minutes = (new_minutes == 0) ? 59 : new_minutes - 1;
      }
      delay(200);
    }

    // handle "PB_OK" button presses
    if (digitalRead(PB_OK) == LOW) {
      delay(200);
      if (!setting_minutes) {
        setting_minutes = true;
      } else {
        int new_utc_offset = new_hours * 3600 + new_minutes * 60; // Convert to seconds
        configTime(new_utc_offset, UTC_OFFSET_DST, NTP_SERVER); // update new time
        display.clearDisplay();
        print_line("Time Zone", 0, 0, 2);
        print_line("Updated..!", 0, 20, 2);
        display.display();
        delay(1000);

        break;
      }
    }
    if (digitalRead(PB_CANCEL) == LOW) {
      delay(200);
      break;
    }
  }
}

// Function to set the alarm
void set_alarm() {
  int alarm_index = 0;  // Select alarm slot (0 or 1)
  int new_hours = hours;
  int new_minutes = minutes;
  bool setting_minutes = false;
  bool selecting_alarm = true;

  while (true) {
    display.clearDisplay();
    if (selecting_alarm) {
      print_line("___Select Alarm___", 5, 0, 1);
      print_line((alarm_index == 0 ? "--> " : "   ") + String("Alarm 1"), 0, 20, 1);
      print_line((alarm_index == 1 ? "--> " : "   ") + String("Alarm 2"), 0, 30, 1);
    } else {
      print_line("_Alarm " + String(alarm_index + 1) + "_", 10, 0, 2);
      print_line(String(new_hours) + ":" + (new_minutes < 10 ? "0" : "") + String(new_minutes), 30, 20, 2);
      print_line(setting_minutes ? "Adjusting Minutes" : "Adjusting Hours", 0, 40, 1);
    }
    display.display();


    // handle "PB_UP" button presses
    if (digitalRead(PB_UP) == LOW) {
      if (selecting_alarm) {
        alarm_index = (alarm_index == 0) ? 1 : 0;
      } else {
        if (!setting_minutes) new_hours = (new_hours + 1) % 24;
        else new_minutes = (new_minutes + 1) % 60;
      }
      delay(200);
    }

    // handle "PB_DOWN" button presses
    if (digitalRead(PB_DOWN) == LOW) {
      if (selecting_alarm) {
        alarm_index = (alarm_index == 0) ? 1 : 0;
      } else {
        if (!setting_minutes) new_hours = (new_hours == 0) ? 23 : new_hours - 1;
        else new_minutes = (new_minutes == 0) ? 59 : new_minutes - 1;
      }
      delay(200);
    }

    // handle "PB_OK" button presses
    if (digitalRead(PB_OK) == LOW) {
      delay(200);
      if (selecting_alarm) {
        selecting_alarm = false;
      } else if (!setting_minutes) {
        setting_minutes = true;
      } else {
        alarm_hours[alarm_index] = new_hours;
        alarm_minutes[alarm_index] = new_minutes;
        break;
      }
    }

    // handle "PB_CANCEL" button presses
    if (digitalRead(PB_CANCEL) == LOW) {
      delay(200);
      break;
    }
  }
}

// Function to view the alarms
void view_alarms() {
  while (true) {
    display.clearDisplay();
    print_line("___Current Alarms___", 0, 0, 1);

    for (int i = 0; i < 2; i++) {
      // Display the alarm time if set, otherwise display "None"
      if (alarm_hours[i] != -1) {
        String alarmText = "Alarm " + String(i + 1) + ": " +
                           String(alarm_hours[i]) + ":" +
                           (alarm_minutes[i] < 10 ? "0" : "") + String(alarm_minutes[i]);
        print_line(alarmText, 0, 15 + (i * 10), 1);
      } else {
        print_line("Alarm " + String(i + 1) + ": None", 0, 15 + (i * 10), 1);
      }
    }

    print_line("Press Cancel key to exit", 0, 45, 1);
    display.display();

    // Exit when any button is pressed
    if (digitalRead(PB_CANCEL) == LOW) {
      delay(200);
      break;
    }
  }
}

// Function to delete an alarm
void delete_alarm() {
  int alarm_index = 0; // Start with the first alarm

  // Loop until an alarm is deleted or the user exits
  while (true) {
    display.clearDisplay();
    print_line("___Delete Alarm___", 0, 0, 1);

    for (int i = 0; i < 2; i++) {
      if (i == alarm_index) {
        print_line("--> Alarm " + String(i + 1) + ": " +
                   String(alarm_hours[i]) + ":" +
                   (alarm_minutes[i] < 10 ? "0" : "") + String(alarm_minutes[i]), 0, 15 + (i * 10), 1);
      } else {
        print_line("Alarm " + String(i + 1) + ": " +
                   String(alarm_hours[i]) + ":" +
                   (alarm_minutes[i] < 10 ? "0" : "") + String(alarm_minutes[i]), 0, 15 + (i * 10), 1);
      }
    }

    print_line("PB_OK to Delete", 0, 40, 1);
    print_line("PB_CANCEL to Exit", 0, 50, 1);
    display.display();

    // Navigate between alarms
    if (digitalRead(PB_UP) == LOW) {
      alarm_index = (alarm_index == 0) ? 1 : 0; // Toggle between 0 and 1
      delay(200);
    }
    if (digitalRead(PB_DOWN) == LOW) {
      alarm_index = (alarm_index == 1) ? 0 : 1; // Toggle between 1 and 0
      delay(200);
    }

    // Delete the selected alarm
    if (digitalRead(PB_OK) == LOW) {
      alarm_hours[alarm_index] = -1;  // Mark as deleted
      alarm_minutes[alarm_index] = -1;  // Mark as deleted

      display.clearDisplay();
      print_line("Alarm", 0, 20, 2);
      print_line("Deleted!", 0, 40, 2);
      display.display();
      delay(1000);
      break;
    }

    // Exit delete menu
    if (digitalRead(PB_CANCEL) == LOW) {
      delay(200);
      break;
    }
  }
}

// Function to navigate the main menu
void go_to_menu() {
  int menu_option = 1;

  // Loop until the user exits the menu
  while (true) {
    display.clearDisplay();

    // display menu according to the selected option
    if (menu_option == 1) {
      print_line("--> Set Time Zone", 0, 0, 1);
    } else {
      print_line("1. Set Time Zone", 0, 0, 1);
    }

    if (menu_option == 2) {
      print_line("--> Set Alarms", 0, 10, 1);
    } else {
      print_line("2. Set Alarms", 0, 10, 1);
    }

    if (menu_option == 3) {
      print_line("--> View Alarms", 0, 20, 1);
    } else {
      print_line("3. View Alarms", 0, 20, 1);
    }

    if (menu_option == 4) {
      print_line("--> Delete Alarm", 0, 30, 1);
    } else {
      print_line("4. Delete Alarm", 0, 30, 1);
    }

    if (menu_option == 5) {
      print_line("--> Exit", 0, 40, 1);
    } else {
      print_line("5. Exit", 0, 40, 1);
    }

    display.display();

    // Navigate menu using PB_UP and PB_DOWN
    if (digitalRead(PB_UP) == LOW) {
      menu_option = (menu_option == 1) ? 5 : menu_option - 1;
      delay(200);
    }
    if (digitalRead(PB_DOWN) == LOW) {
      menu_option = (menu_option == 5) ? 1 : menu_option + 1;
      delay(200);
    }

    // Select option
    if (digitalRead(PB_OK) == LOW) {
      delay(200);
      if (menu_option == 1) set_time();
      if (menu_option == 2) set_alarm();
      if (menu_option == 3) view_alarms();
      if (menu_option == 4) delete_alarm();
      if (menu_option == 5) break; // Exit menu
    }

    // Exit menu when PB_CANCEL is pressed
    if (digitalRead(PB_CANCEL) == LOW) {
      delay(200);
      break;
    }
  }
}


void setup() {
  Serial.begin(9600);

  // set pin modes
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(PB_CANCEL, INPUT_PULLUP);
  pinMode(PB_OK, INPUT_PULLUP);
  pinMode(PB_UP, INPUT_PULLUP);
  pinMode(PB_DOWN, INPUT_PULLUP);

  dhtSensor.setup(DHTPIN, DHTesp::DHT22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    while (true);
  }

  display.display();
  delay(2000);

  // Connect to WiFi
  WiFi.begin("Wokwi-GUEST", "");
  while (WiFi.status() != WL_CONNECTED) {

    display.clearDisplay();
    print_line("Connecting to WiFi...", 0, 0, 1);
    display.display();
    delay(100);
  }

  // show connected wifi info
  display.clearDisplay();
  print_line("Connected to WiFi", 0, 0, 1);
  print_line("WiFi : " + WiFi.SSID(), 0, 20, 1);
  print_line("IP : " + WiFi.localIP().toString(), 0, 10, 1);
  display.display();
  delay(2000);
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}

void loop() {
  update_time_with_check_alarm();
  if (digitalRead(PB_OK) == LOW) {
    delay(200);
    go_to_menu();
  }
}
