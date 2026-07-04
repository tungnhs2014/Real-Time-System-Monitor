// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

import QtQuick 2.15
import "../theme"

Rectangle {
    id: root

    width: 100
    height: 28
    radius: Theme.cardRadius

    property string text: "Button"
    property bool enabled: true
    property bool triggerOnPress: false
    property bool preventInputStealing: false
    property double lastTriggerMs: 0

    property color buttonColor: Theme.accentBlue
    property color textColor: Theme.primaryText
    property color pressedColor: Qt.darker(buttonColor, 1.2)
    property color disabledColor: "#4A5568"

    property bool isPressed: false

    signal clicked()

    function triggerClick() {
        var now = Date.now()
        if (now - lastTriggerMs < 150) {
            return
        }
        lastTriggerMs = now
        clicked()
    }

    color: {
        if (!root.enabled) return root.disabledColor
        if (root.isPressed) return root.pressedColor
        return root.buttonColor
    }

    Behavior on color {
        ColorAnimation {
            duration: uiAnimationDuration
        }
    }

    border.width: 1
    border.color: Qt.darker(root.buttonColor, 1.3)

    opacity: root.enabled ? 1.0 : 0.6

    Text {
        id: buttonText
        anchors.centerIn: parent
        width: parent.width - 8
        text: root.text
        font.family: Theme.fontFamily
        font.pixelSize: 10
        font.bold: true
        color: root.textColor

        renderType: Text.NativeRendering
        antialiasing: false
        font.hintingPreference: Font.PreferFullHinting
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: root.enabled
        cursorShape: root.enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor
        preventStealing: root.preventInputStealing

        onPressed: {
            root.isPressed = true
            if (root.triggerOnPress) {
                root.triggerClick()
            }
        }

        onReleased: {
            root.isPressed = false
        }

        onCanceled: {
            root.isPressed = false
        }

        onClicked: {
            if (!root.triggerOnPress) {
                root.triggerClick()
            }
        }
    }
}
