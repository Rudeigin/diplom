import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    id: formDelegate
    property QtObject form
    property int numberWidth: 50
    property bool isEdit: false

    signal needSave()

    Text {
        id: title
        text: form.title
        wrapMode: Text.WordWrap
        font.bold: true
        font.pixelSize: 18
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Button {
        id: addQstButton
        text: qsTr("Добавить вопрос")
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: title.bottom
        anchors.topMargin: 10
        onClicked: {
            Interface.addQst(form)
        }
        visible: isEdit
    }

    ListView {
        id: view
        model: form.questions
        clip: true
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: addQstButton.bottom
        anchors.bottom: bottomButtons.top
        anchors.margins: 10
        spacing: 10

        delegate: ColumnLayout {
            width: parent.width
            spacing: 5
            RowLayout {
                id: qst
                spacing: 5
                TextField {
                    Layout.maximumWidth: numberWidth
                    text: model.number
                    readOnly: !isEdit

                    onTextChanged: {
                        number = text
                    }

//                    Connections {
//                        target: formDelegate
//                        onNeedSave: {
//                            number = text
//                        }
//                    }

                }
                TextField {
                    text: model.text
                    readOnly: !isEdit
                }

                Button {
                    text: "+"
                    visible: isEdit
                    onClicked: {
                        Interface.addAnsw(Interface.getQst(form, index))
                    }
                }
            }

            RowLayout {
                // заглушка
                Item {
                    width: numberWidth
                }

                ColumnLayout {
                    spacing: 5
                    Repeater {
                        model: answers
                        delegate: RowLayout {
                            id: ans
                            spacing: 5
                            TextField {
                                Layout.maximumWidth: numberWidth
                                text: model.number
                                readOnly: !isEdit
                            }
                            TextField {
                                text: model.text
                                readOnly: !isEdit
                            }
                        }
                    }
                }
            }
        }
    }

    RowLayout {
        id: bottomButtons
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        Button {
            text: qsTr("Просмотр")
        }
        Button {
            text: isEdit ? qsTr("Завершить") : qsTr("Редактировать")
            onClicked: {
                isEdit = !isEdit
                if(!isEdit) {
                    needSave()
                    Interface.createPdf(form)
                }
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
