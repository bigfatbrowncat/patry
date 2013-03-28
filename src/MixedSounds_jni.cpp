/*
 * MixedSounds_jni.cpp
 *
 *  Created on: 25.03.2013
 *      Author: il
 */

#include <stddef.h>

// JNI includes
#include <vam/MixedSounds.h>

// Local includes
#include "MixedSounds.h"
#include "JNITools.h"

using namespace vam;

extern "C"
{

	JNIEXPORT jlong JNICALL Java_vam_MixedSounds_createNativeInstance(JNIEnv * env, jclass mixedSounds_class)
	{
		MixedSounds* nativeInstance = NULL;
		try
		{
			nativeInstance = new MixedSounds();
		}
		catch (const SoundSource::Error& err)
		{
			catchSoundSourceErrors(env, err);
		}


		return (jlong)nativeInstance;
	}

	JNIEXPORT void JNICALL Java_vam_MixedSounds_destroyNativeInstance(JNIEnv * env, jobject mixedSounds_object)
	{
		MixedSounds* nativeInstance = getAndCheckNativeInstance<MixedSounds>(env, mixedSounds_object);

		if (nativeInstance != NULL)	delete nativeInstance;
	}

	JNIEXPORT jfloatArray JNICALL Java_vam_MixedSounds_readSample(JNIEnv * env, jobject mixedSounds_object)
	{
		MixedSounds* nativeInstance = getAndCheckNativeInstance<MixedSounds>(env, mixedSounds_object);

		try
		{
			const float* sample = nativeInstance->readSample();
			int channels = nativeInstance->getChannels();

			jfloatArray sample_array = env->NewFloatArray(channels);
			env->SetFloatArrayRegion(sample_array, 0, channels, sample);
			return sample_array;
		}
		catch (const SoundSource::Error& err)
		{
			catchSoundSourceErrors(env, err);
			return NULL;
		}

	}

	JNIEXPORT void JNICALL Java_vam_MixedSounds_rewind(JNIEnv * env, jobject mixedSounds_object, jdouble position)
	{
		MixedSounds* nativeInstance = getAndCheckNativeInstance<MixedSounds>(env, mixedSounds_object);

		try
		{
			nativeInstance->rewind(position);
		}
		catch (const SoundSource::Error& err)
		{
			catchSoundSourceErrors(env, err);
		}
	}

	JNIEXPORT jdouble JNICALL Java_vam_MixedSounds_getPlayhead(JNIEnv * env, jobject mixedSounds_object)
	{
		MixedSounds* nativeInstance = getAndCheckNativeInstance<MixedSounds>(env, mixedSounds_object);

		return nativeInstance->getPlayhead();
	}

	JNIEXPORT jdouble JNICALL Java_vam_MixedSounds_getStartTime(JNIEnv * env, jobject mixedSounds_object)
	{
		MixedSounds* nativeInstance = getAndCheckNativeInstance<MixedSounds>(env, mixedSounds_object);

		return nativeInstance->getStartTime();
	}

	JNIEXPORT jdouble JNICALL Java_vam_MixedSounds_getEndTime(JNIEnv * env, jobject mixedSounds_object)
	{
		MixedSounds* nativeInstance = getAndCheckNativeInstance<MixedSounds>(env, mixedSounds_object);

		return nativeInstance->getEndTime();
	}

	JNIEXPORT jint JNICALL Java_vam_MixedSounds_getChannels(JNIEnv * env, jobject mixedSounds_object)
	{
		MixedSounds* nativeInstance = getAndCheckNativeInstance<MixedSounds>(env, mixedSounds_object);

		return nativeInstance->getChannels();
	}

	JNIEXPORT jint JNICALL Java_vam_MixedSounds_getRate(JNIEnv * env, jobject mixedSounds_object)
	{
		MixedSounds* nativeInstance = getAndCheckNativeInstance<MixedSounds>(env, mixedSounds_object);

		return nativeInstance->getRate();
	}

	JNIEXPORT void JNICALL Java_vam_MixedSounds_addSoundNative(JNIEnv * env, jobject mixedSounds_object, jobject soundSource_object)
	{
		// Getting the MixedSounds native object
		MixedSounds* mixedSounds_nativeInstance = getAndCheckNativeInstance<MixedSounds>(env, mixedSounds_object);

		// Getting the SoundSource native object
		SoundSource* soundSource_nativeInstance = getAndCheckNativeInstance<MixedSounds>(env, soundSource_object);

		// Connecting
		mixedSounds_nativeInstance->addSound(*soundSource_nativeInstance);
	}

	JNIEXPORT void JNICALL Java_vam_MixedSounds_removeSoundNative(JNIEnv * env, jobject mixedSounds_object, jobject soundSource_object)
	{
		// Getting the MixedSounds native object
		MixedSounds* mixedSounds_nativeInstance = getAndCheckNativeInstance<MixedSounds>(env, mixedSounds_object);

		// Getting the SoundSource native object
		SoundSource* soundSource_nativeInstance = getAndCheckNativeInstance<MixedSounds>(env, soundSource_object);

		// Disconnecting
		mixedSounds_nativeInstance->removeSound(*soundSource_nativeInstance);
	}
}
