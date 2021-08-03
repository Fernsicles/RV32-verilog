COMPILER     ?= g++
OPTIMIZATION ?= -O3 -g
CPPFLAGS     ?= -std=c++17 $(OPTIMIZATION)
GUISRC       := $(shell find gui -name \*.cpp)
GUIOBJ       := $(GUISRC:.cpp=.o)
DEPS         := gtk4 gtkmm-4.0 x11
DEPCFLAGS    := $(shell pkg-config --cflags $(DEPS))
DEPLIBS      := $(shell pkg-config --libs   $(DEPS))
OUTPUT       := cpugui

.PHONY: all test clean

all: $(OUTPUT)

test: $(OUTPUT)
	./$(OUTPUT)

clean:
	rm -rf obj_dir verilated.o CPU_sim gui/**/*.o gui/*.o $(OUTPUT)

obj_dir/VALU.h: ALU.sv
	verilator -Wall -cc ALU.sv
	cd obj_dir && make -f VALU.mk

alutest: obj_dir/VALU.h ALUtest.cpp
	$(COMPILER) $(CPPFLAGS) -Iobj_dir $(shell pkg-config --cflags verilator) ALUtest.cpp /usr/share/verilator/include/verilated.cpp obj_dir/VALU__ALL.o -o alutest

obj_dir/VCPU.h: CPU.sv control_unit.sv registers.sv ALU.sv
ifeq ($(TEST), 1)
	verilator $(OPTIMIZATION) -cc CPU.sv
	cd obj_dir && make -f VCPU.mk
else
	verilator -Wall $(OPTIMIZATION) -cc CPU.sv
	cd obj_dir && make -f VCPU.mk
endif

verilated.o: /usr/share/verilator/include/verilated.cpp
	$(COMPILER) $(CPPFLAGS) -c /usr/share/verilator/include/verilated.cpp

gui/%.o: gui/%.cpp
	@ printf "\e[2m[\e[22;32mcc\e[39;2m]\e[22m $< \e[2m$(CPPFLAGS)\e[22m\n"
	@ $(COMPILER) $(CPPFLAGS) $(DEPCFLAGS) -Iinclude -c $< -o $@

$(OUTPUT): $(GUIOBJ)
	@ printf "\e[2m[\e[22;36mld\e[39;2m]\e[22m $@\n"
	@ $(COMPILER) $^ -o $@ $(DEPLIBS)

CPU_sim: obj_dir/VCPU.h CPU_sim.cpp verilated.o
	$(COMPILER) $(CPPFLAGS) -Wall -pthread -Dcimg_display=1 -Iobj_dir $(shell pkg-config --cflags verilator) CPU_sim.cpp verilated.o obj_dir/VCPU__ALL.o $(shell pkg-config --libs x11) -o CPU_sim

DEPFILE  := .dep
DEPTOKEN := "\# MAKEDEPENDS"

depend:
	@ echo $(DEPTOKEN) > $(DEPFILE)
	makedepend -f $(DEPFILE) -s $(DEPTOKEN) -- $(COMPILER) $(CPPFLAGS) -Iinclude -- $(GUISRC) 2>/dev/null
	@ rm $(DEPFILE).bak

sinclude $(DEPFILE)
