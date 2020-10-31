#include <SoftwareSerial.h>
#include <SPI.h>
#include <Ethernet.h>

// defining pins
#define IR_out A4
#define IR_in A5
#define temp_sensor A0
#define LDR A2
#define motor 5
#define led 6
#define GSM_rx 0
#define GSM_tx 1
#define bluetooth_rx 2
#define bluetooth_tx 3
#define BUZZ_PIN 4
#define MSG_OUT 8
#define MSG_IN 9
#define NEIGHBOUR_OUT 10
#define NEIGHBOUR_IN 11

//#define delay_1s_0 3662
#define delay_1s_0 3662
#define delay_1hr_1 856
#define light_off_message "Lights Turned Off"

SoftwareSerial GSM(GSM_tx, GSM_rx);
SoftwareSerial Bluetooth(bluetooth_tx, bluetooth_rx);

// declaring variables
int timer_Call_wait = 0, timer_AI_wait = 0;
int people_cnt, prev_people_cnt;
int IR_state, IR_val, Call_state;
int light;
double temperature;
int motor_value, led_value;
int buzzer_state;
String gsm_data_recieved, bluetooth_data_recieved, serial_data_recieved;
int safe_mode;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);


// intialization blocks

void init_safe_mode()
{
	safe_mode = 0;
}

void init_buzzer()
{
	buzzer_state = 0;
	pinMode(BUZZ_PIN, OUTPUT);
}

void init_IR()
{
	pinMode(IR_out, INPUT);
	pinMode(IR_in, INPUT);
}

void init_GSM()
{
	GSM.begin(115200);
	gsm_data_recieved = "";
}

void init_Bluetooth()
{
	Bluetooth.begin(9600);
	bluetooth_data_recieved = "";
}

void init_temperature()
{
	pinMode(temp_sensor, INPUT);
	pinMode(motor, OUTPUT);
	motor_value = 255;
}

void init_light()
{
	pinMode(LDR, INPUT);
	pinMode(led, OUTPUT);
	led_value = 255;
}

void init_people_cnt()
{
	people_cnt = 1;
	prev_people_cnt = 0;
	IR_state = 0;
}

void init_timer_call()
{
	TCCR0A = 0; // set entire TCCR2A register to 0
	TCCR0B = 0; // same for TCCR2B
	TCNT0 = 0;	//initialize counter value to 0
	// set compare match register for 2khz increments
	OCR0A = 255; // max_value
	// turn on CTC mode
	TCCR0A |= (1 << WGM01);
	// Set CS01 and CS00 bits for 1024 prescaler
	TCCR0B |= (1 << CS02) | (1 << CS00);
	// enable timer compare interrupt
	TIMSK0 |= (1 << OCIE0A);
	timer_Call_wait = (int)delay_1s_0;
	Serial.println(timer_Call_wait);
	Call_state = 0;
}

void stop_timer_call()
{
	TCCR0B = 0;
	TCNT0 = 0;
}

int suggest_motor_value()
{
	read_temperature();
	int temp = (int)temperature;
	if (temp < 15) // suggest to turn off the fan
		return (0);
	else if (temp > 35) //suggest to use fan on full speed
		return (9);
	else
		return map(temp, 15, 35, 1, 8);
}

int suggest_light_value()
{
	read_light();
	return map(light, 0, 1023, 0, 255);
}

// intializing
void setup()
{
	Serial.begin(9600);
	init_IR();
	init_people_cnt();
	init_temperature();
	init_light();
	power_on_devices();
	init_buzzer();
	init_safe_mode();
	//delay(2000);
	sei();
	Serial.begin(9600);

    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    // start the Ethernet connection and the server:
    Ethernet.begin(mac, ip);
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
}

// reading values blocks
void read_IR()
{

	if (digitalRead(IR_out))
	{
		if (digitalRead(IR_in))
			IR_val = 0;
		else
			IR_val = 2;
	}
	else
	{
		if (digitalRead(IR_in))
			IR_val = 1;
		else
			IR_val = 3;
	}
}

void read_temperature()
{
	temperature = (double)analogRead(temp_sensor) * 500 / 1023;
}

void read_light()
{
	light = analogRead(LDR);
}

// logical blocks
void person_gone_inside()
{
	people_cnt++;
	if (people_cnt == 1)
	{
		stop_timer_call();
		power_on_devices();
	}
}

void person_gone_outside()
{
	people_cnt--;
	if (people_cnt == 0)
		init_timer_call();
}

