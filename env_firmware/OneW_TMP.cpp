#include "OneW_TMP.h"
#include "global.h"


//OneW constructor 
OneW_TMP::OneW_TMP():oneWire(16) {

	current_temperature[0] = 0;
	current_temperature[1] = 0;
	current_temperature[2] = 0;
}

/*
 *  Function:       bool OneW_TMP::setup()
 *  Description:    setup for OneWire sensor
 */
bool OneW_TMP::setup() {

	bool scan = true;
	gpio_sharing_counter++;//increment upon activation
  digitalWrite(19, gpio_sharing_counter);
	delay(300);
	while (scan) {
		//Scan for the next address
		if (!oneWire.search(addr[n_sensors])) {
			oneWire.reset_search();
			delay(250);
			scan = false;
		}
		else {
			//Check id valid address
			if (OneWire::crc8(addr[n_sensors], 7) != addr[n_sensors][7]) {
				scan = false;
			}
			else
			{
				//Test read
				oneWire.reset();
				oneWire.select(addr[n_sensors]);
				oneWire.write(0x44, 1);
				delay(1000);
				byte present = oneWire.reset();
				oneWire.select(addr[n_sensors]);
				oneWire.write(0xBE);

				byte _data[12];
				for (int i = 0; i < 9; i++) {           // we need 9 bytes
					_data[i] = oneWire.read();
				}
				OneWire::crc8(_data, 8);
				int16_t raw = (_data[1] << 8) | _data[0];
				byte cfg = (_data[4] & 0x60);
				if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
				else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
				else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms

				current_temperature[n_sensors] = (float)raw / 16.0;
				n_sensors++;
			}

		}
		gpio_sharing_counter--;//decrement upon deactivation
    digitalWrite(19, gpio_sharing_counter);

		if (n_sensors > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
    
} // end of setup()

/*
 *  Function:       bool OneW_TMP::read()
 *  Description:    read all sensor data 
 */
bool OneW_TMP::read() { 
	
	//Request temperature
	for (int i = 0; i < n_sensors; i++)
	{
		read_temperature(i);
	}
	return true;

} // end of read()

/*
 *  Function:       void  OneW_TMP::print_data()
 *  Description:    print all sensor data
 */
void OneW_TMP::print_data() {

    print_debug(data,                               // data where we store values
                time_data,                          // where we store time data
				OneW_TMP_num_of_variables,           // number of sensor we use
                counter_col,                        // counter for coloumn index
                counter_row,                        // counter for row index
                counter_col_overflow);              // flag for coloumn counter overflow

} // end of print_data()

/*
 *  Function:     OneW_TMP::exec_timer()
 *  Description: execute read if the timer is right
 */
bool OneW_TMP::exec_timer() {

    if(millis() - exec_timer_last >= exec_time) {
        
        if(read()) {
            //print_data();
            #ifdef debug
                serial_debug.println("TSL (exec_timer) - executed read");
            #endif

            exec_timer_last = millis();

            return true;
        }
        return false;
        
    }

    return false;
} // end of exec_timer()

/**************************************************************************************/

/*
 *  Function:       bool  OneW_TMP::read_temperature()
 *  Description:    read the raw temperature value
 */
bool OneW_TMP::read_temperature(int device_idx) {
	gpio_sharing_counter++;//increment upon activation
  digitalWrite(19, gpio_sharing_counter);
	delay(300);

	oneWire.reset();
	oneWire.select(addr[device_idx]);
	oneWire.write(0x44, 1);
	delay(1000);
	byte present = oneWire.reset();
	oneWire.select(addr[device_idx]);
	oneWire.write(0xBE);

	byte _data[12];
	for (int i = 0; i < 9; i++) {           // we need 9 bytes
		_data[i] = oneWire.read();
	}
	OneWire::crc8(_data, 8);
	int16_t raw = (_data[1] << 8) | _data[0];
	byte cfg = (_data[4] & 0x60);
	if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
	else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
	else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms

	current_temperature[device_idx] = (float)raw / 16.0*100;
  gpio_sharing_counter--;//decrement upon deactivation
  digitalWrite(19, gpio_sharing_counter);

    #ifdef debug
        serial_debug.print("OneWire (read_temperature) - temperature:"); 
        serial_debug.println(current_temperature[device_idx]/100);
    #endif

		int send_temp = (int)(current_temperature[device_idx]);

    // updating the data into our data array
    update_16bit(data,                          // our data array
                time_data,                      // our time data array
                (uint8_t)device_idx,            // sensor ID
                counter_col,                    // coloumn counter
                counter_row,                    // row counter
                counter_col_overflow,           // check if coloumn has overflow 
                OneW_TMP_num_of_variables,      // number of variables
				send_temp,                      // the value
                exec_timer_last);               // the last time
                
  return true;

} // end of read_luminosity()
