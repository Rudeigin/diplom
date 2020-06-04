import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    id: formDelegate
    property QtObject form
    property int numberWidth: 50
    property bool isEdit: false

    TextField {
        id: title
        text: form.title
        placeholderText: qsTr("Название анкеты")
        readOnly: !isEdit
        wrapMode: Text.WordWrap
        font.bold: true
        font.pixelSize: 18
        anchors.horizontalCenter: parent.horizontalCenter
        onTextChanged: {
            form.title = text
        }
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
        anchors.top: isEdit ? addQstButton.bottom : title.bottom
        anchors.bottom: bottomButtons.top
        anchors.margins: 10
        spacing: 10

        delegate: ColumnLayout {
            id: dlgate
            width: parent.width
            spacing: 5
            RowLayout {
                id: qst
                spacing: 5
                TextField {
                    Layout.maximumWidth: numberWidth
                    text: model.number
                    placeholderText: qsTr("№")
                    readOnly: !isEdit

                    onTextChanged: {
                        number = text
                    }
                }
                TextField {
                    text: model.text
                    placeholderText: qsTr("Текст вопроса")
                    readOnly: !isEdit
                    onTextChanged: {
                        model.text = text
                    }
                }

                Button {
                    text: "+"
                    visible: isEdit
                    onClicked: {
                        Interface.addAnsw(Interface.getQst(form, index))
                    }
                }
                Button {
                    text: "-"
                    visible: isEdit
                    onClicked: {
                        Interface.deleteQst(form, index)
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
                                placeholderText: qsTr("№")
                                readOnly: !isEdit
                                onTextChanged: {
                                    model.number = text
                                }
                            }
                            TextField {
                                text: model.text
                                placeholderText: qsTr("Текст ответа")
                                readOnly: !isEdit
                                onTextChanged: {
                                    model.text = text
                                }
                            }
                            Button {
                                text: "-"
                                visible: isEdit
                                onClicked: {
                                    dlgate.delAnsw(index)
                                }
                            }
                        }
                    }
                }
            }

            function delAnsw(answIndex) {
                Interface.deleteAnsw(Interface.getQst(form, index), answIndex)
            }
        }
    }

    RowLayout {
        id: bottomButtons
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        Button {
            text: qsTr("Просмотр")
            //https://github.com/dept2/Poppler-QML-plugin/tree/master/src
        }
        Button {
            text: isEdit ? qsTr("Завершить") : qsTr("Редактировать")
            onClicked: {
                isEdit = !isEdit
                if(!isEdit) {
                    Interface.createPdf(form)
                    dummyLoader.sourceComponent = dummy
                    view.opacity = 0.8
                }
            }
        }
    }


    Connections{
        target: Interface
        onPdfCreatingFinished: {
            dummyLoader.sourceComponent = null
            view.opacity = 1
        }
    }

    Loader {
        id: dummyLoader
        anchors.fill: parent
        onLoaded: console.log("loaded")
    }

    Component {
        // заглушка создания
        id: dummy

        Rectangle {
            anchors.fill: parent
            color: "gray"
            opacity: 0.85
            Text {
                anchors.bottom: prB.top
                anchors.bottomMargin: 5
                text: qsTr("Выполняется создание pdf-файла...")
            }

            ProgressBar {
                id: prB
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 100
                width: parent.width
                indeterminate: true
            }
        }
    }
}
