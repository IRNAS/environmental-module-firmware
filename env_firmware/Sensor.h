#ifndef Sensor_h
#define Sensor_h

// Sensor Includes
#include "L0.h"
#include "TSL2561_sensor.h"
#include "TSL2591_sensor.h"
#include "BME.h"
#include "ANEMOMETER.h"
#include "RAIN.h"
#include "CO2.h"
#include "TDR.h"
#include "BATTERY.h"
#include "ANALOG.h"
#include "global.h"
#include "OneW_TMP.h"

#include <Arduino.h>

//Virtual sensor class
class Sensor {
public:
	virtual ~Sensor() {};
	virtual String getName() {};
	virtual int& getCANid() {};
	virtual int getCANidValue() {};
	virtual bool sensorRead() {};
	virtual void printData() {};
	virtual int getCounterCol() {};
	virtual int getCounterRow() {};
	virtual bool getCounterColOverflow() {};
	virtual void execTimer() {};
	virtual bool setup() {};
	virtual void setCANid(int) {};
	virtual void updateValues(byte data[][COL_LENGTH][8], byte time_data[][8]) {};
	virtual	void clearData() {};
	virtual int getGlobalId() {};
	virtual int getNumberOfVariables() {};
	virtual int getLastTime() {};
};

//Tempalate sensor class
template <class mySensorType>
class mySensor : public Sensor {
public:

	mySensorType sensor;

	mySensor(int id = 0, int pin = 0) {
		global_id = id;
		PIN = pin;
		//Setup pin number if aplicable
		if (PIN != 0)
		{
			sensor.PIN = PIN;
		}
	};

	//Get functions
	int getGlobalId() {
		return global_id;
	}
	String getName() {
		return sensor.name;
	};
	int& getCANid() {
		return sensor.CAN_ID;
	};
	int getCANidValue() {
		return sensor.CAN_ID;
	};
	int getCounterCol() {
		return sensor.counter_col;
	};
	int getCounterRow() {
		return sensor.counter_row;
	};
	bool getCounterColOverflow() {
		return sensor.counter_col_overflow;
	};
	int getNumberOfVariables() {
		return NUM_IN_ARRAY(sensor.data);
	}
	int getLastTime() {
		return sensor.exec_timer_last;
	}

	//Set functions
	void setCANid(int id) {
		sensor.CAN_ID = id;
	}

	//Other
	//Run sensor setup
	bool setup() {
		return sensor.setup();
	};
	//Run sensor read
	bool sensorRead() {
		return sensor.read();
	};
	//Run sensor print data
	void printData() {
		sensor.print_data();
	};
	//Run sensor timer
	void execTimer() {
		sensor.exec_timer();
	};
	//Clear sensor data
	void clearData() {
		clear_data(
			sensor.data,
			sensor.time_data,
			sensor.counter_col,
			sensor.counter_row,
			sensor.counter_col_overflow,
      NUM_IN_ARRAY(sensor.data)
		);
	}
	//Update data values
	void updateValues(byte data[][COL_LENGTH][8], byte time_data[][8]) {
		for (int j = 0; j < COL_LENGTH; j++)
		{
			for (int k = 0; k < 8; k++)
			{
				for (int i = 0; i < getNumberOfVariables(); i++)
				{
					data[i][j][k] = sensor.data[i][j][k];
				}
				time_data[j][k] = sensor.time_data[j][k];
			}
		}
	};
private:

	int global_id;
	int PIN;
};

#endif
