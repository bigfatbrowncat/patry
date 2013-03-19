#include <stdio.h>
#include <math.h>

#include <portaudio/portaudio.h>

#define SAMPLE_RATE   (44100)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define TABLE_SIZE   (200)
typedef struct
{
    float sine[TABLE_SIZE];
    int left_phase;
    int right_phase;
}
paTestData;

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
    paTestData *data = (paTestData*)userData;
    float *out = (float*)outputBuffer;
    unsigned int i;
    (void) inputBuffer; /* Prevent unused argument warning. */
    for( i=0; i<framesPerBuffer; i++ )
    {
        *out++ = data->sine[data->left_phase];  /* left */
        *out++ = data->sine[data->right_phase];  /* right */
        data->left_phase += 1;
        if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
        data->right_phase += 3; /* higher pitch so we can distinguish left and right. */
        if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;
    }
    return 0;
}

/*******************************************************************/
int main()
{
    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;
    paTestData data;
    int i;
    printf("PortAudio Test: output sine wave.\n");

    /* initialise sinusoidal wavetable */
    for( i=0; i<TABLE_SIZE; i++ )
    {
        data.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
    }
    data.left_phase = data.right_phase = 0;

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
                         256,               /* Frames per buffer. */
                         paClipOff,         /* No out of range samples expected. */
                         patestCallback,
                         &data );
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;

    printf("Hit ENTER to stop program.\n");
    getchar();

    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;
    Pa_Terminate();

    printf("Test finished.\n");
    return err;

error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}