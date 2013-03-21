UNAME := $(shell uname)

SRC = src
BIN = bin
OBJ = obj
GEN = gen

ifeq ($(UNAME), Darwin)	# OS X
  PLATFORM_LIBS = osx
  PLATFORM_GENERAL_LINKER_OPTIONS = -framework Carbon
  PLATFORM_MULTIMEDIA_LINKER_OPTIONS = -framework CoreAudio -framework AudioToolbox -framework AudioUnit
  PLATFORM_CONSOLE_OPTION = 
  PLATFORM_UI_TARGET = osxui
else ifeq ($(OS), Windows_NT)	# Windows
  PLATFORM = windows
  ARCH = i386
  PLATFORM_LIBS = win32
  PLATFORM_GENERAL_LINKER_OPTIONS = -lmingw32 -mwindows -static-libgcc -static-libstdc++
  PLATFORM_MULTIMEDIA_LINKER_OPTIONS = -lwinmm
  PLATFORM_CONSOLE_OPTION = -mconsole
  PLATFORM_UI_TARGET = win32ui
endif

all: $(BIN)/pasimple $(BIN)/pavorbis $(BIN)/vorbisfile_example $(BIN)/avian-embed $(BIN)/$(PLATFORM_UI_TARGET)

$(BIN)/java/%.class: $(SRC)/java/%.java
	mkdir -p $(BIN)/java
	"$(JAVA_HOME)/bin/javac" -d $(BIN)/java $<

$(GEN)/jni/%.h: $(BIN)/java/%.class
	mkdir -p $(GEN)/jni
	"$(JAVA_HOME)/bin/javah" -classpath "$(BIN)/java" -o $@ $(subst /,.,$(basename $(patsubst $(GEN)/jni/%, %, $@)))


$(BIN)/avian-embed: $(SRC)/avian-embed.cpp $(BIN)/java/app/Application.class $(GEN)/jni/app/Application.h
	mkdir -p $(BIN);

	# Extracting libavian objects
	( \
	    cd $(OBJ); \
	    mkdir -p libavian; \
	    cd libavian; \
	    ar x ../../lib/$(PLATFORM_LIBS)/libavian.a; \
	)
	
	# Making the java class library
	cp lib/java/classpath.jar $(BIN)/boot.jar; \
	( \
	    cd $(BIN); \
	    "$(JAVA_HOME)/bin/jar" u0f boot.jar -C java .; \
	)
	
	# Making an object file from the java class library
	tools/$(PLATFORM_LIBS)/binaryToObject $(BIN)/boot.jar $(OBJ)/boot.jar.o _binary_boot_jar_start _binary_boot_jar_end $(PLATFORM) $(ARCH); \
	g++ -g -O0 -D_JNI_IMPLEMENTATION_ -Llib/$(PLATFORM_LIBS) -Iinclude -I$(GEN)/jni $(OBJ)/boot.jar.o $(OBJ)/libavian/*.o $< $(PLATFORM_GENERAL_LINKER_OPTIONS) -lmingwthrd -lm -lz -lws2_32 -o $@

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
	rm -rf $(OBJ)
	rm -rf $(GEN)
	rm -rf $(BIN)

.PHONY: all