TARGETS=unix_list
BUILD_DIR=build

.PHONY: clean all

all: $(TARGETS)

install: all
	mv $(TARGETS) $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
