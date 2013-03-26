/*
 * SoundSource_jni.cpp
 *
 *  Created on: Mar 26, 2013
 *      Author: imizus
 */

// System includes
#include <jni.h>

// JNI includes
#include <vam/SoundSource.h>

// Local includes
#include "SoundSource.h"
#include "JNITools.h"

using namespace vam;

extern "C"
{

	JNIEXPORT jobject JNICALL Java_vam_SoundSource_getState(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		SoundSource* nativeInstance = (SoundSource*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwSoundSourceResourcesDeallocated(env); return NULL; }

		jclass state_class = env->FindClass("vam/SoundSource$State");
		jmethodID fromValue_method = env->GetMethodID(state_class, "fromValue", "(I)Lvam/SoundSource$State;");

		int state = (int)nativeInstance->getState();

		jobject state_object = env->CallStaticObjectMethod(state_class, fromValue_method, state);
		return state_object;
	}

}
