/*
 * MovedSound_jni.cpp
 *
 *  Created on: Mar 26, 2013
 *      Author: imizus
 */

#include <vam/MovedSound.h>

extern "C"
{
	JNIEXPORT jlong JNICALL Java_vam_MovedSound_createNativeInstance(JNIEnv * env, jclass movedSound_class)
	{

	}

	JNIEXPORT void JNICALL Java_vam_MovedSound_destroyNativeInstance(JNIEnv * env, jobject movedSound_object)
	{

	}

	JNIEXPORT jfloatArray JNICALL Java_vam_MovedSound_readSample(JNIEnv * env, jobject movedSound_object)
	{

	}

	JNIEXPORT jint JNICALL Java_vam_MovedSound_getChannels(JNIEnv * env, jobject movedSound_object)
	{

	}

	JNIEXPORT void JNICALL Java_vam_MovedSound_rewind(JNIEnv * env, jobject movedSound_object, jdouble position)
	{

	}

	JNIEXPORT jdouble JNICALL Java_vam_MovedSound_getPlayhead(JNIEnv * env, jobject movedSound_object)
	{

	}

	JNIEXPORT jdouble JNICALL Java_vam_MovedSound_getLength(JNIEnv * env, jobject movedSound_object)
	{

	}

	JNIEXPORT jint JNICALL Java_vam_MovedSound_getRate(JNIEnv * env, jobject movedSound_object)
	{

	}

	JNIEXPORT void JNICALL Java_vam_MovedSound_setSoundNative(JNIEnv * env, jobject movedSound_object, jobject soundSource_object)
	{

	}
}
