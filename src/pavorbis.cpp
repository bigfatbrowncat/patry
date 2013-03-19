#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>

#include <portaudio/portaudio.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <vector>
#include <string>

#define FRAMES_PER_BUFFER	64

#define LEFT	0
#define RIGHT	1
#define VOLUME	0.9

#define RESULT_NO_INPUT_FILE			1
#define RESULT_INPUT_FILE_CANT_OPEN		2
#define RESULT_INPUT_FILE_NOT_AN_OGG		3
#define RESULT_INPUT_FILE_CANT_READ		4
#define RESULT_INPUT_FILE_VERSION_INCORRECT	5
#define RESULT_INPUT_FILE_BAD_HEADER		6
#define RESULT_INPUT_FILE_MEMORY_FAULT		7
#define RESULT_PORTAUDIO_BASE			100

using namespace std;

class VorbisFileReader
{
public:
	enum ErrorType
	{
		etCantOpen, etNotAnOgg, etCantRead, etVersionIncorrect, etBadHeader, etMemoryFault,
		etHole, etBadLink, etHeadersInvalid, etStrangeError
	};
	enum State 
	{
		sReady, sEndOfData, sError
	};
private:
	OggVorbis_File vf;
	int current_section;

	int bufpos;
	float** buffer;			// This buffer belongs to the Ogg Vorbis decoder
	int buffer_size;

	FILE* file;
	float* read_buffer;		// This buffer is our

	int buffer_size_request;

	State state;
	ErrorType errorType;
	long playhead;
	long length;
	int channels;
	int rate;
	int bitsPerSecond;
	string vendor;
	vector<string> comments;

private:
	void fillBuffer()
	{
		int ret = ov_read_float(&vf, &buffer, buffer_size_request, &current_section);
		if (ret < 0)
		{
			switch (ret)
			{
			case OV_HOLE:
				state = sError;
				errorType = etHole;
				break;
			case OV_EBADLINK:
				state = sError;
				errorType = etBadLink;
				break;
			case OV_EINVAL:
				state = sError;
				errorType = etHeadersInvalid;
				break;
			default:
				state = sError;
				errorType = etStrangeError;
				break;
			}
		}
		else if (ret == 0)
		{
			state = sEndOfData;
		}
		else
		{
			bufpos = 0;
			buffer_size = ret;
			state = sReady;
		}
	}
	
public:
	VorbisFileReader(string file_name, int buffer_size_request) : 
		buffer_size_request(buffer_size_request)
	{
		file = fopen(file_name.c_str(), "rb");
		
		if (file == NULL)
		{
			state = sError;
			errorType = etCantOpen;
			return;
		}
		
		// Opening vorbis file

		int res = ov_open_callbacks(file, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE);
		switch (res)
		{
		case OV_EREAD:
			state = sError;
			errorType = etCantRead;
			break;
		case OV_ENOTVORBIS:
			state = sError;
			errorType = etNotAnOgg;
			break;
		case OV_EVERSION:
			state = sError;
			errorType = etVersionIncorrect;
			break;
		case OV_EBADHEADER:
			state = sError;
			errorType = etBadHeader;
			break;
		case OV_EFAULT:
			state = sError;
			errorType = etMemoryFault;
			break;
		default:
			break;
		}
		
		if (res == 0)
		{
	
			// Reading the comments
			char **ptr = ov_comment(&vf, -1)->user_comments;
			vorbis_info *vi = ov_info(&vf, -1);
			while (*ptr)
			{
				comments.push_back(string(*ptr));
				++ptr;
			}
			
			bitsPerSecond = ov_bitrate(&vf, -1);
			channels = vi->channels;
			rate = vi->rate;
			read_buffer = new float[channels];
			vendor = string(ov_comment(&vf,-1)->vendor);
			length = ov_pcm_total(&vf, -1);
			playhead = 0;
			
			fillBuffer();

		}
		
	}
	
	virtual ~VorbisFileReader()
	{
		delete [] read_buffer;
		ov_clear(&vf);
		if (file != NULL) fclose(file);
	}
	
	const float* readSample()
	{
		bool output_silence = false;
		if (state == sReady)
		{
			// Check if the buffer is filled
			if (bufpos == buffer_size)
			{
				fillBuffer();
			}

			// Buffer is ready, reading a sample
			if (state == sReady)
			{
				for (int i = 0; i < channels; i++)
				{
					read_buffer[i] = buffer[i][bufpos];
				}
				bufpos++;
				playhead++;
				
				return read_buffer;
			}
			else
			{
				output_silence = true;
			}

		}
		else 
		{
			output_silence = true;
		}
		
		if (output_silence)
		{
			// If the reader isn't in the Ready state, returning zero
			for (int i = 0; i < channels; i++)
			{
				read_buffer[i] = 0.f;
			}
			return read_buffer;
		}
		
	}
	
