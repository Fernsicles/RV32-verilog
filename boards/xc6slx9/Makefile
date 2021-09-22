NAME=top
ISEPATH=/opt/Xilinx/14.7/ISE_DS/ISE/bin/lin64/

$(NAME): build/$(NAME)_par.bit
	cd build && sudo xc6slx9_board.sh "pld load 0 $(NAME)_par.bit; exit"

build/$(NAME)_par.bit: build/$(NAME)_par.ncd
	cd build && $(ISEPATH)/bitgen -w $(NAME)_par.ncd

build/$(NAME)_par.ncd: build/$(NAME).ncd
	cd build && $(ISEPATH)par -w $(NAME).ncd $(NAME)_par.ncd

build/$(NAME).ncd: build/$(NAME).ngd
	cd build && $(ISEPATH)map -w $(NAME).ngd

build/$(NAME).ngd: build/$(NAME).edif $(NAME).ucf
	cd build && $(ISEPATH)ngdbuild -p 6slx9tqg144-2 -uc ../$(NAME).ucf $(NAME).edif

build/$(NAME).edif: $(NAME).sv inst.hex build/
	cd build && yosys -p 'read_verilog -sv ../$(NAME).sv; synth_xilinx -edif $(NAME).edif'

build/:
	mkdir build

clean:
	rm -rf build
