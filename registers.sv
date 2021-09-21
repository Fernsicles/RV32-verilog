`default_nettype none

module registers(
    input  var logic i_clk,
    input  var logic [4:0] i_raddr1,
    input  var logic [4:0] i_raddr2,
    input  var logic [4:0] i_waddr,
    input  var logic [31:0] i_wdata,
    input  var logic i_write,
    output var logic [31:0] o_rdata1,
    output var logic [31:0] o_rdata2,
`ifndef YOSYS // Debug stuff to be used with simulator
    input  var logic i_dload,
    input  var logic i_daddr,
    input  var logic [31:0] i_ddata,
    output var logic [31:0] o_regs [31:0]
`endif
);

    logic [31:0] registers [31:1];

    always_comb begin
        if(i_raddr1 == 0) begin
            o_rdata1 = 32'b0;
        end else begin
            o_rdata1 = registers[i_raddr1];
        end if(i_raddr2 == 0) begin
            o_rdata2 = 32'b0;
        end else begin
            o_rdata2 = registers[i_raddr2];
        end
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