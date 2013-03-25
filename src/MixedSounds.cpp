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

	MixedSounds::MixedSounds()
	{
		samples = new float[MAX_CHANNELS];
	}

	const float* MixedSounds::readSample()
	{
		// Checking for playheads equality

		// TODO No buffering yet, but it should be added

		int channels = getChannels();

		for (int i = 0; i < channels; i++)
		{
			samples[i] = 0;
		}

		for (list<SoundSource*>::iterator iter = sounds.begin(); iter != sounds.end(); iter++)
		{
			if (fabs((*iter)->getPlayhead() - playhead) > 1.0 / (*iter)->getRate())
			{
				(*iter)->rewind(playhead);
			}

			const float* iterSample = (*iter)->readSample();
			switch ((*iter)->getChannels())
			{
			case 1:
				for (int i = 0; i < channels; i++)
				{
					samples[i] += iterSample[0] / sounds.size();
				}
				break;
			case 2:
				for (int i = 0; i < channels; i++)
				{
					samples[i] += iterSample[i] / sounds.size();
				}
				break;
			default:
				throw Error(etUnsupportedChannelsNumber, L"readSample");
			}
		}

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

	double MixedSounds::getLength() const
	{
		double length_max = 0;
		for (list<SoundSource*>::const_iterator iter = sounds.begin(); iter != sounds.end(); iter++)
		{
			if ((*iter)->getLength() > length_max)
			{
				length_max = (*iter)->getLength();
			}
		}
		return length_max;
	}


	void MixedSounds::addSound(SoundSource& sound)
	{
		if (sound.getChannels() > MAX_CHANNELS)
		{
			throw Error(etUnsupportedChannelsNumber, L"addSound");
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
