#include <stdint.h>
#include <windows.h>
#include <locale.h>

#include <jni.h>

// Local JNI headers
#include "app/Application.h"

#if (defined __MINGW32__) || (defined _MSC_VER)
#  define EXPORT __declspec(dllexport)
#else
#  define EXPORT __attribute__ ((visibility("default"))) \
  __attribute__ ((used))
#endif

#if (! defined __x86_64__) && ((defined __MINGW32__) || (defined _MSC_VER))
#  define SYMBOL(x) binary_boot_jar_##x
#else
#  define SYMBOL(x) _binary_boot_jar_##x
#endif

extern "C"
{

	extern const uint8_t SYMBOL(start)[];
	extern const uint8_t SYMBOL(end)[];

	EXPORT const uint8_t*
	bootJar(unsigned* size)
	{
		*size = SYMBOL(end) - SYMBOL(start);
		return SYMBOL(start);
	}

} // extern "C"

extern "C"
{

	JNIEXPORT jint JNICALL Java_app_Application_callNative(JNIEnv *, jclass, jint value)
	{
		printf("callNative called. Value is %d", value);
		return value * 2;
	}

}

int main(int argc, const char** argv)
{
	// Getting command line as a wide string
	int wac = 0;
	wchar_t** wav;
	wav = CommandLineToArgvW(GetCommandLineW(), &wac);

	JavaVMInitArgs vmArgs;
	vmArgs.version = JNI_VERSION_1_2;
	vmArgs.nOptions = 1;
	vmArgs.ignoreUnrecognized = JNI_TRUE;

	JavaVMOption options[vmArgs.nOptions];
	vmArgs.options = options;

	options[0].optionString = const_cast<char*>("-Xbootclasspath:[bootJar]");

	JavaVM* vm;
	void* env;
	JNI_CreateJavaVM(&vm, &env, &vmArgs);
	JNIEnv* e = static_cast<JNIEnv*>(env);

	jclass c = e->FindClass("app/Application");
	if (not e->ExceptionCheck())
	{
		jmethodID m = e->GetStaticMethodID(c, "main", "([Ljava/lang/String;)V");
		if (not e->ExceptionCheck())
		{
			jclass stringClass = e->FindClass("java/lang/String");
			if (not e->ExceptionCheck())
			{
				jobjectArray a = e->NewObjectArray(wac - 1, stringClass, 0);
				if (not e->ExceptionCheck())
				{
					for (int i = 1; i < wac; ++i)
					{
						e->SetObjectArrayElement(
						        a,
						        i - 1,
						        e->NewString((jchar*) (wav[i]),
						                wcslen(wav[i])));
					}

					e->CallStaticVoidMethod(c, m, a);
				}
			}
		}
	}

	int exitCode = 0;
	if (e->ExceptionCheck())
	{
		exitCode = -1;
		e->ExceptionDescribe();
	}

	vm->DestroyJavaVM();

	return exitCode;
}
