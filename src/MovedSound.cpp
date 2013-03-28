/*
 * MovedSound.cpp
 *
 *  Created on: Mar 26, 2013
 *      Author: imizus
 */

#include <math.h>
#include <stddef.h>

#include "MovedSound.h"

namespace vam
{
	void MovedSound::checkSoundPosition()
	{
		if (fabs(sound->getPlayhead() - playhead + delay) > 1.0 / sound->getRate())
		{
			sound->rewind(playhead - delay);
		}
	}

	void MovedSound::fillBuffer()
	{
		second_buffer_start_time = playhead;
		if (sound != NULL)
		{
			checkSoundPosition();

			for (int i = 0; i < second_buffer_allocated_size; i++)
			{
				const float* sample = sound->readSample();
				for (int ch = 0; ch < sound->getChannels(); ch++)
				{
					second_buffer[ch][i] = sample[ch];
				}
			}
			second_buffer_actual_size = second_buffer_allocated_size;
		}
		else
		{
			second_buffer_actual_size = 0;
		}

		cursor_position_in_second_buffer = 0;
	}

	void MovedSound::updatePlayhead()
	{
		playhead = second_buffer_start_time + (double)cursor_position_in_second_buffer / getRate();
	}


	// TODO Add second buffer size parameter
	MovedSound::MovedSound() :
			sound(NULL), delay(0), playhead(0),
			second_buffer(NULL),
			cursor_position_in_second_buffer(256),
			second_buffer_allocated_size(256),
			second_buffer_start_time(0),
			second_buffer_actual_size(0)
	{
		second_buffer = new float*[MAX_CHANNELS];
		for (int i = 0; i < MAX_CHANNELS; i++)
		{
			second_buffer[i] = new float[second_buffer_allocated_size];
		}

		fillBuffer();
	}

	const float* MovedSound::readSample()
	{
		bool playhead_was_negative = playhead < getStartTime();

		cursor_position_in_second_buffer ++;
		updatePlayhead();

		if (playhead >= getStartTime() && playhead_was_negative)
		{
			fillBuffer();
		}

		int channels = (sound != NULL ? sound->getChannels() : MAX_CHANNELS);

		if (second_buffer_actual_size > 0)
		{

			if (cursor_position_in_second_buffer >= second_buffer_allocated_size)
			{
				fillBuffer();
			}

			for (int i = 0; i < channels; i++)
			{
				read_buffer[i] = second_buffer[i][cursor_position_in_second_buffer];
			}
		}
		else
		{
			for (int i = 0; i < channels; i++)
			{
				read_buffer[i] = 0.f;
			}
		}

		return read_buffer;
	}

	int MovedSound::getChannels() const
	{
		if (sound == NULL)
			return 0;
		else
			return sound->getChannels();
	}

	void MovedSound::rewind(double position)
	{
		playhead = position;
		fillBuffer();
	}

	double MovedSound::getPlayhead() const
	{
		return playhead;
	}

	double MovedSound::getStartTime() const
	{
		return sound->getStartTime() + delay;
	}

	double MovedSound::getEndTime() const
	{
		return sound->getEndTime() + delay;
	}

	int MovedSound::getRate() const
	{
		return sound->getRate();
	}

	void MovedSound::setSound(SoundSource& sound)
	{
		this->sound = &sound;
	}

	void MovedSound::setDelay(double value)
	{
		this->delay = value;
	}

	MovedSound::~MovedSound()
	{
		for (int i = 0; i < MAX_CHANNELS; i++)
		{
			delete [] second_buffer[i];
		}
		delete [] second_buffer;
	}

} /* namespace vam */
