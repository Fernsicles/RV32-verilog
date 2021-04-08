package RVCPU
import chisel3._
import chisel3.util._

class registerfile extends Module {
	val io = IO(new Bundle {
		val i_raddr1 = Input(UInt(5.W))
		val i_raddr2 = Input(UInt(5.W))
		val i_waddr = Input(UInt(5.W))
		val i_wdata = Input(UInt(32.W))
		val i_write = Input(Bool())
		val o_rdata1 = Output(UInt(32.W))
		val o_rdata2 = Output(UInt(32.W))
	})

	val mem = Mem(32, UInt(32.W))
	when(io.i_write) {
		mem.write(io.i_waddr, io.i_wdata)
	}

	io.o_rdata1 := mem.read(io.i_raddr1)
	io.o_rdata2 := mem.read(io.i_raddr2)
}

object registerfile extends App {
	chisel3.Driver.execute(args, () => new registerfile)
}