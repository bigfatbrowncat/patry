/*
 * JNITools.cpp
 *
 *  Created on: 25.03.2013
 *      Author: il
 */

#include "JNITools.h"

void throwSoundSourceResourcesDeallocated(JNIEnv * env)
{
	jclass resourcesDeallocatedException_class = env->FindClass("vam/ResourcesDeallocatedException");
	env->ThrowNew(resourcesDeallocatedException_class, "Resources of the SoundSource object are deallocated");
}
