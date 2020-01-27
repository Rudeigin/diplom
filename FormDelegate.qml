import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Item {
    property var form
    ListView {
        id: view
        model: form.questions
        delegate: Item {

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

            Repeater {
                anchors.top: qst.bottom
                anchors.topMargin: 5

                model: answers
                delegate: TextField {
                    text: aText
                }
            }
        }
    }
}
