#ifndef IM_JNIUTILS_H
#define IM_JNIUTILS_H

#include <jni.h>
#include <string>


#define TAG "SocketMessageAndroid"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG , __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT  jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved);

#ifdef __cplusplus
}
#endif

class JniUtils {

private:
    JniUtils(){};
    JniUtils(const JniUtils&) = delete;
    JniUtils& operator=(JniUtils&) = delete;
    ~JniUtils(){};
private:
    static JavaVM* s_jvm;

public:
    //设置jvm
    static void SetJavaVM(JavaVM * jvm);

    //新建env删除env。
    static JNIEnv* NewEnv();
    static void DelEnv();

    //C++ String 与 javaString 互相转换。
    static jstring CString2JString(JNIEnv * env, std::string & str);
    static std::string JString2CString(JNIEnv * env, jstring str);

    //调用指定对象的方法。
    static void CallVoidMethod4Object(JNIEnv *env,jobject obj,const char* methodName, const char* methodSig,...);

};


#endif //IM_JNIUTILS_H
