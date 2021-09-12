TEST         := 1
COMPILER     ?= g++
OPTIMIZATION ?= -O3
VERILATOROPT ?= -O3
CPPFLAGS     ?= -std=c++17 $(OPTIMIZATION) -Iobj_dir -IELFIO
GUISRC       := $(shell find gui -name \*.cpp) gui/resources.cpp
GUIOBJ       := $(GUISRC:.cpp=.o)
DEPS         := gtk4 gtkmm-4.0 x11 verilator vte-2.91-gtk4
DEPCFLAGS    := $(shell pkg-config --cflags $(DEPS))
DEPLIBS      := $(shell pkg-config --libs   $(DEPS)) -pthread
OUTPUT       := rvgui
GLIB_COMPILE_RESOURCES := $(shell pkg-config --variable=glib_compile_resources gio-2.0)
ifneq ($(RELEASE),1)
DEBUGFLAGS   := -g
else
DEBUGFLAGS   :=
endif
VERILATORDEFINES := -DEXTI -DEXTM

.PHONY: all test debug clean

all: $(OUTPUT)

test: $(OUTPUT)
	./$(OUTPUT)

debug: $(OUTPUT)
	gdb ./$(OUTPUT)

clean:
	rm -rf obj_dir verilated.o CPU_sim gui/**/*.o gui/*.o $(OUTPUT)

scan: clean
	scan-build --use-cc=$(COMPILER) "make all"

scanj: clean
	scan-build --use-cc=$(COMPILER) "make -j all"

obj_dir/VALU.h: ALU.sv
	verilator -Wall -cc ALU.sv
	cd obj_dir && make -f VALU.mk

alutest: obj_dir/VALU.h ALUtest.cpp
	$(COMPILER) $(CPPFLAGS) -Iobj_dir $(shell pkg-config --cflags verilator) ALUtest.cpp /usr/share/verilator/include/verilated.cpp obj_dir/VALU__ALL.o -o alutest

obj_dir/VCPU.h obj_dir/VCPU__ALL.o: CPU.sv control_unit.sv registers.sv ALU.sv MALU.sv
ifeq ($(TEST), 1)
	verilator $(VERILATOROPT) -CFLAGS "-g" --trace $(VERILATORDEFINES) -cc CPU.sv
	cd obj_dir && make -f VCPU.mk
else
	verilator -Wall $(VERILATOROPT) --trace $(VERILATORDEFINES) -cc CPU.sv
	cd obj_dir && make -f VCPU.mk
endif

verilated.o: /usr/share/verilator/include/verilated.cpp
	$(COMPILER) $(DEBUGFLAGS) $(CPPFLAGS) -c $< -o $@

verilated_vcd_c.o: /usr/share/verilator/include/verilated_vcd_c.cpp
	$(COMPILER) $(DEBUGFLAGS) $(CPPFLAGS) -c $< -o $@

gui/%.o: gui/%.cpp obj_dir/VCPU.h
	@ printf "\e[2m[\e[22;32mcc\e[39;2m]\e[22m $< \e[2m$(DEBUGFLAGS) $(CPPFLAGS)\e[22m\n"
	@ $(COMPILER) $(DEBUGFLAGS) $(CPPFLAGS) $(DEPCFLAGS) -Iinclude -c $< -o $@

gui/resources.cpp: rvgui.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=resources --generate-dependencies rvgui.gresource.xml)
	$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=resources --generate-source $<

$(OUTPUT): $(GUIOBJ) verilated.o verilated_vcd_c.o obj_dir/VCPU__ALL.o
	@ printf "\e[2m[\e[22;36mld\e[39;2m]\e[22m $@\n"
	@ $(COMPILER) $^ -o $@ $(DEPLIBS)

CPU_sim: obj_dir/VCPU.h CPU_sim.cpp verilated.o
	$(COMPILER) $(CPPFLAGS) $(DEBUGFLAGS) -Wall -pthread -Dcimg_display=1 -Iobj_dir $(shell pkg-config --cflags verilator) CPU_sim.cpp verilated.o obj_dir/VCPU__ALL.o $(shell pkg-config --libs x11) -o CPU_sim

DEPFILE  := .dep
DEPTOKEN := "\# MAKEDEPENDS"

depend:
	@ echo $(DEPTOKEN) > $(DEPFILE)
	makedepend -f $(DEPFILE) -s $(DEPTOKEN) -- $(COMPILER) $(CPPFLAGS) -Iinclude -- $(GUISRC) 2>/dev/null
	@ rm $(DEPFILE).bak

sinclude $(DEPFILE)
