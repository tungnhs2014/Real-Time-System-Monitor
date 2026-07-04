// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

import QtQuick 2.15

Item {
    id: root

    width: 128
    height: 38

    // Properties
    property int value: 70
    property int minValue: 0
    property int maxValue: 100
    property int step: 5
    property string suffix: "%"
    property bool enabled: true
    property double lastAdjustMs: 0

    // Note: valueChanged signal is automatically created by the 'value' property
    function adjustValue(direction) {
        if (direction === 0) {
            return
        }

        var now = Date.now()
        if (now - lastAdjustMs < 120) {
            return
        }

        var newValue = root.value + (root.step * direction)
        if (newValue >= root.minValue && newValue <= root.maxValue) {
            root.value = newValue
            lastAdjustMs = now
        } else {
            stopRapidAdjust()
        }
    }

    function stopRapidAdjust() {
        rapidTimer.direction = 0
    }

    Row {
        anchors.fill: parent
        spacing: 0

        // MINUS BUTTON
        Rectangle {
            id: minusBtn
            width: 40
            height: 38
            radius: 4
            color: "#1E2539"
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.2)

            Rectangle {
                anchors.fill: parent
                radius: 4
                color: minusArea.pressed ? "#2196F3" :
                       (minusArea.containsMouse ? "#2A3548" : "transparent")
            }

            Text {
                anchors.centerIn: parent
                text: "−"
                font.pixelSize: 18
                font.bold: true
                font.family: "DejaVu Sans"
                color: root.value > root.minValue && root.enabled ? "#FFFFFF" : "#4A5568"
                renderType: Text.NativeRendering
                antialiasing: false
            }

            MouseArea {
                id: minusArea
                anchors.fill: parent
                hoverEnabled: true
                preventStealing: true
                enabled: root.enabled && root.value > root.minValue

                onPressed: root.adjustValue(-1)

                // Long press for rapid decrease
                onPressAndHold: rapidTimer.direction = -1
                onReleased: root.stopRapidAdjust()
                onCanceled: root.stopRapidAdjust()
            }
        }

        // VALUE DISPLAY
        Rectangle {
            width: 48
            height: 38
            radius: 4
            color: "#1E2539"
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.2)

            Text {
                anchors.centerIn: parent
                text: root.value + root.suffix
                font.family: "DejaVu Sans"
                font.pixelSize: 12
                font.bold: true
                font.hintingPreference: Font.PreferFullHinting
                color: root.enabled ? "#FFFFFF" : "#4A5568"
                renderType: Text.NativeRendering
                antialiasing: false
            }
        }

        // PLUS BUTTON
        Rectangle {
            id: plusBtn
            width: 40
            height: 38
            radius: 4
            color: "#1E2539"
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.2)

            Rectangle {
                anchors.fill: parent
                radius: 4
                color: plusArea.pressed ? "#2196F3" :
                       (plusArea.containsMouse ? "#2A3548" : "transparent")
            }

            Text {
                anchors.centerIn: parent
                text: "+"
                font.pixelSize: 18
                font.bold: true
                font.family: "DejaVu Sans"
                color: root.value < root.maxValue && root.enabled ? "#FFFFFF" : "#4A5568"
                renderType: Text.NativeRendering
                antialiasing: false
            }

            MouseArea {
                id: plusArea
                anchors.fill: parent
                hoverEnabled: true
                preventStealing: true
                enabled: root.enabled && root.value < root.maxValue

                onPressed: root.adjustValue(1)

                // Long press for rapid increase
                onPressAndHold: rapidTimer.direction = 1
                onReleased: root.stopRapidAdjust()
                onCanceled: root.stopRapidAdjust()
            }
        }
    }

    // Timer for rapid value change on long press
    Timer {
        id: rapidTimer
        property int direction: 0
        interval: 120
        repeat: true
        running: direction !== 0

        onTriggered: root.adjustValue(direction)
    }
}
