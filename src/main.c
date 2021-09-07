#include <reg52.h>
#include <intrins.h>

//#define __DEV__

#ifdef __DEV__
// direct the output to P1 and add an extra delay after each command to help debugging.
sbit DATA = P1 ^ 0;
sbit LATCH = P1 ^ 2;
sbit SHIFT = P1 ^ 4;
#define _dev_delay_step() delay(20000)
#define set_data(x) { DATA=((x) ? 0 : 1); _dev_delay_step(); }
#define set_latch(x) { LATCH=((x) ? 0 : 1); _dev_delay_step(); }
#define set_shift(x) { SHIFT=((x) ? 0 : 1); _dev_delay_step(); }

#else
sbit DATA = P2 ^ 0;
sbit LATCH = P2 ^ 2;
sbit SHIFT = P2 ^ 4;
#define set_data(x) DATA=x
#define set_latch(x) LATCH=x
#define set_shift(x) SHIFT=x
#endif




void delay(unsigned int n){
	while(n--);
}

unsigned char animation_type = 0;
unsigned char button_pressed = 0;

/**
 * Send row and column data through 74HC595.
 * Row order: top layer => bottom layer
 * Column order: clockwise
 */
void send_data(unsigned int row, unsigned int column) {
	int i;
	
	set_latch(0);
	
	// row first, then column
	// row is active low, column is active high
	
	row = ~row;
	
	for (i = 0; i < 16; i++) {
		set_shift(0);
		set_data(row & 0x0001);
		row = row >> 1;
		set_shift(1);
	}

	for (i = 0; i < 16; i++) {
		set_shift(0);
		set_data(column & 0x0001);
		column = column >> 1;
		set_shift(1);
	}

	set_latch(1);
	set_shift(0);
	set_latch(0);
}

// Animation 0: Flash all LEDs
void animation_0() {
	unsigned int counter = 0;
	unsigned int row = 0x5555;
	while(1) {
		if (animation_type != 0) return;
		
		send_data(row, 0xffff);
		if (counter == 4) {
			row = _iror_(row, 1);
			counter = 0;
		}
		counter++;
		delay(60000);
	}
}

// Animation 1: two rotating lines
void animation_1() {
	unsigned int column = 0x0101;
	while(1) {
		if (animation_type != 1) return;
		
		send_data(0xffff, column);
		column = _irol_(column, 1);

		delay(15000);
	}
}

// Animation 2: two rotating lines with after image
void animation_2() {
	unsigned int column = 0x0f0f;
	
	while(1) {
		if (animation_type != 2) return;
		
		send_data(0xffff, column);
		column = _irol_(column, 1);

		delay(10000);
	}	
}

// Animation 3: rings fall down and stack
void animation_3() {
	unsigned int row = 0x0001;
	int i = 0;
  int	j = 15;
	int fast_flashing_counter = 16;
	
	while(1) {
		if (animation_type != 3) return;
		send_data(row, 0xffff);
		if (i < j) {
			row = row & ~(1 << i);
			i++;
			row = row | (1 << i);
		} else {
			j--;
			// if last row completed, reset
			if (j == 0) {
				
				// all flash 4 times
				for (i = 0; i < 4; i++) {
					send_data(0xffff, 0xffff);
					delay(20000);
					send_data(0x0000, 0x0000);
					delay(20000);
				}
				
				row = 0x0001;
				i = 0;
				j = 15;
			} else {
				i = 0;
				row = row | (1 << i);
			}
		}
		
		fast_flashing_counter--;
		if (!fast_flashing_counter) {
			// turn off all LED to prevent built-in fast flashing
			send_data(0x0000, 0x0000);
			send_data(row, 0xffff);
			fast_flashing_counter = 16;
		}
		
		delay(10000);
	}
}

// Animation 4: large ring rises up
void animation_4() {
	unsigned int row = 0xf000;
	
	while(1) {
		if (animation_type != 4) return;
		
		send_data(row, 0xffff);
		
		row = _iror_(row, 1);
		delay(17000);
	}
}

// Animation 5: four rings moving up
void animation_5() {
	unsigned int row = 0x1111;
	unsigned int counter = 0;
	
	while(1) {
		if (animation_type != 5) return;

		send_data(row, 0xffff);
		if (counter == 4) {
			row = _iror_(row, 1);
			counter = 0;
		}
		
		delay(60000);
		counter++;
	}
}

// Animation 6: two rings moving from center to top/bottom
void animation_6() {
	
	unsigned char row_top = 0xc0;
	unsigned char row_bottom = 0x03;
	unsigned int row;
	
	while(1) {
		if (animation_type != 6) return;
		
		row = row_top | (row_bottom << 8);
		send_data(row, 0xffff);
		row_top = _cror_(row_top, 1);
		row_bottom = _crol_(row_bottom, 1);
		delay(40000);
	}
}

// Animation 7: two rings moving from top/bottom to center
void animation_7() {
	
	unsigned char row_top = 0x03;
	unsigned char row_bottom = 0xc0;
	unsigned int row;
	
	while(1) {
		if (animation_type != 7) return;
		
		row = row_top |(row_bottom << 8);
		send_data(row, 0xffff);
		row_top = _crol_(row_top, 1);
		row_bottom = _cror_(row_bottom, 1);
		delay(40000);
	}
}


void main(){
	// enable EX0 for the switch button
	EA = 1;
	EX0 = 1;
	TCON = 0x01;
	
	while(1){
		delay(50000);
		button_pressed = 0;
		if (animation_type == 0) {
			animation_0();
		} else if (animation_type == 1) {
			animation_1();
		} else if (animation_type == 2) {
			animation_2();
		} else if (animation_type == 3) {
			animation_3();
		} else if (animation_type == 4) {
			animation_4();
		} else if (animation_type == 5) {
			animation_5();
		} else if (animation_type == 6) {
			animation_6();
		} else if (animation_type == 7) {
			animation_7();
		}
	}
}

// Switch Animation button
void switch_button_handler() interrupt 0 {
	if (!button_pressed) {
		animation_type++;
		if (animation_type == 8) {
			animation_type = 0;
		}
		button_pressed = 1;
	}
}
