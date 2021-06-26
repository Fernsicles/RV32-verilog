module register(
	input wire i_clk,
	input wire[4:0] i_raddr1,
	input wire[4:0] i_raddr2,
	input wire[4:0] i_waddr,
	input wire[31:0] i_wdata,
	input wire i_write,
	output wire[31:0] o_rdata1,
	output wire[31:0] o_rdata2
);

	reg[31:0] registers[31:1];

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

endmodule