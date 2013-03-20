/*
 * PortAudioWriter.cpp
 *
 *  Created on: Mar 20, 2013
 *      Author: imizus
 */

#include <stddef.h>

#include "PortAudioPlayer.h"

namespace vam
{

	PortAudioPlayer::PortAudioPlayer(int channels, int rate, int frames_per_buffer)
	{
		outputParameters.device = Pa_GetDefaultOutputDevice(); // default output device
		if (outputParameters.device == paNoDevice)
		{
			throwError(etNoDevice);
		}

		outputParameters.channelCount = channels;
		outputParameters.sampleFormat = paFloat32;             // 32 bit floating point output
		outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
		outputParameters.hostApiSpecificStreamInfo = NULL;

		checkError((PaError)Pa_OpenStream(&stream,
				 NULL,              // No input.
				 &outputParameters, // As above.
				 rate,
				 frames_per_buffer,               // Frames per buffer.
				 paClipOff,         // No out of range samples expected.
				 portAudioCallback,
				 this));

		state = sStopped;
	}

	void PortAudioPlayer::play()
	{
		if (state != sStopped)
		{
			throwError(etInvalidOperationInThisState);
		}

		checkError(Pa_StartStream(stream));
		state = sPlaying;
	}

	void PortAudioPlayer::stop()
	{
		if (state != sPlaying)
		{
			throwError(etInvalidOperationInThisState);
		}

		checkError(Pa_StopStream(stream));
		state = sStopped;
	}

	PortAudioPlayer::~PortAudioPlayer()
	{
		Pa_CloseStream(stream);
	}

} /* namespace va */
