#include <Arduino.h>
#include <ThingifyEsp.h>
#include <EasyButton.h>
#include <EEPROM.h>


const int ButtonPin = 0;
const int AdditinalButtonPin = 9;

const int RelayPin = 12;
const int LedPin = 13;
//const int EepromAddress = 51;

EasyButton button(ButtonPin);
EasyButton additionalButton(AdditinalButtonPin, 300, false, false);


Node *enabledNode;
Node *longPressCount;

ThingifyEsp thing("Sonoff R3");

bool OnRelayValueChanged(void*_, Node *node)
{
	digitalWrite(RelayPin, node->Value.AsBool());
	//EEPROM.write(EepromAddress, node->Value.AsBool());
	//EEPROM.commit();
	return true;
}

void OnAdditionaButtonPressed()
{
	auto value = !enabledNode->Value.AsBool();
	enabledNode->SetValue(NodeValue::Boolean(value));
}
void OnButtonLongPressed()
{
	thing.ResetConfiguration();
}
void OnAdditionaButtonLongPressed()
{
 	longPressCount->SetValue(NodeValue::Int(longPressCount->Value.AsInt()+1));
}
void setup()
{
	Serial.begin(500000);
	
	pinMode(RelayPin, OUTPUT);
	pinMode(LedPin, OUTPUT);

	bool initalState = false;
	button.begin();
	additionalButton.begin();
	button.onPressed(OnAdditionaButtonPressed);	
	button.onPressedFor(5000,OnButtonLongPressed);
 
	additionalButton.onPressed(OnAdditionaButtonPressed);	
	additionalButton.onPressedFor(2000, OnAdditionaButtonLongPressed);


	enabledNode = thing.AddBoolean("Enabled")->OnChanged(OnRelayValueChanged);
	enabledNode->SetValue(NodeValue::Boolean(initalState));

	thing.AddDiagnostics();
	thing.AddStatusLed(LedPin, true);

	longPressCount = thing.AddInt("long_press_count");

	thing.Start();
}



void loop()
{
	button.read();
	additionalButton.read();
	thing.Loop();
}
