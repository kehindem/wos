#include <Arduino.h> // for Serial

char ZoneStatusMapping[4][10] =
{
  "Vacant",
  "Occupied"
};

void print_debug_information(int distance, int interruptCounter, \
int leftZoneStatus, int rightZoneStatus)
{
		Serial.print("Sensor Interrupt!!");
		Serial.println();
		Serial.print("Distance: ");
		Serial.print(distance);
		Serial.println();
		Serial.print("Interrupt Counter: ");
		Serial.print(interruptCounter);
		Serial.println();
		Serial.print("Left Zone Status: ");
		Serial.print(ZoneStatusMapping[leftZoneStatus]);
		Serial.println();
		Serial.print("Right Zone Status: ");
		Serial.print(ZoneStatusMapping[rightZoneStatus]);
		Serial.println();
		Serial.println();
}
