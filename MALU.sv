module MALU(
    input  [2:0]  i_op,
    input  [31:0] i_x,
    input  [31:0] i_y,
    output logic [31:0] o_res
);

logic [63:0] x, y;
logic [63:0] prod = x * y;
logic [31:0] quotient = x[31:0] / y[31:0];
logic [31:0] remainder = x[31:0] % y[31:0];

always_comb begin
    case(i_op)
        3'b010: begin
                    x = $signed({{32{i_x[31]}}, i_x});
                    y = $unsigned({32'b0, i_y});
                end
        3'b011,
        3'b101,
        3'b111: begin
                    x = $unsigned({32'b0, i_x});
                    y = $unsigned({32'b0, i_y});
                end
        default: begin
                    x = $signed({{32{i_x[31]}}, i_x});
                    y = $signed({{32{i_x[31]}}, i_y});
                end
    endcase

    case(i_op)
        3'b000: o_res = prod[31:0];  // MUL
        3'b001,                      // MULH
        3'b010,                      // MULHSU
        3'b011: o_res = prod[63:32]; // MULHU
        3'b100: begin                // DIV
            if(i_y == 32'b0)
                o_res = -32'h1;
            else if(i_x == 32'h80000000 && i_y == -32'b1)
                o_res = -32'b1 << 31;
            else
                o_res = quotient;
        end
        3'b101: begin                // DIVU
            if(i_y == 32'b0)
                o_res = -32'h1;
            else
                o_res = quotient;
        end
        3'b110: begin                // REM
            if(i_y == 32'b0)
                o_res = i_x;
            else if(i_x == 32'h80000000 && i_y == -32'b1)
                o_res = 32'b0;
            else
                o_res = remainder;
        end
        3'b111: begin                // REMU
            if(i_y == 32'b0)
                o_res = i_x;
            else
                o_res = remainder;
        end
        default: o_res = 32'b0;
    endcase
end

endmodule