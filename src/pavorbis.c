#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>

#include <portaudio/portaudio.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>


#define SAMPLE_RATE		44100
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

int eovf;
OggVorbis_File vf;
int current_section;
long playhead;

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
	float *out = (float*)outputBuffer;
	unsigned int i;
	(void) inputBuffer; /* Prevent unused argument warning. */

	float **buffer;

	long ret = 0;

	if (!eovf)
	{
		ret = ov_read_float(&vf, &buffer, framesPerBuffer, &current_section);
	
		if (ret == 0)
		{
			// EOF
			eovf = 1;

			// Outputting silence
			for(i = 0; i < framesPerBuffer; i++)
			{
				*out++ = 0;	// left
				*out++ = 0;	// right
			}
		} 
		else if (ret < 0) 
		{
			// error in the stream.  Not a problem, just reporting it in
			// case we (the app) cares.  In this case, we don't. 
			for(i = 0; i < framesPerBuffer; i++)
			{
				*out++ = 0;	// left
				*out++ = 0;	// right
			}
		} 
		else 
		{
			for(i = 0; i < framesPerBuffer; i++)
			{
				if (i < ret)
				{
					*out++ = buffer[LEFT][i] * VOLUME;
					*out++ = buffer[RIGHT][i] * VOLUME;
					playhead ++;
				}
				else
				{
					*out++ = 0;
					*out++ = 0;
				}
			}
		}
	
	}
	else
	{
		// Outputting silence
		for(i = 0; i < framesPerBuffer; i++)
		{
			*out++ = 0;	// left
			*out++ = 0;	// right
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
	FILE* input_file = fopen(argv[1], "rb");
	
	if (input_file == NULL)
	{
		fprintf(stderr, "Can't open the input file\n");
		exit(RESULT_INPUT_FILE_CANT_OPEN);
	}
	
	// Opening vorbis file
	eovf=0;

	int res = ov_open_callbacks(input_file, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE);
	switch (res)
	{
	case OV_EREAD:
		fprintf(stderr, "Can't read from the input file");
		exit(RESULT_INPUT_FILE_CANT_READ);
		break;
	case OV_ENOTVORBIS:
		fprintf(stderr, "Input file does not appear to be an Ogg bitstream");
		exit(RESULT_INPUT_FILE_NOT_AN_OGG);
		break;
	case OV_EVERSION:
		fprintf(stderr, "Input file is an Ogg bitstream, but its version is incompatible with the decoder");
		exit(RESULT_INPUT_FILE_VERSION_INCORRECT);
		break;
	case OV_EBADHEADER:
		fprintf(stderr, "Input file is an Ogg bitstream, but its header is corrupted");
		exit(RESULT_INPUT_FILE_BAD_HEADER);
		break;
	case OV_EFAULT:
		fprintf(stderr, "Input file couldn't be opened cause of a memory fault");
		exit(RESULT_INPUT_FILE_MEMORY_FAULT);
		break;
	}

	// Reading the comments
	char **ptr = ov_comment(&vf,-1)->user_comments;
	vorbis_info *vi = ov_info(&vf,-1);
	while(*ptr)
	{
		printf("%s\n",*ptr);
		++ptr;
	}
	printf("\nBitstream is %d channel, %ldHz\n",vi->channels,vi->rate);
	printf("Encoded by: %s\n\n",ov_comment(&vf,-1)->vendor);
  
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
	
	outputParameters.channelCount = 2;                     /* stereo output */
	outputParameters.sampleFormat = paFloat32;             /* 32 bit floating point output */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	err = Pa_OpenStream( &stream,
			 NULL,              /* No input. */
			 &outputParameters, /* As above. */
			 SAMPLE_RATE,
			 FRAMES_PER_BUFFER,               /* Frames per buffer. */
			 paClipOff,         /* No out of range samples expected. */
			 patestCallback,
			 NULL );
	
	if(err != paNoError) goto error;

	err = Pa_StartStream(stream);
	if( err != paNoError ) goto error;

	long length = ov_pcm_total(&vf, -1);
	int lmin = length / SAMPLE_RATE / 60;
	int lsec = length / SAMPLE_RATE % 60;
	int lsp10 = length / (SAMPLE_RATE / 10) % 10;

	printf("\n");

	playhead = 0;
	while (!eovf)
	{
		usleep(1000);
		int min = playhead / SAMPLE_RATE / 60;
		int sec = playhead / SAMPLE_RATE % 60;
		int sp10 = playhead / (SAMPLE_RATE / 10) % 10;
		printf("Playing the file.... [ %02d:%02d.%d / %02d:%02d.%d ]\r", min, sec, sp10, lmin, lsec, lsp10);
	}
	printf("                                                                  \r");
	printf("The sound file has ended.\n");

	err = Pa_CloseStream( stream );
	if( err != paNoError ) goto error;
	Pa_Terminate();

	printf("Bye.\n");
	return err;

error:
	Pa_Terminate();
	ov_clear(&vf);
	if (input_file != NULL) fclose(input_file);

	printf("An error occured while using the portaudio stream\n" );
	printf("Error number: %d\n", err );
	printf("Error message: %s\n", Pa_GetErrorText( err ) );
	return err;
}
