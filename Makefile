UNAME := $(shell uname)

SRC = src
BIN = bin
OBJ = obj
GEN = gen

ifeq ($(UNAME), Darwin)	# OS X
  PLATFORM = darwin
  ARCH = x86_64
  PLATFORM_LIBS = osx
  PLATFORM_GENERAL_LINKER_OPTIONS = -framework Carbon
  PLATFORM_MULTIMEDIA_LINKER_OPTIONS = -framework CoreAudio -framework AudioToolbox -framework AudioUnit
  PLATFORM_CONSOLE_OPTION = 
  PLATFORM_UI_TARGET = osxui
  EXE_EXT=
  STRIP_OPTIONS=-S -x
else ifeq ($(OS), Windows_NT)	# Windows
  PLATFORM = windows
  ARCH = i386
  PLATFORM_LIBS = win32
  PLATFORM_GENERAL_LINKER_OPTIONS = -lmingw32 -lmingwthrd -lws2_32 -mwindows -static-libgcc -static-libstdc++
  PLATFORM_MULTIMEDIA_LINKER_OPTIONS = -lwinmm
  PLATFORM_CONSOLE_OPTION = -mconsole
  PLATFORM_UI_TARGET = win32ui
  EXE_EXT=.exe
  STRIP_OPTIONS=--strip-all
endif

all: $(BIN)/avian-embed $(BIN)/$(PLATFORM_UI_TARGET)

$(BIN)/java/%.class: $(SRC)/java/%.java
	mkdir -p $(BIN)/java
	"$(JAVA_HOME)/bin/javac" -sourcepath "$(SRC)/java" -classpath "$(BIN)/java" -d $(BIN)/java $<

$(GEN)/jni/%.h: $(BIN)/java/%.class
	mkdir -p $(GEN)/jni
	"$(JAVA_HOME)/bin/javah" -classpath "$(BIN)/java" -o $@ $(subst /,.,$(basename $(patsubst $(GEN)/jni/%, %, $@)))


JAVA_CLASSES = $(BIN)/java/app/Application.class $(BIN)/java/vam/VorbisFileReader.class $(BIN)/java/vam/ResourcesDeallocatedException.class
JNI_HEADERS = $(GEN)/jni/vam/VorbisFileReader.h $(GEN)/jni/vam/PortAudioPlayer.h
JNI_OBJECTS = $(OBJ)/VorbisFileReader_jni.o $(OBJ)/PortAudioPlayer_jni.o
NATIVE_OBJECTS = $(OBJ)/VorbisFileReader.o $(OBJ)/PortAudioClass.o $(OBJ)/PortAudioPlayer.o

$(OBJ)/%.o: $(SRC)/%.cpp $(SRC)/*.h
	mkdir -p $(OBJ)
	g++ -g -O0 -c -Iinclude -I$(GEN)/jni/ $< -o $@

$(BIN)/avian-embed: $(SRC)/avian-embed.cpp $(JAVA_CLASSES) $(JNI_HEADERS) $(JNI_OBJECTS) $(NATIVE_OBJECTS)
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
	g++ -g -O0 -D_JNI_IMPLEMENTATION_ -Llib/$(PLATFORM_LIBS) -Iinclude -I$(GEN)/jni/ $(OBJ)/boot.jar.o $(OBJ)/libavian/*.o $(JNI_OBJECTS) $(NATIVE_OBJECTS) $< -lvorbisfile -lvorbis -logg -lportaudio $(PLATFORM_MULTIMEDIA_LINKER_OPTIONS) $(PLATFORM_GENERAL_LINKER_OPTIONS) $(PLATFORM_CONSOLE_OPTION) -lm -lz -o $@
	strip $(STRIP_OPTIONS) $@$(EXE_EXT)

$(BIN)/win32ui: $(SRC)/win32ui.c
	mkdir -p $(BIN)
	gcc -Llib/$(PLATFORM_LIBS) -Iinclude $< $(PLATFORM_GENERAL_LINKER_OPTIONS) -o $@

$(BIN)/osxui:
	#todo...



clean:
	rm -rf $(OBJ)
	rm -rf $(GEN)
	rm -rf $(BIN)

.PHONY: all