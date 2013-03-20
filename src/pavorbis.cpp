// C standard library
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>

// C++ standard library
#include <vector>
#include <string>

// Non-standard libraries
#include <portaudio/portaudio.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

// Local headers
#include "VorbisFileReader.h"
#include "PortAudioPlayer.h"

#define FRAMES_PER_BUFFER	64

#define LEFT	0
#define RIGHT	1
#define VOLUME	0.9

#define RESULT_SUCCESS					0

#define RESULT_NO_INPUT_FILE			1
#define RESULT_INPUT_FILE_CANT_OPEN		2
#define RESULT_INPUT_FILE_NOT_AN_OGG		3
#define RESULT_INPUT_FILE_CANT_READ		4
#define RESULT_INPUT_FILE_VERSION_INCORRECT	5
#define RESULT_INPUT_FILE_BAD_HEADER		6
#define RESULT_INPUT_FILE_MEMORY_FAULT		7

#define RESULT_NO_DEFAULT_AUDIO_OUTPUT			8
#define RESULT_INVALID_OPERATION_IN_THIS_STATE	9
#define RESULT_PORTAUDIO_ERROR					10

#define RESULT_STRANGE_ERROR			100

using namespace std;
using namespace vam;

class PAVorbisPlayer : public PortAudioPlayer
{
private:
	VorbisFileReader& reader;

protected:
	virtual void onNextBuffer(float* out, int framesPerBuffer)
	{
		for (int i = 0; i < framesPerBuffer; i++)
		{
			const float *buffer = reader.readSample();
			for (int chan = 0; chan < reader.getChannels(); chan++)
			{
				*out++ = buffer[chan] * VOLUME;
			}
		}
	}
	
public:
	PAVorbisPlayer(int channels, int rate, int frames_per_buffer, VorbisFileReader& reader) :
		reader(reader), PortAudioPlayer(channels, rate, frames_per_buffer)
	{

	}

};

/*******************************************************************/
int main(int argc, char* argv[])
{
	// Opening the input file
	if (argc != 2)
	{
		fprintf(stderr, "No input file.\n");
		exit(RESULT_NO_INPUT_FILE);
	}
	
	printf("Input file: %s\n", argv[1]);
	
	try
	{
		VorbisFileReader vfr(argv[1], 512);

		printf("\nBitstream has %d channel(s), %ldHz, %dKbps (on average)\n",vfr.getChannels(), vfr.getRate(), (int)round(((float)vfr.getBitsPerSecond()) / 1000) );
		printf("Encoded by: %s\n\n", vfr.getVendor().c_str());
		
		if (vfr.getComments().size() > 0)
		{
			printf("Comments:\n");
		}
		for (int i = 0; i < vfr.getComments().size(); i++)
		{
			printf("  %s\n", vfr.getComments()[i].c_str());
		}
		
		printf("\n");

		PAVorbisPlayer pavp(vfr.getChannels(), vfr.getRate(), FRAMES_PER_BUFFER, vfr);
		pavp.play();

		while (vfr.getState() != VorbisFileReader::sEndOfData)
		{
			usleep(1000);
			int min = (int)(vfr.getPlayhead()) / 60;
			int sec = (int)(vfr.getPlayhead()) % 60;
			int sp10 = (int)(vfr.getPlayhead() * 100) % 100;
			
			int lmin = (int)(vfr.getLength()) / 60;
			int lsec = (int)(vfr.getLength()) % 60;
			int lsp10 = (int)(vfr.getLength() * 100) % 100;
			
			printf("Playing the file... [ %02d:%02d.%02d / %02d:%02d.%02d ]\r", min, sec, sp10, lmin, lsec, lsp10);
		}
		printf("                                                                  \r");
		printf("The sound file has ended.\n");

		printf("Bye.\n");

		return RESULT_SUCCESS;
	}
	catch (const VorbisFileReader::Error& err)
	{
		fprintf(stderr, "Error occured in Vorbis File Reader, the caller is %s\n", err.getCaller().c_str());
		switch (err.getType())
		{
		case VorbisFileReader::etCantOpen:
			fprintf(stderr, "Can't open the input file\n");
			exit(RESULT_INPUT_FILE_CANT_OPEN);
			break;
		case VorbisFileReader::etCantRead:
			fprintf(stderr, "Can't read from the input file\n");
			exit(RESULT_INPUT_FILE_CANT_READ);
			break;
		case VorbisFileReader::etCantSeek:
			fprintf(stderr, "Can't seek to the specified position\n");
			exit(RESULT_INPUT_FILE_CANT_READ);
			break;
		case VorbisFileReader::etNotAnOgg:
			fprintf(stderr, "Input file does not appear to be an Ogg bitstream\n");
			exit(RESULT_INPUT_FILE_NOT_AN_OGG);
			break;
		case VorbisFileReader::etVersionIncorrect:
			fprintf(stderr, "Input file is an Ogg bitstream, but its version is incompatible with the decoder\n");
			exit(RESULT_INPUT_FILE_VERSION_INCORRECT);
			break;
		case VorbisFileReader::etBadHeader:
			fprintf(stderr, "Input file is an Ogg bitstream, but its header is corrupted\n");
			exit(RESULT_INPUT_FILE_BAD_HEADER);
			break;
		case VorbisFileReader::etInvalidArgument:
			fprintf(stderr, "Invalid argument has been passed to a function\n");
			exit(RESULT_INPUT_FILE_BAD_HEADER);
			break;
		case VorbisFileReader::etMemoryFault:
			fprintf(stderr, "Input file couldn't be opened cause of a memory fault\n");
			exit(RESULT_INPUT_FILE_MEMORY_FAULT);
			break;
		case VorbisFileReader::etSeekOutOfRange:
			fprintf(stderr, "Trying to seek out of the sound length\n");
			exit(RESULT_INPUT_FILE_MEMORY_FAULT);
			break;
		default:
			fprintf(stderr, "Strange error (code = %d)\n", err.getCode());
			exit(RESULT_STRANGE_ERROR);
			break;
		}
	}
	catch (const PortAudioClass::Error& err)
	{
		fprintf(stderr, "PortAudio Player error occured: ");

		if (const PortAudioPlayer::Error* papErr = dynamic_cast<const PortAudioPlayer::Error*>(&err))
		{
			switch (papErr->getType())
			{
			case PortAudioPlayer::etNoDevice:
				fprintf(stderr, "No default audio output device present\n");
				exit(RESULT_NO_DEFAULT_AUDIO_OUTPUT);
				break;

			case PortAudioPlayer::etInvalidOperationInThisState:
				fprintf(stderr, "Invalid operation in this state\n");
				exit(RESULT_INVALID_OPERATION_IN_THIS_STATE);
				break;

			default:
				break;
			}
		}

		fprintf(stderr, "error code is %d\n", err.getCode());
		exit(RESULT_PORTAUDIO_ERROR);
	}
}
