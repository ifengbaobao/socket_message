#ifndef IM_DATATYPEUTILS_H
#define IM_DATATYPEUTILS_H

#include <jni.h>
#include <vector>
#include "SocketMessageSdkDataType.h"


class DataTypeUtils {

private:
    DataTypeUtils(){};
    DataTypeUtils(const DataTypeUtils&) = delete;
    DataTypeUtils& operator=(DataTypeUtils&) = delete;
    ~DataTypeUtils(){};

public:

    static jobject UserInfoC2J(JNIEnv * env,jclass userInfoClass, UserInfo& userInfo);
    static jobject GroupInfoC2J(JNIEnv * env,jclass groupInfoClass,jclass userInfoClass, GroupInfo& groupInfo);
    static jobject ChatMsgC2J(JNIEnv * env,jclass chatMsgClass,jclass eMsgTypeClass,ChatMsg& msg);
    static jobject EMsgTypeC2J(JNIEnv * env,jclass eMsgTypeClass,EMsgType & eMsgType);
    static EMsgType EMsgTypeJ2C(JNIEnv * env,jobject jEMsgType);

    static jobject NewJavaListObject(JNIEnv *env);
    static void JavaListAddObj(JNIEnv *env,jobject list,jobject obj);

    static jobject GetUserInfoList(JNIEnv *env, jclass userInfoClass,std::vector<UserInfo>& userVector);
    static jobject GetGroupInfoList(JNIEnv *env,jclass groupInfoClass,jclass userInfoClass,std::vector<GroupInfo>& groupVector);
    static jobject GetChatMsgList(JNIEnv *env,jclass chatMsgClass,jclass eMsgTypeClass,std::vector<ChatMsg> &msgVector);
};


#endif //IM_DATATYPEUTILS_H
