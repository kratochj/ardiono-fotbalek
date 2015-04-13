#include <FotbalekSounds.h>

#include <SoftwareSerial.h>

// Buttons
#define PORT_BTN_END_OF_GAME 6

// Status LEDs
#define PORT_LED_TEAM_A 7
#define PORT_LED_TEAM_B 8
#define PORT_LED_GAME_ON 9

// Other
#define PORT_BEEPER 10
#define PORT_GOAL_TEAM_A 11
#define PORT_GOAL_TEAM_B 12

// RFID readers
SoftwareSerial RFID_TEAM_A(2, 3);
SoftwareSerial RFID_TEAM_B(4, 5);

FotbalekSounds sounds(PORT_BEEPER);

String team_a_m1;
String team_a_m2;
String team_b_m1;
String team_b_m2;

byte score_a;
byte score_b;

boolean game_on;

char c;

void setup() {
  pinMode(PORT_BEEPER, OUTPUT);
  pinMode(PORT_LED_TEAM_A, OUTPUT);
  pinMode(PORT_LED_TEAM_B, OUTPUT);
  pinMode(PORT_BTN_END_OF_GAME, INPUT);

  Serial.begin(9600);
  Serial.println("Serial Ready");

  RFID_TEAM_A.begin(9600);
  Serial.println("RFID TEAM A Ready");
  RFID_TEAM_B.begin(9600);
  Serial.println("RFID TEAM B Ready");

  // init variables
  game_on = false;

  reset_game();
}

void loop() {
  // Process team "A"
  String rfid_code = read_rfid(RFID_TEAM_A);
  if (rfid_code.length() > 10) {
    if (team_a_m1 == "") {
      // This is member 1
      team_a_m1 = rfid_code;
      blink_led(PORT_LED_TEAM_A);
      digitalWrite(PORT_LED_TEAM_A, HIGH);
      game_on = true;
      Serial.println("Member 1 of team A logged");
    } else if (team_a_m2 == "") {
      // This is member 2
      team_a_m2 = rfid_code;
      blink_led(PORT_LED_TEAM_A);
      digitalWrite(PORT_LED_TEAM_A, HIGH);
      game_on = true;
      Serial.println("Member 2 of team A logged");
    } else {
      // nobody else
      Serial.println("Team A already has all members");
      tone(PORT_BEEPER, 440, 1000);
    }
  }

  // read reset button
  int reset_button_state = digitalRead(PORT_BTN_END_OF_GAME);
  if (reset_button_state == HIGH) {
    Serial.println("Reseting game");
    reset_game();
  }

  if (game_on) {
    // goal - team A
    if (digitalRead(PORT_GOAL_TEAM_A) == HIGH) {
      score_a++;
      Serial.println("Goal team A!");
    }
    // goal - team B
    if (digitalRead(PORT_GOAL_TEAM_B) == HIGH) {
      score_b++;
      Serial.println("Goal team B!");
    }

    if ((score_a >= 10) || (score_b >= 10)) {
      sounds.play_end_of_game();
      game_on = false;
    }
  }

}

/**
* Read RFID chip
*/
String read_rfid(SoftwareSerial serial) {
  String msg;
  while (serial.available() > 0) {
    c = serial.read();
    msg += c;
  }
  msg = msg.substring(1, 13);
  Serial.print("Got chipcode: ");
  Serial.println(msg);
  tone(PORT_BEEPER, 440, 100);
}

/**
 * Reset game
 */
void reset_game() {
  game_on = false;

  team_a_m1 = "";
  team_a_m2 = "";
  team_b_m1 = "";
  team_b_m2 = "";

  score_a = 0;
  score_b = 0;


  // Reset status LEDs
  digitalWrite(PORT_LED_TEAM_A, LOW);
  digitalWrite(PORT_LED_TEAM_B, LOW);
  digitalWrite(PORT_LED_GAME_ON, LOW);

  sounds.play_end_of_game();
}


void blink_led(int port) {
  for (int i = 1; i < 3; i++) {
    digitalWrite(port, HIGH);
    delay(100);
    digitalWrite(port, LOW);
    delay(100);
  }
}





