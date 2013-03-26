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

jobject errorType(JNIEnv * env, MixedSounds::ErrorType errorType)
{
	jclass error_class = env->FindClass("vam/MixedSounds$ErrorType");
	jmethodID fromValue_method = env->GetStaticMethodID(error_class, "fromValue", "(I)Lvam/MixedSounds$ErrorType;");

	int value = (int)errorType;

	return env->CallStaticObjectMethod(error_class, fromValue_method, value);
}

void throwJavaMixedSoundsError(JNIEnv * env, const MixedSounds::Error& err)
{
	jclass error_class = env->FindClass("vam/MixedSounds$Error");
	jmethodID error_constructor = env->GetMethodID(
			error_class, "<init>", "(Lvam/MixedSounds$ErrorType;Ljava/lang/String;)V");

	jobject errorType_object = errorType(env, err.getType());

#if __SIZEOF_WCHAR_T__ == 2
	// (It's likely mingw32) Here we have equality between wchar_t and jchar
	const jchar* caller_jchar = (const jchar*)err.getCaller().c_str();
#else
	// Converting wchar_t string to jchar string
	jchar caller_jchar[err.getCaller().length()];
	const wchar_t* caller_wchar = err.getCaller().c_str();
	for (int i = 0; i < err.getCaller().length(); i++)
	{
		caller_jchar[i] = (jchar)caller_wchar[i];
	}
#endif

	jstring caller_string = env->NewString(caller_jchar, err.getCaller().length());

	jthrowable error_exception = (jthrowable)env->NewObject(error_class, error_constructor, errorType_object, caller_string);
	env->Throw(error_exception);
}

void throwMixedSoundsResourcesDeallocated(JNIEnv * env)
{
	jclass resourcesDeallocatedException_class = env->FindClass("vam/ResourcesDeallocatedException");
	env->ThrowNew(resourcesDeallocatedException_class, "Resources of the MixedSounds object are deallocated");
}

