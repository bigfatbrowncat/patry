all: out/pasimple out/pavorbis out/vorbisfile_example

UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)	# OS X
  PLATFORM_LIBS = osx
  PLATFORM_GENERAL_OPTIONS = -framework Carbon
  PLATFORM_MULTIMEDIA_OPTIONS = -framework CoreAudio -framework AudioToolbox -framework AudioUnit
else ifeq ($(OS), Windows_NT)	# Windows
  PLATFORM_LIBS = win32
  PLATFORM_GENERAL_OPTIONS = -lstdc++ -static-libstdc++ -static-libgcc
  PLATFORM_MULTIMEDIA_OPTIONS = -lwinmm
endif

out/pasimple: src/pasimple.c
	mkdir -p out
	gcc -Llib/$(PLATFORM_LIBS) -Iinclude src/pasimple.c -lportaudio $(PLATFORM_MULTIMEDIA_OPTIONS) $(PLATFORM_GENERAL_OPTIONS) -o out/pasimple

out/pavorbis: src/pavorbis.cpp
	mkdir -p out
	gcc -Llib/$(PLATFORM_LIBS) -Iinclude src/pavorbis.cpp -lvorbisfile -lvorbis -logg -lportaudio $(PLATFORM_MULTIMEDIA_OPTIONS) $(PLATFORM_GENERAL_OPTIONS) -o out/pavorbis

out/vorbisfile_example: src/vorbisfile_example.c
	mkdir -p out
	gcc -Llib/$(PLATFORM_LIBS) -Iinclude src/vorbisfile_example.c -lvorbisfile -lvorbis -logg $(PLATFORM_GENERAL_OPTIONS) -o out/vorbisfile_example

clean:
	rm -rf out

.PHONY: all