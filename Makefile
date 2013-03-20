UNAME := $(shell uname)

BIN = bin
OBJ = obj
SRC = src

ifeq ($(UNAME), Darwin)	# OS X
  PLATFORM_LIBS = osx
  PLATFORM_GENERAL_LINKER_OPTIONS = -framework Carbon
  PLATFORM_MULTIMEDIA_LINKER_OPTIONS = -framework CoreAudio -framework AudioToolbox -framework AudioUnit
  PLATFORM_CONSOLE_OPTION = 
  PLATFORM_UI_TARGET = osxui
else ifeq ($(OS), Windows_NT)	# Windows
  PLATFORM_LIBS = win32
  PLATFORM_GENERAL_LINKER_OPTIONS = -lmingw32 -mwindows -static-libgcc -static-libstdc++
  PLATFORM_MULTIMEDIA_LINKER_OPTIONS = -lwinmm
  PLATFORM_CONSOLE_OPTION = -mconsole
  PLATFORM_UI_TARGET = win32ui
endif

all: $(BIN)/pasimple $(BIN)/pavorbis $(BIN)/vorbisfile_example $(BIN)/$(PLATFORM_UI_TARGET)

$(BIN)/win32ui: $(SRC)/win32ui.c
	mkdir -p $(BIN)
	gcc -Llib/$(PLATFORM_LIBS) -Iinclude $< $(PLATFORM_GENERAL_LINKER_OPTIONS) -o $@

$(BIN)/osxui:
	#todo...

$(BIN)/pasimple: $(SRC)/pasimple.c
	mkdir -p $(BIN)
	gcc -Llib/$(PLATFORM_LIBS) -Iinclude $(SRC)/pasimple.c -lportaudio $(PLATFORM_MULTIMEDIA_LINKER_OPTIONS) $(PLATFORM_GENERAL_LINKER_OPTIONS) $(PLATFORM_CONSOLE_OPTION) -o $(BIN)/pasimple

$(BIN)/vorbisfile_example: $(SRC)/vorbisfile_example.c
	mkdir -p $(BIN)
	gcc -Llib/$(PLATFORM_LIBS) -Iinclude $(SRC)/vorbisfile_example.c -lvorbisfile -lvorbis -logg $(PLATFORM_GENERAL_LINKER_OPTIONS) $(PLATFORM_CONSOLE_OPTION) -o $(BIN)/vorbisfile_example

# PAVorbis targets

$(OBJ)/%.o: $(SRC)/%.cpp $(SRC)/*.h
	mkdir -p $(OBJ)
	g++ -g -O0 -c -Iinclude $< -o $@

$(BIN)/pavorbis: $(OBJ)/VorbisFileReader.o $(OBJ)/PortAudioClass.o $(OBJ)/PortAudioPlayer.o $(OBJ)/pavorbis.o 
	mkdir -p $(BIN)
	g++ -Iinclude -Llib/$(PLATFORM_LIBS) $^ -lvorbisfile -lvorbis -logg -lportaudio $(PLATFORM_MULTIMEDIA_LINKER_OPTIONS) $(PLATFORM_GENERAL_LINKER_OPTIONS) $(PLATFORM_CONSOLE_OPTION) -o $@


clean:
	rm -rf obj
	rm -rf bin

.PHONY: all