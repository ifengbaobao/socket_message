package com.ifengbaobao.socket_message_android;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    private SocketMessageAndroid socketMessageAndroid;
    private List<SocketMessageAndroid.UserInfo> userlist = new ArrayList<>();
    private List<SocketMessageAndroid.GroupInfo> groupList = new ArrayList<>();

    private List<String> redvIds = new ArrayList<>();
    ArrayAdapter<String> adapter;
    Spinner spiner;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
//        TextView tv = findViewById(R.id.sample_text);
//        tv.setText(stringFromJNI());

        findViewById(R.id.btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                EditText et = findViewById(R.id.et);
                String content = et.getText().toString();
                if(content.isEmpty())
                {
                    Toast.makeText(MainActivity.this,"发送内容不能为空", Toast.LENGTH_LONG).show();
                    return;
                }
                String recvStr = (String) spiner.getSelectedItem();
                int redv =Integer.parseInt(recvStr);
                boolean b = socketMessageAndroid.sendMsg(SocketMessageAndroid.EMsgType.MsgTypeText,redv,redv < 100000, content);
                Toast.makeText(MainActivity.this,b?"发送成功":"发送失败",Toast.LENGTH_LONG).show();
            }
        });

        spiner = findViewById(R.id.spiner);
        adapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1,redvIds);
        spiner.setAdapter(adapter);

        Log.d(this.getClass().getName(),"测试log开始了哦。");

        socketMessageAndroid = new SocketMessageAndroid();
        socketMessageAndroid.setSdkCallback(new SocketMessageAndroid.SdkCallback() {
            @Override
            public void loginCallback(int userId, String userName) {
                Log.d(this.getClass().getName(),"登录回调:"+userId +"   "+userName);
            }

            @Override
            public void userListCallback(List<SocketMessageAndroid.UserInfo> userlist) {
                MainActivity.this.userlist.clear();
                MainActivity.this.userlist.addAll(userlist);
                updateSpinner();

                Log.d(this.getClass().getName(),"所有的用户的回调:"+userlist.size());
                for(SocketMessageAndroid.UserInfo userInfo : userlist)
                {
                    Log.d(this.getClass().getName(),"id:"+userInfo.userId+"  UserName:"+userInfo.userName);
                }
            }

            @Override
            public void groupListCallback(List<SocketMessageAndroid.GroupInfo> groupList) {
                MainActivity.this.groupList.clear();
                MainActivity.this.groupList.addAll(groupList);
                updateSpinner();
                Log.d(this.getClass().getName(),"所有的群组的回调:"+groupList.size());
                for(SocketMessageAndroid.GroupInfo groupInfo : groupList)
                {
                    Log.d(this.getClass().getName(),"id:"+groupInfo.groupId +"  GroupName:"+groupInfo.groupName +" adminId:"+groupInfo.adminId+" 人数："+groupInfo.userList.size());
                    for(SocketMessageAndroid.UserInfo userInfo : groupInfo.userList)
                    {
                        Log.d(this.getClass().getName(),"id:"+userInfo.userId+"  UserName:"+userInfo.userName);
                    }
                }
                //获取离线消息。
                Log.d(this.getClass().getName(),"获取离线消息:" + socketMessageAndroid.getOfflineMsg(0));
            }

            @Override
            public void msgListCallback(List<SocketMessageAndroid.ChatMsg> msgList) {
                Log.d(this.getClass().getName(),"获取离线消息："+msgList.size());
                for(SocketMessageAndroid.ChatMsg msg : msgList)
                {
                    Log.d(this.getClass().getName(),msg.toString());
                }
            }

            @Override
            public void msgCallback(SocketMessageAndroid.ChatMsg msg) {
                Log.d(this.getClass().getName(),"实时消息："+msg.toString());
            }
        });
        boolean b = socketMessageAndroid.newSdk();
        Log.d(this.getClass().getName(),"new sdk:"+b);
        if(!b)
        {
            return;
        }
        b = socketMessageAndroid.connectServer("192.168.0.104",1234);
        Log.d(this.getClass().getName(),"connect server:"+b);
        if(!b)
        {
            return;
        }
        b = socketMessageAndroid.loggin(100000,"000000");
        Log.d(this.getClass().getName(),"login:"+b);



    }

    private void updateSpinner()
    {
        redvIds.clear();
        for(SocketMessageAndroid.UserInfo userInfo : userlist)
        {
            redvIds.add(String.valueOf(userInfo.userId));
        }
        for(SocketMessageAndroid.GroupInfo groupInfo: groupList)
        {
            redvIds.add(String.valueOf(groupInfo.groupId));
        }
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                adapter.notifyDataSetChanged();
            }
        });
    }


}
