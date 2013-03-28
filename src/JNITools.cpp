/*
 * JNITools.cpp
 *
 *  Created on: 25.03.2013
 *      Author: il
 */

#include "JNITools.h"

#include "VorbisFileReader.h"
#include "MovedSound.h"
#include "MixedSounds.h"

using namespace vam;

void catchSoundSourceErrors(JNIEnv * env, const SoundSource::Error& error)
{
	{
		// For VorbisFileReader
		const VorbisFileReader::Error* vfr_error = dynamic_cast<const VorbisFileReader::Error*>(&error);
		if (vfr_error != NULL)
		{
			throwErrorForClass<VorbisFileReader>(env, *vfr_error);
			return;
		}
	}

	{
		// For MovedSound
		const MovedSound::Error* mos_error = dynamic_cast<const MovedSound::Error*>(&error);
		if (mos_error != NULL)
		{
			throwErrorForClass<MovedSound>(env, *mos_error);
			return;
		}
	}

	{
		// For MixedSounds
		const MixedSounds::Error* mis_error = dynamic_cast<const MixedSounds::Error*>(&error);
		if (mis_error != NULL)
		{
			throwErrorForClass<MixedSounds>(env, *mis_error);
			return;
		}
	}

}