extern "C"
{

	JNIEXPORT jlong JNICALL Java_vam_MixedSounds_createNativeInstance(JNIEnv * env, jclass mixedSounds_class)
	{
		MixedSounds* nativeInstance = NULL;
		try
		{
			nativeInstance = new MixedSounds();
		}
		catch (const MixedSounds::Error& err)
		{
			throwJavaMixedSoundsError(env, err);
		}

		return (jlong)nativeInstance;
	}

	JNIEXPORT void JNICALL Java_vam_MixedSounds_destroyNativeInstance(JNIEnv * env, jobject mixedSounds_object)
	{
		jclass mixedSounds_class = env->GetObjectClass(mixedSounds_object);
		jfieldID nativeInstance_field = env->GetFieldID(mixedSounds_class, "nativeInstance", "J");
		MixedSounds* nativeInstance = (MixedSounds*)env->GetLongField(mixedSounds_object, nativeInstance_field);

		if (nativeInstance != NULL)	delete nativeInstance;
	}

	JNIEXPORT jfloatArray JNICALL Java_vam_MixedSounds_readSample(JNIEnv * env, jobject mixedSounds_object)
	{
		jclass mixedSounds_class = env->GetObjectClass(mixedSounds_object);
		jfieldID nativeInstance_field = env->GetFieldID(mixedSounds_class, "nativeInstance", "J");
		MixedSounds* nativeInstance = (MixedSounds*)env->GetLongField(mixedSounds_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwMixedSoundsResourcesDeallocated(env); return NULL; }

		try
		{
			const float* sample = nativeInstance->readSample();
			int channels = nativeInstance->getChannels();

			jfloatArray sample_array = env->NewFloatArray(channels);
			env->SetFloatArrayRegion(sample_array, 0, channels, sample);
			return sample_array;
		}
		catch (const MixedSounds::Error& err)
		{
			throwJavaMixedSoundsError(env, err);
		}
		return NULL;

	}

	JNIEXPORT void JNICALL Java_vam_MixedSounds_rewind(JNIEnv * env, jobject mixedSounds_object, jdouble position)
	{
		jclass mixedSounds_class = env->GetObjectClass(mixedSounds_object);
		jfieldID nativeInstance_field = env->GetFieldID(mixedSounds_class, "nativeInstance", "J");
		MixedSounds* nativeInstance = (MixedSounds*)env->GetLongField(mixedSounds_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwMixedSoundsResourcesDeallocated(env); return; }

		try
		{
			nativeInstance->rewind(position);
		}
		catch (const MixedSounds::Error& err)
		{
			throwJavaMixedSoundsError(env, err);
		}
	}

	JNIEXPORT jdouble JNICALL Java_vam_MixedSounds_getPlayhead(JNIEnv * env, jobject mixedSounds_object)
	{
		jclass mixedSounds_class = env->GetObjectClass(mixedSounds_object);
		jfieldID nativeInstance_field = env->GetFieldID(mixedSounds_class, "nativeInstance", "J");
		MixedSounds* nativeInstance = (MixedSounds*)env->GetLongField(mixedSounds_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwMixedSoundsResourcesDeallocated(env); return 0; }

		return nativeInstance->getPlayhead();
	}

	JNIEXPORT jdouble JNICALL Java_vam_MixedSounds_getLength(JNIEnv * env, jobject mixedSounds_object)
	{
		jclass mixedSounds_class = env->GetObjectClass(mixedSounds_object);
		jfieldID nativeInstance_field = env->GetFieldID(mixedSounds_class, "nativeInstance", "J");
		MixedSounds* nativeInstance = (MixedSounds*)env->GetLongField(mixedSounds_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwMixedSoundsResourcesDeallocated(env); return 0; }

		return nativeInstance->getLength();
	}

	JNIEXPORT jint JNICALL Java_vam_MixedSounds_getChannels(JNIEnv * env, jobject mixedSounds_object)
	{
		jclass mixedSounds_class = env->GetObjectClass(mixedSounds_object);
		jfieldID nativeInstance_field = env->GetFieldID(mixedSounds_class, "nativeInstance", "J");
		MixedSounds* nativeInstance = (MixedSounds*)env->GetLongField(mixedSounds_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwMixedSoundsResourcesDeallocated(env); return 0; }

		return nativeInstance->getChannels();
	}

	JNIEXPORT jint JNICALL Java_vam_MixedSounds_getRate(JNIEnv * env, jobject mixedSounds_object)
	{
		jclass mixedSounds_class = env->GetObjectClass(mixedSounds_object);
		jfieldID nativeInstance_field = env->GetFieldID(mixedSounds_class, "nativeInstance", "J");
		MixedSounds* nativeInstance = (MixedSounds*)env->GetLongField(mixedSounds_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwMixedSoundsResourcesDeallocated(env); return 0; }

		return nativeInstance->getRate();
	}

	JNIEXPORT void JNICALL Java_vam_MixedSounds_addSoundNative(JNIEnv * env, jobject mixedSounds_object, jobject soundSource_object)
	{
		// Getting the PortAudio native object
		jclass mixedSounds_class = env->GetObjectClass(mixedSounds_object);
		jfieldID mixedSounds_nativeInstance_field = env->GetFieldID(mixedSounds_class, "nativeInstance", "J");
		MixedSounds* mixedSounds_nativeInstance = (MixedSounds*)env->GetLongField(mixedSounds_object, mixedSounds_nativeInstance_field);
		if (mixedSounds_nativeInstance == NULL) { throwMixedSoundsResourcesDeallocated(env); return; }

		// Getting the SoundSource native object
		jclass soundSource_class = env->GetObjectClass(soundSource_object);
		jfieldID soundSource_nativeInstance_field = env->GetFieldID(mixedSounds_class, "nativeInstance", "J");
		SoundSource* soundSource_nativeInstance = (SoundSource*)env->GetLongField(soundSource_object, soundSource_nativeInstance_field);
		if (soundSource_nativeInstance == NULL) { throwSoundSourceResourcesDeallocated(env); return; }

		// Connecting
		mixedSounds_nativeInstance->addSound(*soundSource_nativeInstance);
	}

	JNIEXPORT void JNICALL Java_vam_MixedSounds_removeSoundNative(JNIEnv * env, jobject mixedSounds_object, jobject soundSource_object)
	{
		// Getting the PortAudio native object
		jclass mixedSounds_class = env->GetObjectClass(mixedSounds_object);
		jfieldID mixedSounds_nativeInstance_field = env->GetFieldID(mixedSounds_class, "nativeInstance", "J");
		MixedSounds* mixedSounds_nativeInstance = (MixedSounds*)env->GetLongField(mixedSounds_object, mixedSounds_nativeInstance_field);
		if (mixedSounds_nativeInstance == NULL) { throwMixedSoundsResourcesDeallocated(env); return; }

		// Getting the SoundSource native object
		jclass soundSource_class = env->GetObjectClass(soundSource_object);
		jfieldID soundSource_nativeInstance_field = env->GetFieldID(mixedSounds_class, "nativeInstance", "J");
		SoundSource* soundSource_nativeInstance = (SoundSource*)env->GetLongField(soundSource_object, soundSource_nativeInstance_field);
		if (soundSource_nativeInstance == NULL) { throwSoundSourceResourcesDeallocated(env); return; }

		// Connecting
		mixedSounds_nativeInstance->removeSound(*soundSource_nativeInstance);
	}
}
