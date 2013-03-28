/*
 * JNITools.h
 *
 *  Created on: 25.03.2013
 *      Author: il
 *
 */

#ifndef JNITOOLS_H_
#define JNITOOLS_H_

#include <stddef.h>
#include <jni.h>

#include "SoundSource.h"

using namespace vam;

void catchSoundSourceErrors(JNIEnv * env, const SoundSource::Error& error);

#define TO_STRING(id)		#id

/**
 * This function is a piece of template magic used for constructing
 * a java ErrorType object for class T.
 *
 * It assumes that there is a class named T in "vam" Java package (in Java space) and that class T contains
 * a static class (preferably, enum) named ErrorType with static converter fromValue which takes a value equal
 * to T::ErrorType integer value.
 *
 * Example
 * -------
 *
 * C++ side:
 *
 * class SomeClass
 * {
 * public:
 *     enum ErrorType
 *     {
 *         value1 = 0,
 *         value2 = 1, ...
 *     }
 * }
 *
 *
 * Java side:
 *
 * public class SomeClass
 * {
 *     public static enum ErrorType
 *     {
 *         value1(0),
 *         value2(1), ...;
 *
 *         public static ErrorType fromValue(int i)
 *         {
 *             swutch (i)
 *             {
 *                 case 0: return value1;
 *                 case 1: return value2;
 *                 ...
 *             }
 *         }
 *     }
 * }
 *
 */
template<class T> jobject errorTypeForClass(JNIEnv * env, typename T::ErrorType errorType)
{
	jclass vorbisFileReaderError_class = env->FindClass("vam/" TO_STRING(T) "$ErrorType");
	jmethodID fromValue_method = env->GetStaticMethodID(vorbisFileReaderError_class, "fromValue", "(I)Lvam/" TO_STRING(T) "$ErrorType;");

	int value = (int)errorType;

	return env->CallStaticObjectMethod(vorbisFileReaderError_class, fromValue_method, value);
}

/**
 * This function is a piece of template magic used for throwing a proper
 * exception in Java space. It assumes everything that errorTypeForClass assumes and
 * in addition there should be an Error subclass for class T in C++ and in Java code,
 * which is a Throwable in Java. In C++ it should contain getType() which returns
 * an object of T::ErrorType compatible with the errorTypeForClass function, it
 * should contain getCaller() which has wstring type (and contain the information about
 * where the exception has been initiated).
 * T.Error should contain a constructor which takes its error type and the caller string.
 *
 * Example
 * -------
 *
 * C++ side:
 *
 * class SomeClass
 * {
 *     enum ErrorType
 *     {
 *         <the same as described in errorTypeForClass>
 *     }
 *
 *     class Error
 *     {
 *     public:
 *         ErrorType getType() { ... }
 *         wstring getCaller() { ... }
 *     }
 * }
 *
 *
 * Java side:
 *
 * public class SomeClass
 * {
 *     enum ErrorType
 *     {
 *         <the same as described in errorTypeForClass>
 *     }
 *
 *     public class Error
 *     {
 *         public Error(SomeClass.ErrorType errorType, String caller) { ... }
 *     }
 * }
 *
 */

template<class T> void throwErrorForClass(JNIEnv * env, const typename T::Error& error)
{
	jclass error_class = env->FindClass("vam/" TO_STRING(T) "$Error");
	jmethodID error_constructor = env->GetMethodID(
			error_class, "<init>", "(Lvam/" TO_STRING(T) "$ErrorType;ILjava/lang/String;)V");

	jobject errorType_object = errorTypeForClass<T>(env, error.getType());

#if __SIZEOF_WCHAR_T__ == 2
	// (It's likely mingw32) Here we have equality between wchar_t and jchar
	const jchar* caller_jchar = (const jchar*)error.getCaller().c_str();
#else
	// Converting wchar_t string to jchar string
	jchar caller_jchar[err.getCaller().length()];
	const wchar_t* caller_wchar = err.getCaller().c_str();
	for (int i = 0; i < err.getCaller().length(); i++)
	{
		caller_jchar[i] = (jchar)caller_wchar[i];
	}
#endif

	jstring caller_string = env->NewString(caller_jchar, error.getCaller().length());

	jthrowable error_exception = (jthrowable)env->NewObject(error_class, error_constructor, errorType_object, caller_string);
	env->Throw(error_exception);
}

template<class T> void throwResourcesDeallocatedErrorForClass(JNIEnv * env)
{
	jclass resourcesDeallocatedException_class = env->FindClass("vam/ResourcesDeallocatedException");
	env->ThrowNew(resourcesDeallocatedException_class, "Resources of the " TO_STRING(T) " object are deallocated");
}

template<class T> T* getAndCheckNativeInstance(JNIEnv * env, jobject object)
{
	jclass theclass = env->GetObjectClass(object);
	jfieldID nativeInstance_field = env->GetFieldID(theclass, "nativeInstance", "J");
	T* nativeInstance = (T*)env->GetLongField(object, nativeInstance_field);
	if (nativeInstance == NULL)
	{
		throwResourcesDeallocatedErrorForClass<T>(env);
		return NULL;
	}
	else
		return nativeInstance;
}

#endif /* JNITOOLS_H_ */
