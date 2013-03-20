/*
 * PortAudioWriter.h
 *
 *  Created on: Mar 20, 2013
 *      Author: imizus
 */

#ifndef PORTAUDIOWRITER_H_
#define PORTAUDIOWRITER_H_

#include "PortAudioClass.h"

namespace vam
{
	class PortAudioPlayer : public PortAudioClass
	{
		static int portAudioCallback(const void*                     inputBuffer,
		                                      void*                           outputBuffer,
		                                      unsigned long                   framesPerBuffer,
		                                      const PaStreamCallbackTimeInfo* timeInfo,
		                                      PaStreamCallbackFlags           statusFlags,
		                                      void*                           userData)
		{
			(void) inputBuffer; // Prevent unused argument warning.
			float *out = (float*)outputBuffer;
			((PortAudioPlayer*)userData)->onNextBuffer(out, framesPerBuffer);

		/*
			for (int i = 0; i < framesPerBuffer; i++)
			{
				const float *buffer = pvfr->readSample();
				for (int chan = 0; chan < pvfr->getChannels(); chan++)
				{
					*out++ = buffer[chan] * VOLUME;
				}
			}
			*/
			return 0;
		}
	public:
		enum State { sStopped, sPlaying, sError };

		enum ErrorType { etPortAudioError, etNoDevice, etInvalidOperationInThisState };

		class Error : public PortAudioClass::Error
		{
			friend class PortAudioPlayer;

		private:
			ErrorType type;

		protected:
			Error(ErrorType type, int code) : type(type), PortAudioClass::Error(code) {}

		public:
			ErrorType getType() const { return type; }
		};

	private:
		PaStreamParameters outputParameters;
		PaStream *stream;
		State state;

		virtual void checkError(PaError code)
		{
			if (code != paNoError)
			{
				state = sError;
				throw Error(etPortAudioError, code);
			}
		}

	protected:
		void throwError(ErrorType type)
		{
			throw Error(type, 0);
		}

		virtual void onNextBuffer(float* out, int framesPerBuffer) = 0;

	public:
		PortAudioPlayer(int channels, int rate, int frames_per_buffer);
		void play();
		void stop();
		virtual ~PortAudioPlayer();
	};

} /* namespace va */
#endif /* PORTAUDIOWRITER_H_ */
