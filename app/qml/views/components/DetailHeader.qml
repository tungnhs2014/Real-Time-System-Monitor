// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

import QtQuick 2.15
import "../theme"

Rectangle {
    id: root

    // PROPERTIES
    width: Theme.screenWidth
    height: Theme.headerHeight

    // Customizable properties
    property string title: "DETAIL"
    property color backgroundColor: Theme.pageBackground
    property color textColor: Theme.primaryText
    property int fontSize: 12
    property bool showBackButton: true
    property bool showSettingsIcon: true
    property double lastBackPressMs: 0
    property double lastSettingsPressMs: 0

    // SIGNALS
    signal backClicked()
    signal settingsClicked()

    function requestBack() {
        var now = Date.now()
        if (now - lastBackPressMs < 150) {
            return
        }
        lastBackPressMs = now
        backClicked()
    }

    function requestSettings() {
        var now = Date.now()
        if (now - lastSettingsPressMs < 150) {
            return
        }
        lastSettingsPressMs = now
        settingsClicked()
    }

    // BACKGROUND
    color: backgroundColor

    // Bottom border line
    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 1
        color: Theme.borderSubtle
    }

    // BACK BUTTON
    Item {
        id: backButton
        anchors {
            left: parent.left
            leftMargin: 8
            verticalCenter: parent.verticalCenter
        }
        width: Theme.touchTarget
        height: Theme.touchTarget
        visible: root.showBackButton

        // Back arrow icon (PNG)
        Image {
            id: backIcon
            anchors.centerIn: parent
            source: "qrc:/assets/icons/arrow-left.png"
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
                backIcon.opacity = 0.6
            }
            onReleased: backIcon.opacity = 1.0
            onCanceled: backIcon.opacity = 1.0
            onClicked: root.requestBack()
        }
    }

    // TITLE TEXT
    Text {
        id: titleText
        anchors.centerIn: parent
        width: parent.width - 112
        text: root.title
        font.family: Theme.fontFamily
        font.pixelSize: root.fontSize
        font.bold: true
        color: root.textColor
        renderType: Text.NativeRendering
        antialiasing: false
        font.hintingPreference: Font.PreferFullHinting
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
    }

    // SETTINGS ICON
    Item {
        id: settingsButton
        anchors {
            right: parent.right
            rightMargin: 8
            verticalCenter: parent.verticalCenter
        }
        width: Theme.touchTarget
        height: Theme.touchTarget
        visible: root.showSettingsIcon

        // Settings gear icon (PNG)
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
