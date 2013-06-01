// Proof of concept firmware for ArduinIO board.
//
// This could be optimised and improved in various ways, but it's just a PoC
// for the minute. It's designed to do as much as the JIO board could (with
// the exception of fewer analogue inputs), and no more.

const int LED = 13; // pin on which the hardwired arduino LED is located
const int VERSION = 1; // version number of this software
const int OUTPUT_SET = 0; // command IDs
const int OUTPUT_GET = 1;
const int INPUT_A = 2;
const int INPUT_D = 3;
const int RESET_STATE = 4;

const int OUTPUTS[] = {
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9
};

const int DINPUTS[] = {
  11,
  12,
  A0,
  A1,
  A2,
  A3,
  A4,
  A5
};

const int AINPUTS[] = {
  A0,
  A1,
  A2,
  A3,
  A4,
  A5
};

void flashSR() // flash SR in morse code from the LED so we know the firmware is installed
{
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(300);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(300);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
}

void flashVer() // not morse code, but number of flashes = version
{
  int i;
  for(i = 0; i < VERSION; i++)
  {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(200);
  }
}

void setPinModes()
{
  int i;
  pinMode(LED, OUTPUT);
  for(i = 0; i < sizeof(OUTPUTS)/sizeof(int); i++)
  {
    pinMode(OUTPUTS[i], OUTPUT);
  }
  for(i = 0; i < sizeof(DINPUTS)/sizeof(int); i++)
  {
    pinMode(DINPUTS[i], INPUT);
  }
  for(i = 0; i < sizeof(AINPUTS)/sizeof(int); i++)
  {
    pinMode(AINPUTS[i], INPUT);
  }
}

void setup()
{
  setPinModes();
  flashSR();
  delay(500);
  flashVer();
  Serial.begin(9600); // start serial comms
}

void output_set(int *message, int length)
{
  if(length != 1)
    return; // something went wrong...
  int outputs = message[0];
  int i;
  for(i = 0; i < sizeof(OUTPUTS)/sizeof(int); i++, outputs >>= 1)
  {
    digitalWrite(OUTPUTS[i], outputs & 1 ? HIGH : LOW);
  }
}

void output_get()
{
  int mask = 1;
  int outputs = 0;
  int i;
  for(i = 0; i < sizeof(OUTPUTS)/sizeof(int); i++, mask <<= 1)
  {
    if(digitalRead(OUTPUTS[i]) == HIGH)
    {
      outputs |= mask;
    }
  }
  Serial.write(1);
  Serial.write(outputs);
}

void input_a()
{
  int i;
  Serial.write((sizeof(AINPUTS)/sizeof(int))*2);
  for(i = 0; i < sizeof(AINPUTS)/sizeof(int); i++)
  {
    int toWrite = analogRead(AINPUTS[i]);
    Serial.write(toWrite & 255);
    Serial.write(toWrite >> 8);
  }
}

void input_d()
{
  int mask = 1;
  int inputs = 0;
  int i;
  for(i = 0; i < sizeof(DINPUTS)/sizeof(int); i++, mask <<= 1)
  {
    if(digitalRead(DINPUTS[i]) == HIGH)
    {
      inputs |= mask;
    }
  }
  Serial.write(1);
  Serial.write(inputs);
}

void reset_state()
{
  Serial.write(1);
  Serial.write(0);
}

void loop()
{
  if(Serial.available() > 0)
  {
    int length = Serial.read();
    int i;
    int message[length];
    for(i = 0; i < length; i++)
    {
      while(Serial.available() == 0)
        ; // wait for enough bytes to actually be received
      message[i] = Serial.read();
    }
    switch(message[0]) // command
    {
      case OUTPUT_SET:
        output_set(message + 1, length - 1);
        break;
      case OUTPUT_GET:
        output_get();
        break;
      case INPUT_A:
        input_a();
        break;
      case INPUT_D:
        input_d();
        break;
      case RESET_STATE:
        reset_state();
        break;
      default:
        break;
    }
  }
}

