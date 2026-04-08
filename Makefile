CPP_RAW := $(shell dir /s /b src\*.cpp 2>nul)
CPP_RAW := $(strip $(CPP_RAW))

UNIALGO_DIR := lib/uni-algo
UNIALGO_SRC := $(UNIALGO_DIR)/src/data.cpp
UNIALGO_OBJ := obj_ext/uni_algo_data.o

UNIXID_DIR  := lib/unicode-xid-cpp

CPP_FILES := $(wildcard src/*.cpp src/*/*.cpp src/*/*/*.cpp src/*/*/*/*.cpp)
RC_FILES := $(wildcard res/*.rc)

O_FILES := $(CPP_FILES:src/%.cpp=obj/%.o) \
           $(RC_FILES:res/%.rc=obj/%.o) \
           $(UNIALGO_OBJ)

COMPILED_FLAGS := -g -pedantic -W -Wall \
				-Wno-missing-field-initializers -Wno-pedantic -Wno-comment -fno-inline -O0 -std=c++23
LINKED_FLAGS   :=

INCLUDE   := -Iinclude -I$(UNIALGO_DIR)/include -I$(UNIXID_DIR)
LIBRARIES := -Llib
LIB_FILES := -static-libstdc++ -static-libgcc -static

make-dir = $(shell mkdir -p $(@D) 2>nul || mkdir $(subst /,\,$(@D)) 2>nul)

all: $(O_FILES) | bin
	g++ $^ -o bin/debug/fog.exe $(LIBRARIES) $(LIB_FILES) $(LINKED_FLAGS)

main: $(O_FILES) | bin
	g++ $^ -o bin/main.exe $(LIBRARIES) $(LIB_FILES) $(LINKED_FLAGS)

obj/%.o: src/%.cpp
	$(make-dir)
	g++ -c $< -o $@ $(INCLUDE) $(COMPILED_FLAGS)

obj/%.o: res/%.rc
	$(make-dir)
	windres $< -o $@ $(INCLUDE)

$(UNIALGO_OBJ): $(UNIALGO_SRC)
	$(make-dir)
	g++ -c $< -o $@ $(INCLUDE) $(COMPILED_FLAGS)

bin:
	mkdir bin

clean:
	del /s /q obj\*.o bin\main.exe 2>nul || rm -rf obj bin