#ifndef _JAVASOFT_JNI_MD_H_
#define _JAVASOFT_JNI_MD_H_

#ifdef __APPLE__

#  define JNIEXPORT __attribute__((visibility("default")))
#  define JNIIMPORT
#  define JNICALL

#  if defined(__LP64__) && __LP64__ /* for -Wundef */
      typedef int jint;
#  else
      typedef long jint;
#  endif
typedef long long jlong;
typedef signed char jbyte;

#else

#ifdef MSW
#include <windows.h>
#endif

#  define JNIEXPORT __declspec(dllexport)
#  define JNIIMPORT __declspec(dllimport)
#  define JNICALL __stdcall

typedef long jint;
typedef __int64 jlong;
typedef signed char jbyte;

#endif


#endif /* !_JAVASOFT_JNI_MD_H_ */
