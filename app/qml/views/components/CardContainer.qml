// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

import QtQuick 2.15
import "../theme"

Rectangle {
    width: 100
    height: 75
    color: Theme.cardBackgroundAlt
    radius: Theme.cardRadius
    border.width: 1
    border.color: Qt.rgba(1, 1, 1, 0.15)

    default property alias content: contentArea.data

    Item {
        id: contentArea
        anchors.fill: parent
        anchors.margins: 8
    }
}
