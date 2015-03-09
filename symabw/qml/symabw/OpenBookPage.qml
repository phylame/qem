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
import pw.symbian.imabw 1.0
import "com"
import "js/main.js" as Script

/// Select and open book file.
Page {
    id: root

    property string file: ""
    signal opened(string file, string format, variant args)

    tools: ToolBarLayout {
        BackToolButton {}

        TipToolButton {
            text: qsTr("Open")
            tipText: qsTr("Open and show book")
            enabled: file.length !== 0
            onClicked: {
                var format = formatItem.subTitle.toLowerCase();
                app.closePage(root);
                root.opened(file, format, argumentsItem.arguments);
            }
        }
        TipToolButton {
            iconSource: "toolbar-add"
            tipText: qsTr("Add one argument")
            onClicked: argumentsItem.add()
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
                text: qsTr("File")
            }
            // book file
            SelectionListItem {
                title: file.length === 0 ? qsTr("Tap to select file") : utility.dirName(file)
                subTitle: file.length === 0 ? "" : utility.fileName(file)
                platformInverted: app.platformInverted
                onClicked: {
                    file = Script.selectBook(qsTr("Select book"), settings.lastDirectory);
                    if (file !== "") {
                        formatItem.subTitle = utility.extensionName(file).toUpperCase();
                        settings.lastDirectory = utility.dirName(file);
                    }
                }
            }
            SelectionListItem {
                id: formatItem
                title: qsTr("Format");
                subTitle: file.length !== 0 ? utility.extensionName(file).toUpperCase() : subTitle
                platformInverted: app.platformInverted
                onClicked: {
                    selectionDialog.open();
                }

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
                        var formats = utility.getSupportedParser();
                        for (var ix = 0; ix < formats.length; ++ix) {
                            formatsModel.append({"name": formats[ix].toUpperCase()});
                        }
                    }
                }
            }
            // arguments
            SectionHeader {
                text: qsTr("Arguments")
            }
            ArgumentsItem {
                id: argumentsItem
//                width: parent.width
            }
        }
    }

    ScrollDecorator {
        flickableItem: flickable
        platformInverted: app.platformInverted
    }

    function setOpenMethod(work, path) {
        if (work) {
            root.opened.connect(work);
        }
        if (path) {
            root.file = path;
        }
    }
}
