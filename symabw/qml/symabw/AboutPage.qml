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

Page {
    id: root

    tools: ToolBarLayout {
        BackToolButton {}
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: content.height
        Column {
            id: content

            Row{
                Image {
                    width: 44
                    height: 44
                    source: "gfx/icon.svg"
                }
                Label {
                    text: qsTr("Powered by") + " " + imabw.vendor
                    anchors.verticalCenter: parent.verticalCenter
                    platformInverted: app.platformInverted
                }
            }

            // Application information
            SectionHeader {
                text: qsTr("Imabw")
            }
            Label {
                text: info()
                textFormat: Text.RichText
                anchors.left: parent.left
                anchors.leftMargin: 6
                wrapMode: Text.WrapAnywhere
                platformInverted: app.platformInverted
                onLinkActivated: Qt.openUrlExternally(link)

                function info() {
                    var text = imabw.description + " v" + imabw.version + "<br/>" +
                            qsTr("Author: ") + imabw.author + "<br/>" +
                            qsTr("Home: ") + "<a href=\"" + imabw.home + "\">" + imabw.home + "</a>";
                    return text;
                }
            }
            Label {
                text: imabw.rights
                anchors {
                    left: parent.left
                    leftMargin: 6
                }
                wrapMode: Text.WrapAnywhere
                platformInverted: app.platformInverted
            }
            // Change log
            SectionHeader {
                text: qsTr("Change log")
            }
            Label {
                text: imabw.features
                anchors.left: parent.left
                anchors.leftMargin: 6
                wrapMode: Text.WrapAnywhere
                platformInverted: app.platformInverted
            }

            // Qem
            SectionHeader {
                text: qsTr("Qem")
            }
            Label {
                text: imabw.qem
                anchors.left: parent.left
                anchors.leftMargin: 6
                wrapMode: Text.WrapAnywhere
                platformInverted: app.platformInverted
            }
        }
    }

    ScrollDecorator {
        flickableItem: flickable
        platformInverted: app.platformInverted
    }

}
