import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

/*
  1. Предпросмотр pdf
  2. Редактирование анкеты
  3. Выбор шрифта, размера и размера листа для анкеты
  4. Выбор папки с файлами и ожидание обработки
  */

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
        width: win.width
        Rectangle {
            Layout.alignment: Qt.AlignLeft
            width: back.width + 10
            height: back.height + 10
            color: "lightgrey"
            Image {
                id: back
                source: "qrc:///back.png"
                sourceSize.width: 35
                anchors.centerIn: parent

                MouseArea {
                    anchors.fill: parent
                    onClicked: stack.pop()
                }
            }
        }

//        Item {
            Text {
//                width: 210
                text: tabTitle
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
                font.pixelSize: 18
            }
//        }

        Rectangle {
            Layout.alignment: Qt.AlignRight
            width: home.width + 10
            height: width
            color: "lightgrey"
            Image {
                id: home
                source: "qrc:///home.png"
                sourceSize.width: 35
                anchors.centerIn: parent

                MouseArea {
                    anchors.fill: parent
                    onClicked: stack.pop(null)
                }
            }
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
                model: Interface.model
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
                            stack.push(formView, {"form" : Interface.getForm(formsLView.currentIndex)})
                            break;
                        case 3:
                            //Открыть базу данных
                            stack.push(baseView)
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

    ListModel {
        id: base
        ListElement {
            fio: "Петров А. А."
            answ1: "3"
            answ2: "1, 2"
            answ3: "1"
            answ4: "5"
            answ5: "3"
            answ6: "4"
            answ7: "2, 4"
            answ8: "3"
        }
        ListElement {
            fio: "Козин С. Е."
            answ1: "2"
            answ2: "1, 3"
            answ3: "2"
            answ4: "4"
            answ5: "3"
            answ6: "4"
            answ7: "1, 4"
            answ8: "1"
        }
        ListElement {
            fio: "Сидорчукова В. В."
            answ1: "2"
            answ2: "3, 4"
            answ3: "2"
            answ4: "3"
            answ5: "1"
            answ6: "2"
            answ7: "2, 3"
            answ8: "3"
        }
    }

    Component {
        id: baseView

        TableView {
            id: table
            anchors.fill: parent
            clip: true
            model: base

            TableViewColumn { role: "fio"; title: "ФИО"; width: table.width * 20 / 100;  }
            TableViewColumn { role: "answ1"; title: "1"; width: table.width * 10 / 100 }
            TableViewColumn { role: "answ2"; title: "2"; width: table.width * 10 / 100 }
            TableViewColumn { role: "answ3"; title: "3"; width: table.width * 10 / 100 }
            TableViewColumn { role: "answ4"; title: "4"; width: table.width * 10 / 100 }
            TableViewColumn { role: "answ5"; title: "5"; width: table.width * 10 / 100 }
            TableViewColumn { role: "answ6"; title: "6"; width: table.width * 10 / 100 }
            TableViewColumn { role: "answ7"; title: "7"; width: table.width * 10 / 100 }
            TableViewColumn { role: "answ8"; title: "8"; width: table.width * 10 / 100 }
        }
    }
}
