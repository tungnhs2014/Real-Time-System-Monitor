// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

pragma Singleton

import QtQuick 2.15

QtObject {
    readonly property int screenWidth: 320
    readonly property int screenHeight: 240
    readonly property int headerHeight: 30
    readonly property int navHeight: 45
    readonly property int touchTarget: 44
    readonly property int margin: 8
    readonly property int cardRadius: 6

    readonly property string fontFamily: "DejaVu Sans"
    readonly property color pageBackground: "#0F1419"
    readonly property color dashboardBackground: "#0A0E14"
    readonly property color cardBackground: "#1E2539"
    readonly property color cardBackgroundAlt: "#1C3A5C"
    readonly property color primaryText: "#FFFFFF"
    readonly property color secondaryText: "#B0B8C8"
    readonly property color accentBlue: "#2196F3"
    readonly property color accentGreen: "#4CAF50"
    readonly property color accentOrange: "#FF9800"
    readonly property color accentRed: "#F44336"
    readonly property color accentCyan: "#00BCD4"
    readonly property color borderSubtle: Qt.rgba(1, 1, 1, 0.1)
}
