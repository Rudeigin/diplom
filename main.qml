import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    property int mode: 0

    RowLayout {
        width: parent.width
        Button {
            Layout.alignment: Qt.AlignLeft
            text: "<-"
            onClicked: stack.pop()
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
            if(depth == 1)
                mode = 0
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
                            //Открыть заглушку поверх анкеты ожидания
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
            }
        }
    }
}
