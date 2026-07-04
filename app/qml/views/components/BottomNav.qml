// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

import QtQuick 2.15
import "../theme"

Rectangle {
    id: root
    width: Theme.screenWidth
    height: Theme.navHeight
    color: Theme.pageBackground

    property int currentIndex: 0
    property double lastNavigationPressMs: 0
    signal navigationRequested(int index)

    function requestIndex(index) {
        var now = Date.now()
        if (now - lastNavigationPressMs < 150) {
            return
        }
        lastNavigationPressMs = now
        navigationRequested(index)
    }

    Row {
        anchors.fill: parent
        spacing: 0

        NavButton {
            width: root.width / 5
            iconSource: "qrc:/assets/icons/home.png"
            active: root.currentIndex === 0
            onClicked: root.requestIndex(0)
        }

        NavButton {
            width: root.width / 5
            iconSource: "qrc:/assets/icons/cpu.png"
            active: root.currentIndex === 1
            onClicked: root.requestIndex(1)
        }

        NavButton {
            width: root.width / 5
            iconSource: "qrc:/assets/icons/memory.png"
            active: root.currentIndex === 2
            onClicked: root.requestIndex(2)
        }

        NavButton {
            width: root.width / 5
            iconSource: "qrc:/assets/icons/hard-drive.png"
            active: root.currentIndex === 3
            onClicked: root.requestIndex(3)
        }

        NavButton {
            width: root.width / 5
            iconSource: "qrc:/assets/icons/wifi.png"
            active: root.currentIndex === 4
            onClicked: root.requestIndex(4)
        }
    }

    component NavButton: Item {
        id: navButton
        width: Theme.touchTarget
        height: Theme.touchTarget

        property string iconSource: ""
        property bool active: false
        signal clicked()

        Column {
            anchors.centerIn: parent
            spacing: 4

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                source: navButton.iconSource
                width: 24
                height: 24
                sourceSize: Qt.size(24, 24)
                opacity: navButton.active ? 1.0 : 0.5
                smooth: false
                Behavior on opacity {
                    NumberAnimation { duration: uiAnimationDuration }
                }
            }

            Rectangle {
                width: 20
                height: 2
                anchors.horizontalCenter: parent.horizontalCenter
                color: Theme.accentBlue
                visible: navButton.active
            }
        }

        MouseArea {
            anchors.fill: parent
            preventStealing: true
            onPressed: parent.clicked()
        }
    }
}
