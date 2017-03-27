#define CSN 47
#define CE 46
#define IRQ 45

// Arduino pins for the shift register
#define MLATCH 12
#define MCLK 4
#define MENABLE 7
#define MDATA 8

// 8-bit bus after the 74HC595 shift register
// (not Arduino pins)
// These are used to set the direction of the bridge driver.
#define M1_A 2
#define M1_B 3
#define M2_A 1
#define M2_B 4
#define M3_A 5
#define M3_B 7
#define M4_A 0
#define M4_B 6

// Arduino pins for the PWM signals.
#define M1_PWM 11
#define M2_PWM 3
#define M3_PWM 6
#define M4_PWM 5
#define SERVO1 10
#define SERVO2 9

// Codes for the motor function.
#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4