void modify_IR_state()
{
	switch (IR_state)
	{
	case 1:
		if (IR_val == 0)
			IR_state = 0;
		else if (IR_val == 3)
			IR_state = 3;
		break;
	case 2:
		if (IR_val == 0)
			IR_state = 0;
		else if (IR_val == 3)
			IR_state = 4;
		break;
	case 3:
		if (IR_val == 1)
			IR_state = 1;
		else if (IR_val == 2)
			IR_state = 5;
		break;
	case 4:
		if (IR_val == 2)
			IR_state = 2;
		else if (IR_val == 1)
			IR_state = 6;
		break;
	case 5:
		if (IR_val == 3)
			IR_state = 3;
		else if (IR_val == 0)
		{
			IR_state = 0;
			person_gone_inside();
		}

		break;
	case 6:
		if (IR_val == 3)
			IR_state = 4;
		else if (IR_val == 0)
		{
			IR_state = 0;
			person_gone_outside();
		}
		break;
	default:
		if (IR_val == 1)
			IR_state = 1;
		else if (IR_val == 2)
			IR_state = 2;
	};
}

// output values set blocks
void write_motor(int motor_val)
{
	analogWrite(motor, motor_val);
}

void write_led(int led_val)
{
	analogWrite(led, led_val);
}

void power_off_devices()
{
	write_motor(0);
	write_led(0);
}

void power_on_devices()
{
	write_motor(motor_value);
	write_led(led_value);
}

void make_a_call()
{
	GSM.write("ATD09106990302;\r");
}

void send_sms(String message, int state)
{

	if (state == 2)
		GSM.write("AT+CMGF=1;\r");
	else if (state == 1)
		GSM.write("AT+CMGS=\"09106990302\";\r");
	else if (state == 0)
	{
		GSM.write((message + 'r').c_str());
		GSM.write(0x1A);
	}
}

// for call interrupt
ISR(TIMER0_COMPA_vect)
{
	timer_Call_wait--;
	Serial.print("x");
	Serial.println(timer_Call_wait);
	if (timer_Call_wait == 0)
	{
		if (Call_state)
		{
			stop_timer_call();
			power_off_devices();
			send_sms(light_off_message, timer_Call_wait);
			Serial.println("time_out");
			Call_state = 0;
		}
		else
		{
			Serial.println("Calling");
			make_a_call();
			timer_Call_wait = delay_1s_0;
			Call_state = 1;
		}
	}
	else if (timer_Call_wait < 3 && Call_state)
		send_sms(light_off_message, timer_Call_wait);
}
void read_Bluetooth()
{
	if (Bluetooth.available())
	{
		bluetooth_data_recieved += Bluetooth.read();
		check_for_command();
	}
}

void check_for_command()
{
	if (bluetooth_data_recieved.length() > 13 && bluetooth_data_recieved.substring(bluetooth_data_recieved.length() - 14) == "turn on lights")
	{
		Serial.println("Light on thai");
		bluetooth_data_recieved = "";
	}
	else if (bluetooth_data_recieved.length() > 14 && bluetooth_data_recieved.substring(bluetooth_data_recieved.length() - 15) == "turn off lights")
	{
		Serial.println("Light off thai");
		bluetooth_data_recieved = "";
	}
	else if (bluetooth_data_recieved.length() > 10 && bluetooth_data_recieved.substring(bluetooth_data_recieved.length() - 11) == "turn on fan")
	{
		Serial.println("Fan on thai");
		bluetooth_data_recieved = "";
	}
	else if (bluetooth_data_recieved.length() > 11 && bluetooth_data_recieved.substring(bluetooth_data_recieved.length() - 12) == "turn off fan")
	{
		Serial.println("Fan off thai");
		bluetooth_data_recieved = "";
	}
	else if (bluetooth_data_recieved.length() > 8 && bluetooth_data_recieved.substring(bluetooth_data_recieved.length() - 12, bluetooth_data_recieved.length() - 3) == "set light")
	{
		if (bluetooth_data_recieved[9] == ' ' && bluetooth_data_recieved[10] >= '0' && bluetooth_data_recieved[10] <= '9' && bluetooth_data_recieved[11] >= '0' && bluetooth_data_recieved[11] <= '9')
			Serial.println(bluetooth_data_recieved);
		bluetooth_data_recieved = "";
	}
	else if (bluetooth_data_recieved.length() > 6 && bluetooth_data_recieved.substring(bluetooth_data_recieved.length() - 10, bluetooth_data_recieved.length() - 3) == "set fan")
	{
		if (bluetooth_data_recieved[7] == ' ' && bluetooth_data_recieved[8] >= '0' && bluetooth_data_recieved[8] <= '9' && bluetooth_data_recieved[9] >= '0' && bluetooth_data_recieved[9] <= '9')
			Serial.println(bluetooth_data_recieved);
		bluetooth_data_recieved = "";
	}
	else if (bluetooth_data_recieved.length() > 8 && bluetooth_data_recieved.substring(bluetooth_data_recieved.length() - 12, bluetooth_data_recieved.length() - 3) == "set count")
	{
		if (bluetooth_data_recieved[9] == ' ' && bluetooth_data_recieved[10] >= '0' && bluetooth_data_recieved[10] <= '9' && bluetooth_data_recieved[11] >= '0' && bluetooth_data_recieved[11] <= '9')
			Serial.println(bluetooth_data_recieved);
		bluetooth_data_recieved = "";
	}
	else if (bluetooth_data_recieved.length() > 8 && bluetooth_data_recieved.substring(bluetooth_data_recieved.length() - 12, bluetooth_data_recieved.length() - 3) == "set timer")
	{
		if (bluetooth_data_recieved[9] == ' ' && bluetooth_data_recieved[10] >= '0' && bluetooth_data_recieved[10] <= '9' && bluetooth_data_recieved[11] >= '0' && bluetooth_data_recieved[11] <= '9')
		{
			Serial.println(bluetooth_data_recieved);
			bluetooth_data_recieved = "";
		}
	}
}

