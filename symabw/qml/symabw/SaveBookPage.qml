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
import com.nokia.extras 1.1
import pw.symbian.imabw 1.0
import "com"

Page {
    id: root

    signal saved(string file, string format, variant args)

    tools: ToolBarLayout {
        BackToolButton {}

        TipToolButton {
            text: qsTr("Save")
            tipText: qsTr("Save book to file")
            enabled: dirItem.subTitle.length !== 0
            onClicked: {
                var format = formatItem.subTitle.toLowerCase();
                var path = dirItem.subTitle + "/" + nameItem.text + "." + format;
                app.closePage(root);
                root.saved(path, format, argumentsItem.arguments);
            }
        }
        TipToolButton {
            iconSource: "toolbar-add"
            tipText: qsTr("Add one argument")
        }
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: content.height
        Column {
            id: content
            SectionHeader {
                text: qsTr("Destination")
            }
            // destination directory
            SelectionListItem {
                id: dirItem
                title: qsTr("Tap to select directory")
                platformInverted: app.platformInverted
                onClicked: {
                    var dir = utility.selectFolder(qsTr("Select directory"), settings.lastDirectory);
                    if (dir !== "") {
                        dirItem.subTitle = dir;
                        settings.lastDirectory = dir;
                    }
                }
            }
            Label {
                text: qsTr("File name:")
                anchors.left: parent.left
                anchors.leftMargin: 6
                platformInverted: app.platformInverted
            }
            TextField {
                id: nameItem
                width: parent.width
                platformInverted: app.platformInverted
            }

            SelectionListItem {
                id: formatItem
                title: qsTr("Output format");
                subTitle: "EPUB"
                platformInverted: app.platformInverted
                onClicked: selectionDialog.open()

                SelectionDialog {
                    id: selectionDialog
                    titleText: qsTr("Select format")
                    platformInverted: formatItem.platformInverted
                    selectedIndex: -1
                    onAccepted: {
                        if (formatsModel.count > 0) {
                            formatItem.subTitle = formatsModel.get(selectedIndex).name;
                        } else {
                            formatItem.subTitle = "";
                        }
                    }
                    model: ListModel {
                        id: formatsModel
                    }
                    Component.onCompleted: {
                        var formats = utility.getSupportedMaker();
                        for (var ix = 0; ix < formats.length; ++ix) {
                            formatsModel.append({"name": formats[ix].toUpperCase()});
                        }
                    }
                }
            }

            // arguments
            SectionHeader {
                id: argumentsHeader
                text: qsTr("Arguments")
            }
            ArgumentsItem {
                id: argumentsItem
            }
        }
    }

    ScrollDecorator {
        flickableItem: flickable
        platformInverted: app.platformInverted
    }

    function setSaveMethod(work, bookName, dir) {
        if (work) {
            root.saved.connect(work);
        }
        nameItem.text = bookName;
        if (dir) {
            dirItem.subTitle = dir;
        }
    }
}
