#include <jni.h>
#include "com_ifengbaobao_socket_message_android_SocketMessageAndroid.h"
#include <android/log.h>

#include "SocketMessageSdk.h"
#include "JniUtils.h"
#include "DataTypeUtils.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_ifengbaobao_socket_message_android_SocketMessageAndroid
 * Method:    cNewSdk
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_ifengbaobao_socket_1message_1android_SocketMessageAndroid_cNewSdk(JNIEnv * env, jobject thiz)
{
    SocketMessageSdk *pSdk = new SocketMessageSdk();
    //    LOGD("newsdk:%d",pSdk);
    jobject jObjSdk = env->NewGlobalRef(thiz); //获取全局引用
    pSdk->SetLoginCallback([jObjSdk,pSdk]( unsigned int id, unsigned int userId, std::string& userName) {
        LOGD("login callback 登录结果：%d   %s", userId, userName.c_str());
        JNIEnv *env = JniUtils::NewEnv();
        if(env == NULL)
        {
            LOGD("LoginCallback 获取 evn为空");
            return;
        }
        JniUtils::CallVoidMethod4Object(env, jObjSdk,"cLoginCallback","(JIILjava/lang/String;)V",pSdk,id,userId,JniUtils::CString2JString(env, userName));
        JniUtils::DelEnv();
    });
    jclass userInfoClass = (jclass) env->NewGlobalRef(env->FindClass("com/ifengbaobao/socket_message_android/SocketMessageAndroid$UserInfo"));
    pSdk->SetUserListCallback([userInfoClass,jObjSdk,pSdk](unsigned int id, std::vector<UserInfo>& userVector){
        JNIEnv *env = JniUtils::NewEnv();
        if(env == NULL || userInfoClass == NULL)
        {
            LOGD("UserListCallback 获取 evn为空 或者 UserInfoClass 为空");
            if(env != NULL)
            {
                JniUtils::DelEnv();
            }
            return;
        }
        jobject obj = DataTypeUtils::GetUserInfoList(env, userInfoClass, userVector);
        JniUtils::CallVoidMethod4Object(env, jObjSdk, "cUserListCallback","(JILjava/util/List;)V",pSdk,id,obj);
        JniUtils::DelEnv();
    });

    jclass groupInfoClass = (jclass) env->NewGlobalRef(env->FindClass("com/ifengbaobao/socket_message_android/SocketMessageAndroid$GroupInfo"));
    pSdk->SetGroupListCallback([groupInfoClass,userInfoClass,jObjSdk,pSdk](unsigned int id, std::vector<GroupInfo>& groupVector) {
        JNIEnv *env = JniUtils::NewEnv();
        if(env == NULL || userInfoClass == NULL || groupInfoClass == NULL)
        {
            LOGD("GroupListCallback 获取 evn为空 或者 UserInfoClass 为空 或者 groupInfoClass 为空");
            if(env != NULL)
            {
                JniUtils::DelEnv();
            }
            return;
        }
        jobject obj = DataTypeUtils::GetGroupInfoList(env, groupInfoClass, userInfoClass, groupVector);
        JniUtils::CallVoidMethod4Object(env, jObjSdk,"cGroupListCallback","(JILjava/util/List;)V",pSdk,id,obj);

        JniUtils::DelEnv();
    });

    jclass eMsgTypeClass = (jclass) env->NewGlobalRef(env->FindClass("com/ifengbaobao/socket_message_android/SocketMessageAndroid$EMsgType"));
    jclass chatMsgClass = (jclass) env->NewGlobalRef(env->FindClass("com/ifengbaobao/socket_message_android/SocketMessageAndroid$ChatMsg"));
    pSdk->SetDataMsgListCallback([chatMsgClass,eMsgTypeClass,jObjSdk,pSdk](unsigned int id, std::vector<ChatMsg>& msgVector) {
        //        std::cout << "离线消息：" << id << std::endl << "----------" << std::endl;
        JNIEnv *env = JniUtils::NewEnv();
        if(env == NULL || chatMsgClass == NULL || eMsgTypeClass == NULL)
        {
            LOGD("DataMsgListCallback 获取 evn为空 或者 chatMsgClass 为空 或者 eMsgTypeClass 为空");
            if(env != NULL)
            {
                JniUtils::DelEnv();
            }
            return;
        }
        jobject obj = DataTypeUtils::GetChatMsgList(env, chatMsgClass, eMsgTypeClass, msgVector);
        JniUtils::CallVoidMethod4Object(env, jObjSdk,"cMsgListCallback","(JILjava/util/List;)V",pSdk,id,obj);

        JniUtils::DelEnv();
    });

    pSdk->SetDataMsgCallback([chatMsgClass,eMsgTypeClass,jObjSdk,pSdk](unsigned int id, ChatMsg& msg) {
        //        std::cout << "接收到实时消息：" << id << std::endl << "----------" << std::endl;

        JNIEnv *env = JniUtils::NewEnv();
        if(env == NULL || chatMsgClass == NULL || eMsgTypeClass == NULL)
        {
            LOGD("DataMsgListCallback 获取 evn为空 或者 chatMsgClass 为空 或者 eMsgTypeClass 为空");
            if(env != NULL)
            {
                JniUtils::DelEnv();
            }
            return;
        }
        jobject obj = DataTypeUtils::ChatMsgC2J(env, chatMsgClass, eMsgTypeClass, msg);
        JniUtils::CallVoidMethod4Object(env, jObjSdk,"cMsgCallback","(JILcom/ifengbaobao/socket_message_android/SocketMessageAndroid$ChatMsg;)V",pSdk,id,obj);

        JniUtils::DelEnv();
    });
    return (jlong)pSdk;
}

