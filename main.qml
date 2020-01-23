import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("вуф вуф")

    property int mode: 0
    property bool blocked: false
    property real progress: 0
    property string tabTitle: ""

    Connections {
        // TODO blocked = false
        // TODO progress
    }

    RowLayout {
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
        anchors.fill: parent
        onDepthChanged: {
            //Начальное отображение
            if(depth == 1) {
                mode = 0
                tabTitle = "Вундервафля приветствует"
            }
        }
    }

    Component {
        id: mainView

        Item {
            anchors.fill: parent
            ColumnLayout {
                anchors.fill: parent
                Button {
                    Layout.alignment: Qt.AlignCenter
                    text: "Загрузить результаты"
                    onClicked: {
                        mode = 1
                        stack.push()
                    }
                }
                Button {
                    Layout.alignment: Qt.AlignCenter
                    text: "Шаблоны анкеты"
                    onClicked: {
                        mode = 2
                        stack.push(qstView)
                    }
                }
                Button {
                    Layout.alignment: Qt.AlignCenter
                    text: "База данных"
                    onClicked: {
                        mode = 3
                        stack.push(qstView)
                    }
                }
            }
        }
    }

    Component {
        id: qstView

        Item {
            Component.onCompleted: {
                tabTitle = "Выберите анкету:"
            }

            ColumnLayout {
                anchors.fill: parent

                ListView {
                    Layout.alignment: Qt.AlignCenter
                    anchors.fill: parent
                    delegate: Button {
                        width: parent.width
                        height: 80

                        onClicked: {
                            switch(mode) {
                            case 1:
                                //Передать номер анкеты -> запустить cv
                                blocked = true //Открыть заглушку поверх анкеты ожидания
                                break;
                            case 2:
                                //Открыть анкету
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
                }
                Button {
                    Layout.alignment: Qt.AlignRight
                    text: "Добавить новую анкету"

                    onClicked: {
// добавление новой анкеты
                    }
                }
            }

            Rectangle {
                id: cap
                visible: blocked

                anchors.fill: parent
                color: "gray"
                opacity: 0.5

                ColumnLayout {
                    width: parent.width * 0.6
                    anchors.centerIn: parent
                    Text {
                        text: "Выполняется обработка... Пожалуйста, не выключайте компьютер"
                        width: parent.width*(0.8)
                        Layout.alignment: Qt.AlignCenter
                    }
                    ProgressBar {
                        to: 100
                        value: progress
                        width: parent.width
                        Layout.alignment: Qt.AlignCenter
                    }
                }
            }
        }
    }
}
