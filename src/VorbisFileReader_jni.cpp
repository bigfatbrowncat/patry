// System includes
#include <jni.h>
#include <string>

// JNI includes
#include <vam/VorbisFileReader.h>

// Local includes
#include "VorbisFileReader.h"

using namespace vam;
using namespace std;

extern "C"
{

	JNIEXPORT void JNICALL Java_vam_VorbisFileReader_createNativeInstance(
			JNIEnv * env, jobject vorbisFileReader_object, jstring file_name, jint buffer_size_request)
	{
		const wchar_t* fileName = (const wchar_t*)env->GetStringChars(file_name, NULL);

		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");

		VorbisFileReader* nativeInstance = new VorbisFileReader(wstring(fileName), buffer_size_request);

		env->SetLongField(vorbisFileReader_object, nativeInstance_field, (long)nativeInstance);
	}

	JNIEXPORT void JNICALL Java_vam_VorbisFileReader_destroyNativeInstance(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);

		delete nativeInstance;
	}

	JNIEXPORT jfloatArray JNICALL Java_vam_VorbisFileReader_readSample(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);

		const float* sample = nativeInstance->readSample();
		int channels = nativeInstance->getChannels();

		jfloatArray sample_array = env->NewFloatArray(channels);
		env->SetFloatArrayRegion(sample_array, 0, channels, sample);

		return sample_array;
	}

	JNIEXPORT void JNICALL Java_vam_VorbisFileReader_rewind(JNIEnv * env, jobject vorbisFileReader_object, jdouble position)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);

		nativeInstance->rewind(position);
	}

	JNIEXPORT jobject JNICALL Java_vam_VorbisFileReader_getState(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);

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

		return nativeInstance->getPlayhead();
	}

	JNIEXPORT jdouble JNICALL Java_vam_VorbisFileReader_getLength(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);

		return nativeInstance->getLength();
	}

	JNIEXPORT jlong JNICALL Java_vam_VorbisFileReader_getChannels(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);

		return nativeInstance->getChannels();
	}

	JNIEXPORT jlong JNICALL Java_vam_VorbisFileReader_getRate(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);

		return nativeInstance->getRate();
	}

	JNIEXPORT jint JNICALL Java_vam_VorbisFileReader_getBitsPerSecond(JNIEnv * env, jobject vorbisFileReader_object)
	{
		jclass vorbisFileReader_class = env->GetObjectClass(vorbisFileReader_object);
		jfieldID nativeInstance_field = env->GetFieldID(vorbisFileReader_class, "nativeInstance", "J");
		VorbisFileReader* nativeInstance = (VorbisFileReader*)env->GetLongField(vorbisFileReader_object, nativeInstance_field);

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
