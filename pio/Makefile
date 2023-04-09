.PHONY: diff init all clean

all :
	@echo Building...
	@mkdir build/ 2>/dev/null; exit 0
	@cp ../src/*.pio .
	@cd build; cmake ..
	@cd build; make
	@cp build/*.pio.h ../src
	@cp pololu-3pi-2040-robot/LICENSE* ../src/pololu-3pi-2040-robot/
	@cp pololu-3pi-2040-robot/c/pololu_3pi_2040_robot/*.c ../src/pololu-3pi-2040-robot/
	@cp pololu-3pi-2040-robot/c/pololu_3pi_2040_robot/*.pio ../src/pololu-3pi-2040-robot/
	@cp pololu-3pi-2040-robot/c/pololu_3pi_2040_robot/include/*.h ../src/pololu-3pi-2040-robot/
	@patch <patch.diff
	@patch <patch_h.diff
	build/pioasm/pioasm -o c-sdk ../src/pololu-3pi-2040-robot/qtr_sensor_counter.pio ../src/pololu-3pi-2040-robot/qtr_sensor_counter.pio.h

diff :
	@echo Creating patches...
	@diff -u -x "LICENSE*" -x "*.h" pololu-3pi-2040-robot/c/pololu_3pi_2040_robot/ ../src/pololu-3pi-2040-robot/ >patch.diff ; exit 0
	@diff -u -x "LICENSE*" -x "*.c" -x "*.pio" -x "*.pio.h" pololu-3pi-2040-robot/c/pololu_3pi_2040_robot/include ../src/pololu-3pi-2040-robot/ >patch_h.diff ; exit 0

init :
	@echo Initializing git submodules...
	@git submodule update --init
	@cd pico-sdk; git submodule update --init

clean :
	@echo Removing build output for clean build...
	@rm -rf build/
	@rm *.pio
