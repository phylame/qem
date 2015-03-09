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

CommonDialog {
    id: root

    buttonTexts: [qsTr("Save"), qsTr("Discard"), qsTr("Cancel")]
    platformInverted: app.platformInverted
    privateCloseIcon: true

    signal discarded

    onStatusChanged: {
        if (status === DialogStatus.Open)
            scrollBar.flash()
    }

    onButtonClicked: {
        if (index === 0) {
            accepted();
        } else if (index === 1) {
            discarded();
        } else {
            rejected();
        }
        root.destroy();
    }

    content: Item {
        height: {
            if (root.height > 0)
                return Math.min(Math.max(privateStyle.dialogMinSize, root.height) - privateTitleHeight - privateButtonsHeight, root.platformContentMaximumHeight)
            else
                return Math.min(label.paintedHeight, root.platformContentMaximumHeight)
        }
        width: parent.width

        Item {
            anchors {
                top: parent.top; topMargin: platformStyle.paddingLarge
                bottom: parent.bottom; bottomMargin: platformStyle.paddingLarge
                left: parent.left; leftMargin: platformStyle.paddingLarge
                right: parent.right
            }

            Flickable {
                id: flickable
                width: parent.width
                height: parent.height
                anchors { left: parent.left; top: parent.top }
                contentHeight: label.paintedHeight
                flickableDirection: Flickable.VerticalFlick
                clip: true
                interactive: contentHeight > height

                Label {
                    id: label
                    anchors.fill: parent
                    anchors.rightMargin: privateStyle.scrollBarThickness
                    width: flickable.width - privateStyle.scrollBarThickness
                    platformInverted: root.platformInverted
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignLeft
                }
            }

            ScrollBar {
                id: scrollBar
                height: parent.height
                anchors { top: flickable.top; right: flickable.right }
                flickableItem: flickable
                interactive: false
                orientation: Qt.Vertical
                platformInverted: root.platformInverted
            }
        }
    }

    function showAsking(title, text, accept, discard, cancel) {
        root.titleText = title;
        label.text = text.concat("\n");
        if (accept) {
            root.accepted.connect(accept);
        }
        if (discard) {
            root.discarded.connect(discard);
        }
        if (cancel) {
            root.rejected.connect(cancel);
        }
        root.open();
    }
}
