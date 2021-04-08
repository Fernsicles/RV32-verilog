package RVCPU

import org.scalatest._
import chiseltest._
import chisel3._



class ALUTest extends FlatSpec with ChiselScalatestTester {
	def mask(s: Int): Long = (1.toLong << s) - 1
	behavior of "ALU"
	it should "add" in {
		test(new ALU) { c =>
			c.io.i_op.poke(0.U)
			c.io.i_op2.poke(0.U)
			c.io.i_x.poke(8.U)
			for(x <- 0 until 1000) {
				c.io.i_y.poke(x.U)
				c.io.o_res.expect((x + 8).U)
			}
		}
	}
	it should "subtract" in {
		test(new ALU) { c =>
			c.io.i_op.poke(0.U)
			c.io.i_op2.poke(1.U)
			c.io.i_x.poke(8.U)
			for(x <- 0 until 1000) {
				c.io.i_y.poke(x.U)
				c.io.o_res.expect(((8 - x) & mask(32)).U)
			}
		}
	}
	it should "shift left" in {
		test(new ALU) { c =>
			c.io.i_op.poke(1.U)
			c.io.i_op2.poke(0.U)
			c.io.i_x.poke(15.U)
			for(x <- 0 until 32) {
				c.io.i_y.poke(x.U)
				c.io.o_res.expect(((15 << x) & mask(32)).U)
			}
		}
	}
	it should "set less than" in {
		test(new ALU) { c =>
			c.io.i_op.poke(2.U)
			c.io.i_op2.poke(0.U)
			c.io.i_x.poke(0.U)
			for(x <- -500 until 500) {
				c.io.i_y.poke((x & mask(32)).U)
				if(0 < x) {
					c.io.o_res.expect(1.U)
				} else {
					c.io.o_res.expect(0.U)
				}
			}
		}
	}
	it should "set less than unsigned" in {
		test(new ALU) { c =>
			c.io.i_op.poke(2.U)
			c.io.i_op2.poke(0.U)
			c.io.i_x.poke(500.U)
			for(x <- 0 until 1000) {
				c.io.i_y.poke((x & mask(32)).U)
				if(500 < (x & mask(32))) {
					c.io.o_res.expect(1.U)
				} else {
					c.io.o_res.expect(0.U)
				}
			}
			for(x <- -500 until -1) {
				c.io.i_y.poke((x & mask(32)).U)
				c.io.o_res.expect(0.U)
			}
		}
	}
}