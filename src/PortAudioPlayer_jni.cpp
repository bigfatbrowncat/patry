/*
 * PortAudioPlayer_jni.cpp
 *
 *  Created on: Mar 22, 2013
 *      Author: imizus
 */

// JNI includes
#include <vam/PortAudioPlayer.h>

// Local includes
#include "PortAudioPlayer.h"

using namespace vam;

extern "C"
{

	JNIEXPORT jlong JNICALL Java_vam_PortAudioPlayer_createNativeInstance(JNIEnv * env, jclass portAudioPlayer_class, jint channels, jint rate, jint frames_per_buffer)
	{
		PortAudioPlayer* nativeInstance = NULL;
//		try
		{
			nativeInstance = new PortAudioPlayer(channels, rate, frames_per_buffer);
		}
//		catch (const PortAudioPlayer::Error& err)
		{
//			throwJavaVorbisFileReaderError(env, err);
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

		// Getting the SoundSource native object
		jclass soundSource_class = env->GetObjectClass(soundSource_object);
		jfieldID soundSource_nativeInstance_field = env->GetFieldID(portAudioPlayer_class, "nativeInstance", "J");
		SoundSource* soundSource_nativeInstance = (SoundSource*)env->GetLongField(soundSource_object, soundSource_nativeInstance_field);

		// Connecting
		portAudioPlayer_nativeInstance->setSoundSource(*soundSource_nativeInstance);
	}

	JNIEXPORT void JNICALL Java_vam_PortAudioPlayer_play(JNIEnv * env, jobject portAudioPlayer_object)
	{
		// Getting the PortAudio native object
		jclass portAudioPlayer_class = env->GetObjectClass(portAudioPlayer_object);
		jfieldID portAudioPlayer_nativeInstance_field = env->GetFieldID(portAudioPlayer_class, "nativeInstance", "J");
		PortAudioPlayer* portAudioPlayer_nativeInstance = (PortAudioPlayer*)env->GetLongField(portAudioPlayer_object, portAudioPlayer_nativeInstance_field);

		portAudioPlayer_nativeInstance->play();
	}

	JNIEXPORT void JNICALL Java_vam_PortAudioPlayer_stop(JNIEnv * env, jobject portAudioPlayer_object)
	{
		// Getting the PortAudio native object
		jclass portAudioPlayer_class = env->GetObjectClass(portAudioPlayer_object);
		jfieldID portAudioPlayer_nativeInstance_field = env->GetFieldID(portAudioPlayer_class, "nativeInstance", "J");
		PortAudioPlayer* portAudioPlayer_nativeInstance = (PortAudioPlayer*)env->GetLongField(portAudioPlayer_object, portAudioPlayer_nativeInstance_field);

		portAudioPlayer_nativeInstance->stop();
	}

}
