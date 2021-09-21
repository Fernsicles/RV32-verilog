`default_nettype none

module ALU(
    input  var logic [2:0] i_op,
    input  var logic i_op2,
    input  var logic [31:0] i_x,
    input  var logic [31:0] i_y,
    output var logic [31:0] o_result,
    output var logic o_zero
);

    always_comb begin
        case(i_op)
            3'b000: case(i_op2)
                1'b0: o_result = i_x + i_y;
                1'b1: o_result = i_x - i_y;
            endcase
            3'b001: o_result = i_x << i_y;
            3'b010: o_result = {31'b0, $signed(i_x) < $signed(i_y)};
            3'b011: o_result = {31'b0, $unsigned(i_x) < $unsigned(i_y)};
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
