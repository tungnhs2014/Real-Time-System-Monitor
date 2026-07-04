// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

import QtQuick 2.15
import "../components"
import "../theme"

pragma ComponentBehavior: Bound

Rectangle {
    id: root

    // PROPERTIES
    width: Theme.screenWidth
    height: Theme.screenHeight
    color: Theme.pageBackground

    // SIGNALS FOR NAVIGATION
    signal backRequested()
    signal settingsRequested()

    // HEADER
    DetailHeader {
        id: header
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        title: "SETTINGS"
        onBackClicked: root.backRequested()
        onSettingsClicked: {}
    }

    // TAB BAR
    TabBar {
        id: tabBar
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
        }
        tabs: ["System", "Monitor", "Warning", "Logs"]
        currentIndex: 0
        barHeight: 35
        fontPixelSize: 8
        indicatorHeight: 2

        onTabClicked: function(index) {
            if (QML_DEBUG_ENABLED) console.log("Tab switched to:", tabs[index])
        }
    }

    // CONTENT AREA
    Item {
        id: contentArea
        anchors {
            top: tabBar.bottom
            left: parent.left
            right: parent.right
            bottom: (tabBar.currentIndex === 1 || tabBar.currentIndex === 2) ? saveButton.top : parent.bottom
            margins: 8
        }

        // TAB 1: SYSTEM INFO
        Flickable {
            anchors.fill: parent
            contentHeight: Math.max(height, systemColumn.implicitHeight)
            boundsBehavior: Flickable.StopAtBounds
            interactive: contentHeight > height
            clip: true
            visible: tabBar.currentIndex === 0

            Column {
                id: systemColumn
                width: parent.width
                spacing: 10

                InfoRow { label: "Hostname:"; value: systemInfo.hostname }
                InfoRow { label: "OS Version:"; value: systemInfo.osVersion }
                InfoRow { label: "Kernel:"; value: systemInfo.kernelVersion }
                InfoRow { label: "Uptime:"; value: systemInfo.uptime }
                InfoRow { label: "System Time:"; value: systemInfo.systemTime }

                Item { height: 10 }

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 140
                    height: 28
                    text: "Reboot"
                    buttonColor: "#FF9800"
                    preventInputStealing: true
                    enabled: !rebootDialog.visible && !shutdownDialog.visible
                    onClicked: rebootDialog.show()
                }

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 140
                    height: 28
                    text: "Shutdown"
                    buttonColor: "#F44336"
                    preventInputStealing: true
                    enabled: !rebootDialog.visible && !shutdownDialog.visible
                    onClicked: shutdownDialog.show()
                }
            }
        }

        // TAB 2: MONITOR SETTINGS
        Column {
            anchors.fill: parent
            spacing: 12
            visible: tabBar.currentIndex === 1

            // Update interval
            Row {
                width: parent.width
                spacing: 10

                Text {
                    text: "Update Interval:"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 9
                    color: "#FFFFFF"
                    anchors.verticalCenter: parent.verticalCenter
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }

                // Dropdown (simplified as button for now)
                Rectangle {
                    width: 60
                    height: 24
                    radius: 4
                    color: "#1E2539"
                    border.width: 1
                    border.color: Qt.rgba(1, 1, 1, 0.2)

                    Row {
                        anchors.centerIn: parent
                        spacing: 4

                        Text {
                            text: systemInfo.updateInterval + "s"
                            font.pixelSize: 9
                            color: "#FFFFFF"
                            renderType: Text.NativeRendering
                            antialiasing: false
                            font.hintingPreference: Font.PreferFullHinting
                        }

                        Text {
                            text: "▼"
                            font.family: "DejaVu Sans"
                            font.pixelSize: 7
                            color: "#B0B8C8"
                            renderType: Text.NativeRendering
                            antialiasing: false
                            font.hintingPreference: Font.PreferFullHinting
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: if (QML_DEBUG_ENABLED) console.log("Update interval dropdown clicked")
                    }
                }
            }

            // Removed Dark Mode toggle
        }

        // TAB 3: WARNING THRESHOLDS
        Flickable {
            anchors.fill: parent
            contentWidth: width
            contentHeight: Math.max(height, warningColumn.implicitHeight)
            boundsBehavior: Flickable.StopAtBounds
            interactive: visible && contentHeight > height
            maximumFlickVelocity: 700
            flickDeceleration: 1800
            clip: true
            visible: tabBar.currentIndex === 2

            Column {
                id: warningColumn
                width: parent.width
                spacing: 5

                Text {
                    text: "CPU Thresholds"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 10
                    font.bold: true
                    color: "#FFFFFF"
                    renderType: Text.NativeRendering
                }

                Row {
                    width: parent.width
                    height: 38
                    spacing: 8

                    Text {
                        text: "Warning:"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 9
                        color: "#FFFFFF"
                        anchors.verticalCenter: parent.verticalCenter
                        width: 68
                        renderType: Text.NativeRendering
                    }

                    SpinBox {
                        value: systemInfo.cpuWarnThreshold
                        minValue: 0
                        maxValue: 95
                        step: 5
                        suffix: "%"
                        anchors.verticalCenter: parent.verticalCenter
                        onValueChanged: systemInfo.cpuWarnThreshold = value
                    }
                }

                // CPU critical
                Row {
                    width: parent.width
                    height: 38
                    spacing: 8

                    Text {
                        text: "Critical:"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 9
                        color: "#FFFFFF"
                        anchors.verticalCenter: parent.verticalCenter
                        width: 68
                        renderType: Text.NativeRendering
                    }

                    SpinBox {
                        value: systemInfo.cpuCritThreshold
                        minValue: 0
                        maxValue: 100
                        step: 5
                        suffix: "%"
                        anchors.verticalCenter: parent.verticalCenter
                        onValueChanged: systemInfo.cpuCritThreshold = value
                    }
                }

                Item { height: 2 }

                Text {
                    text: "RAM Thresholds"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 10
                    font.bold: true
                    color: "#FFFFFF"
                    renderType: Text.NativeRendering
                }

                Row {
                    width: parent.width
                    height: 38
                    spacing: 8

                    Text {
                        text: "Warning:"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 9
                        color: "#FFFFFF"
                        anchors.verticalCenter: parent.verticalCenter
                        width: 68
                        renderType: Text.NativeRendering
                    }

                    SpinBox {
                        value: systemInfo.ramWarnThreshold
                        minValue: 0
                        maxValue: 95
                        step: 5
                        suffix: "%"
                        anchors.verticalCenter: parent.verticalCenter
                        onValueChanged: systemInfo.ramWarnThreshold = value
                    }
                }
            }
        }

        // TAB 4: SYSTEM LOGS
        Column {
            anchors.fill: parent
            spacing: 4
            visible: tabBar.currentIndex === 3

            Text {
                text: "Recent System Logs"
                font.family: Theme.fontFamily
                font.pixelSize: 10
                font.bold: true
                color: Theme.primaryText
                renderType: Text.NativeRendering
            }

            ListView {
                id: logsList
                width: parent.width
                height: Math.max(0, parent.height - 18)
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                model: tabBar.currentIndex === 3 ? systemInfo.systemLogs : []

                delegate: Rectangle {
                    id: logDelegate
                    required property int index
                    required property var modelData

                    width: logsList.width
                    height: 24
                    color: logDelegate.index % 2 === 0 ? "#20283A" : "#111722"

                    Row {
                        anchors.fill: parent
                        anchors.leftMargin: 4
                        spacing: 6

                        Text {
                            width: 56
                            text: logDelegate.modelData.time
                            font.family: "DejaVu Sans Mono"
                            font.pixelSize: 8
                            color: "#F2F6FF"
                            anchors.verticalCenter: parent.verticalCenter
                            renderType: Text.NativeRendering
                            elide: Text.ElideRight
                        }

                        Rectangle {
                            width: 34
                            height: 14
                            radius: 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: {
                                switch(logDelegate.modelData.level) {
                                    case "ERROR": return "#F44336"
                                    case "WARN": return "#FF9800"
                                    case "INFO": return "#4CAF50"
                                    default: return "#2196F3"
                                }
                            }

                            Text {
                                anchors.centerIn: parent
                                text: logDelegate.modelData.level
                                font.family: "DejaVu Sans"
                                font.pixelSize: 7
                                font.bold: true
                                color: "#FFFFFF"
                                renderType: Text.NativeRendering
                            }
                        }

                        Text {
                            text: logDelegate.modelData.message
                            font.family: "DejaVu Sans"
                            font.pixelSize: 9
                            color: "#FFFFFF"
                            anchors.verticalCenter: parent.verticalCenter
                            renderType: Text.NativeRendering
                            elide: Text.ElideRight
                            width: Math.max(80, logDelegate.width - 114)
                        }
                    }
                }
            }
        }
    }

    Button {
        id: saveButton
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 8
        }
        width: 304
        height: 24
        text: "Save Changes"
        buttonColor: "#4CAF50"
        triggerOnPress: true
        visible: tabBar.currentIndex === 1 || tabBar.currentIndex === 2

        onClicked: {
            if (QML_DEBUG_ENABLED) console.log("Saving settings...")
            if (QML_DEBUG_ENABLED) console.log("Update interval:", systemInfo.updateInterval)
            if (QML_DEBUG_ENABLED) console.log("Sound alert:", systemInfo.soundAlert)
            if (QML_DEBUG_ENABLED) console.log("CPU warn:", systemInfo.cpuWarnThreshold)
            if (QML_DEBUG_ENABLED) console.log("CPU crit:", systemInfo.cpuCritThreshold)
            if (QML_DEBUG_ENABLED) console.log("RAM warn:", systemInfo.ramWarnThreshold)

            systemInfo.saveSettings()

            if (QML_DEBUG_ENABLED) console.log("Settings saved successfully!")
        }
    }

    ConfirmDialog {
        id: rebootDialog
        title: "Reboot System"
        message: "Are you sure you want to reboot?\nAll unsaved data will be lost."
        confirmText: "Reboot"
        confirmColor: "#FF9800"
        onAccepted: systemInfo.reboot()
    }

    ConfirmDialog {
        id: shutdownDialog
        title: "Shutdown System"
        message: "Are you sure you want to shutdown?\nThe system will power off."
        confirmText: "Shutdown"
        confirmColor: "#F44336"
        onAccepted: systemInfo.shutdown()
    }

    component InfoRow: Row {
        id: infoRow
        property string label: ""
        property string value: ""
        spacing: 8

        Text {
            text: infoRow.label
            font.family: Theme.fontFamily
            font.pixelSize: 9
            color: Theme.secondaryText
            width: 76
            renderType: Text.NativeRendering
            elide: Text.ElideRight
        }

        Text {
            text: infoRow.value
            font.family: Theme.fontFamily
            font.pixelSize: 9
            font.bold: true
            color: Theme.primaryText
            renderType: Text.NativeRendering
            width: 210
            elide: Text.ElideRight
        }
    }
}
