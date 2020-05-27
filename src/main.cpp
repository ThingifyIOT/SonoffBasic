#include <Arduino.h>
#include <ThingifyEsp.h>
#include <EasyButton.h>
#include <EEPROM.h>


const int ButtonPin = 0;
const int RelayPin = 12;
const int LedPin = 13;
//const int EepromAddress = 51;

EasyButton button(ButtonPin);
ElapsedTimer timer1;
ElapsedTimer timer2;
Node *enabledNode;


ThingifyEsp thing("Sonoff R3");

bool OnRelayValueChanged(void*_, Node *node)
{
	digitalWrite(RelayPin, node->Value.AsBool());
	//EEPROM.write(EepromAddress, node->Value.AsBool());
	//EEPROM.commit();
	return true;
}

void OnButtonPressed()
{
	auto value = !enabledNode->Value.AsBool();
	enabledNode->SetValue(NodeValue::Boolean(value));
}
void OnButtonLongPressed()
{
	thing.ResetConfiguration();
}
void setup()
{
	Serial.begin(500000);

	pinMode(RelayPin, OUTPUT);
	pinMode(LedPin, OUTPUT);

	//EEPROM.begin(150);
	//bool initalState = EEPROM.read(EepromAddress);

	bool initalState = false;
	button.begin();
	button.onPressed(OnButtonPressed);	
	button.onPressedFor(5000,OnButtonLongPressed);

	enabledNode = thing.AddBoolean("Enabled")->OnChanged(OnRelayValueChanged);
	enabledNode->SetValue(NodeValue::Boolean(initalState));

	thing.AddDiagnostics();
	thing.AddStatusLed(LedPin);
	thing.Start();
	timer1.Start();
}



void loop()
{
	button.read();
	thing.Loop();
}
