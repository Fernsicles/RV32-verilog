`default_nettype none

module MALU(
    input  var logic [2:0]  i_op,
    input  var logic [31:0] i_x,
    input  var logic [31:0] i_y,
    output var logic [31:0] o_res
);

logic [63:0] x = fx(i_op, i_x, i_y);
logic [63:0] y = fy(i_op, i_x, i_y);
logic [63:0] prod = x * y;
logic [31:0] quotient = x[31:0] / y[31:0];
logic [31:0] remainder = x[31:0] % y[31:0];

assign o_res = res(i_op, i_x, i_y, prod, quotient, remainder);

function automatic [63:0] fx(
    input [2:0]  i_op,
    input [31:0] i_x,
    input [31:0] i_y
);
    case(i_op)
        3'b010: begin
                    fx = $signed({{32{i_x[31]}}, i_x});
                end
        3'b011,
        3'b101,
        3'b111: begin
                    fx = $unsigned({32'b0, i_x});
                end
        default: begin
                    fx = $signed({{32{i_x[31]}}, i_x});
                end
    endcase
endfunction

function automatic [63:0] fy(
    input [2:0]  i_op,
    input [31:0] i_x,
    input [31:0] i_y
);
    case(i_op)
        3'b010: begin
                    fy = $unsigned({32'b0, i_y});
                end
        3'b011,
        3'b101,
        3'b111: begin
                    fy = $unsigned({32'b0, i_y});
                end
        default: begin
                    fy = $signed({{32{i_x[31]}}, i_y});
                end
    endcase
endfunction

function automatic [31:0] res(
    input [2:0]  i_op,
    input [31:0] i_x,
    input [31:0] i_y,
    input [63:0] prod,
    input [31:0] quotient,
    input [31:0] remainder
);
    case(i_op)
        3'b000: res = prod[31:0];  // MUL
        3'b001,                    // MULH
        3'b010,                    // MULHSU
        3'b011: res = prod[63:32]; // MULHU
        3'b100: begin              // DIV
            if(i_y == 32'b0) begin
                res = -32'h1;
            end else if(i_x == 32'h80000000 && i_y == -32'b1) begin
                res = -32'b1 << 31;
            end else begin
                res = quotient;
            end
        end
        3'b101: begin              // DIVU
            if(i_y == 32'b0) begin
                res = -32'h1;
            end else begin
                res = quotient;
            end
        end
        3'b110: begin              // REM
            if(i_y == 32'b0) begin
                res = i_x;
            end else if(i_x == 32'h80000000 && i_y == -32'b1) begin
                res = 32'b0;
            end else begin
                res = remainder;
            end
        end
        3'b111: begin              // REMU
            if(i_y == 32'b0) begin
                res = i_x;
            end else begin
                res = remainder;
            end
        end
        default: res = 32'b0;
    endcase
endfunction
endmodule