#include <stdint.h>
#include <locale.h>
#include <string.h>

#ifdef __MINGW32__
#include <windows.h>
#endif

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

int main(int argc, const char** argv)
{
#ifdef __MINGW32__
	// Getting command line as a wide string
	int wac = 0;
	const wchar_t** wav;
	wav = CommandLineToArgvW(GetCommandLineW(), &wac);
#else
	int wac = argc;
	const char** wav = argv;
#endif

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
#ifdef __MINGW32__
						int arglen = wcslen(wav[i]));
						jstring arg = e->NewString((jchar*) (wav[i]), arglen);
#else
						int arglen = strlen(wav[i]);
						jstring arg = e->NewStringUTF((char*) (wav[i]));
#endif
						e->SetObjectArrayElement(a, i - 1, arg);
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
