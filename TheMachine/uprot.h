#include <stdint.h>

class DataCtrl{
public:
	int16_t joy_x;
	int16_t joy_y;
    uint16_t joy_sel;
  
    uint16_t key_a;
    uint16_t key_b;

	int16_t acc_x;
	int16_t acc_y;
	int16_t acc_z;

	void setJoy(int16_t joy_x, int16_t joy_y, bool joy_sel);
	void setKeys(bool key_a, bool key_b);
	void setAcc(int16_t acc_x, int16_t acc_y, int16_t acc_z);

};

class DataResp{
public:
  float battery;
	uint16_t delta_time;
	int16_t servo_a;
	int16_t servo_b;
};
