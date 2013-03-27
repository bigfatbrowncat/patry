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
		if (sound != NULL)
		{
			if (fabs(sound->getPlayhead() - playhead + delay) > 1.0 / sound->getRate())
			{
				sound->rewind(playhead - delay);
			}
		}
	}

	void MovedSound::fillBuffer()
	{
		//if (second_buffer_position >= second_buffer_size)
		//{
			checkSoundPosition();

			second_buffer_start_time = playhead;

			if (sound != NULL)
			{
				for (int i = 0; i < second_buffer_size; i++)
				{
					const float* sample = sound->readSample();
					for (int ch = 0; ch < sound->getChannels(); ch++)
					{
						second_buffer[ch][i] = sample[ch];
					}
				}
				state = sound->getState();
			}
			else
			{
				for (int i = 0; i < second_buffer_size; i++)
				{
					for (int ch = 0; ch < MAX_CHANNELS; ch++)
					{
						second_buffer[ch][i] = 0;
					}
				}
				state = sAfterEnd;
			}

			cursor_position_in_second_buffer = 0;
	//	}
	}

	void MovedSound::updatePlayhead()
	{
		playhead = second_buffer_start_time + (double)cursor_position_in_second_buffer / getRate();
	}


	// TODO Add second buffer size parameter
	MovedSound::MovedSound() :
			sound(NULL), delay(0), playhead(0),
			second_buffer(NULL),
			cursor_position_in_second_buffer(256), second_buffer_size(256), second_buffer_start_time(0)
	{
		second_buffer = new float*[MAX_CHANNELS];
		for (int i = 0; i < MAX_CHANNELS; i++)
		{
			second_buffer[i] = new float[second_buffer_size];
		}

		fillBuffer();
	}

	const float* MovedSound::readSample()
	{
		cursor_position_in_second_buffer ++;
		updatePlayhead();

		if (cursor_position_in_second_buffer >= second_buffer_size)
		{
			fillBuffer();
		}

		int channels = (sound != NULL ? sound->getChannels() : MAX_CHANNELS);
		if (state == sReading)
		{
			for (int i = 0; i < channels; i++)
			{
				read_buffer[i] = second_buffer[i][cursor_position_in_second_buffer];
			}
			updatePlayhead();
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
	}

	double MovedSound::getPlayhead() const
	{
		return playhead;
	}

	double MovedSound::getLength() const
	{
		// TODO Rename "Length" to "End" and add "Begin" (which would be -delay here)
		return sound->getLength() + delay;
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
