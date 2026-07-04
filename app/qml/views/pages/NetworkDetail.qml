// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

import QtQuick 2.15
import "../components"
import "../theme"

Rectangle {
    id: root

    // PROPERTIES
    width: Theme.screenWidth
    height: Theme.screenHeight
    color: Theme.pageBackground

    // SIGNALS FOR NAVIGATION
    signal backRequested()
    signal settingsRequested()
    signal navigationRequested(int index)

    // DEBUG
    Component.onCompleted: {
        if (QML_DEBUG_ENABLED) console.log("=== NetworkDetail DEBUG ===")
        if (QML_DEBUG_ENABLED) console.log("Interface:", systemInfo.networkInterface)
        if (QML_DEBUG_ENABLED) console.log("IP:", systemInfo.ipAddress)
        if (QML_DEBUG_ENABLED) console.log("MAC:", systemInfo.macAddress)
        if (QML_DEBUG_ENABLED) console.log("Upload history length:", systemInfo.netUpHistory.length)
        if (QML_DEBUG_ENABLED) console.log("Download history length:", systemInfo.netDownHistory.length)
    }

    Connections {
        target: systemInfo
        function onNetUpHistoryChanged() {
            if (QML_DEBUG_ENABLED) console.log("Upload history updated, length:", systemInfo.netUpHistory.length)
        }
        function onNetDownHistoryChanged() {
            if (QML_DEBUG_ENABLED) console.log("Download history updated, length:", systemInfo.netDownHistory.length)
        }
    }

    // HEADER
    DetailHeader {
        id: header
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        title: "NETWORK"

        onBackClicked: {
            root.backRequested()
        }

        onSettingsClicked: {
            root.settingsRequested()
        }
    }

    // CONTENT AREA
    Item {
        id: contentArea
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: bottomNav.top
        }

        // NETWORK INFO CARD
        Rectangle {
            id: infoCard
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 8
            }
            width: 304
            height: 30
            color: Theme.cardBackground
            radius: Theme.cardRadius
            border.width: 1
            border.color: Theme.borderSubtle

            Row {
                anchors {
                    left: parent.left
                    leftMargin: 8
                    verticalCenter: parent.verticalCenter
                }
                spacing: 8

                // Interface info
                Column {
                    width: 128
                    spacing: 2

                    Text {
                        width: parent.width
                        text: "Interface: " + systemInfo.networkInterface
                        font.family: Theme.fontFamily
                        font.pixelSize: 8
                        color: Theme.primaryText
                        renderType: Text.NativeRendering
                        antialiasing: false
                        font.hintingPreference: Font.PreferFullHinting
                        elide: Text.ElideRight
                    }

                    Text {
                        width: parent.width
                        text: "MAC: " + systemInfo.macAddress
                        font.family: Theme.fontFamily
                        font.pixelSize: 7
                        color: Theme.secondaryText
                        renderType: Text.NativeRendering
                        antialiasing: false
                        font.hintingPreference: Font.PreferFullHinting
                        elide: Text.ElideRight
                    }
                }

                // IP address
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 152
                    text: "IP: " + systemInfo.ipAddress
                    font.family: Theme.fontFamily
                    font.pixelSize: 8
                    color: Theme.primaryText
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                    elide: Text.ElideRight
                }
            }
        }

        // TRAFFIC CHART CARD
        Rectangle {
            id: chartCard
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: infoCard.bottom
                topMargin: 8
            }
            width: 304
            height: 60
            color: Theme.cardBackground
            radius: Theme.cardRadius
            border.width: 1
            border.color: Theme.borderSubtle

            Column {
                anchors.fill: parent
                anchors.margins: 6
                spacing: 2

                // Title
                Text {
                    text: "Network Traffic"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 8
                    font.bold: true
                    color: "#B0B8C8"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }

                // Dual-line chart
                Item {
                    width: 292
                    height: 40

                    // Calculate shared max for both charts
                    property real sharedMax: {
                        var maxDown = Math.max.apply(null, systemInfo.netDownHistory.length > 0 ? systemInfo.netDownHistory : [0]);
                        var maxUp = Math.max.apply(null, systemInfo.netUpHistory.length > 0 ? systemInfo.netUpHistory : [0]);
                        return Math.max(maxDown, maxUp, 10); // Minimum 10
                    }

                    // Download line (behind, orange)
                    LineChart {
                        anchors.fill: parent
                        dataPoints: systemInfo.netDownHistory
                        lineColor: "#FF9800"  // Orange for download
                        smoothLine: true
                        lineWidth: 2
                        showAxisLabels: false
                        yAxisUnit: "KB/s"
                        showGrid: true
                        maxValue: parent.sharedMax
                        minValue: 0
                        showValues: true  // Show value!
                        showLastValueOnly: true  // Only latest
                        valueFontSize: 8  // Smaller for dual chart
                    }

                    // Upload line (front, cyan)
                    LineChart {
                        anchors.fill: parent
                        dataPoints: systemInfo.netUpHistory
                        lineColor: "#00BCD4"  // Cyan for upload
                        smoothLine: true
                        lineWidth: 2
                        showAxisLabels: false  // Download chart shows labels
                        showGrid: false
                        maxValue: parent.sharedMax
                        minValue: 0
                        showValues: true  // Show value!
                        showLastValueOnly: true  // Only latest
                        valueFontSize: 8
                        valueColor: "#00BCD4"  // Match line color
                    }

                    // Legend
                    Row {
                        anchors {
                            right: parent.right
                            top: parent.top
                            topMargin: 2
                        }
                        spacing: 8

                        // Upload legend
                        Row {
                            spacing: 3

                            Rectangle {
                                width: 12
                                height: 2
                                color: "#00BCD4"
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Text {
                                text: "↑Up"
                                font.family: "DejaVu Sans"
                                font.pixelSize: 7
                                color: "#00BCD4"
                                renderType: Text.NativeRendering
                                antialiasing: false
                                font.hintingPreference: Font.PreferFullHinting
                            }
                        }

                        // Download legend
                        Row {
                            spacing: 3

                            Rectangle {
                                width: 12
                                height: 2
                                color: "#FF9800"
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Text {
                                text: "↓Down"
                                font.family: "DejaVu Sans"
                                font.pixelSize: 7
                                color: "#FF9800"
                                renderType: Text.NativeRendering
                                antialiasing: false
                                font.hintingPreference: Font.PreferFullHinting
                            }
                        }
                    }
                }
            }
        }

        // NETWORK STATS
        Column {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: chartCard.bottom
                topMargin: 8
            }
            width: 304
            spacing: 4

            // Current speeds
            Row {
                width: parent.width
                spacing: 10

                Text {
                    width: 136
                    text: "↑ Up: " + systemInfo.netUpSpeed
                    font.family: Theme.fontFamily
                    font.pixelSize: 10
                    color: Theme.accentCyan
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                    elide: Text.ElideRight
                }

                Text {
                    width: 158
                    text: "↓ Down: " + systemInfo.netDownSpeed
                    font.family: Theme.fontFamily
                    font.pixelSize: 10
                    color: Theme.accentOrange
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                    elide: Text.ElideRight
                }
            }

            // Additional stats
            Row {
                width: parent.width
                spacing: 12

                Text {
                    width: 166
                    text: "Packets: " + systemInfo.packetRate
                    font.family: Theme.fontFamily
                    font.pixelSize: 8
                    color: Theme.secondaryText
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                    elide: Text.ElideRight
                }

                Text {
                    width: 126
                    text: "Connections: " + systemInfo.activeConnections
                    font.family: Theme.fontFamily
                    font.pixelSize: 8
                    color: Theme.secondaryText
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                    elide: Text.ElideRight
                }
            }
        }
    }

    // BOTTOM NAVIGATION
    BottomNav {
        id: bottomNav
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        currentIndex: 4  // Network tab (index 4)

        // Forward navigation signal to Main.qml
        onNavigationRequested: function(index) {
            root.navigationRequested(index)
        }
    }
}
