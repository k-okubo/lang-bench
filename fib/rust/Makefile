TARGET := main
COMPILER := rustc
FLAGS := -C opt-level=3 -C debuginfo=0 -C debug-assertions=no 

all: $(TARGET)

$(TARGET): main.rs
	$(COMPILER) $(FLAGS) $<

.PHONY: clean
clean:
	rm -f $(TARGET)

.PHONY: run
run: $(TARGET)
	cat ../input | TIME="user:%U sys:%S" /usr/bin/time ./$(TARGET)