	State getState() const { return state; }
	ErrorType getErrorType() const { return errorType; }
	long getPlayHead() const { return playhead; }
	long getLength() const { return length; }
	long getChannels() const { return channels; }
	long getRate() const { return rate; }
	int getBitsPerSecond() const { return bitsPerSecond; }
	const string& getVendor() const { return vendor; }
	const vector<string> getComments() const { return comments; }
};

VorbisFileReader* pvfr;


/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback(const void*                     inputBuffer,
                          void*                           outputBuffer,
                          unsigned long                   framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags           statusFlags,
                          void*                           userData)
{
	(void) inputBuffer; /* Prevent unused argument warning. */
	float *out = (float*)outputBuffer;

	for (int i = 0; i < framesPerBuffer; i++)
	{
		const float *buffer = pvfr->readSample();
		for (int chan = 0; chan < pvfr->getChannels(); chan++)
		{
			*out++ = buffer[chan] * VOLUME;	
		}
	}
	
	return 0;
}

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
	
	VorbisFileReader vfr(argv[1], 512);
	pvfr = &vfr;
	if (vfr.getState() == VorbisFileReader::sError)
	{
		switch (vfr.getErrorType())
		{
		case VorbisFileReader::etCantOpen:
			fprintf(stderr, "Can't open the input file");
			exit(RESULT_INPUT_FILE_CANT_OPEN);
			break;
		case VorbisFileReader::etCantRead:
			fprintf(stderr, "Can't read from the input file");
			exit(RESULT_INPUT_FILE_CANT_READ);
			break;
		case VorbisFileReader::etNotAnOgg:
			fprintf(stderr, "Input file does not appear to be an Ogg bitstream");
			exit(RESULT_INPUT_FILE_NOT_AN_OGG);
			break;
		case VorbisFileReader::etVersionIncorrect:
			fprintf(stderr, "Input file is an Ogg bitstream, but its version is incompatible with the decoder");
			exit(RESULT_INPUT_FILE_VERSION_INCORRECT);
			break;
		case VorbisFileReader::etBadHeader:
			fprintf(stderr, "Input file is an Ogg bitstream, but its header is corrupted");
			exit(RESULT_INPUT_FILE_BAD_HEADER);
			break;
		case VorbisFileReader::etMemoryFault:
			fprintf(stderr, "Input file couldn't be opened cause of a memory fault");
			exit(RESULT_INPUT_FILE_MEMORY_FAULT);
			break;
		}
		
	}
	
	// Initializing PortAudio
	PaStreamParameters outputParameters;
	PaStream *stream;
	PaError err;
	int i;

	err = Pa_Initialize();
	if (err != paNoError) goto error;

	outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
	if (outputParameters.device == paNoDevice)
	{
		fprintf(stderr,"No default output device.\n");
		goto error;
	}
	
	outputParameters.channelCount = vfr.getChannels();
	outputParameters.sampleFormat = paFloat32;             /* 32 bit floating point output */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	err = Pa_OpenStream(&stream,
			 NULL,              /* No input. */
			 &outputParameters, /* As above. */
			 vfr.getRate(),
			 FRAMES_PER_BUFFER,               /* Frames per buffer. */
			 paClipOff,         /* No out of range samples expected. */
			 patestCallback,
			 NULL);
	
	if(err != paNoError) goto error;

	err = Pa_StartStream(stream);
	if( err != paNoError ) goto error;

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

	while (vfr.getState() != VorbisFileReader::sEndOfData)
	{
		usleep(1000);
		int min = vfr.getPlayHead() / vfr.getRate() / 60;
		int sec = vfr.getPlayHead() / vfr.getRate() % 60;
		int sp10 = vfr.getPlayHead() / (vfr.getRate() / 10) % 10;
		
		int lmin = vfr.getLength() / vfr.getRate() / 60;
		int lsec = vfr.getLength() / vfr.getRate() % 60;
		int lsp10 = vfr.getLength() / (vfr.getRate() / 10) % 10;
		
		printf("Playing the file... [ %02d:%02d.%d / %02d:%02d.%d ]\r", min, sec, sp10, lmin, lsec, lsp10);
	}
	printf("                                                                  \r");
	printf("The sound file has ended.\n");

	err = Pa_CloseStream( stream );
	if (err != paNoError) goto error;
	Pa_Terminate();

	printf("Bye.\n");
	return 0;

error:
	Pa_Terminate();

	printf("An error occured while using the portaudio stream\n" );
	printf("Error number: %d\n", err );
	printf("Error message: %s\n", Pa_GetErrorText( err ) );
	return RESULT_PORTAUDIO_BASE + err;
}
