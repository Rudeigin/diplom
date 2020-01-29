import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Window {
    id: win
    visible: true
    width: 300
    height: 370
    title: qsTr("Система обработки результатов анкетирования")

    property int mode: 0
    property bool blocked: false
    property real progress: 0
    property string tabTitle: ""

    Connections {
        // TODO blocked = false
        // TODO progress
    }

    RowLayout {
        id: topBar
        visible: !blocked
        width: parent.width
        Button {
            Layout.alignment: Qt.AlignLeft
            text: "<-"
            onClicked: stack.pop()
        }

        Text {
            text: tabTitle
        }

        Button {
            Layout.alignment: Qt.AlignRight
            text: "H"
            onClicked: stack.pop(null)
        }
    }

    StackView {
        id: stack
        initialItem: mainView

        width: parent.width
        anchors.bottom: parent.bottom
        anchors.top: topBar.bottom
        anchors.topMargin: 5

        onDepthChanged: {
            //Начальное отображение
            if(depth == 1) {
                mode = 0
                tabTitle = "Выберите действие"
            }
        }
    }

    Component {
        id: mainView

        ColumnLayout {
            Button {
                Layout.alignment: Qt.AlignCenter
                text: "Загрузить результаты"
                implicitHeight: 80
                implicitWidth: 200
                onClicked: {
                    mode = 1
                    stack.push()
                }
            }
            Button {
                Layout.alignment: Qt.AlignCenter
                text: "Шаблоны анкет"
                implicitHeight: 80
                implicitWidth: 200
                onClicked: {
                    mode = 2
                    stack.push(qstView)
                }
            }
            Button {
                Layout.alignment: Qt.AlignCenter
                text: "База данных"
                implicitHeight: 80
                implicitWidth: 200
                onClicked: {
                    mode = 3
                    stack.push(qstView)
                }
            }
        }
    }

    Component {
        id: qstView

        ColumnLayout {
            Component.onCompleted: {
                tabTitle = "Выберите анкету:"
            }
            ListView {
                id: formsLView
                width: parent.width
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignHCenter
                model: Interface.model()
                spacing: 7
                delegate: Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    implicitHeight: 80
                    implicitWidth: 250
                    text: title

                    onClicked: {
                        switch(mode) {
                        case 1:
                            //Передать номер анкеты -> запустить cv
                            blocked = true //Открыть заглушку поверх анкеты ожидания
                            break;
                        case 2:
                            //Открыть анкету
                            stack.push(formView, {"form" : formsLView.currentItem})
                            break;
                        case 3:
                            //Открыть базу данных
                            break;
                        default:
                            stack.pop(null)
                            break;
                        }
                    }
                }

//                Rectangle {
//                    z: aaa.z + 1
//                    anchors.top: parent.top
//                    anchors.topMargin: -topBar.height - 5
//                    color: "gray"
//                    opacity: 0.7
//                    width: 300
//                    height: 410
//                    Text {
//                        anchors.bottom: prB.top
//                        anchors.bottomMargin: 5
//                        text: "Обработано анкет: 2/10..."
//                    }

//                    ProgressBar {
//                        id: prB
//                        anchors.bottom: parent.bottom
//                        anchors.bottomMargin: 100
//                        width: parent.width
//                        from: 0
//                        to: 10
//                        value: 2
//                    }
//                }
            }
            Button {
                id: aaa
                Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                text: "Добавить новую анкету"

                onClicked: {
                    // добавление новой анкеты
                    Interface.addForm("sssss")
                }
//                Rectangle {
//                    color: "gray"
//                    opacity: 0.7
//                    anchors.fill: parent
//                }
            }
        }
    }

    Component {
        id: formView

        FormDelegate {}
    }
}



//Rectangle {
//    id: cap
//    visible: blocked

//    anchors.fill: parent
//    color: "gray"
//    opacity: 0.5

//    ColumnLayout {
//        width: parent.width * 0.6
//        anchors.centerIn: parent
//        Text {
//            text: "Выполняется обработка... Пожалуйста, не выключайте компьютер"
//            width: parent.width*(0.8)
//            Layout.alignment: Qt.AlignCenter
//        }
//        ProgressBar {
//            to: 100
//            value: progress
//            width: parent.width
//            Layout.alignment: Qt.AlignCenter
//        }
//    }
//}
