#ifndef GPIO_HH
#define GPIO_HH


namespace GPIO
{
	enum PINMODE {INPUT, OUTPUT};
	const int HIGH = 1;
	const int LOW = 0;

	bool setPinMode(int pin, PINMODE mode = OUTPUT);
	bool digitalWrite(int pin, int value);
	int digitalRead(int pin);

}

#endif // GPIO_HH