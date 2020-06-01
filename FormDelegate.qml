import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    property QtObject form

    ColumnLayout {
        spacing: 10
        Button {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Добавить вопрос")
            onClicked: {
                addQ.open()
            }
        }

        ListView {
            id: view
            model: form.questions
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter

            delegate: ColumnLayout {
                width: parent.width
                spacing: 5
                RowLayout {
                    id: qst
                    spacing: 5
                    Text {
                        text: number
                    }
                    Text {
                        text: qsTr(qText)
                    }
                }

//                Button {
//                    Layout.alignment: Qt.AlignHCenter
//                    text: qsTr("Добавить ответ")
//                    onClicked: {
//                        //addA.qst = view.currentItem
//                        addA.open()
//                    }
//                }

//                Repeater {
//                    Layout.alignment: Qt.AlignHCenter
//                    Layout.fillWidth: true
//                    Layout.fillHeight: true
//                    model: answers
//                    delegate: TextField {
//                        text: aText
//                    }
//                }
            }
        }
    }

    Dialog {
        id: addQ
    }

    Dialog {
        id: addA
        property QtObject qst
    }
}
