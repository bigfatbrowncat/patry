all: out/pasimple out/pavorbis out/vorbisfile_example

out/pasimple: src/pasimple.c
	mkdir -p out
	gcc -Llib/win32 -Iinclude src/pasimple.c -lportaudio -lwinmm -static-libgcc -o out/pasimple

out/pavorbis: src/pavorbis.c
	mkdir -p out
	gcc -Llib/win32 -Iinclude src/pavorbis.c -lvorbisfile -lvorbis -logg -lportaudio -lwinmm -static-libgcc -o out/pavorbis

out/vorbisfile_example: src/vorbisfile_example.c
	mkdir -p out
	gcc -Llib/win32 -Iinclude src/vorbisfile_example.c -lvorbisfile -lvorbis -logg -static-libgcc -o out/vorbisfile_example

clean:
	rm -rf out

.PHONY: all