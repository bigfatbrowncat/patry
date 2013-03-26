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

	MovedSound::MovedSound() :
			sound(NULL), delay(0), playhead(0)
	{
	}

	void MovedSound::checkSoundPosition()
	{
		if (fabs(sound->getPlayhead() - playhead - delay) > 1.0 / sound->getRate())
		{
			sound->rewind(playhead);
		}
	}

	const float* MovedSound::readSample()
	{
		checkSoundPosition();
		return sound->readSample();
	}

	int MovedSound::getChannels() const
	{
		return sound->getChannels();
	}

	void MovedSound::rewind(double position)
	{
		playhead = position;
		checkSoundPosition();
	}

	double MovedSound::getPlayhead() const
	{
		return playhead;
	}

	double MovedSound::getLength() const
	{
		// TODO Rename "Length" to "End" and add "Begin" (which would be -delay here)
		return sound->getLength() - delay;
	}

	int MovedSound::getRate() const
	{
		return sound->getRate();
	}


	MovedSound::~MovedSound()
	{
	}

} /* namespace vam */
