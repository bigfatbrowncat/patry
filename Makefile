UNAME := $(shell uname)

BIN = bin
OBJ = obj
SRC = src

ifeq ($(UNAME), Darwin)	# OS X
  PLATFORM_LIBS = osx
  PLATFORM_GENERAL_OPTIONS = -framework Carbon
  PLATFORM_MULTIMEDIA_OPTIONS = -framework CoreAudio -framework AudioToolbox -framework AudioUnit
else ifeq ($(OS), Windows_NT)	# Windows
  PLATFORM_LIBS = win32
  PLATFORM_GENERAL_OPTIONS = -lmingw32 -mwindows -mconsole -static-libgcc -static-libstdc++
  PLATFORM_MULTIMEDIA_OPTIONS = -lwinmm
endif

all: $(BIN)/pasimple $(BIN)/pavorbis $(BIN)/vorbisfile_example

$(BIN)/pasimple: $(SRC)/pasimple.c
	mkdir -p $(BIN)
	gcc -Llib/$(PLATFORM_LIBS) -Iinclude $(SRC)/pasimple.c -lportaudio $(PLATFORM_MULTIMEDIA_OPTIONS) $(PLATFORM_GENERAL_OPTIONS) -o $(BIN)/pasimple

$(BIN)/vorbisfile_example: $(SRC)/vorbisfile_example.c
	mkdir -p $(BIN)
	gcc -Llib/$(PLATFORM_LIBS) -Iinclude $(SRC)/vorbisfile_example.c -lvorbisfile -lvorbis -logg $(PLATFORM_GENERAL_OPTIONS) -o $(BIN)/vorbisfile_example

# PAVorbis targets

$(OBJ)/%.o: $(SRC)/%.cpp $(SRC)/*.h
	mkdir -p $(OBJ)
	g++ -g -O0 -c -Iinclude $< -o $@

$(BIN)/pavorbis: $(OBJ)/VorbisFileReader.o $(OBJ)/PortAudioClass.o $(OBJ)/PortAudioPlayer.o $(OBJ)/pavorbis.o 
	mkdir -p $(BIN)
	g++ -Iinclude -Llib/$(PLATFORM_LIBS) $^ -lvorbisfile -lvorbis -logg -lportaudio $(PLATFORM_MULTIMEDIA_OPTIONS) $(PLATFORM_GENERAL_OPTIONS) -o $@


clean:
	rm -rf obj
	rm -rf bin

.PHONY: all