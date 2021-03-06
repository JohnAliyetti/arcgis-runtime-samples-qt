
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

import QtQuick 2.6
import QtQuick.Controls 1.4

Rectangle {
    id: rootOptionsPage
    color: "#F4F4F4"

    property real pageWidth: rootOptionsPage.width - flickable.anchors.margins * 2
    signal statisticButtonClicked()
    property var fields: null
    property var groupingFields: []
    property var statisticTypes: ["Average", "Count", "Maximum", "Minimum", "Standard Deviation", "Sum", "Variance"]
    property alias statisticsModel: statisticsModel
    property alias orderByModel: orderByModel
    property real labelTextSize: 12 * scaleFactor

    // Setup a list model with some defaults pre-set
    ListModel {
        id: statisticsModel

        ListElement {
            field: "POP2007"
            statistic: "Sum"
        }
        ListElement {
            field: "POP2007"
            statistic: "Average"
        }
        ListElement {
            field: "AGE_5_17"
            statistic: "Minimum"
        }
    }

    // Setup a list model with some defaults pre-set
    ListModel {
        id: orderByModel

        ListElement {
            field: "SUB_REGION"
            order: "Ascending"
        }
    }

    // Create a Title Bar for the Options Page
    Rectangle {
        id: titleBar
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        color: "#005e95"
        height: 40 * scaleFactor
        clip: true

        Text {
            anchors.centerIn: parent
            text: "Statistics: US States"
            color: "white"
            font.pixelSize: 28 * scaleFactor
        }
    }

    // Create a Flickable Column to hold the various options for configuring statistics
    Flickable {
        id: flickable
        anchors {
            top: titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 10 * scaleFactor
        }
        contentWidth: column.width
        contentHeight: column.height
        flickableDirection: Flickable.VerticalFlick
        clip: true

        Column {
            id: column
            width: pageWidth
            spacing: 10 * scaleFactor

            Rectangle {
                width: pageWidth
                height: 225 * scaleFactor
                color: "transparent"
                clip: true
                border {
                    width: 1 * scaleFactor
                    color: "black"
                }

                Column {
                    anchors {
                        fill: parent
                        margins: 10 * scaleFactor
                    }
                    spacing: 5 * scaleFactor

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 5 * scaleFactor

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Field"
                            font.pixelSize: labelTextSize
                        }

                        ComboBox {
                            id: fieldComboBox
                            anchors.verticalCenter: parent.verticalCenter
                            model: fields
                            width: 100 * scaleFactor
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Type"
                            font.pixelSize: labelTextSize
                        }

                        ComboBox {
                            id: statisticComboBox
                            anchors.verticalCenter: parent.verticalCenter
                            model: statisticTypes
                            width: 100 * scaleFactor
                        }

                        Button {
                            text: "+"
                            width: 30 * scaleFactor
                            height: width
                            onClicked: {
                                for (var i = 0; i < statisticsModel.count; i++) {
                                    if (statisticsModel.get(i).field === fieldComboBox.currentText) {
                                        if (statisticsModel.get(i).statistic === statisticComboBox.currentText)
                                            return;
                                    }
                                }

                                statisticsModel.append({field: fieldComboBox.currentText, statistic: statisticComboBox.currentText});
                            }
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: 125 * scaleFactor
                        color: "transparent"
                        clip: true
                        border {
                            color: "black"
                            width:  1 * scaleFactor
                        }

                        ListView {
                            id: statisticView
                            anchors {
                                fill: parent
                                margins: 5 * scaleFactor
                            }
                            highlightFollowsCurrentItem: true
                            model: statisticsModel
                            clip: true
                            highlight: highlightRectangle
                            highlightResizeVelocity: 1000000
                            highlightMoveVelocity: 1000000

                            delegate: Item {
                                width: parent.width
                                height: 25 * scaleFactor

                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 10 * scaleFactor
                                    Text {
                                        text: field
                                        font.pixelSize: labelTextSize
                                    }
                                    Text {
                                        text: "(%1)".arg(statistic)
                                        font.pixelSize: labelTextSize
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: statisticView.currentIndex = index;
                                }
                            }
                        }
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Remove Statistic"
                        onClicked: statisticsModel.remove(statisticView.currentIndex, 1);
                    }
                }
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                height: 175 * scaleFactor
                spacing: 5 * scaleFactor

                Column {
                    width: parent.width * 0.45 - parent.spacing
                    height: parent.height
                    spacing: 5 * scaleFactor

                    Text {
                        text: "Group Field(s):"
                        font.pixelSize: labelTextSize
                    }

                    Rectangle {
                        width: parent.width
                        height: 125 * scaleFactor
                        color: "transparent"
                        border {
                            width: 1 * scaleFactor
                            color: "black"
                        }
                        clip: true

                        ListView {
                            id: fieldView
                            anchors {
                                fill: parent
                                margins: 5 * scaleFactor
                            }
                            highlight: highlightRectangle
                            highlightResizeVelocity: 1000000
                            highlightMoveVelocity: 1000000
                            clip: true
                            model: fields
                            delegate: Item {
                                width: parent.width
                                height: 25 * scaleFactor

                                CheckBox {
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: modelData
                                    checked: text === "SUB_REGION"
                                    onCheckedChanged: {
                                        if (checked) {
                                            groupingFields.push(text);
                                        } else {
                                            // remove the item from the selected list
                                            var i = groupingFields.indexOf(text);
                                            if (i !== -1)
                                                groupingFields.splice(i, 1);

                                            // remove the item from the orderBy list
                                            for (var j = 0; j < orderByModel.count; j++) {
                                                if (orderByModel.get(j).field === text)
                                                    orderByModel.remove(j, 1);
                                            }
                                        }
                                    }

                                    onClicked: fieldView.currentIndex = index;
                                }
                            }
                        }
                    }
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width * 0.1 - parent.spacing
                    spacing: 5 * scaleFactor

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 30 * scaleFactor
                        height: width
                        text: ">"
                        onClicked: {
                            // return if the field is not selected
                            if (groupingFields.indexOf(fields[fieldView.currentIndex]) === -1)
                                return;

                            // return if the field is already added
                            for (var i =  0; i < orderByModel.count; i++) {
                                if (orderByModel.get(i).field === fields[fieldView.currentIndex])
                                    return;
                            };

                            // add the data to the model
                            orderByModel.append({field: fields[fieldView.currentIndex], order: "Ascending"});
                        }
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 30 * scaleFactor
                        height: width
                        text: "<"
                        onClicked: orderByModel.remove(groupingView.currentIndex, 1);
                    }
                }

                Column {
                    width: parent.width * 0.45
                    height: parent.height
                    spacing: 5 * scaleFactor

                    Text {
                        text: "Order by Field:"
                        font.pixelSize: labelTextSize
                    }

                    Rectangle {
                        width: parent.width
                        height: 100 * scaleFactor
                        color: "transparent"
                        border {
                            width: 1 * scaleFactor
                            color: "black"
                        }
                        clip: true

                        ListView {
                            id: groupingView
                            anchors {
                                fill: parent
                                margins: 5 * scaleFactor
                            }
                            highlight: highlightRectangle
                            highlightResizeVelocity: 1000000
                            highlightMoveVelocity: 1000000

                            clip: true
                            model: orderByModel
                            delegate: Item {
                                width: parent.width
                                height: 25 * scaleFactor

                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 10 * scaleFactor
                                    Text {
                                        text: field
                                        font.pixelSize: labelTextSize
                                    }
                                    Text {
                                        text: "(%1)".arg(order)
                                        font.pixelSize: labelTextSize
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: groupingView.currentIndex = index;
                                }
                            }
                        }
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Change Sort Order"
                        onClicked: {
                            var i = groupingView.currentIndex;
                            if (orderByModel.get(i).order === "Ascending")
                                orderByModel.get(i).order = "Descending";
                            else
                                orderByModel.get(i).order = "Ascending";
                        }
                    }
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 250 * scaleFactor
                height: 50 * scaleFactor
                text: "<u><b>Get Statistics</b></u>"
                onClicked: statisticButtonClicked()
            }
        }
    }

    Component {
        id: highlightRectangle
        Rectangle {
            color: "cyan"
            opacity: 0.4
        }
    }
}
