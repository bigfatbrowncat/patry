/*
 * MixedSounds.h
 *
 *  Created on: Mar 25, 2013
 *      Author: imizus
 */

#ifndef MIXEDSOUNDS_H_
#define MIXEDSOUNDS_H_

#include <list>
#include <string>

#include "SoundSource.h"

using namespace std;

namespace vam
{

	class MixedSounds: public SoundSource
	{
	public:
		enum ErrorType
		{
			etUnsupportedChannelsNumber		= 0,
			etInequalRate = 1
		};

		class Error
		{
			friend class MixedSounds;
		private:
			ErrorType type;
			wstring caller;
		protected:
			Error(ErrorType type, wstring caller) : type(type), caller(caller)
			{
			}
		public:
			ErrorType getType() const { return type; }
			wstring getCaller() const { return caller; }
		};

	private:
		list<SoundSource*> sounds;
		double playhead;
		float* samples;

		void checkSoundPositions();
	public:
		MixedSounds();
		virtual ~MixedSounds();

		virtual const float* readSample();
		virtual int getChannels() const;
		virtual void rewind(double position);
		virtual double getPlayhead() const { return playhead; }
		virtual double getStartTime() const;
		virtual double getEndTime() const;
		virtual int getRate() const;

		void addSound(SoundSource& sound);
		void removeSound(SoundSource& sound);
		const list<SoundSource*> getSounds() const { return sounds; }
	};

} /* namespace vam */
#endif /* MIXEDSOUNDS_H_ */
