#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <portaudio/portaudio.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#define SAMPLE_RATE		44100
#define FRAMES_PER_BUFFER	64

#define LEFT	0
#define RIGHT	1
#define VOLUME	0.9

float buffer_left[8192];
float buffer_right[8192];
int eovf;
OggVorbis_File vf;
int current_section;

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

    
    long ret = ov_read_float(&vf, &buffer, framesPerBuffer, &current_section);
    
    
	if (ret == 0) {
		/* EOF */
		eovf=1;
		*out++ = 0;	// left
		*out++ = 0;	// right
	} 
	else if (ret < 0) 
	{
		/* error in the stream.  Not a problem, just reporting it in
		 case we (the app) cares.  In this case, we don't. */
		*out++ = 0;	// left
		*out++ = 0;	// right
	} 
	else 
	{
		/* we don't bother dealing with sample rate changes, etc, but
		 you'll have to */
		 
		//short* value = (short*)pcmout;
		for( i=0; i<framesPerBuffer; i++ )
		{
			*out++ = buffer[LEFT][i] * VOLUME;
			*out++ = buffer[RIGHT][i] * VOLUME;
		// *out++ = data->sine[data->left_phase];  /* left */
		// *out++ = data->sine[data->right_phase];  /* right */
		}
	}

    
    return 0;
}

/*******************************************************************/
int main()
{
    // Vorbis
  eovf=0;

#ifdef _WIN32
  _setmode( _fileno( stdin ), _O_BINARY );
  _setmode( _fileno( stdout ), _O_BINARY );
#endif

  if(ov_open_callbacks(stdin, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0) {
      fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
      exit(1);
  }

  {
    char **ptr=ov_comment(&vf,-1)->user_comments;
    vorbis_info *vi=ov_info(&vf,-1);
    while(*ptr){
      fprintf(stderr,"%s\n",*ptr);
      ++ptr;
    }
    fprintf(stderr,"\nBitstream is %d channel, %ldHz\n",vi->channels,vi->rate);
    fprintf(stderr,"Encoded by: %s\n\n",ov_comment(&vf,-1)->vendor);
  }
  
  

    // PulseAudio
    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;
    int i;
    printf("PortAudio Test: output sine wave.\n");

    /* initialise sinusoidal wavetable */

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
      fprintf(stderr,"Error: No default output device.\n");
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
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;

    printf("Playing...\n");
    //Sleep(ov_time_total(&vf, -1) * 1000);	// As much as needed
	Sleep(2000);
    
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;
    Pa_Terminate();

    ov_clear(&vf);
    
    printf("Test finished.\n");
    return err;

error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}
