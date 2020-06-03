import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
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
    width: 500
    height: 500
    title: qsTr("Система обработки результатов анкетирования")

    property int mode: 0
    property bool blocked: false
    property real progress: 0
    property string tabTitle: ""

    Connections {
        // TODO blocked = false
        // TODO progress
    }

    // верхняя строка
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

        Text {
            text: tabTitle
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
            font.pixelSize: 18
        }

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

    // вьюха в центре
    StackView {
        id: stack
        initialItem: mainView

        width: parent.width
        anchors.bottom: parent.bottom
        anchors.top: topBar.bottom
        anchors.topMargin: 5

        // смена не слайдом, а через плавное появление
        pushEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 0
                to:1
                duration: 100
            }
        }
        pushExit: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 1
                to:0
                duration: 100
            }
        }

        popEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 0
                to:1
                duration: 100
            }
        }
        popExit: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 1
                to:0
                duration: 100
            }
        }

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
                    stack.push(qstView)
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
                            fileDialog.form = Interface.getForm(index)
                            fileDialog.open()
                            break;
                        case 2:
                            //Открыть анкету
                            stack.push(formView, {"form" : Interface.getForm(index)})
                            break;
                        case 3:
                            //Открыть базу данных
                            stack.push(baseView, {"tableModel" : Interface.getSqlModel(Interface.getForm(index)),
                                                  "form" : Interface.getForm(index)})
                            break;
                        default:
                            stack.pop(null)
                            break;
                        }
                    }
                }
            }
            Button {
                id: aaa
                Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                text: "Добавить новую анкету"

                onClicked: {
                    // добавление новой анкеты
                    Interface.addForm("sssss")
                }
            }
        }
    }

    FileDialog {
        id: fileDialog

        property QtObject form
        title: "Выберите папку"
        folder: shortcuts.home
        selectFolder: true
        selectMultiple: false
        selectExisting: true
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrls)
            dummy.visible = true
            Interface.processForms(form, fileDialog.fileUrls)
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    Connections{
        target: Interface
        onFormProcessingTotalCount: {
            dummy.totalCount = count
        }
        onFormProcessingProgress: {
            dummy.progress = progress
        }
        onFormProcessingFinished: {
            dummy.progress = 0
            dummy.visible = false
        }
    }

    // заглушка обработки
    Rectangle {
        id: dummy
        property int progress: 0
        property int totalCount: 0

        anchors.fill: parent
        color: "gray"
        opacity: 0.7
        Text {
            anchors.bottom: prB.top
            anchors.bottomMargin: 5
            text: qsTr("Обработано анкет: %1/%2...").arg(dummy.progress).arg(dummy.totalCount)
        }

        ProgressBar {
            id: prB
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 100
            width: parent.width
            from: 0
            to: dummy.totalCount
            value: dummy.progress
        }
        visible: false
    }

    Component {
        id: formView

        FormDelegate {
            Component.onCompleted: {
                tabTitle = "Редактор"
            }
        }
    }

    Component {
        id: baseView

        ColumnLayout {
            id: baseLt
            property QtObject tableModel
            property QtObject form

            Text {
                id: title
                text: baseLt.form.title
                wrapMode: Text.WordWrap
                font.bold: true
                font.pixelSize: 18
            }

            TableView {
                id: table
                width: parent.width
                Layout.fillHeight: true
                clip: true
                model: baseLt.tableModel
            }
        }
    }
}
