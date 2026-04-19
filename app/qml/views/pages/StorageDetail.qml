/*
 * ============================================
 * File: qml/views/pages/StorageDetail.qml
 * Description: Storage detail with partitions and I/O stats
 * ============================================
 */

import QtQuick 2.15
import "../components"

Rectangle {
    id: root

    // ==================== PROPERTIES ====================
    width: 320
    height: 240
    color: "#0F1419"

    // ==================== SIGNALS FOR NAVIGATION ====================
    signal backRequested()
    signal settingsRequested()
    signal navigationRequested(int index)

    // ==================== HEADER ====================

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

    // ==================== CONTENT AREA ====================

    Item {
        id: contentArea
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: bottomNav.top
            bottomMargin: 55
        }

        Column {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 8
            }
            spacing: 8

            // ==================== ROOT PARTITION ====================

            PartitionBar {
                width: 304
                partitionName: "Root Partition"
                totalSize: systemInfo.hddTotal
                usage: systemInfo.hddUsage
                usedSize: systemInfo.hddUsed
                freeSize: systemInfo.hddFree
            }

            // ==================== SWAP PARTITION ====================

            PartitionBar {
                width: 304
                partitionName: "Swap Partition"
                totalSize: systemInfo.swapTotal
                usage: systemInfo.swapUsage
                usedSize: systemInfo.swapUsed
                freeSize: systemInfo.swapFree
            }

            // ==================== I/O STATISTICS CARD ====================

            Rectangle {
                width: 304
                height: 40
                color: "#1E2539"
                radius: 8
                border.width: 1
                border.color: Qt.rgba(1, 1, 1, 0.1)

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
                        height: 20
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

            // ==================== I/O SPEEDS ====================

            Row {
                width: 304
                spacing: 10

                Text {
                    text: "Read: " + systemInfo.ioRead
                    font.family: "DejaVu Sans"
                    font.pixelSize: 8
                    color: "#B0B8C8"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }

                Text {
                    text: "Write: " + systemInfo.ioWrite
                    font.family: "DejaVu Sans"
                    font.pixelSize: 8
                    color: "#B0B8C8"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }
            }
        }
    }

    // ==================== BOTTOM NAVIGATION ====================

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