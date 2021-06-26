`include "ALU.sv"
`include "register.sv"
`include "control_unit.sv"

module CPU(
	input wire i_clk,           // The clock signal
	input wire [31:0] i_inst,   // The instruction to execute
	input wire [31:0] i_mem,    // The data to be loaded from memory
	output wire o_write,        // Whether to write to memory or not
	output wire [31:0] o_pc,    // The next instruction to fetch
	output wire [31:0] o_mem,   // The data to write to memory
	output wire [31:0] o_addr,  // The memory address to write to
	output reg  [1:0] o_memsize // The size of memory to be written
);

	reg [31:0] pc; // Program counter register, keeps track of current instruction

	// Control unit
	wire [31:0] c_imm;
	wire c_rsel;
	control_unit control(i_inst, a_op, a_op2, a_ysel, r_write, c_imm, o_write, c_rsel);

	// Register file connections
	wire [4:0] r_raddr1;
	wire [4:0] r_raddr2;
	wire [4:0] r_waddr;
	reg  [31:0] r_wdata;
	wire r_write;
	wire [31:0] r_rdata1;
	wire [31:0] r_rdata2;
	assign r_raddr1 = i_inst[19:15];
	assign r_raddr2 = i_inst[24:20];
	assign r_waddr  = i_inst[11:7];
	assign o_mem    = r_rdata2[31:0];
	register rfile(i_clk, r_raddr1, r_raddr2, r_waddr, r_wdata, r_write, r_rdata1, r_rdata2);

	// ALU connections
	wire [2:0] a_op;   // ALU operation
	wire a_op2;        // Secondary operation (subtraction/shifts)
	wire [31:0] a_x;   // First operand
	reg  [31:0] a_y;   // Second operand
	wire [31:0] a_res; // Result of ALU operation
	wire a_ysel;       // Source of the second operand, immediate or register
	wire a_zero;       // Whether the result of the ALU op was 0 or not
	assign a_x = r_rdata1;
	ALU alu(a_op, a_op2, a_x, a_y, a_res, a_zero);

	always_comb begin
		// Source of second ALU operand
		if(a_ysel)
			a_y = c_imm;    // Immediate
		else
			a_y = r_rdata2; // Register
		
		// Source of data to write to register
		if(c_rsel) // Load from memory
			case(i_inst[14:12]) // Choose size of data to be loaded
				3'b000:  r_wdata = {{24{i_mem[7]}},  i_mem[7:0]};  // 8-bits
				3'b001:  r_wdata = {{16{i_mem[15]}}, i_mem[15:0]}; // 16-bits
				3'b010:  r_wdata = i_mem;                          // 32-bits
				3'b100:  r_wdata = {24'b0, i_mem[7:0]};            // 8-bits unsigned
				3'b101:  r_wdata = {16'b0, i_mem[15:0]};           // 16-bits unsigned
				default: r_wdata = 32'b0;                          // Load 0 by default (there needs to be a default)
			endcase
		else // Load the ALU result otherwise
			r_wdata = a_res;

		// Memory output, sets memsize to the appropriate value for the amount to be stored:
		// 01 for 8 bits, 10 for 16 bits, 11 for 32 bits, and 00 for no store.
		case(i_inst[14:12])
			3'b000:  o_memsize = 2'b01;
			3'b001:  o_memsize = 2'b10;
			3'b010:  o_memsize = 2'b11;
			default: o_memsize = 2'b00;
		endcase
	end

	// Update PC and instruction fetch
	assign o_pc = pc;
	always_ff @(posedge i_clk) begin
		pc <= pc + 4;
	end
endmodule