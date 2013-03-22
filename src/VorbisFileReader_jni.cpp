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

void throwJavaVorbisFileReaderError(JNIEnv * env, const VorbisFileReader::Error& err)
{
	jclass vorbisFileReaderError_class = env->FindClass("vam/VorbisFileReader$Error");
	jmethodID vorbisFileReaderError_constructor = env->GetMethodID(
			vorbisFileReaderError_class, "<init>", "(Lvam/VorbisFileReader$ErrorType;ILjava/lang/String;)V");

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

	jthrowable error_exception = (jthrowable)env->NewObject(vorbisFileReaderError_class, vorbisFileReaderError_constructor, errorType_object, err.getCode(), caller_string);
	env->Throw(error_exception);
}

void throwVorbisFileReaderResourcesDeallocated(JNIEnv * env)
{
	jclass resourcesDeallocatedException_class = env->FindClass("vam/ResourcesDeallocatedException");
	env->ThrowNew(resourcesDeallocatedException_class, "Resources of the VorbisFileReader object are deallocated");
}

extern "C"
{

	JNIEXPORT jlong JNICALL Java_vam_VorbisFileReader_createNativeInstance(
			JNIEnv * env, jclass vorbisFileReader_class, jstring file_name, jint buffer_size_request)
	{
#ifdef __MINGW32__
		const wchar_t* fileName = (const wchar_t*)env->GetStringChars(file_name, NULL);
#else
		const char* fileName = (char*)env->GetStringUTFChars(file_name, NULL);
#endif

		//jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");

		VorbisFileReader* nativeInstance = NULL;
		try
		{
#ifdef __MINGW32__
			nativeInstance = new VorbisFileReader(wstring(fileName), buffer_size_request);
#else
			nativeInstance = new VorbisFileReader(string(fileName), buffer_size_request);
#endif
		}
		catch (const VorbisFileReader::Error& err)
		{
			throwJavaVorbisFileReaderError(env, err);
		}

#ifdef __MINGW32__
		env->ReleaseStringChars(file_name, (const jchar*)fileName);
#else
		env->ReleaseStringUTFChars(file_name, fileName);
#endif
		return (jlong)nativeInstance;
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
		if (nativeInstance == NULL) { throwVorbisFileReaderResourcesDeallocated(env); return NULL; }

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
			throwJavaVorbisFileReaderError(env, err);
		}
		return NULL;
	}

	JNIEXPORT void JNICALL Java_vam_VorbisFileReader_rewind(JNIEnv * env, jobject vorbisFileReader_object, jdouble position)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwVorbisFileReaderResourcesDeallocated(env); return; }

		try
		{
			nativeInstance->rewind(position);
		}
		catch (const VorbisFileReader::Error& err)
		{
			throwJavaVorbisFileReaderError(env, err);
		}
	}

	JNIEXPORT jobject JNICALL Java_vam_VorbisFileReader_getState(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwVorbisFileReaderResourcesDeallocated(env); return NULL; }

		jclass state_class = env->FindClass("vam/VorbisFileReader$State");
		jmethodID fromValue_method = env->GetMethodID(state_class, "fromValue", "(I)Lvam/VorbisFileReader$State;");

		int state = (int)nativeInstance->getState();

		jobject state_object = env->CallStaticObjectMethod(state_class, fromValue_method, state);
		return state_object;
	}

	JNIEXPORT jdouble JNICALL Java_vam_VorbisFileReader_getPlayhead(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwVorbisFileReaderResourcesDeallocated(env); return 0; }

		return nativeInstance->getPlayhead();
	}

	JNIEXPORT jdouble JNICALL Java_vam_VorbisFileReader_getLength(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwVorbisFileReaderResourcesDeallocated(env); return 0; }

		return nativeInstance->getLength();
	}

	JNIEXPORT jint JNICALL Java_vam_VorbisFileReader_getChannels(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwVorbisFileReaderResourcesDeallocated(env); return 0; }

		return nativeInstance->getChannels();
	}

	JNIEXPORT jint JNICALL Java_vam_VorbisFileReader_getRate(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwVorbisFileReaderResourcesDeallocated(env); return 0; }

		return nativeInstance->getRate();
	}

	JNIEXPORT jint JNICALL Java_vam_VorbisFileReader_getBitsPerSecond(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwVorbisFileReaderResourcesDeallocated(env); return 0; }

		return nativeInstance->getBitsPerSecond();
	}

	JNIEXPORT jstring JNICALL Java_vam_VorbisFileReader_getVendor(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwVorbisFileReaderResourcesDeallocated(env); return 0; }

		string s = nativeInstance->getVendor();
		jstring res = env->NewStringUTF(s.c_str());

		return res;
	}

	JNIEXPORT jobjectArray JNICALL Java_vam_VorbisFileReader_getComments(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);
		if (nativeInstance == NULL) { throwVorbisFileReaderResourcesDeallocated(env); return 0; }

		vector<string> ss = nativeInstance->getComments();

		jobjectArray res = env->NewObjectArray(ss.size(), env->FindClass("java/lang/String"), NULL);
		for (int i = 0; i < ss.size(); i++)
		{
			env->SetObjectArrayElement(res, i, env->NewStringUTF(ss[i].c_str()));
		}
		return res;
	}

}
