#include "DataTypeUtils.h"
#include "JniUtils.h"

jobject DataTypeUtils::UserInfoC2J(JNIEnv * env,jclass userInfoClass, UserInfo& userInfo)
{
    jmethodID userInfoMethodId = env->GetMethodID(userInfoClass, "<init>", "()V");
    jobject userInfoObj = env->NewObject(userInfoClass, userInfoMethodId);

    jfieldID userInfoFieldUserId = env->GetFieldID(userInfoClass,"userId","I");
    env->SetIntField(userInfoObj, userInfoFieldUserId, userInfo.UserId);

    jfieldID userInfoFieldUserName = env->GetFieldID(userInfoClass,"userName","Ljava/lang/String;");
    env->SetObjectField(userInfoObj,userInfoFieldUserName,JniUtils::CString2JString(env,userInfo.UserName));

    return userInfoObj;
}


jobject DataTypeUtils::GroupInfoC2J(JNIEnv * env,jclass groupInfoClass,jclass userInfoClass, GroupInfo& groupInfo)
{
    jmethodID groupInfoMethodId = env->GetMethodID(groupInfoClass, "<init>", "()V");
    jobject groupInfoObj = env->NewObject(groupInfoClass, groupInfoMethodId);

    jfieldID groupInfoFieldGroupId = env->GetFieldID(groupInfoClass,"groupId","I");
    env->SetIntField(groupInfoObj, groupInfoFieldGroupId, groupInfo.GroupId);

    jfieldID groupInfoFieldGroupName = env->GetFieldID(groupInfoClass,"groupName","Ljava/lang/String;");
    env->SetObjectField(groupInfoObj,groupInfoFieldGroupName,JniUtils::CString2JString(env,groupInfo.GroupName));

    jfieldID groupInfoFieldAdminId = env->GetFieldID(groupInfoClass,"adminId","I");
    env->SetIntField(groupInfoObj, groupInfoFieldAdminId, groupInfo.AdminId);

    jfieldID groupInfoFieldUserList = env->GetFieldID(groupInfoClass,"userList","Ljava/util/List;");
    env->SetObjectField(groupInfoObj,groupInfoFieldUserList,GetUserInfoList(env,userInfoClass,groupInfo.UserList));

    return groupInfoObj;
}


jobject DataTypeUtils::EMsgTypeC2J(JNIEnv * env,jclass eMsgTypeClass,EMsgType & eMsgType)
{
    const char * eMsgTypeName = NULL;
    switch(eMsgType)
    {
        case EMsgType::MsgTypeText :
            eMsgTypeName = "MsgTypeText";
            break;

        //默认类型为文本类型。
        default:
            eMsgTypeName = "MsgTypeText";
            break;
    }
    jfieldID fieldId = env->GetStaticFieldID(eMsgTypeClass, eMsgTypeName, "Lcom/ifengbaobao/socket_message_android/SocketMessageAndroid$EMsgType;");
    return env->GetStaticObjectField(eMsgTypeClass, fieldId);
}

EMsgType DataTypeUtils::EMsgTypeJ2C(JNIEnv * env,jobject jEMsgType)
{
    jclass emsTypeClass= env->GetObjectClass(jEMsgType);
    jmethodID eMsgTypeMothod = env->GetMethodID(emsTypeClass, "name", "()Ljava/lang/String;");
    jstring eMsgTypeName = (jstring)env->CallObjectMethod(jEMsgType, eMsgTypeMothod);
    std::string cEMsgTypeName = JniUtils::JString2CString(env,eMsgTypeName);
    if(cEMsgTypeName == std::string("MsgTypeText"))
    {
        return EMsgType::MsgTypeText;
    }

    //默认类型为文本类型：
    return EMsgType::MsgTypeText;
}

jobject DataTypeUtils::ChatMsgC2J(JNIEnv * env,jclass chatMsgClass,jclass eMsgTypeClass,ChatMsg& msg)
{
    jmethodID chatMsgMethodId = env->GetMethodID(chatMsgClass, "<init>", "()V");
    jobject chatMsgObj = env->NewObject(chatMsgClass, chatMsgMethodId);

    env->SetIntField(chatMsgObj, env->GetFieldID(chatMsgClass,"msgId","I"), msg.MsgId);
    env->SetObjectField(chatMsgObj, env->GetFieldID(chatMsgClass,"msgType","Lcom/ifengbaobao/socket_message_android/SocketMessageAndroid$EMsgType;"), EMsgTypeC2J(env,eMsgTypeClass,msg.MsgType));
    env->SetIntField(chatMsgObj, env->GetFieldID(chatMsgClass,"sender","I"), msg.Sender);
    env->SetIntField(chatMsgObj, env->GetFieldID(chatMsgClass,"receiver","I"), msg.Receiver);
    env->SetBooleanField(chatMsgObj, env->GetFieldID(chatMsgClass,"isGroup","Z"),msg.IsGroup);
    env->SetIntField(chatMsgObj, env->GetFieldID(chatMsgClass,"createdTime","I"), msg.CreatedTime);
    env->SetObjectField(chatMsgObj, env->GetFieldID(chatMsgClass,"msg","Ljava/lang/String;"), JniUtils::CString2JString(env,msg.Msg));
    env->SetObjectField(chatMsgObj, env->GetFieldID(chatMsgClass,"extend","Ljava/lang/String;"), JniUtils::CString2JString(env,msg.Extend));

    return chatMsgObj;
}


jobject DataTypeUtils::NewJavaListObject(JNIEnv *env)
{
    jclass listClass = env->FindClass("java/util/ArrayList");
    jmethodID listMethodId = env->GetMethodID(listClass, "<init>", "()V");
    return env->NewObject(listClass,listMethodId);
}


void DataTypeUtils::JavaListAddObj(JNIEnv *env,jobject list,jobject obj)
{
    jclass listClass = env->FindClass("java/util/ArrayList");
    jmethodID listClassAddMethod = env->GetMethodID(listClass,"add","(Ljava/lang/Object;)Z");
    env->CallBooleanMethod(list,listClassAddMethod,obj);
}


jobject DataTypeUtils::GetUserInfoList(JNIEnv *env, jclass userInfoClass, std::vector<UserInfo> & userVector)
{
    jobject listObj = NewJavaListObject(env);
    for(UserInfo & userInfo : userVector)
    {
        JavaListAddObj(env,listObj,UserInfoC2J(env,userInfoClass,userInfo));
    }
    return listObj;
}


jobject DataTypeUtils::GetGroupInfoList(JNIEnv *env,jclass groupInfoClass,jclass userInfoClass,std::vector<GroupInfo>& groupVector)
{
    jobject listObj = NewJavaListObject(env);
    for(GroupInfo & groupInfo : groupVector)
    {
        JavaListAddObj(env,listObj,GroupInfoC2J(env,groupInfoClass,userInfoClass,groupInfo));
    }
    return listObj;
}

jobject DataTypeUtils::GetChatMsgList(JNIEnv *env,jclass chatMsgClass,jclass eMsgTypeClass,std::vector<ChatMsg> &msgVector)
{
    jobject listObj = NewJavaListObject(env);
    for(ChatMsg & msg : msgVector)
    {
        JavaListAddObj(env,listObj,ChatMsgC2J(env,chatMsgClass,eMsgTypeClass,msg));
    }
    return listObj;
}