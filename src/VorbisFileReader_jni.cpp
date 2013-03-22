// System includes
#include <jni.h>
#include <string>

// JNI includes
#include <vam/VorbisFileReader.h>

// Local includes
#include "VorbisFileReader.h"

using namespace vam;
using namespace std;

jobject errorType(JNIEnv * env, VorbisFileReader::ErrorType errorType)
{
	jclass vorbisFileReaderError_class = env->FindClass("vam/VorbisFileReader$ErrorType");
	jmethodID fromValue_method = env->GetStaticMethodID(vorbisFileReaderError_class, "fromValue", "(I)Lvam/VorbisFileReader$ErrorType;");

	int value = (int)errorType;

	return env->CallStaticObjectMethod(vorbisFileReaderError_class, fromValue_method, value);
}

void throwJavaVorbisFileReaderError(JNIEnv * env, jobject vorbisFileReader_object, const VorbisFileReader::Error& err)
{
	jclass vorbisFileReaderError_class = env->FindClass("vam/VorbisFileReader$Error");
	jmethodID vorbisFileReaderError_constructor = env->GetMethodID(
			vorbisFileReaderError_class, "<init>", "(Lvam/VorbisFileReader;Lvam/VorbisFileReader$ErrorType;ILjava/lang/String;)V");

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

	jstring caller_string = env->NewString(caller_jchar, err.getCaller().length()/* * sizeof(jchar)*/);	// TODO Resolve this strange behaviour

	jthrowable error_exception = (jthrowable)env->NewObject(vorbisFileReaderError_class, vorbisFileReaderError_constructor, vorbisFileReader_object, errorType_object, err.getCode(), caller_string);
	env->Throw(error_exception);
}

void throwResourcesDeallocated(JNIEnv * env)
{
	jclass vorbisFileReaderResourcesDeallocatedException_class = env->FindClass("vam/ResourcesDeallocatedException");
	env->ThrowNew(vorbisFileReaderResourcesDeallocatedException_class, "Resources of the VorbisFileReader object are deallocated");
}

extern "C"
{

	JNIEXPORT void JNICALL Java_vam_VorbisFileReader_createNativeInstance(
			JNIEnv * env, jobject vorbisFileReader_object, jstring file_name, jint buffer_size_request)
	{
#ifdef __MINGW32__
		const wchar_t* fileName = (const wchar_t*)env->GetStringChars(file_name, NULL);
#else
		const char* fileName = (char*)env->GetStringUTFChars(file_name, NULL);
#endif

		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");

		try
		{
#ifdef __MINGW32__
			VorbisFileReader* nativeInstance = new VorbisFileReader(wstring(fileName), buffer_size_request);
#else
			VorbisFileReader* nativeInstance = new VorbisFileReader(string(fileName), buffer_size_request);
#endif
			env->SetLongField(vorbisFileReader_object, nativeInstance_field, (long)nativeInstance);
		}
		catch (const VorbisFileReader::Error& err)
		{
			throwJavaVorbisFileReaderError(env, vorbisFileReader_object, err);
		}

#ifdef __MINGW32__
		env->ReleaseStringChars(file_name, (const jchar*)fileName);
#else
		env->ReleaseStringUTFChars(file_name, fileName);
#endif
	}

	JNIEXPORT void JNICALL Java_vam_VorbisFileReader_destroyNativeInstance(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);

		if (nativeInstance != NULL)	delete nativeInstance;
	}

	JNIEXPORT jfloatArray JNICALL Java_vam_VorbisFileReader_readSample(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwResourcesDeallocated(env); return NULL; }

		try
		{
			const float* sample = nativeInstance->readSample();
			int channels = nativeInstance->getChannels();

			jfloatArray sample_array = env->NewFloatArray(channels);
			env->SetFloatArrayRegion(sample_array, 0, channels, sample);
			return sample_array;
		}
		catch (const VorbisFileReader::Error& err)
		{
			throwJavaVorbisFileReaderError(env, vorbisFileReader_object, err);
		}
		return NULL;
	}

	JNIEXPORT void JNICALL Java_vam_VorbisFileReader_rewind(JNIEnv * env, jobject vorbisFileReader_object, jdouble position)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwResourcesDeallocated(env); return; }

		try
		{
			nativeInstance->rewind(position);
		}
		catch (const VorbisFileReader::Error& err)
		{
			throwJavaVorbisFileReaderError(env, vorbisFileReader_object, err);
		}
	}

	JNIEXPORT jobject JNICALL Java_vam_VorbisFileReader_getState(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwResourcesDeallocated(env); return NULL; }

		jclass state_class = env->FindClass("vam/VorbisFileReader$State");
		jmethodID fromValue_method = env->GetMethodID(state_class, "fromValue", "(I)Lvam/VorbisFileReader$State");

		int state = (int)nativeInstance->getState();

		jobject state_object = env->CallStaticObjectMethod(state_class, fromValue_method, state);
		return state_object;
	}

	JNIEXPORT jobject JNICALL Java_vam_VorbisFileReader_getErrorType(JNIEnv * env, jobject vorbisFileReader_object)
	{
		// TODO Implement
	}

	JNIEXPORT jdouble JNICALL Java_vam_VorbisFileReader_getPlayhead(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwResourcesDeallocated(env); return 0; }

		return nativeInstance->getPlayhead();
	}

	JNIEXPORT jdouble JNICALL Java_vam_VorbisFileReader_getLength(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwResourcesDeallocated(env); return 0; }

		return nativeInstance->getLength();
	}

	JNIEXPORT jlong JNICALL Java_vam_VorbisFileReader_getChannels(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwResourcesDeallocated(env); return 0; }

		return nativeInstance->getChannels();
	}

	JNIEXPORT jlong JNICALL Java_vam_VorbisFileReader_getRate(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwResourcesDeallocated(env); return 0; }

		return nativeInstance->getRate();
	}

	JNIEXPORT jint JNICALL Java_vam_VorbisFileReader_getBitsPerSecond(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwResourcesDeallocated(env); return 0; }

		return nativeInstance->getBitsPerSecond();
	}

	JNIEXPORT jstring JNICALL Java_vam_VorbisFileReader_getVendor(JNIEnv * env, jobject vorbisFileReader_object)
	{
		// TODO Implement
	}

	JNIEXPORT jobjectArray JNICALL Java_vam_VorbisFileReader_getComments(JNIEnv * env, jobject vorbisFileReader_object)
	{
		// TODO Implement
	}

}
