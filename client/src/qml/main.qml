// import com.sustech.messaging 1.0

import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    // 遍历 userList，每个元素是一个 QVariant 包装的 User 对象
    // userList.forEach(function(user) {
    //   userListModel.append({
    //     userName: user.userName,
    //});
    //});
    //var testMessages1 = [
    //  {userName: "Alice", timestamp: "2024-06-01 08:30:00", text: "Good morning!"},
    // {userName: "Bob", timestamp: "2024-06-01 08:35:00", text: "Hello Alice, how are you today?"}
    //];

    id: root

    function onNewMessageReceived(message) {
        var parts = message.split("\n");
        var now = new Date();
        messageModel.append({
            "userName": parts[0],
            "timestamp": Qt.formatTime(now, "hh:mm:ss"),
            "text": parts[1]
        });
        console.log("New message received: " + message);
    }

    function onUserListUpdated(user) {
        userListModel.append({
            "userName": user.userName,
            "IP": user.IP
        });
    }

    function clearAllData() {
        messageModel.clear();
        userListModel.clear();
    }

    function updateDataFromLists(messageList) {
        function clearAllMessage() {
            messageModel.clear();
        }

        // 遍历 messageList，每个元素是一个 QVariant 包装的 Message 对象
        messageList.forEach(function(message) {
            console.log("QML Message: " + message.userName + " " + message.timestamp + " " + message.text);
            messageModel.append({
                "userName": message.userName,
                "timestamp": message.timestamp,
                "text": message.text
            });
        });
    }
    function loadList(messageList, roomName){
        messageList.forEach(function(message) {
            console.log("QML Message: " + message.userName + " " + message.timestamp + " " + message.text);
            messageModel.append({
                "userName": message.userName,
                "timestamp": message.timestamp,
                "text": message.text
            });
        });
    }

    visible: true
    width: 960
    height: 640
    minimumWidth: 640
    minimumHeight: 480
    title: qsTr("Chat Room")
    onClosing: {
        Qt.quit();
    }
    Component.onCompleted: {
        // 连接 ClientController 的 messageSent 信号到 QML 中的处理函数
        clientManager.messageSent.connect(onNewMessageReceived);
    }

    //top menu bar
    Rectangle {
        id: topbar

        visible: true
        width: parent.width // 宽度和窗口宽度相同
        height: 50 // 设置条的高度
        color: "#498C8B" // 设置条的颜色
        anchors.top: parent.top // 放置在窗口顶部

        Row {
            anchors.fill: parent

            Rectangle {
                border.color: "#498C8B" // 左侧rectangle边框颜色
                border.width: 3 // 左侧rectangle边框宽度
                width: parent.width / 7 // 左侧rectangle宽度为bar的1/7
                radius: 20
                height: parent.height
                color: "#E2F5EA"

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        popup.x = (root.width - popup.width) / 2;
                        popup.y = (root.height - popup.height) / 2;
                        popup.visible = true;
                    }
                }

                Text {
                    anchors.centerIn: parent
                    text: "Create"
                }

            }

            Rectangle {
                border.color: "#498C8B" // 左侧rectangle边框颜色
                border.width: 3 // 左侧rectangle边框宽度
                width: parent.width / 7
                height: parent.height
                radius: 20
                color: "#E2F5EA"

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        popup1.x = (root.width - popup1.width) / 2;
                        popup1.y = (root.height - popup1.height) / 2;
                        popup1.visible = true;
                    }
                }

                Text {
                    anchors.centerIn: parent
                    text: "Enter"
                }

            }

            Rectangle {
                border.color: "#498C8B" // 左侧rectangle边框颜色
                border.width: 3 // 左侧rectangle边框宽度
                width: parent.width / 7
                height: parent.height
                radius: 20
                color: "#E2F5EA"

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        clientController.exportMessage(clientManager);
                    }
                }

                Text {
                    anchors.centerIn: parent
                    text: "Export"
                }

            }

        }

    }

    Rectangle {
        // Sidebar content, such as logo and menu items, can be added here

        id: sidebar

        visible: true
        color: "#3D5F69"
        width: 240
        height: parent.height - 50
        anchors.top: parent.top - 50
        anchors.bottom: parent.bottom

        ListView {
            // ListElement { text: "Room 1" }
            // ListElement { text: "Room 2" }
            // ListElement { text: "Room 3" }
            // ListElement { text: "Room 4" }

            id: sidebarlist

            width: 240
            height: parent.height

            model: ListModel {
                id: chatRoomList
            }

            delegate: Item {
                width: parent.width
                height: 50

                Row {
                    anchors.fill: parent
                    spacing: 10

                    Button {
                        width : 240
                        text: model.text
                        background: Rectangle {
                            color: "#a9d3c4"
                            width: parent.width 
                            height: parent.height
                        }
                        onClicked: {
                            console.log(model.text + " button clicked");
                            clearAllData();
                            clientController.setRoom(clientManager, model.text);
                            updateDataFromLists(clientController.getCurrentRoomMsgs(clientManager));
                        }
                    }

                }

            }

        }

    }

    Rectangle {
        id: content

        color: "#2e3e4e"
        visible: false
        width: parent.width - 240
        height: parent.height - 50
        anchors.left: sidebar.right
        anchors.top: parent.top - 50
        anchors.bottom: parent.bottom
        clip: true
        Column {
            anchors.fill: parent
            spacing: 10

            ListView {
                id: messageList

                width: parent.width
                height: parent.height - inputArea.height - 20
                model: messageModel

                delegate: Rectangle {
                    width: parent.width - 100
                    height: 70
                    color: "#354759"
                    border.color: "#354759"
                    border.width: 1

                    Column {
                        anchors.fill: parent

                        Text {
                            text: model.timestamp.toLocaleString(Qt.locale(), "yyyy-MM-dd hh:mm:ss")
                            color: "grey"
                        }

                        Text {
                            text: model.userName
                            color: "white"
                            font.bold: true
                            font.pixelSize : 16
                        }

                        Text {
                            text: model.text
                            color: "white"
                            font.pixelSize : 16
                        }

                    }

                }

            }

            Rectangle {
                id: inputArea

                color: "#354759"
                height: 50
                width: parent.width - 300 // 留出按钮空间
                anchors.bottom: parent.bottom

                TextField {
                    id: inputField

                    anchors.fill: parent
                    placeholderText: qsTr("Type your message here...")
                    color: "white"
                     background: Rectangle {
                        color: "#354759"
                        radius: 5
                    }
                    Keys.onReturnPressed: {
                        if (inputField.text !== "") {
                            clientController.sendMessage(clientManager, inputField.text)
                        // TODO: delete this line (12012710)
                        // clientController.getCurrentRoomMsgs(clientManager);
                            inputField.text = ""
                        }
                    }
                }
                
            }

            Row {
                spacing: 10
                height: 50
                width: 280 // 给按钮足够空间
                anchors.right: parent.right // 与输入框右侧对齐
                anchors.bottom: parent.bottom

                Button {
                    text: "Send"
                    background: Rectangle {
                        color: "#6a7b82"  
                        radius: 20
                        width: parent.width 
                        height: parent.height
                        }
                    contentItem: Text {
                        text: qsTr("Send")
                        color: "white"  
                        font.pixelSize: 16 
                        horizontalAlignment: Text.AlignHCenter 
                        verticalAlignment: Text.AlignVCenter 
                        anchors.centerIn: parent
                    }
                    width: 95
                    height:50
                    onClicked: {
                        // 发送按键逻辑
                        clientController.sendMessage(clientManager, inputField.text);
                        // TODO: delete this line (12012710)
                        // clientController.getCurrentRoomMsgs(clientManager);
                        inputField.text = "";
                    }
                }

                Button {
                    text: "Load"
                    background: Rectangle {
                        color: "#6a7b82"  
                        radius: 20
                        width: parent.width 
                        height: parent.height
                        }
                    contentItem: Text {
                        text: qsTr("Load")
                        color: "white" 
                        font.pixelSize: 16 
                        horizontalAlignment: Text.AlignHCenter 
                        verticalAlignment: Text.AlignVCenter 
                        anchors.centerIn: parent
                    }
                    width: 95
                    height:50
                    onClicked: {
                        popup2.x = (root.width - popup2.width) / 2;
                        popup2.y = (root.height - popup2.height) / 2;
                        popup2.visible = true;
                    }
                }

            }

        }

        ListView {
            width: 100
            height: parent.height
            anchors.right: parent.right
            model: userListModel

            delegate: Rectangle {
                width: parent.width
                height: 50
                color: "#649DAD"
                border.color: "#649DAD"
                border.width: 1

                Row {
                    anchors.fill: parent

                    Text {
                        text: "User: " + model.userName
                        color: "black"
                    }

                }

            }

        }

    }

    //popup window
    Rectangle {
        id: popup

        width: 450
        height: 300
        color: "lightblue"
        border.color: "black"
        border.width: 2
        visible: false
        radius: 10

        Column {
            anchors.centerIn: parent
            spacing: 10

            TextField {
                id: roomNameField

                placeholderText: "Room Name"
                width: parent.width
            }

            TextField {
                id: passwordField

                placeholderText: "Password"
                width: parent.width
            }

            TextField {
                id: nicknameField

                placeholderText: "Nickname"
                width: parent.width
            }

            Row {
                spacing: 10
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    text: "Create Room"
                    onClicked: {
                        // 在此处处理创建房间的逻辑
                        console.log("Room Name:", roomNameField.text);
                        console.log("Password:", passwordField.text);
                        console.log("Nickname:", nicknameField.text);
                        clientManager.setUserName(nicknameField.text);
                        clientController.createRoom(clientManager, serverManager, roomNameField.text, passwordField.text);
                        clientController.joinRoom(clientManager, serverManager, roomNameField.text, passwordField.text);
                        // popup.createRoom(roomNameField.text, passwordField.text, nicknameField.text)
                        popup.visible = false;
                        roomNameField.text = "";
                        passwordField.text = "";
                        nicknameField.text = "";
                    }
                }

                Button {
                    text: "Close"
                    onClicked: popup.visible = false
                }

            }

        }

    }

    //弹窗二代码
    Rectangle {
        id: popup1

        signal joinRoom(string roomName, string password, string nick)

        width: 450
        height: 300
        color: "lightblue"
        border.color: "black"
        border.width: 2
        visible: false
        radius: 10

        Column {
            anchors.centerIn: parent
            spacing: 10

            TextField {
                id: roomNameField1

                placeholderText: "Room Name"
                width: parent.width
            }

            TextField {
                id: passwordField1

                placeholderText: "Password"
                width: parent.width
            }

            TextField {
                id: nicknameField1

                placeholderText: "Nickname"
                width: parent.width
            }

            Row {
                spacing: 10
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    text: "Join Room"
                    onClicked: {
                        // 在此处处理创建房间的逻辑
                        clientManager.setUserName(nicknameField1.text);
                        clientController.joinRoom(clientManager, serverManager, roomNameField1.text, passwordField1.text);
                        // popup1.joinRoom(roomNameField1.text, passwordField1.text, nicknameField1.text)
                        //console.log("Room Name:", roomNameField1.text)
                        //console.log("Password:", passwordField1.text)
                        //console.log("Nickname:", nicknameField1.text)
                        popup1.visible = false;
                        roomNameField1.text = "";
                        passwordField1.text = "";
                        nicknameField1.text = "";
                    }
                }

                Button {
                    text: "Close"
                    onClicked: popup1.visible = false
                }

            }

        }

    }

    Rectangle {
        id: popup2

        width: 450
        height: 300
        color: "lightblue"
        border.color: "black"
        border.width: 2
        visible: false
        radius: 10

        Column {
            anchors.centerIn: parent
            spacing: 10

            TextField {
                id: roomName

                placeholderText: "Room Name For Load"
                width: parent.width
            }

            Row {
                spacing: 10
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    text: "Load"
                    onClicked: {
                        clearAllData();
                        loadList(clientController.loadMessage(clientManager, roomName.text));
                        sidebar.visible = true;
                        topbar.visible = true;
                        content.visible = true;
                        roomName.text = "";
                    }
                }

                Button {
                    text: "Close"
                    onClicked: popup2.visible = false
                }

            }

        }

    }

    Rectangle {
        id: initialPopup

        width: 450
        height: 300
        color: "lightblue"
        border.color: "black"
        x: (root.width - initialPopup.width) / 2
        y: (root.height - initialPopup.height) / 2
        border.width: 2
        visible: true
        radius: 10

        Column {
            anchors.centerIn: parent
            spacing: 10

            TextField {
                id: ip

                placeholderText: "Server IP"
                width: parent.width
            }

            Row {
                spacing: 10
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    text: "Run"
                    onClicked: {
                        clientController.connectToServer(serverManager, ip.text, "11451");
                        initialPopup.visible = false;
                        content.visible = true;
                        ip.text = "";
                    }
                }

            }

        }

    }

    //load失败
    Rectangle {
        id: popup4

        width: 300
        height: 200
        color: "lightblue"
        border.color: "black"
        border.width: 2
        visible: false
        radius: 10

        Column {
            anchors.centerIn: parent
            spacing: 10

            Text {
                text: "Load Fail"
                color: "black"
                font.pixelSize: 20
            }

            Button {
                text: "Close"
                onClicked: {
                    popup4.visible = false;
                }
            }

        }

    }

    ListModel {
        // ListElement { userName: "Alice"; timestamp: "2021-01-01 10:00:00"; text: "Hello, welcome to the chat!" }
        // ListElement { userName: "Bob"; timestamp: "2021-01-01 10:05:00"; text: "This is a test message." }
        // 可以继续添加更多消息

        id: messageModel
    }

    ListModel {
        // ListElement { userName: "User1" }
        // ListElement { userName: "User2" }
        // 可以继续添加更多用户

        id: userListModel
    }

    Connections {
        target: clientController
        //server应该有一个signal为chatRoomListChanged（string roomName）, 传入参数为新房间的名字
        //当chatroom列表发生变化时，会向qml发送chatRoomListChanged信号，当qml收到信号时，会做出如下的变化（在sidebar的list append新的list element）
        onRoomJoined: {
            chatRoomList.append({
                "text": roomName
            });
        }
    }

}
