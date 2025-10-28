CPP_RAW := $(shell dir /s /b src\*.cpp 2>nul)
CPP_RAW := $(strip $(CPP_RAW))

CPP_FILES := $(wildcard src/*.cpp src/*/*.cpp src/*/*/*.cpp src/*/*/*/*.cpp)
RC_FILES := $(wildcard res/*.rc)

O_FILES := $(CPP_FILES:src/%.cpp=obj/%.o) \
           $(RC_FILES:res/%.rc=obj/%.o)

COMPILED_FLAGS := -pedantic -W -Wall -Wno-missing-field-initializers -Wno-pedantic -Wno-comment -O3 -std=c++20
LINKED_FLAGS   := -s

INCLUDE   := -Iinclude
LIBRARIES := -Llib
LIB_FILES := -static-libstdc++ -static-libgcc -static

make-dir = $(shell mkdir -p $(@D) 2>nul || mkdir $(subst /,\,$(@D)) 2>nul)

main: $(O_FILES) | bin
	g++ $^ -o bin/main.exe $(LIBRARIES) $(LIB_FILES) $(LINKED_FLAGS)

obj/%.o: src/%.cpp
	$(make-dir)
	g++ -c $< -o $@ $(INCLUDE) $(COMPILED_FLAGS)

obj/%.o: res/%.rc
	$(make-dir)
	windres $< -o $@ $(INCLUDE)

bin:
	mkdir bin

clean:
	del /s /q obj\*.o bin\main.exe 2>nul || rm -rf obj bin