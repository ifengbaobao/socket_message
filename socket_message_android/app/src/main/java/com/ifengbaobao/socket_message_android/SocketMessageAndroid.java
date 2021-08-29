package com.ifengbaobao.socket_message_android;

import java.util.List;



public class SocketMessageAndroid {

    // 加载so库。
    static {
        System.loadLibrary("socket_message_android");
    }

    // C++中结构体对象。
    public static class UserInfo
    {
        public int userId;
        public String userName;
    }

    public static class GroupInfo
    {
        public int groupId;
        public String groupName;
        public int adminId;
        public List<UserInfo> userList;
    }

    //消息的类型
    public static enum EMsgType
    {
        MsgTypeText(1);//文本类型。
        //后续可以扩展图片，文件，命令，等等。

        private int value;
        private EMsgType(int value)
        {
            this.value = value;
        }
    }

    public static class ChatMsg
    {
        public int msgId;//消息id
        public EMsgType msgType;// '消息类型',
        public int sender; //'发送者',
        public int receiver; // '接收者',
        public boolean isGroup; // '接收者是否群组',
        public int createdTime; //'创建时间',
        public String msg;	// '消息内容',
        public String extend;	//'扩展内容',

        @Override
        public String toString() {
            return "ChatMsg{" +
                    "msgId=" + msgId +
                    ", msgType=" + msgType +
                    ", sender=" + sender +
                    ", receiver=" + receiver +
                    ", isGroup=" + isGroup +
                    ", createdTime=" + createdTime +
                    ", msg='" + msg + '\'' +
                    ", extend='" + extend + '\'' +
                    '}';
        }
    }

    //回调的接口。
    public static interface SdkCallback{
        //登录的回调。
        public void loginCallback(int userId,String userName);
        //用户列表的回调。
        public void userListCallback(List<UserInfo> userlist);
        //群列表的回调。
        public void groupListCallback(List<GroupInfo> groupList);
        //离线消息的回调。
        public void msgListCallback(List<ChatMsg> msgList);
        //实时在线消息的回调。
        public void msgCallback(ChatMsg msg);
    }


    //c++的类指针。
    private long cSdk = 0;
    private int cConnectId = 0;
    private SdkCallback callback = null;
    private int userId = 0;

    public void setSdkCallback(SdkCallback callback)
    {
        this.callback = callback;
    }

    //1,先初始，创建c对象。
    public boolean newSdk()
    {
        if(cSdk != 0)
        {
            return true;
        }
        cSdk = cNewSdk();
        if(cSdk == 0)
        {
            return false;
        }
        return true;
    }

    public void deleteSdk()
    {
        if(cSdk != 0)
        {
            cDeleteSdk(cSdk);
            cSdk = 0;
        }
    }

    //指定的ip和端口连接服务器。
    public boolean connectServer(String ip,int port)
    {
        if(cConnectId > 0)
        {
            return true;
        }
        if(!newSdk())
        {
            return false;
        }
        cConnectId = cConnectServer(cSdk,ip,port);
        return cConnectId > 0;
    }

    //指定的用户名和密码登录。
    public boolean loggin(int userId,String password)
    {
        if(cConnectId <= 0)
        {
            return false;
        }
        return cLogin(cSdk,cConnectId,userId,password);
    }

    public boolean getOfflineMsg(int lastMsgId)
    {
        if(userId <= 0)
        {
            return false;
        }
        return cGetOfflineMsg(cSdk,cConnectId,lastMsgId);
    }

    //发送消息。
    public boolean sendMsg(EMsgType msgType,int recv,boolean isGroup,String msg)
    {
        if(userId <= 0)
        {
            return false;
        }
        return cSendMsg(cSdk,cConnectId,msgType, recv,isGroup,msg);
    }

    //用于C++的回调。
    //登录的回调。
    public void cLoginCallback(long sdk, int connectId,int userId,String userName)
    {
        this.userId = userId;
        if(callback != null)
        {
            callback.loginCallback(userId,userName);
        }
    }

    //用户列表的回调。
    public void cUserListCallback(long sdk, int connectId,List<UserInfo> userlist)
    {
        if(callback != null)
        {
            callback.userListCallback(userlist);
        }
    }

    //群列表的回调。
    public void cGroupListCallback(long sdk, int connectId, List<GroupInfo> groupList)
    {
        if(callback != null)
        {
            callback.groupListCallback(groupList);
        }
    }

    //离线消息的回调。
    public void cMsgListCallback(long sdk, int connectId, List<ChatMsg> msgList)
    {
        if(callback != null)
        {
            callback.msgListCallback(msgList);
        }
    }
    //实时在线消息的回调。
    public void cMsgCallback(long sdk, int connectId, ChatMsg msg)
    {
        if(callback != null)
        {
            callback.msgCallback(msg);
        }
    }

    //native 方法。
    private native long cNewSdk();

    private native void cDeleteSdk(long sdk);

    //连接服务器。
    private native int cConnectServer(long sdk,String ip,int port);

    //登录用户名密码。
    private native boolean cLogin(long sdk, int connectId,int userId,String password);

    //发送消息。
    private native boolean cSendMsg(long sdk, int connectId,EMsgType msgType,int recv,boolean isGroup,String msg);

    //获取离线消息。
    private native boolean cGetOfflineMsg(long sdk, int connectId,int lastMsgId);
}
