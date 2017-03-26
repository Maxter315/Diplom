#include "uprot.h"

void DataCtrl::setJoy(int16_t x, int16_t y, bool sel){
	joy_x = x;
	joy_y = y;
	joy_sel = sel;
}

void DataCtrl::setKeys(bool a, bool b){
	key_a = a;
	key_b = b;
}

void DataCtrl::setAcc(int16_t x, int16_t y, int16_t z){
	acc_x = x;
	acc_y = y;
	acc_z = z;
}
