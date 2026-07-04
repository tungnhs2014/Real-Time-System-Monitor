// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

import QtQuick 2.15
import "../theme"

Rectangle {
    id: root
    width: Theme.screenWidth
    height: Theme.headerHeight
    color: Theme.dashboardBackground

    property string currentTime: "03:01"
    property string hostname: "raspberrypi"
    property double lastSettingsPressMs: 0

    // ADD: Settings signal
    signal settingsClicked()

    function requestSettings() {
        var now = Date.now()
        if (now - lastSettingsPressMs < 150) {
            return
        }
        lastSettingsPressMs = now
        settingsClicked()
    }

    // Time display
    Text {
        x: 16
        y: 11
        width: 58
        text: root.currentTime
        font.family: Theme.fontFamily
        font.pixelSize: 14
        font.bold: true
        font.hintingPreference: Font.PreferFullHinting
        color: Theme.primaryText
        renderType: Text.NativeRendering
        antialiasing: false
        elide: Text.ElideRight
    }

    // Hostname display
    Text {
        anchors.centerIn: parent
        width: 150
        text: root.hostname
        font.family: Theme.fontFamily
        font.pixelSize: 11
        font.bold: true
        font.hintingPreference: Font.PreferFullHinting
        color: Theme.secondaryText
        renderType: Text.NativeRendering
        antialiasing: false
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
    }

    Item {
        id: settingsButton
        anchors {
            right: parent.right
            rightMargin: 8
            verticalCenter: parent.verticalCenter
        }
        width: Theme.touchTarget
        height: Theme.touchTarget

        Image {
            id: settingsIcon
            anchors.centerIn: parent
            source: "qrc:/assets/icons/settings.png"
            width: 20
            height: 20
            sourceSize: Qt.size(20, 20)
            smooth: false
            opacity: 1.0
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            preventStealing: true
            onPressed: {
                settingsIcon.opacity = 0.6
            }
            onReleased: settingsIcon.opacity = 1.0
            onCanceled: settingsIcon.opacity = 1.0
            onClicked: root.requestSettings()
        }
    }
}
