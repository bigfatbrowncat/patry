/*
 * MixedSounds.cpp
 *
 *  Created on: Mar 25, 2013
 *      Author: imizus
 */

#include <math.h>

#include "MixedSounds.h"

namespace vam
{

	MixedSounds::MixedSounds() : playhead(0)
	{
		samples = new float[MAX_CHANNELS];
	}

	void MixedSounds::checkSoundPositions()
	{
		for (list<SoundSource*>::iterator iter = sounds.begin(); iter != sounds.end(); iter++)
		{
			if (fabs((*iter)->getPlayhead() - playhead) > 1.0 / (*iter)->getRate())
			{
				(*iter)->rewind(playhead);
			}
		}
	}

	const float* MixedSounds::readSample()
	{
		// TODO No buffering yet, but it should be added

		int channels = getChannels();

		for (int i = 0; i < channels; i++)
		{
			samples[i] = 0;
		}

		// Checking for playheads equality
		checkSoundPositions();

		for (list<SoundSource*>::iterator iter = sounds.begin(); iter != sounds.end(); iter++)
		{
			const float* iterSample = (*iter)->readSample();
			switch ((*iter)->getChannels())
			{
			case 1:
				for (int i = 0; i < channels; i++)
				{
					samples[i] += iterSample[0];
				}
				break;
			case 2:
				for (int i = 0; i < channels; i++)
				{
					samples[i] += iterSample[i];
				}
				break;
			default:
				throw Error(etUnsupportedChannelsNumber, L"readSample");
			}

		}

		playhead += 1.0 / getRate();

		return samples;
	}

	int MixedSounds::getChannels() const
	{
		int channels_max = 0;
		for (list<SoundSource*>::const_iterator iter = sounds.begin(); iter != sounds.end(); iter++)
		{
			if ((*iter)->getChannels() > channels_max)
			{
				channels_max = (*iter)->getChannels();
			}
		}
		return channels_max;
	}

	void MixedSounds::rewind(double position)
	{
		playhead = position;
		for (list<SoundSource*>::iterator iter = sounds.begin(); iter != sounds.end(); iter++)
		{
			(*iter)->rewind(position);
		}
	}

	double MixedSounds::getStartTime() const
	{
		double start_min = 0;
		for (list<SoundSource*>::const_iterator iter = sounds.begin(); iter != sounds.end(); iter++)
		{
			if ((*iter)->getStartTime() < start_min)
			{
				start_min = (*iter)->getStartTime();
			}
		}
		return start_min;
	}

	double MixedSounds::getEndTime() const
	{
		double end_max = 0;
		for (list<SoundSource*>::const_iterator iter = sounds.begin(); iter != sounds.end(); iter++)
		{
			if ((*iter)->getEndTime() > end_max)
			{
				end_max = (*iter)->getEndTime();
			}
		}
		return end_max;
	}

	int MixedSounds::getRate() const
	{
		if (sounds.size() == 0)
		{
			return 44100;	// By default
		}
		else
		{
			return sounds.front()->getRate();
		}
	}

	void MixedSounds::addSound(SoundSource& sound)
	{
		if (sound.getChannels() > MAX_CHANNELS)
		{
			throw Error(etUnsupportedChannelsNumber, L"addSound");
		}

		if (sounds.size() > 0 && sound.getRate() != sounds.front()->getRate())
		{
			throw Error(etInequalRate, L"addSound");
		}

		sounds.push_back(&sound);
	}

	void MixedSounds::removeSound(SoundSource& sound)
	{
		sounds.remove(&sound);
	}

	MixedSounds::~MixedSounds()
	{
		delete [] samples;
	}

} /* namespace vam */
