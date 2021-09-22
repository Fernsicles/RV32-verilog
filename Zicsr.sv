`default_nettype none

module Zicsr(
    input  var logic i_clk,
    input  var logic [4:0] i_raddr,
    input  var logic [4:0] i_waddr,
    input  var logic [31:0] i_wdata,
    input  var logic i_write,
    output var logic [31:0] o_rdata
);
endmodule