module MALU(
    input  [2:0]  i_op,
    input  [31:0] i_x,
	input  [31:0] i_y,
	output logic [31:0] o_result
);

logic [63:0] x, y;
logic [63:0] prod = x * y;

always_comb begin
    case(i_op)
        3'b010: begin x = $signed(i_x); y = $unsigned(i_y); end
        3'b011: begin x = $unsigned(i_x); y = $unsigned(i_y); end
        default: begin x = $signed(i_x); y = $signed(i_y); end
    endcase

    case(i_op)
        3'b000: o_result = prod[31:0];  // MUL
        3'b001,                         // MULH
        3'b010,                         // MULHSU
        3'b011: o_result = prod[63:32]; // MULHU
        default: o_result = 32'b0;
    endcase
end

endmodule