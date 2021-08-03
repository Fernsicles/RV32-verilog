module registers(
	input wire i_clk,
	input wire [4:0] i_raddr1,
	input wire [4:0] i_raddr2,
	input wire [4:0] i_waddr,
	input wire [31:0] i_wdata,
	input wire i_write,
	output reg [31:0] o_rdata1,
	output reg [31:0] o_rdata2,
`ifndef YOSYS // Debug stuff to be used with simulator
	input  wire i_dload,
	input  wire i_daddr,
	input  wire [31:0] i_ddata,
	output wire [31:0] o_regs [31:0]
`endif
);

	reg [31:0] registers [31:1];

	always_comb begin
		if(i_raddr1 == 0)
			o_rdata1 = 32'b0;
		else
			o_rdata1 = registers[i_raddr1];
		if(i_raddr2 == 0)
			o_rdata2 = 32'b0;
		else
			o_rdata2 = registers[i_raddr2];
	end

	always_ff @(posedge i_clk) begin
		if(i_write) begin
			registers[i_waddr] <= i_wdata;
		end
	end

`ifndef YOSYS
	assign o_regs[0] = 32'b0;
	assign o_regs[31:1] = registers;
	always_ff @(posedge i_clk) begin
		if(i_write) begin
			registers[i_daddr] <= i_ddata;
		end
	end
`endif
endmodule