/*
 * Copyright 2014-2015 Peng Wan
 *
 * This file is part of Qem.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import QtQuick 1.1
import com.nokia.symbian 1.1
import "com"
import "js/main.js" as Script

Page {
    id: root

    tools: ToolBarLayout {
        BackToolButton {}

        ToolButton {
            text: qsTr("Reset")
            platformInverted: app.platformInverted
            onClicked: settings.reset()
        }
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: content.height
        Column {
            id: content
            // General
//            SectionHeader {
//                text: qsTr("General")
//            }

            // Appearance
            SectionHeader {
                text: qsTr("Appearance")
            }
            // white theme
            MenuCheckBox {
                text: qsTr("Day theme:")
                checked: settings.theme
                onCheckedChanged: settings.theme = checked
            }
            // show status bar
            MenuCheckBox {
                text: qsTr("Show status bar:")
                checked: settings.statusBar
                onCheckedChanged: settings.statusBar = checked
            }
            // backgroung image
            SelectionListItem {
                title: qsTr("Background Image(Long press to default)")
                platformInverted: app.platformInverted
                subTitle: settings.background.length >0 ? settings.background : qsTr("Default")
                onClicked: {
                    var path = Script.selectImage(qsTr("Select background"));
                    if (path)
                        settings.background = path;
                }
                onPressAndHold: settings.background = ""
            }

            // Operations
            SectionHeader {
                text: qsTr("Operations")
            }
            MenuCheckBox {
                text: qsTr("Enter chapter after adding:")
                checked: settings.enterAfterAdd
                onCheckedChanged: settings.enterAfterAdd = checked
            }

            Label {
                text: qsTr("Maximum shown parts: ") + sdShownParts.value
                anchors.left: parent.left
                anchors.leftMargin: 6
                platformInverted: app.platformInverted
            }
            Row {
                width: parent.width
                Label {
                    id: lbSdMin
                    text: sdShownParts.minimumValue
                    anchors.verticalCenter: parent.verticalCenter
                    platformInverted: app.platformInverted
                }
                Slider {
                    id: sdShownParts
                    minimumValue: 1
                    maximumValue: 400
                    value: settings.maxShownParts
                    platformInverted: app.platformInverted
                    width: parent.width - lbSdMin.width - lbSdMax.width
                    stepSize: 1
                    valueIndicatorVisible: true
                    onPressedChanged: settings.maxShownParts = value
                }
                Label {
                    id: lbSdMax
                    text: sdShownParts.maximumValue
                    anchors.verticalCenter: parent.verticalCenter
                    platformInverted: app.platformInverted
                }
            }
        }
    }

    ScrollDecorator {
        flickableItem: flickable
        platformInverted: app.platformInverted
    }

}
