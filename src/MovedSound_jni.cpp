/*
 * MovedSound_jni.cpp
 *
 *  Created on: Mar 26, 2013
 *      Author: imizus
 */

#include <stddef.h>

// JNI includes
#include <vam/MovedSound.h>

// Local includes
#include "MovedSound.h"
#include "JNITools.h"

using namespace vam;

void throwMovedSoundResourcesDeallocated(JNIEnv * env)
{
	jclass resourcesDeallocatedException_class = env->FindClass("vam/ResourcesDeallocatedException");
	env->ThrowNew(resourcesDeallocatedException_class, "Resources of the MovedSound object are deallocated");
}

extern "C"
{
	JNIEXPORT jlong JNICALL Java_vam_MovedSound_createNativeInstance(JNIEnv * env, jclass movedSound_class)
	{
		MovedSound* nativeInstance = new MovedSound();
		return (jlong)nativeInstance;
	}

	JNIEXPORT void JNICALL Java_vam_MovedSound_destroyNativeInstance(JNIEnv * env, jobject movedSound_object)
	{
		jclass movedSound_class = env->GetObjectClass(movedSound_object);
		jfieldID nativeInstance_field = env->GetFieldID(movedSound_class, "nativeInstance", "J");
		MovedSound* nativeInstance = (MovedSound*)env->GetLongField(movedSound_object, nativeInstance_field);

		if (nativeInstance != NULL)	delete nativeInstance;
	}

	JNIEXPORT jfloatArray JNICALL Java_vam_MovedSound_readSample(JNIEnv * env, jobject movedSound_object)
	{
		jclass movedSound_class = env->GetObjectClass(movedSound_object);
		jfieldID nativeInstance_field = env->GetFieldID(movedSound_class, "nativeInstance", "J");
		MovedSound* nativeInstance = (MovedSound*)env->GetLongField(movedSound_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwMovedSoundResourcesDeallocated(env); return NULL; }

/*		try
		{*/
			const float* sample = nativeInstance->readSample();
			int channels = nativeInstance->getChannels();

			jfloatArray sample_array = env->NewFloatArray(channels);
			env->SetFloatArrayRegion(sample_array, 0, channels, sample);
			return sample_array;
/*		}
		catch (const MovedSound::Error& err)
		{
			throwJavaMovedSoundError(env, err);
		}*/
		return NULL;

	}

	JNIEXPORT void JNICALL Java_vam_MovedSound_rewind(JNIEnv * env, jobject movedSound_object, jdouble position)
	{
		jclass movedSound_class = env->GetObjectClass(movedSound_object);
		jfieldID nativeInstance_field = env->GetFieldID(movedSound_class, "nativeInstance", "J");
		MovedSound* nativeInstance = (MovedSound*)env->GetLongField(movedSound_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwMovedSoundResourcesDeallocated(env); return; }

//		try
		{
			nativeInstance->rewind(position);
		}
/*		catch (const MovedSound::Error& err)
		{
			throwJavaMovedSoundError(env, err);
		}*/
	}

	JNIEXPORT jdouble JNICALL Java_vam_MovedSound_getPlayhead(JNIEnv * env, jobject movedSound_object)
	{
		jclass movedSound_class = env->GetObjectClass(movedSound_object);
		jfieldID nativeInstance_field = env->GetFieldID(movedSound_class, "nativeInstance", "J");
		MovedSound* nativeInstance = (MovedSound*)env->GetLongField(movedSound_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwMovedSoundResourcesDeallocated(env); return 0; }

		return nativeInstance->getPlayhead();
	}

	JNIEXPORT jdouble JNICALL Java_vam_MovedSound_getStartTime(JNIEnv * env, jobject movedSound_object)
	{
		jclass movedSound_class = env->GetObjectClass(movedSound_object);
		jfieldID nativeInstance_field = env->GetFieldID(movedSound_class, "nativeInstance", "J");
		MovedSound* nativeInstance = (MovedSound*)env->GetLongField(movedSound_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwMovedSoundResourcesDeallocated(env); return 0; }

		return nativeInstance->getStartTime();
	}

	JNIEXPORT jdouble JNICALL Java_vam_MovedSound_getEndTime(JNIEnv * env, jobject movedSound_object)
	{
		jclass movedSound_class = env->GetObjectClass(movedSound_object);
		jfieldID nativeInstance_field = env->GetFieldID(movedSound_class, "nativeInstance", "J");
		MovedSound* nativeInstance = (MovedSound*)env->GetLongField(movedSound_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwMovedSoundResourcesDeallocated(env); return 0; }

		return nativeInstance->getEndTime();
	}

	JNIEXPORT jint JNICALL Java_vam_MovedSound_getChannels(JNIEnv * env, jobject movedSound_object)
	{
		jclass movedSound_class = env->GetObjectClass(movedSound_object);
		jfieldID nativeInstance_field = env->GetFieldID(movedSound_class, "nativeInstance", "J");
		MovedSound* nativeInstance = (MovedSound*)env->GetLongField(movedSound_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwMovedSoundResourcesDeallocated(env); return 0; }

		return nativeInstance->getChannels();
	}

	JNIEXPORT jint JNICALL Java_vam_MovedSound_getRate(JNIEnv * env, jobject movedSound_object)
	{
		jclass movedSound_class = env->GetObjectClass(movedSound_object);
		jfieldID nativeInstance_field = env->GetFieldID(movedSound_class, "nativeInstance", "J");
		MovedSound* nativeInstance = (MovedSound*)env->GetLongField(movedSound_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwMovedSoundResourcesDeallocated(env); return 0; }

		return nativeInstance->getRate();
	}


	JNIEXPORT void JNICALL Java_vam_MovedSound_setSoundNative(JNIEnv * env, jobject movedSound_object, jobject soundSource_object)
	{
		// Getting the MovedSound native object
		jclass movedSound_class = env->GetObjectClass(movedSound_object);
		jfieldID movedSound_nativeInstance_field = env->GetFieldID(movedSound_class, "nativeInstance", "J");
		MovedSound* movedSound_nativeInstance = (MovedSound*)env->GetLongField(movedSound_object, movedSound_nativeInstance_field);
		if (movedSound_nativeInstance == NULL) { throwMovedSoundResourcesDeallocated(env); return; }

		// Getting the SoundSource native object
		jclass soundSource_class = env->GetObjectClass(soundSource_object);
		jfieldID soundSource_nativeInstance_field = env->GetFieldID(movedSound_class, "nativeInstance", "J");
		SoundSource* soundSource_nativeInstance = (SoundSource*)env->GetLongField(soundSource_object, soundSource_nativeInstance_field);
		if (soundSource_nativeInstance == NULL) { throwSoundSourceResourcesDeallocated(env); return; }

		// Connecting
		movedSound_nativeInstance->setSound(*soundSource_nativeInstance);

	}

	JNIEXPORT void JNICALL Java_vam_MovedSound_setDelay(JNIEnv * env, jobject movedSound_object, jdouble value)
	{
		jclass movedSound_class = env->GetObjectClass(movedSound_object);
		jfieldID nativeInstance_field = env->GetFieldID(movedSound_class, "nativeInstance", "J");
		MovedSound* nativeInstance = (MovedSound*)env->GetLongField(movedSound_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwMovedSoundResourcesDeallocated(env); return; }

		nativeInstance->setDelay(value);
	}

}
