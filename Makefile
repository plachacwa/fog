CPP_FILES := $(shell dir /s /b src\*.cpp)
RC_FILES := $(wildcard res/*.rc)
O_FILES := $(patsubst %.cpp,%.o,$(CPP_FILES)) $(subst .rc,.o,$(RC_FILES))
COMPILED_FLAGS := -pedantic -W -Wall -O3 -std=c++20
LINKED_FLAGS  := -s

INCLUDE    := -Iinclude
LIBRARIES   := -Llib
LIB_FILES := -static-libstdc++ -static-libgcc -static

main: $(O_FILES)
	g++ $^ -o $@ $(LIBRARIES) $(LIB_FILES) $(LINKED_FLAGS)

%.o: %.cpp
	g++ -c $< -o $@ $(INCLUDE) $(COMPILED_FLAGS)	

%.o: %.rc
	windres $^ -o $@ $(INCLUDE)

clean:
	del /s /q src\*.o
	del main.exe