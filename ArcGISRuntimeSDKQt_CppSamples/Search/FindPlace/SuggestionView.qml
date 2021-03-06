// [WriteFile Name=FindPlace, Category=Search]
// [Legal]
// Copyright 2017 Esri.

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

Rectangle {
    color: "#f7f8fa"
    opacity: 0.85

    signal suggestionClicked(var label)
    property int count: suggestView.count

    ListView {
        id: suggestView
        model: findPlaceSample.suggestions
        height: parent.height
        delegate: Component {

            Rectangle {
                width: poiTextField.width
                height: 20 * scaleFactor
                color: "#f7f8fa"
                border.color: "darkgray"

                Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        leftMargin: 5 * scaleFactor
                        rightMargin: 5 * scaleFactor
                    }

                    font {
                        weight: Font.Black
                        pixelSize: 12 * scaleFactor
                    }

                    width: parent.width
                    text: label
                    elide: Text.ElideRight
                    leftPadding: 5 * scaleFactor
                    renderType: Text.NativeRendering
                    color: "black"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        suggestionClicked(label)
                    }
                }
            }
        }
    }
}