/*
 * Class:     com_ifengbaobao_socket_message_android_SocketMessageAndroid
 * Method:    cDeleteSdk
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_ifengbaobao_socket_1message_1android_SocketMessageAndroid_cDeleteSdk(JNIEnv * env, jobject, jlong pSdk)
{
    delete (SocketMessageSdk*)pSdk;
}

/*
 * Class:     com_ifengbaobao_socket_message_android_SocketMessageAndroid
 * Method:    cConnectServer
 * Signature: (JLjava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_ifengbaobao_socket_1message_1android_SocketMessageAndroid_cConnectServer (JNIEnv *env, jobject, jlong pSdk, jstring ip, jint port)
{
    std::string cIp = JniUtils::JString2CString(env,ip);
    bool isConnect = false;
    unsigned int connectId = ((SocketMessageSdk*)pSdk)->ConnectServer(cIp.c_str(),(unsigned short)port,isConnect);
    LOGD("connect id:%d",connectId);
    return (jint)connectId;
}

/*
 * Class:     com_ifengbaobao_socket_message_android_SocketMessageAndroid
 * Method:    cLogin
 * Signature: (JIILjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_ifengbaobao_socket_1message_1android_SocketMessageAndroid_cLogin(JNIEnv * env, jobject, jlong pSdk, jint connectId, jint userId, jstring password)
{
    std::string cPassword = JniUtils::JString2CString(env,password);
    LOGD("login  SendLogin connectId:%d,uid:%d,password:%s",connectId,userId,cPassword.c_str());
    return ((SocketMessageSdk*)pSdk)->SendLogin(connectId,userId,cPassword);
}

/*
 * Class:     com_ifengbaobao_socket_message_android_SocketMessageAndroid
 * Method:    cSendMsg
 * Signature: (JILcom/ifengbaobao/socket_message_android/SocketMessageAndroid/EMsgType;IZLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_ifengbaobao_socket_1message_1android_SocketMessageAndroid_cSendMsg(JNIEnv *env, jobject, jlong pSdk, jint connectId, jobject msgType, jint recvUserId, jboolean isGroup, jstring msg)
{
    return (jboolean)((SocketMessageSdk*)pSdk)->SendDataMsg(connectId,DataTypeUtils::EMsgTypeJ2C(env,msgType),recvUserId,isGroup,JniUtils::JString2CString(env,msg));
}


/*
 * Class:     com_ifengbaobao_socket_message_android_SocketMessageAndroid
 * Method:    cGetOfflineMsg
 * Signature: (JII)Z
 */
JNIEXPORT jboolean JNICALL Java_com_ifengbaobao_socket_1message_1android_SocketMessageAndroid_cGetOfflineMsg(JNIEnv *env, jobject, jlong pSdk, jint connectId, jint lastMsgId)
{
    return (jboolean)((SocketMessageSdk*)pSdk)->SendGetDataMsgList(connectId, lastMsgId);
}

#ifdef __cplusplus
}
#endif
