module control_unit(
	input wire[31:0] i_inst,
	output wire[2:0] o_op,
	output wire o_op2,
	output wire o_y, // selects whether second ALU operand is register or immediate value
	output wire o_rwrite,
	output wire[31:0] o_imm,
	output wire o_mwrite,
	output wire o_rsel
);
	// ALU operation signal
	always_comb begin
		case(i_inst[6:2])
			5'b00100, 5'b01100: o_op = i_inst[14:12];
			5'b11000: case(i_inst[14:12])
				3'b000, 3'b001: o_op = 3'b000;
				3'b100, 3'b101: o_op = 3'b010;
				3'b110, 3'b111: o_op = 3'b011;
				default: o_op = 3'b000;
			endcase
			default: o_op = 3'b000;
		endcase

		// Secondary op signal
		// For subtraction and right shifts
		case(i_inst[6:2])
			5'b11000: case(i_inst[14:12])
				3'b000, 3'b001: o_op2 = 1;
				default: o_op2 = 0;
			endcase
			5'b00100: o_op2 = i_inst[30] && i_inst[14:12] == 3'b101;
			5'b01100: o_op2 = i_inst[30];
			default: o_op2 = 0;
		endcase

		// ALU operand 2 source select
		case(i_inst[6:2])
			5'b01101, 5'b00101, 5'b11011, 5'b11001, 5'b11000, 5'b0, 5'b01000, 5'b00100: o_y = 1;
			default: o_y = 0;
		endcase

		// Register write signal
		case(i_inst[6:2])
			5'b01101, 5'b00101, 5'b11011, 5'b11001, 5'b0, 5'b00100, 5'b01100, 5'b00011: o_rwrite = 1;
			default: o_rwrite = 0;
		endcase

		// Immediate value
		case(i_inst[6:2])
			5'b01101, 5'b00101: o_imm = {{12{i_inst[31]}}, i_inst[31:12]};
			5'b11011: o_imm = {{12{i_inst[31]}}, {i_inst[31], i_inst[19:12], i_inst[20], i_inst[30:21]}}; // JAL immediate encoding is strange
			5'b00000, 5'b11001: o_imm = {{20{i_inst[31]}}, i_inst[31:20]};
			5'b11000: case(i_inst[14:12])
				3'b110, 3'b111: o_imm = {25'b0, i_inst[31:25]};
				default: o_imm = {{25{i_inst[31]}}, i_inst[31:25]};
			endcase
			5'b01000: o_imm = {{25{i_inst[31]}}, i_inst[31:25]};
			5'b00100: case(i_inst[14:12])
				3'b011: o_imm = {20'b0, i_inst[31:20]};
				3'b001, 3'b101: o_imm = {27'b0, i_inst[24:20]};
				default: o_imm = {{20{i_inst[31]}}, i_inst[31:20]};
			endcase
			default: o_imm = 32'b0;
		endcase

		// Memory write enable
		case(i_inst[6:2])
			5'b01000: o_mwrite = 1'b1;
			default: o_mwrite = 1'b0;
		endcase

		// Register write source select
		case(i_inst[6:2])
			5'b0: o_rsel = 1'b1;
			default: o_rsel = 1'b0;
		endcase
	end
endmodule