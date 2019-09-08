#include <Arduino.h>
#include <ContiEsp.h>
#include <DebugNodesModule.h>
#include <EasyButton.h>
#include <EEPROM.h>


const int ButtonPin = 0;
const int RelayPin = 12;
const int LedPin = 13;
const int EepromAddress = 51;

EasyButton button(ButtonPin);
ElapsedTimer timer1;
ElapsedTimer timer2;
Node *enabledNode;


ContiEsp thing("mGZarwsg4OSfxw0Qce1A", "Sonoff R3");

bool OnRelayValueChanged(void*_, Node *node)
{
	digitalWrite(RelayPin, node->Value.AsBool());
	EEPROM.write(EepromAddress, node->Value.AsBool());
	EEPROM.commit();
	return true;
}

void OnButtonPressed()
{
	auto value = !enabledNode->Value.AsBool();
	enabledNode->SetValue(NodeValue::Boolean(value));
}
void setup()
{
	Serial.begin(500000);

	pinMode(RelayPin, OUTPUT);
	pinMode(LedPin, OUTPUT);

	EEPROM.begin(150);
	bool initalState = EEPROM.read(EepromAddress);


	button.begin();
	button.onPressed(OnButtonPressed);
	
	thing.AddAp("toya88806789", "56744461");
	thing.AddAp("gx0", "zxcvb123");

	enabledNode = thing.AddBoolean("Enabled")->OnChanged(OnRelayValueChanged);
	enabledNode->SetValue(NodeValue::Boolean(initalState));

	auto debugNodes = new DebugNodesModule(thing);
	debugNodes->UpdateIntervalInMs = 1000;
	thing.AddModule(debugNodes);
	thing.WatchdogEnabled = true;

	thing.Start();
	timer1.Start();
}



void loop()
{
	button.read();	

	uint64_t ledOnTime;
	uint64_t ledOffTime;

	if(thing.GetCurrentState() == ThingState::Online)
	{
		ledOnTime = 100;
		ledOffTime = 2000;
	}
	else if(thing.GetCurrentState() == ThingState::DisconnectedFromMqtt || thing.GetCurrentState() == ThingState::Disabled)
	{
		ledOnTime = 4000;
		ledOffTime = 200;
	}
	else
	{		
		ledOnTime = 200;
		ledOffTime = 200;		
	}
	

	if(timer1.IsStarted() && timer1.ElapsedMs() > ledOffTime)
	{
		digitalWrite(LedPin, 0); // on
		timer1.Stop();
		timer2.Start();
	}
	if(timer2.IsStarted() && timer2.ElapsedMs() > ledOnTime)
	{
		digitalWrite(LedPin, 1); // off
		timer2.Stop();
		timer1.Start();
	}	

	thing.Loop();
}
