/*
 * PortAudioPlayer_jni.cpp
 *
 *  Created on: Mar 22, 2013
 *      Author: imizus
 */

#include <stddef.h>

// JNI includes
#include <vam/PortAudioPlayer.h>

// Local includes
#include "PortAudioPlayer.h"
#include "JNITools.h"

using namespace vam;

jobject errorType(JNIEnv * env, PortAudioPlayer::ErrorType errorType)
{
	jclass vorbisFileReaderError_class = env->FindClass("vam/PortAudioPlayer$ErrorType");
	jmethodID fromValue_method = env->GetStaticMethodID(vorbisFileReaderError_class, "fromValue", "(I)Lvam/PortAudioPlayer$ErrorType;");

	int value = (int)errorType;

	return env->CallStaticObjectMethod(vorbisFileReaderError_class, fromValue_method, value);
}

void throwJavaPortAudioPlayerError(JNIEnv * env, const PortAudioPlayer::Error& err)
{
	jclass vorbisFileReaderError_class = env->FindClass("vam/PortAudioPlayer$Error");
	jmethodID vorbisFileReaderError_constructor = env->GetMethodID(
			vorbisFileReaderError_class, "<init>", "(Lvam/PortAudioPlayer$ErrorType;I)V");

	jobject errorType_object = errorType(env, err.getType());

	jthrowable error_exception = (jthrowable)env->NewObject(vorbisFileReaderError_class, vorbisFileReaderError_constructor, errorType_object, err.getCode());
	env->Throw(error_exception);
}

void throwPortAudioPlayerResourcesDeallocated(JNIEnv * env)
{
	jclass resourcesDeallocatedException_class = env->FindClass("vam/ResourcesDeallocatedException");
	env->ThrowNew(resourcesDeallocatedException_class, "Resources of the PortAudioPlayer object are deallocated");
}

extern "C"
{

	JNIEXPORT jlong JNICALL Java_vam_PortAudioPlayer_createNativeInstance(JNIEnv * env, jclass portAudioPlayer_class, jint channels, jint rate, jint frames_per_buffer)
	{
		PortAudioPlayer* nativeInstance = NULL;
		try
		{
			nativeInstance = new PortAudioPlayer(channels, rate, frames_per_buffer);
		}
		catch (const PortAudioPlayer::Error& err)
		{
			throwJavaPortAudioPlayerError(env, err);
		}

		return (jlong)nativeInstance;
	}

	JNIEXPORT void JNICALL Java_vam_PortAudioPlayer_destroyNativeInstance(JNIEnv * env, jobject portAudioPlayer_object)
	{
		jclass portAudioPlayer_class = env->GetObjectClass(portAudioPlayer_object);
		jfieldID nativeInstance_field = env->GetFieldID(portAudioPlayer_class, "nativeInstance", "J");
		PortAudioPlayer* nativeInstance = (PortAudioPlayer*)env->GetLongField(portAudioPlayer_object, nativeInstance_field);

		if (nativeInstance != NULL)	delete nativeInstance;

	}

	JNIEXPORT void JNICALL Java_vam_PortAudioPlayer_setNativeSoundSource(JNIEnv * env, jobject portAudioPlayer_object, jobject soundSource_object)
	{
		// Getting the PortAudio native object
		jclass portAudioPlayer_class = env->GetObjectClass(portAudioPlayer_object);
		jfieldID portAudioPlayer_nativeInstance_field = env->GetFieldID(portAudioPlayer_class, "nativeInstance", "J");
		PortAudioPlayer* portAudioPlayer_nativeInstance = (PortAudioPlayer*)env->GetLongField(portAudioPlayer_object, portAudioPlayer_nativeInstance_field);
		if (portAudioPlayer_nativeInstance == NULL) { throwPortAudioPlayerResourcesDeallocated(env); return; }

		// Getting the SoundSource native object
		jclass soundSource_class = env->GetObjectClass(soundSource_object);
		jfieldID soundSource_nativeInstance_field = env->GetFieldID(portAudioPlayer_class, "nativeInstance", "J");
		SoundSource* soundSource_nativeInstance = (SoundSource*)env->GetLongField(soundSource_object, soundSource_nativeInstance_field);
		if (soundSource_nativeInstance == NULL) { throwSoundSourceResourcesDeallocated(env); return; }

		// Connecting
		portAudioPlayer_nativeInstance->setSoundSource(*soundSource_nativeInstance);
	}

	JNIEXPORT void JNICALL Java_vam_PortAudioPlayer_play(JNIEnv * env, jobject portAudioPlayer_object)
	{
		// Getting the PortAudio native object
		jclass portAudioPlayer_class = env->GetObjectClass(portAudioPlayer_object);
		jfieldID portAudioPlayer_nativeInstance_field = env->GetFieldID(portAudioPlayer_class, "nativeInstance", "J");
		PortAudioPlayer* portAudioPlayer_nativeInstance = (PortAudioPlayer*)env->GetLongField(portAudioPlayer_object, portAudioPlayer_nativeInstance_field);
		if (portAudioPlayer_nativeInstance == NULL) { throwPortAudioPlayerResourcesDeallocated(env); return; }

		try
		{
			portAudioPlayer_nativeInstance->play();
		}
		catch (const PortAudioPlayer::Error& err)
		{
			throwJavaPortAudioPlayerError(env, err);
		}

	}

	JNIEXPORT void JNICALL Java_vam_PortAudioPlayer_stop(JNIEnv * env, jobject portAudioPlayer_object)
	{
		// Getting the PortAudio native object
		jclass portAudioPlayer_class = env->GetObjectClass(portAudioPlayer_object);
		jfieldID portAudioPlayer_nativeInstance_field = env->GetFieldID(portAudioPlayer_class, "nativeInstance", "J");
		PortAudioPlayer* portAudioPlayer_nativeInstance = (PortAudioPlayer*)env->GetLongField(portAudioPlayer_object, portAudioPlayer_nativeInstance_field);
		if (portAudioPlayer_nativeInstance == NULL) { throwPortAudioPlayerResourcesDeallocated(env); return; }

		try
		{
			portAudioPlayer_nativeInstance->stop();
		}
		catch (const PortAudioPlayer::Error& err)
		{
			throwJavaPortAudioPlayerError(env, err);
		}
	}

}
