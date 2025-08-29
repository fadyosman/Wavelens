import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
//TODO: Save project.
ApplicationWindow {
    width: 1280
    height: 1024
    visible: true
    title: qsTr("WaveLens")
    function next() {
        if(listView.currentIndex !== listView.count-1){
            listView.currentIndex++;
        }
    }
    function previous() {
        if(listView.currentIndex !== 0){
            listView.currentIndex--;
        }
    }

    Connections {
        target: screenshotTool
        function onProgressChanged(progress) {
            progressBar.value = progress
            if(progress === 1) {
                progressBar.visible = false
                stopButton.visible = false
                loadUrlsButton.enabled = true
                openProjectButton.enabled = true
                saveProjectButton.enabled = true
            }
        }
        function onScreenshotCompleted(url) {
            urlsModel.append({"text":url});
        }
        function onClear() {
            urlsModel.clear()
        }
    }

    Shortcut {
        sequences: ["Left","Up"]
        onActivated: previous()
    }
    Shortcut {
        sequences: ["Right","Down"]
        onActivated: next()
    }

    ListModel {
        id: urlsModel
    }

    FileDialog {
        id: openUrlListDialog
        title: "Please choose a URLs list"
        nameFilters: ["Text files (*.txt)", "Other files (*.*)"]
        //folder: shortcuts.home
        onAccepted: {
            saveProjectButton.enabled = true
            screenshotTool.screenShotFileList(selectedFile)
            progressBar.visible = true
            stopButton.visible = true
            loadUrlsButton.enabled = false
            openProjectButton.enabled = false
            saveProjectButton.enabled = false
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    FileDialog {
        id: openProjectDialog
        title: "Please choose a project file."
        nameFilters: ["Wavelens project files (*.wlp)"]
        //folder: shortcuts.home
        onAccepted: {
            saveProjectButton.enabled = true
            screenshotTool.loadFromProject(selectedFile)
        }
        onRejected: {
            console.log("Canceled")
        }
        //Component.onCompleted: visible = true
    }

    FileDialog {
        id: saveProjectDialog
        title: "Choose project file name to save."
        fileMode: FileDialog.SaveFile
        nameFilters: ["Wavelens project files (*.wlp)"]
        //folder: shortcuts.home
        onAccepted: {
            screenshotTool.saveToProject(selectedFile)
        }
        onRejected: {
            console.log("Canceled")
        }
        //Component.onCompleted: visible = true
    }

    RowLayout {
        id:rowLayout
        width: parent.width
        Button {
            id: loadUrlsButton
            Layout.alignment: Qt.AlignLeft
            text: "Load URLs"
            Layout.margins: 5
            onClicked: {openUrlListDialog.visible = true}
            highlighted: true
        }
        Button {
            id: openProjectButton
            Layout.alignment: Qt.AlignLeft
            text: "Open Project"
            Layout.margins: 5
            highlighted: true
            onClicked: openProjectDialog.visible = true
        }
        Button {
            id: saveProjectButton
            Layout.alignment: Qt.AlignLeft
            text: "Save Project"
            Layout.margins: 5
            highlighted: true
            enabled: false
            onClicked: saveProjectDialog.visible = true
        }
        Item {
            // spacer item
            Layout.fillWidth: true
            Rectangle { anchors.fill: parent; color: "#ffaaaa" } // to visualize the spacer
        }
        ProgressBar {
            id: progressBar
            visible: false
            Layout.alignment: Qt.AlignRight
            Layout.rightMargin: 20
            value: 0
        }
        Button {
            id: stopButton
            Layout.alignment: Qt.AlignRight
            text: "Stop"
            visible: false
            Layout.margins: 5
            highlighted: true
            onClicked: screenshotTool.stop()
        }
        // Button {
        //     id: settingsButton
        //     Layout.alignment: Qt.AlignLeft
        //     text: "Settings"
        //     Layout.margins: 5
        //     highlighted: true
        // }
    }

    SplitView {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top:rowLayout.bottom
        spacing: 12


        ScrollView {
            id: scrollview
            SplitView.preferredWidth: 256
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ListView {
                id: listView
                highlight: highlightBar
                highlightFollowsCurrentItem: false
                model: urlsModel
                width: parent.width
                delegate: ItemDelegate {
                    text: modelData
                    width: parent.width
                    MouseArea {
                        id: delegateMouseArea
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        onClicked: {
                            listView.currentIndex = index;
                            if(mouse.button === Qt.RightButton) {
                                contextMenu.popup();
                            }
                        }
                    }
                    Menu {
                        id: contextMenu
                        MenuItem {
                            text: "Copy"
                            onClicked: {
                                copyHelper.selectAll();
                                copyHelper.copy();
                            }
                        }
                        MenuItem { text: "Delete" }
                    }
                }
                TextEdit {
                    id: copyHelper
                    visible: false
                    text: listView.currentItem.text
                }
            }
        }

        Component {
            id: highlightBar
            Rectangle {
                width: listView.currentItem.width
                height: listView.currentItem.height
                color: "#8888FF"
                y: listView.currentItem.y;
                Behavior on y { SpringAnimation { spring: 1; damping: .2} }
            }
        }

        Rectangle {
            SplitView.preferredWidth: 1024
            SplitView.fillWidth: true
            color: "black"
            Image {
                id: image
                fillMode: Image.PreserveAspectFit
                anchors.fill: parent
                focus: true
                source: "image://screenshots/" + listView.currentItem.text
                MouseArea {
                    anchors.fill: parent
                    onWheel: {
                        if (wheel.angleDelta.y/120 == 1) {
                            next();
                        } else if (wheel.angleDelta.y/120 == -1) {
                            previous();
                        }
                    }
                }
            }
            RowLayout {
                anchors.fill:parent
                Button {
                    id: previousButton
                    text: "Previous"
                    highlighted: true
                    Layout.leftMargin: 12
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    onClicked: previous();
                    visible: listView.count > 0
                }
                Button {
                    id:nextButton
                    text: "Next"
                    highlighted: true
                    Layout.rightMargin: 12
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    onClicked: next();
                    visible: listView.count > 0
                }
            }
        }
    }
}
