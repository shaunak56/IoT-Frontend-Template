#include <Keypad.h>
#include <Servo.h>

Servo myservo; // create servo object to control a servo

const int DOOR_OPEN = 90;
const int DOOR_CLOSE = 0;
const int ROW_NUM = 4;    //four rows
const int COLUMN_NUM = 4; //four columns
const int DOOR_PIN = 12;
const int MSG_IN = 10;
const int MSG_OUT = 11;

int val; // variable to read the value from the analog pin
char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6};      //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

const String password = "1234"; // change your password here
String input_password;

void setup()
{
    myservo.attach(DOOR_PIN);
    Serial.begin(9600);
    pinMode(MSG_IN, INPUT);
    pinMode(MSG_OUT, OUTPUT);
    pinMode(DOOR_PIN, OUTPUT);
    input_password.reserve(32); // maximum input characters is 33, change if needed
    val = DOOR_OPEN;
}

void read_keypad()
{
    char key = keypad.getKey();
    if (key)
    {
        Serial.println(key);

        if (key == '*')
        {
            input_password = ""; // clear input password
        }
        else if (key == '#')
        {
            if (password == input_password)
            {
                Serial.println("password is correct");
                digitalWrite(MSG_OUT, HIGH);
            }
            else
            {
                Serial.println("password is incorrect, try again");
            }
            input_password = ""; // clear input password
        }
        else
        {
            if (input_password.length() < 32)
            {
                input_password += key; // append new character to input password string
            }
            else
            {
                input_password = "";
            }
        }
    }
}

void loop()
{
    myservo.write(val);
    int arduino_1 = digitalRead(MSG_IN);
    if (digitalRead(MSG_IN) == HIGH)
    {
        val = DOOR_CLOSE;
        read_keypad();
    }
    else
    {
        digitalWrite(MSG_OUT, LOW);
        val = DOOR_OPEN;
    }
}