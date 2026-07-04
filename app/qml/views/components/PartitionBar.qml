// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

import QtQuick 2.15
import "../theme"

Item {
    id: root

    // PROPERTIES
    width: 304
    height: 44

    property string partitionName: "Partition"
    property string totalSize: "0 GB"
    property int usage: 0               // Usage percentage (0-100)
    property string usedSize: "0 GB"
    property string freeSize: "0 GB"

    // Auto-calculated properties
    readonly property color effectiveBarColor: _calculateColor()

    // PARTITION LABEL
    Text {
        id: labelText
        anchors {
            left: parent.left
            top: parent.top
        }
        text: root.partitionName + " (" + root.totalSize + ")"
        font.family: Theme.fontFamily
        font.pixelSize: 10
        font.bold: true
        color: "#FFFFFF"

        // RGB565 optimizations
        renderType: Text.NativeRendering
        antialiasing: false
        font.hintingPreference: Font.PreferFullHinting
        width: parent.width
        elide: Text.ElideRight
    }

    // PROGRESS BAR ROW
    Row {
        id: barRow
        anchors {
            left: parent.left
            top: labelText.bottom
            topMargin: 3
        }
        spacing: 8

        // BAR CONTAINER
        Item {
            width: root.width - 56
            height: 8

            // Background bar
            Rectangle {
                id: backgroundBar
                anchors.fill: parent
                radius: 4
                color: Qt.rgba(1, 1, 1, 0.1)
            }

            // Fill bar
            Rectangle {
                id: fillBar
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }
                width: (parent.width * root.usage) / 100
                height: parent.height
                radius: parent.height / 2
                color: root.effectiveBarColor

                // Smooth width animation
                Behavior on width {
                    NumberAnimation {
                        duration: uiAnimationDuration
                        easing.type: Easing.OutCubic
                    }
                }

                // Smooth color transition
                Behavior on color {
                    ColorAnimation {
                        duration: uiAnimationDuration
                    }
                }
            }
        }

        // PERCENTAGE LABEL
        Text {
            id: percentageText
            text: root.usage + "%"
            font.family: Theme.fontFamily
            font.pixelSize: 10
            font.bold: true
            color: root.effectiveBarColor
            anchors.verticalCenter: parent.verticalCenter

            // RGB565 optimizations
            renderType: Text.NativeRendering
            antialiasing: false
            font.hintingPreference: Font.PreferFullHinting
            width: 48
            elide: Text.ElideRight

            // Smooth color transition
            Behavior on color {
                ColorAnimation {
                    duration: uiAnimationDuration
                }
            }
        }
    }

    // USED/FREE DETAILS
    Text {
        id: detailsText
        anchors {
            left: parent.left
            top: barRow.bottom
            topMargin: 3
        }
        text: "Used: " + root.usedSize + " | Free: " + root.freeSize
        font.family: Theme.fontFamily
        font.pixelSize: 8
        color: "#B0B8C8"

        // RGB565 optimizations
        renderType: Text.NativeRendering
        antialiasing: false
        font.hintingPreference: Font.PreferFullHinting
        width: parent.width
        elide: Text.ElideRight
    }

    // COLOR LOGIC
    function _calculateColor() {
        if (root.usage < 75) {
            return "#4CAF50";   // Green - healthy, plenty of space
        }
        else if (root.usage < 90) {
            return "#FF9800";   // Orange - warning, running low
        }
        else {
            return "#F44336";   // Red - critical, almost full
        }
    }

}
