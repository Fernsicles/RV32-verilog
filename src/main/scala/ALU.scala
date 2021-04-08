package RVCPU

import chisel3._
import chisel3.util._

class ALU extends Module {
	val io = IO(new Bundle {
		val i_op = Input(UInt(2.W))
		val i_op2 = Input(UInt(1.W))
		val i_x = Input(UInt(32.W))
		val i_y = Input(UInt(32.W))
		val o_res = Output(UInt(32.W))
		val o_zero = Output(Bool())
	})

	io.o_res := 0.U;
	switch(io.i_op) {
		is("b000".U) {
			switch(io.i_op2) {
				is("b0".U) {io.o_res := (io.i_x.asSInt + io.i_y.asSInt).asUInt}
				is("b1".U) {io.o_res := (io.i_x.asSInt - io.i_y.asSInt).asUInt}
			}
		}
		is("b001".U) {io.o_res := io.i_x << io.i_y(4, 0)}
		is("b010".U) {io.o_res := (io.i_x.asSInt < io.i_y.asSInt).asUInt}
		is("b011".U) {io.o_res := io.i_x < io.i_y}
		is("b100".U) {io.o_res := io.i_x ^ io.i_y}
		is("b101".U) {
			switch(io.i_op2) {
				is("b0".U) {io.o_res := io.i_x >> io.i_y}
				is("b1".U) {io.o_res := (io.i_x.asSInt >> io.i_y).asUInt}
			}
		}
		is("b110".U) {io.o_res := io.i_x | io.i_y}
		is("b111".U) {io.o_res := io.i_x & io.i_y}
	}

	io.o_zero := ~io.o_res.orR
}

object ALU extends App {
	chisel3.Driver.execute(args, () => new ALU)
}