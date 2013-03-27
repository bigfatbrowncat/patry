/*
 * MovedSound.h
 *
 *  Created on: Mar 26, 2013
 *      Author: imizus
 */

#ifndef MOVEDSOUND_H_
#define MOVEDSOUND_H_

#include "SoundSource.h"

namespace vam
{

	class MovedSound: public SoundSource
	{
	private:
		float** second_buffer;		// This buffer is ours
		double second_buffer_start_time;
		int cursor_position_in_second_buffer;
		int second_buffer_size;

		float read_buffer[MAX_CHANNELS];

		SoundSource* sound;
		double delay;
		double playhead;

		void checkSoundPosition();

		void fillBuffer();
		void updatePlayhead();

	public:
		MovedSound();
		virtual ~MovedSound();

		virtual const float* readSample();
		virtual int getChannels() const;
		virtual void rewind(double position);
		virtual double getPlayhead() const;
		virtual double getLength() const;
		virtual int getRate() const;

		void setSound(SoundSource& sound);
		void setDelay(double value);
	};

} /* namespace vam */
#endif /* MOVEDSOUND_H_ */
