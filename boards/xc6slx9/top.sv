`include "../../CPU.sv"

module top (
	input wire i_clk,
	output wire [7:0] o_led
);
	reg [7:0] mems [0:256];
	reg [31:0] insts [0:128];
	initial begin
		$readmemh("inst.hex", insts);
	end
	
	reg [31:0] inst;
	wire [31:0] pc;
	wire [31:0] addr;
	wire [31:0] data;
	wire [1:0] memsize;
	wire memwrite;
	wire memload;
	CPU cpu(.i_clk(i_clk), .i_inst(inst), .i_mem({mems[addr + 3], mems[addr + 2], mems[addr + 1], mems[addr]}), .o_write(memwrite), .o_load(memload), .o_pc(pc), .o_mem(data), .o_addr(addr), .o_memsize(memsize));

	// o_led = mems[0];
	assign inst = insts[pc >> 2];
	// assign o_led = 8'b11111111;
	always_ff @(posedge i_clk) begin
		o_led <= mems[0];
		// inst <= insts[pc];
		if(memwrite) begin
			case(memsize)
				2'b01: mems[addr] <= data[7:0];
				2'b10: mems[addr:addr + 1] <= data[15:0];
				2'b11: mems[addr:addr + 3] <= data[31:0];
				default:;
			endcase
		end
	end

	// reg [22:0] counter;
	// wire clk;
	// assign clk = counter[20];
	// always_ff @(posedge i_clk) begin
	// 	counter <= counter + 23'h1;
	// 	if(counter >= 23'd5000000) begin
	// 		counter <= 23'h0;
	// 	end
	// end
endmodule