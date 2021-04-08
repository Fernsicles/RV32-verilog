module ALU(
	input logic[2:0] i_op,
	input logic i_op2,
	input logic[31:0] i_x,
	input logic[31:0] i_y,
	output wire[31:0] o_result,
	output wire o_zero
	);

	always_comb
	begin
		case(i_op)
			3'b000: case(i_op2)
				1'b0: o_result = i_x + i_y;
				1'b1: o_result = i_x - i_y;
			endcase
			3'b001: o_result = i_x << i_y;
			3'b010: o_result = {31'b0, $signed(i_x) < $signed(i_y)};
			3'b011: o_result = {31'b0, i_x < i_y};
			3'b100: o_result = i_x ^ i_y;
			3'b101: case(i_op2)
				1'b0: o_result = i_x >> i_y;
				1'b1: o_result = $signed(i_x) >>> i_y;
			endcase
			3'b110: o_result = i_x | i_y;
			3'b111: o_result = i_x & i_y;
			default: o_result = 0;
		endcase
		o_zero = ~(|o_result);
	end
endmodule