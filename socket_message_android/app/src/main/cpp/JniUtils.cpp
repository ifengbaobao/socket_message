#include "JniUtils.h"
#include <cstdarg>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT  jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    JniUtils::SetJavaVM(vm);
    return JNI_VERSION_1_6;
}

#ifdef __cplusplus
}
#endif

JavaVM* JniUtils::s_jvm = nullptr;

void JniUtils::SetJavaVM(JavaVM * jvm)
{
    JniUtils::s_jvm = jvm;
}

//新建env删除env。
JNIEnv* JniUtils::NewEnv()
{
    JNIEnv *env = NULL;
    if(JNI_OK != s_jvm->AttachCurrentThread(&env, NULL))
    {
        return NULL;
    }
    return env;
}

void JniUtils::DelEnv()
{
    s_jvm->DetachCurrentThread();
}


jstring JniUtils::CString2JString(JNIEnv * env,std::string & str)
{
    return env->NewStringUTF(str.c_str());
}

std::string JniUtils::JString2CString(JNIEnv * env, jstring str)
{
    const char* p = env->GetStringUTFChars(str, NULL);
    if(p == NULL) {
        return std::string(); /* OutOfMemoryError already thrown */
    }
    std::string cStr = p;
    env->ReleaseStringUTFChars(str, p);
    return cStr;
}

//调用指定对象的方法。
void JniUtils::CallVoidMethod4Object(JNIEnv *env, jobject obj,const char* methodName, const char* methodSig,...)
{
    jclass  clazz = env->GetObjectClass(obj);
    if(clazz == NULL)
    {
        return;
    }
    jmethodID  methodID = env->GetMethodID(clazz, methodName,methodSig);
    if(methodID == NULL){
        return;
    }
    va_list args;
    va_start(args, methodSig);
    env->CallVoidMethodV(obj, methodID, args);
    va_end(args);
}

