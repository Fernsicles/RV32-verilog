#include "obj_dir/VALU.h"
#include "verilated.h"
#include <string>
#include <random>
#include <iostream>

using namespace std;

bool add_sub(VALU *tb) {
	random_device rand;
	tb->i_op = 0b000;
	tb->i_op2 = 0b0;
	for(int i = 0; i < 10000; i++) {
		int x = rand();
		int y = rand();
		tb->i_x = x;
		tb->i_y = y;
		tb->eval();
		if(tb->o_result != x + y) {
			cout << "Failed: Add\t" << x << " + " << y << " = " << tb->o_result << endl;
			cout << "Expected: " << x + y << endl;
			return false;
		}
	}
	cout << "Passed: Add" << endl;

	tb->i_op2 = 0b1;
	for(int i = 0; i < 10000; i++) {
		int x = rand();
		int y = rand();
		tb->i_x = x;
		tb->i_y = y;
		tb->eval();
		if(tb->o_result != x - y) {
			cout << "Failed: Sub\t" << x << " - " << y << " = " << tb->o_result << endl;
			cout << "Expected: " << x - y << endl;
			return false;
		}
	}
	cout << "Passed: Sub" << endl;
	return true;
}

bool shift_left(VALU *tb) {
	random_device rand;
	tb->i_op = 0b001;
	for(int i = 0; i < 10000; i++) {
		int x = rand();
		int y = rand() % 32;
		tb->i_x = x;
		tb->i_y = y;
		tb->eval();
		int res = x << y;
		if(tb->o_result != res) {
			cout << "Failed: Sll\t" << x << " << " << y << " = " << tb->o_result << endl;
			cout << "Expected: " << res << endl;
			return false;
		}
	}
	cout << "Passed: Sll" << endl;
	return true;
}

bool set_less(VALU *tb) {
	random_device rand;
	tb->i_op = 0b010;
	for(int i = 0; i < 10000; i++) {
		int x = rand();
		int y = rand();
		tb->i_x = x;
		tb->i_y = y;
		tb->eval();
		int res = x < y;
		if(tb->o_result != res) {
			cout << "Failed: Slt\t" << x << " < " << y << " = " << tb->o_result << endl;
			cout << "Expected: " << res << endl;
			return false;
		}
	}
	cout << "Passed: Slt" << endl;
	return true;
}

bool set_less_unsigned(VALU *tb) {
	random_device rand;
	tb->i_op = 0b011;
	for(int i = 0; i < 10000; i++) {
		unsigned int x = rand();
		unsigned int y = rand();
		tb->i_x = x;
		tb->i_y = y;
		tb->eval();
		int res = x < y;
		if(tb->o_result != res) {
			cout << "Failed: Sltu\t" << x << " < " << y << " = " << tb->o_result << endl;
			cout << "Expected: " << res << endl;
			return false;
		}
	}
	cout << "Passed: Sltu" << endl;
	return true;
}

bool exclusive_or(VALU *tb) {
	random_device rand;
	tb->i_op = 0b100;
	for(int i = 0; i < 10000; i++) {
		unsigned int x = rand();
		unsigned int y = rand();
		tb->i_x = x;
		tb->i_y = y;
		tb->eval();
		int res = x ^ y;
		if(tb->o_result != res) {
			cout << "Failed: Xor\t" << x << " ^ " << y << " = " << tb->o_result << endl;
			cout << "Expected: " << res << endl;
			return false;
		}
	}
	cout << "Passed: Xor" << endl;
	return true;
}

bool shift_right(VALU *tb) {
	random_device rand;
	tb->i_op = 0b101;
	tb->i_op2 = 0b0;
	for(int i = 0; i < 10000; i++) {
		unsigned int x = rand();
		unsigned int y = rand() % 32;
		tb->i_x = x;
		tb->i_y = y;
		tb->eval();
		unsigned int res = x >> y;
		if(tb->o_result != res) {
			cout << "Failed: Srl\t" << x << " >> " << y << " = " << tb->o_result << endl;
			cout << "Expected: " << res << endl;
			return false;
		}
	}
	cout << "Passed: Srl" << endl;
	
	tb->i_op2 = 0b1;
	for(int i = 0; i < 10000; i++) {
		int x = rand();
		int y = rand() % 32;
		tb->i_x = x;
		tb->i_y = y;
		tb->eval();
		int res = x >> y;
		if(tb->o_result != res) {
			cout << "Failed: Sra\t" << x << " >> " << y << " = " << tb->o_result << endl;
			cout << "Expected: " << res << endl;
			return false;
		}
	}
	cout << "Passed: Sra" << endl;
	return true;
}

int main(int argc, char **argv) {
	Verilated::commandArgs(argc, argv);
	VALU tb;

	int op;

	if(argc == 1) {
		op = -1;
	} else {
		op = stoi(argv[1]);
	}

	switch(op) {
		case 0b000:
			add_sub(&tb);
			break;
		case 0b001:
			shift_left(&tb);
			break;
		case 0b010:
			set_less(&tb);
			break;
		case 0b011:
			set_less_unsigned(&tb);
			break;
		case 0b100:
			exclusive_or(&tb);
			break;
		case 0b101:
			shift_right(&tb);
			break;
		case 0b110:
		case 0b111:
		default:
			add_sub(&tb);
			shift_left(&tb);
			set_less(&tb);
			set_less_unsigned(&tb);
			exclusive_or(&tb);
			shift_right(&tb);
			break;
	}
}