// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import "views/pages"
import "views/theme"

Window {
    id: root

    width: Theme.screenWidth
    height: Theme.screenHeight

    visible: true
    title: "ILI9341 System Monitor"

    flags: Qt.FramelessWindowHint

    color: Theme.pageBackground

    Component.onCompleted: {
        contentItem.smooth = false
        contentItem.antialiasing = false
    }

    readonly property int pageAnimationDuration: uiAnimationDuration
    property bool navigationInputBlocked: false

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: dashboardComponent

        Component {
            id: dashboardComponent

            Dashboard {
                objectName: "dashboard"
            }
        }

        pushEnter: Transition {
            PropertyAnimation {
                property: "x"
                from: root.width
                to: 0
                duration: root.pageAnimationDuration
                easing.type: Easing.OutCubic
            }
            PropertyAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: root.pageAnimationDuration
            }
        }

        pushExit: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: root.pageAnimationDuration
            }
        }

        popExit: Transition {
            PropertyAnimation {
                property: "x"
                from: 0
                to: root.width
                duration: root.pageAnimationDuration
                easing.type: Easing.InCubic
            }
            PropertyAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: root.pageAnimationDuration
            }
        }

        popEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: root.pageAnimationDuration
            }
        }
    }

    function pageNameForIndex(index) {
        if (index === 0) return "dashboard"
        if (index === 1) return "cpuDetail"
        if (index === 2) return "memoryDetail"
        if (index === 3) return "storageDetail"
        if (index === 4) return "networkDetail"
        return ""
    }

    function pageComponentForIndex(index) {
        if (index === 0) return dashboardComponent
        if (index === 1) return cpuDetailComponent
        if (index === 2) return memoryDetailComponent
        if (index === 3) return storageDetailComponent
        if (index === 4) return networkDetailComponent
        return null
    }

    function navigateToIndex(index) {
        var targetName = pageNameForIndex(index)
        if (targetName === "" || (stackView.currentItem && stackView.currentItem.objectName === targetName)) {
            return
        }

        if (!beginNavigation()) {
            return
        }

        stackView.pop(null)

        var targetComponent = pageComponentForIndex(index)
        if (index !== 0 && targetComponent !== null) {
            stackView.push(targetComponent)
        }
    }

    function beginNavigation() {
        if (navigationInputBlocked) {
            return false
        }

        navigationInputBlocked = true
        navigationInputGuard.restart()
        return true
    }

    function goBack() {
        if (stackView.depth > 1 && beginNavigation()) {
            stackView.pop()
        }
    }

    function openSettings() {
        if (stackView.currentItem.objectName !== "settings" && beginNavigation()) {
            stackView.push(settingsComponent)
        }
    }

    Timer {
        id: navigationInputGuard
        interval: 300
        repeat: false
        onTriggered: root.navigationInputBlocked = false
    }

    MouseArea {
        anchors.fill: parent
        visible: root.navigationInputBlocked
        enabled: visible
        z: 900
        preventStealing: true
    }

    // CONNECTIONS FOR NAVIGATION
    // Listen for navigation events from current page
    Connections {
        target: stackView.currentItem
        ignoreUnknownSignals: true  // Ignore signals that don't exist on current page

        // Handle back button clicks from DetailHeader
        function onBackRequested() {
            root.goBack()
        }

        // Handle settings button clicks from DetailHeader
        function onSettingsRequested() {
            root.openSettings()
        }

        // Handle bottom nav clicks
        function onNavigationRequested(index) {
            root.navigateToIndex(index)
        }
    }

    // PAGE COMPONENTS
    Component {
        id: cpuDetailComponent
        CpuDetail {
            objectName: "cpuDetail"
        }
    }

    Component {
        id: memoryDetailComponent
        MemoryDetail {
            objectName: "memoryDetail"
        }
    }

    Component {
        id: storageDetailComponent
        StorageDetail {
            objectName: "storageDetail"
        }
    }

    Component {
        id: networkDetailComponent
        NetworkDetail {
            objectName: "networkDetail"
        }
    }

    Component {
        id: settingsComponent
        Settings {
            objectName: "settings"
        }
    }

    // KEYBOARD SHORTCUTS (Desktop Testing)
    Item {
        id: keyboardHandler
        focus: true

        Keys.onPressed: function(event) {
            // Esc or Backspace = Go back
            if (event.key === Qt.Key_Escape || event.key === Qt.Key_Backspace) {
                root.goBack()
                event.accepted = true
            }

            // Number keys for quick navigation (Desktop testing)
            else if (event.key === Qt.Key_0) {
                root.navigateToIndex(0)
                event.accepted = true
            }
            else if (event.key === Qt.Key_1) {
                root.navigateToIndex(1)
                event.accepted = true
            }
            else if (event.key === Qt.Key_2) {
                root.navigateToIndex(2)
                event.accepted = true
            }
            else if (event.key === Qt.Key_3) {
                root.navigateToIndex(3)
                event.accepted = true
            }
            else if (event.key === Qt.Key_4) {
                root.navigateToIndex(4)
                event.accepted = true
            }
            else if (event.key === Qt.Key_9) {
                root.openSettings()
                event.accepted = true
            }
        }
    }

    // DEBUG INFO (Optional)
    Text {
        id: debugInfo
        visible: false  // Set to true for debugging
        anchors {
            top: parent.top
            left: parent.left
            margins: 4
        }
        text: "Stack: " + stackView.depth + " | Page: " +
              (stackView.currentItem ? stackView.currentItem.objectName : "none")
        font.pixelSize: 7
        color: "#FFFFFF"
        z: 1000

        renderType: Text.NativeRendering
        antialiasing: false
        font.hintingPreference: Font.PreferFullHinting
    }
}
