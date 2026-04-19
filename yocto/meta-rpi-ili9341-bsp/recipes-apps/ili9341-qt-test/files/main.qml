import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: root
    visible: true
    width: 320
    height: 240
    
    Rectangle {
        anchors.fill: parent
        
        gradient: Gradient {
            GradientStop { position: 0.0; color: colorAnim.color1 }
            GradientStop { position: 1.0; color: colorAnim.color2 }
        }
        
        Column {
            anchors.centerIn: parent
            spacing: 20
            
            Text {
                text: "Hello Son Tung"
                font.pixelSize: 32
                font.bold: true
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
            }
            
            Text {
                text: "ILI9341 Display"
                font.pixelSize: 24
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
            }
            
            Text {
                text: touchArea.pressed ? "Touch Detected!" : "Touch to Test"
                font.pixelSize: 18
                color: touchArea.pressed ? "yellow" : "white"
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
        
        MouseArea {
            id: touchArea
            anchors.fill: parent
        }
        
        QtObject {
            id: colorAnim
            property color color1: "#FF6B6B"
            property color color2: "#4ECDC4"
            
            SequentialAnimation on color1 {
                loops: Animation.Infinite
                ColorAnimation { to: "#4ECDC4"; duration: 3000 }
                ColorAnimation { to: "#FFE66D"; duration: 3000 }
                ColorAnimation { to: "#FF6B6B"; duration: 3000 }
            }
            
            SequentialAnimation on color2 {
                loops: Animation.Infinite
                ColorAnimation { to: "#FFE66D"; duration: 3000 }
                ColorAnimation { to: "#FF6B6B"; duration: 3000 }
                ColorAnimation { to: "#4ECDC4"; duration: 3000 }
            }
        }
    }
}
