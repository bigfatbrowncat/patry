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
		SoundSource* sound;
		double delay;
		double playhead;

		void checkSoundPosition();
	public:
		MovedSound();
		virtual ~MovedSound();

		virtual const float* readSample();
		virtual int getChannels() const;
		virtual void rewind(double position);
		virtual double getPlayhead() const;
		virtual double getLength() const;
		virtual int getRate() const;
	};

} /* namespace vam */
#endif /* MOVEDSOUND_H_ */
