// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

import QtQuick 2.15
import "../theme"

Rectangle {
    id: root

    width: Theme.screenWidth
    height: root.barHeight
    color: Theme.cardBackground

    property var tabs: ["Tab 1", "Tab 2", "Tab 3", "Tab 4"]
    property int currentIndex: 0
    property int barHeight: Theme.touchTarget
    property int fontPixelSize: 9
    property int indicatorHeight: 3
    property double lastTabPressMs: 0

    property color activeColor: Theme.accentBlue
    property color inactiveColor: Theme.secondaryText
    property color backgroundColor: Theme.cardBackground
    property color indicatorColor: Theme.accentBlue

    signal tabClicked(int index)

    function requestTab(index) {
        var now = Date.now()
        if (now - lastTabPressMs < 150) {
            return
        }
        lastTabPressMs = now
        currentIndex = index
        tabClicked(index)
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: 1
        color: Qt.rgba(1, 1, 1, 0.1)
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 1
        color: Qt.rgba(1, 1, 1, 0.1)
    }

    Row {
        id: tabRow
        anchors.fill: parent

        Repeater {
            id: tabRepeater
            model: root.tabs

            Item {
                width: root.width / root.tabs.length
                height: root.height

                Text {
                    anchors.centerIn: parent
                    text: modelData
                    width: parent.width - 4
                    font.family: Theme.fontFamily
                    font.pixelSize: root.fontPixelSize
                    font.bold: index === root.currentIndex
                    color: index === root.currentIndex ? root.activeColor : root.inactiveColor

                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight

                    Behavior on color {
                        ColorAnimation { duration: uiAnimationDuration }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor

                    onPressed: root.requestTab(index)
                }
            }
        }
    }

    Rectangle {
        id: activeIndicator
        anchors {
            bottom: parent.bottom
            bottomMargin: 1
        }
        width: root.width / root.tabs.length
        height: root.indicatorHeight
        color: root.indicatorColor

        x: (root.width / root.tabs.length) * root.currentIndex

        Behavior on x {
            NumberAnimation {
                duration: uiAnimationDuration
                easing.type: Easing.OutCubic
            }
        }
    }
}
