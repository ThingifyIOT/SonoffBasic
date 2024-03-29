#include <Arduino.h>
#include <ThingifyEsp.h>
#include <EasyButton.h>
#include <EEPROM.h>
#include <Logging/Logger.h>

const int ButtonPin = 0;
const int AdditinalButtonPin = 3; // Use UART RX as input pin for touch sensor
const int RelayPin = 12;
const int LedPin = 13;

EasyButton button(ButtonPin);
EasyButton additionalButton(AdditinalButtonPin, 300, false, false);

Node *enabledNode;
Node *longPressCount;

ThingifyEsp thing("Sonoff Basic");

bool OnRelayValueChanged(void*_, Node *node)
{
	digitalWrite(RelayPin, node->Value.AsBool());
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
    LoggerInstance.LogEnabled = false;
	//Serial.begin(500000); - dont initialize uart - we use it as additional button
	thing.Initialize();

	pinMode(RelayPin, OUTPUT);
	pinMode(LedPin, OUTPUT);

	
	button.begin();
	button.onPressed(OnAdditionaButtonPressed);	
	button.onPressedFor(4000,OnButtonLongPressed);

 	additionalButton.begin();
	additionalButton.onPressed(OnAdditionaButtonPressed);	
	additionalButton.onPressedFor(2000, OnAdditionaButtonLongPressed);


	enabledNode = thing.AddBoolean("Enabled")->OnChanged(OnRelayValueChanged);
	longPressCount = thing.AddInt("long_press_count");

	enabledNode->SetValue(NodeValue::Boolean(false));

	thing.AddDiagnostics();
	thing.AddStatusLed(LedPin, true);

	thing.Start();
}

void loop()
{
	button.read();
	additionalButton.read();
	thing.Loop();
}
