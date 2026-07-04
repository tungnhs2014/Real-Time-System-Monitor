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

    // HEADER
    DetailHeader {
        id: header
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        title: "STORAGE"

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

        Column {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 6
            }
            spacing: 5

            // ROOT PARTITION
            PartitionBar {
                width: 304
                partitionName: "Root Partition"
                totalSize: systemInfo.hddTotal
                usage: systemInfo.hddUsage
                usedSize: systemInfo.hddUsed
                freeSize: systemInfo.hddFree
            }

            // SWAP PARTITION
            PartitionBar {
                width: 304
                partitionName: "Swap Partition"
                totalSize: systemInfo.swapTotal
                usage: systemInfo.swapUsage
                usedSize: systemInfo.swapUsed
                freeSize: systemInfo.swapFree
            }

            // I/O STATISTICS CARD
            Rectangle {
                width: 304
                height: 38
                color: Theme.cardBackground
                radius: Theme.cardRadius
                border.width: 1
                border.color: Theme.borderSubtle

                Column {
                    anchors.fill: parent
                    anchors.margins: 2
                    spacing: 2

                    // Title
                    Text {
                        text: "I/O Statistics"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 8
                        font.bold: true
                        color: "#B0B8C8"
                        renderType: Text.NativeRendering
                        antialiasing: false
                        font.hintingPreference: Font.PreferFullHinting
                    }

                    // Mini chart
                    LineChart {
                        width: 288
                        height: 18
                        dataPoints: systemInfo.ioHistory
                        lineColor: "#FFEB3B"  // Yellow for I/O
                        smoothLine: true
                        showAxisLabels: false
                        yAxisUnit: "MB/s"
                        showGrid: false
                        showValues: true  // Show value!
                        showLastValueOnly: true
                        valueFontSize: 8
                    }
                }
            }

            // I/O SPEEDS
            Row {
                width: 304
                spacing: 10

                Text {
                    width: 144
                    text: "Read: " + systemInfo.ioRead
                    font.family: "DejaVu Sans"
                    font.pixelSize: 8
                    color: "#B0B8C8"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                    elide: Text.ElideRight
                }

                Text {
                    width: 150
                    text: "Write: " + systemInfo.ioWrite
                    font.family: "DejaVu Sans"
                    font.pixelSize: 8
                    color: "#B0B8C8"
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
        currentIndex: 3  // Storage tab (index 3)

        // Forward navigation signal to Main.qml
        onNavigationRequested: function(index) {
            root.navigationRequested(index)
        }
    }
}
