CC = g++

CFLAGS = -g -O2 -Wall -pthread

SRCS = src/main.cpp src/yuri.cpp

OUT_DIR = bin/

OUT = -o $(OUT_DIR)yuri

MKDIR = mkdir -p ${OUT_DIR}

.PHONY: all

${OUT_DIR}:
		${MKDIR} 

all:  yuri

yuri: src/main.cpp src/yuri.cpp
	if [ -d "bin/" ]; then echo "dir exists"; else ${MKDIR}; fi
	$(CC) $(CFLAGS) $(SRCS) $(OUT)

clean:
	rm *o ${OUT_DIR}/yuri