void read_GSM()
{
	if (GSM.available())
	{
		gsm_data_recieved += GSM.read();
	}
}

void read_Serial()
{

	if (Serial.available())
	{
		char c = Serial.read();
		if (c == '1')
			safe_mode = 1;
		else if (c == '0')
			safe_mode = 0;
		Serial.print("Serial : ");
		Serial.println(c);
	}
}

// serial console print block
int print_changes()
{
	if (prev_people_cnt != people_cnt)
	{
		prev_people_cnt = people_cnt;
		Serial.print("Number of people inside room are now : ");
		Serial.println(people_cnt);
		return 1;
	}
	return 0;
}

void modify_buzzer()
{
	if (buzzer_state)
	{
		digitalWrite(BUZZ_PIN, HIGH);
		digitalWrite(NEIGHBOUR_OUT, HIGH);
		digitalWrite(MSG_OUT, HIGH);
		if (digitalRead(MSG_IN) == HIGH)
		{
			buzzer_state = 0;
		}
	}
	else
	{
		digitalWrite(BUZZ_PIN, digitalRead(NEIGHBOUR_IN));
		digitalWrite(NEIGHBOUR_OUT, LOW);
		digitalWrite(MSG_OUT, LOW);
	}
}

void update_on_website()
{
	// listen for incoming clients
	EthernetClient client = server.available();
	if (client)
	{
		Serial.println("new client");
		// an http request ends with a blank line
		boolean currentLineIsBlank = true;
		while (client.connected())
		{
			if (client.available())
			{
				char c = client.read();
				Serial.write(c);
				// if you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				// so you can send a reply
				if (c == '\n' && currentLineIsBlank)
				{
					// send a standard http response header
					client.println("HTTP/1.1 200 OK");
					client.println("Content-Type: text/html");
					client.println("Connection: close"); // the connection will be closed after completion of the response
					client.println("Refresh: 5");		 // refresh the page automatically every 5 sec
					client.println();
					client.println("<!DOCTYPE HTML>");
					client.println("<html>");

					client.print("Number of people in the room are: ");
					client.print(people_cnt);
					client.println("<br />");
					client.print("The safe mode is ");
					if (safe_mode)
					{
						client.print("on");
					}
					else
					{
						client.print("off");
					}
					client.println("<br />");
					client.print("Temperature of the room is: ");
					client.print(temperature);
					client.println("<br />");

					client.print("Light intensity of the room is: ");
					client.print(light);
					client.println("<br />");

					client.println("</html>");
					break;
				}
				if (c == '\n')
				{
					// you're starting a new line
					currentLineIsBlank = true;
				}
				else if (c != '\r')
				{
					// you've gotten a character on the current line
					currentLineIsBlank = false;
				}
			}
		}
		// give the web browser time to receive the data
		delay(1);
		// close the connection:
		client.stop();
		Serial.println("client disconnected");
	}
}

// main loop
void loop()
{
	Serial.print(IR_val);
	Serial.print("   ");
	Serial.print(people_cnt);
	Serial.print("   ");
	Serial.print(buzzer_state);
	Serial.print("   ");
	Serial.print(safe_mode);
	Serial.print("   ");
	Serial.print(digitalRead(MSG_IN));
	Serial.print("   ");
	Serial.println(prev_people_cnt);

	read_IR();
	modify_IR_state();
	read_Bluetooth();
	read_GSM();
	read_Serial();
	write_motor(suggest_motor_value());
	write_led(suggest_light_value());
	if (print_changes() == 1 && safe_mode == 1)
		buzzer_state = 1;
	else if (safe_mode == 0)
		buzzer_state = 0;
	modify_buzzer();
	update_on_website();
}