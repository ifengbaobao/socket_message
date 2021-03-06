/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_ifengbaobao_socket_message_android_SocketMessageAndroid */

#ifndef _Included_com_ifengbaobao_socket_message_android_SocketMessageAndroid
#define _Included_com_ifengbaobao_socket_message_android_SocketMessageAndroid
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_ifengbaobao_socket_message_android_SocketMessageAndroid
 * Method:    cNewSdk
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_ifengbaobao_socket_1message_1android_SocketMessageAndroid_cNewSdk
  (JNIEnv *, jobject);

/*
 * Class:     com_ifengbaobao_socket_message_android_SocketMessageAndroid
 * Method:    cDeleteSdk
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_ifengbaobao_socket_1message_1android_SocketMessageAndroid_cDeleteSdk
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_ifengbaobao_socket_message_android_SocketMessageAndroid
 * Method:    cConnectServer
 * Signature: (JLjava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_ifengbaobao_socket_1message_1android_SocketMessageAndroid_cConnectServer
  (JNIEnv *, jobject, jlong, jstring, jint);

/*
 * Class:     com_ifengbaobao_socket_message_android_SocketMessageAndroid
 * Method:    cLogin
 * Signature: (JIILjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_ifengbaobao_socket_1message_1android_SocketMessageAndroid_cLogin
  (JNIEnv *, jobject, jlong, jint, jint, jstring);

/*
 * Class:     com_ifengbaobao_socket_message_android_SocketMessageAndroid
 * Method:    cSendMsg
 * Signature: (JILcom/ifengbaobao/socket_message_android/SocketMessageAndroid/EMsgType;IZLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_ifengbaobao_socket_1message_1android_SocketMessageAndroid_cSendMsg
  (JNIEnv *, jobject, jlong, jint, jobject, jint, jboolean, jstring);

/*
 * Class:     com_ifengbaobao_socket_message_android_SocketMessageAndroid
 * Method:    cGetOfflineMsg
 * Signature: (JII)Z
 */
JNIEXPORT jboolean JNICALL Java_com_ifengbaobao_socket_1message_1android_SocketMessageAndroid_cGetOfflineMsg
  (JNIEnv *, jobject, jlong, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
