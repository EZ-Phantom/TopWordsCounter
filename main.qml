import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import QtCharts 2.12

ApplicationWindow {
    id: rootId

    width: 640
    height: 480
    visible: true
    title: qsTr("Word Counter")

    ColumnLayout {
        anchors.fill: parent
        spacing: 1

        Button {
            id: buttonId

            Layout.preferredHeight: 50
            Layout.fillWidth: true

            text: qsTr("Select File")

            onClicked: fileDialog.open()
        }

        ProgressBar {
            id: progressBarId

            Layout.preferredHeight: 5
            Layout.fillWidth: true

            visible: false
        }

        Flickable {
            id: flickableId

            Layout.fillHeight: true
            Layout.fillWidth: true

            contentWidth: chartViewId.width
            contentHeight: chartViewId.height
            clip: true

            ChartView {
                id: chartViewId
                // hack for minimumHeight to display all items without squashing
                property var heightChart: 200
                property var heightRow: 20
                property var minHeight: heightChart + controller.getTopWordsCount() * heightRow

                height: flickableId.height > minHeight ? flickableId.height : minHeight
                width: rootId.width
                title: qsTr("Top Words Frequency")
                legend.alignment: Qt.AlignBottom
                antialiasing: true

                HorizontalBarSeries {
                    id: mySeries

                    axisY: BarCategoryAxis {
                        id: oY;
                        categories: []
                    }

                    axisX: ValueAxis {
                        id: oX;
                        min: 0
                        labelFormat: "%.0f"
                    }

                    BarSet {
                        id: labelsId;
                        label: qsTr("word frequency");
                        values: []
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog

        visible: false
        selectMultiple: false

        onAccepted: {
            controller.calculateFile(fileUrl)

            progressBarId.visible = true
            buttonId.enabled = false
            buttonId.text = qsTr("Processing...")
        }
    }

    MessageDialog {
        id: errorDialogId

        title: qsTr("Error")
        text: qsTr("Can not open selected file")
    }

    Connections {
        target: controller
        onReadyToFetch: {
            oY.categories = controller.getWords()
            oX.max = controller.getMaxFrequence()
            labelsId.values = controller.getCounts()
            resetUi()
        }

        onProgress: {
            progressBarId.value = value
        }

        onCanNotOpenFile: {
            errorDialogId.visible = true
            resetUi()
        }
    }

    function resetUi () {
        progressBarId.visible = false
        buttonId.enabled = true
        buttonId.text = qsTr("Select New file")
    }
}
