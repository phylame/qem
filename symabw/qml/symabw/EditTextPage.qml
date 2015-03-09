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
import "com"
import "js/main.js" as Scritp

Page {
    id: page

    signal saved(string text)
    property bool changed: false

    tools: ToolBarLayout {
        TipToolButton {
            iconSource: "toolbar-back"
            tipText: qsTr("Back")
            onClicked: {
                if (!page.changed) {
                    closePage(page);
                } else {
                    var askDialog = app.createElement("com/AskSavingDialog.qml", page);
                    var save = function() {
                        page.saved(textArea.text);
                        closePage(page);
                    };
                    var discard = function() {
                        closePage(page);
                    };
                    askDialog.showAsking(qsTr("Back"), qsTr("Save text modifications to chapter?"),
                                         save, discard);
                }
            }
        }
        TipToolButton {
            text: qsTr("Save")
            tipText: qsTr("Save modifications to chapter")
            onClicked: {
                page.saved(textArea.text);
                page.changed = false;
            }
        }
        TipToolButton {
            iconSource: "toolbar-menu"
            tipText: qsTr("Menu")
            onClicked: mainMenu.open()
        }
    }

    Menu {
        id: mainMenu
        platformInverted: app.platformInverted
        MenuLayout {
            MenuItem {
                text: qsTr("Load file")
                platformInverted: mainMenu.platformInverted
                onClicked: internal.loadFile()
            }
            MenuItem {
                text: qsTr("Clear")
                platformInverted: mainMenu.platformInverted
                onClicked: textArea.text = ""
            }
        }
    }

    TextArea {
        id: textArea
        platformInverted: app.platformInverted
        anchors {
            top: parent.top; bottom: splitViewInput.top
            left: parent.left; right: parent.right;
        }
        placeholderText: qsTr("Tap to write")
        Behavior on height { PropertyAnimation { duration: 200 } }
        onTextChanged: page.changed = true
    }

    Item {
        id: splitViewInput
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
        Behavior on height { PropertyAnimation { duration: 200 } }
        states: [
            State {
                name: "Visible"; when: inputContext.visible
                PropertyChanges { target: splitViewInput; height: inputContext.height - tools.height }
            },
            State {
                name: "Hidden"; when: !inputContext.visible
                PropertyChanges { target: splitViewInput; height: 0 }
            }
        ]
    }

    function showText(text, saved) {
        textArea.text = text;
        page.changed = false;
        if (saved) {
            page.saved.connect(saved);
        }
    }

    QtObject {
        id: internal

        /// Load text file.
        function loadFile() {
            var path = Scritp.selectText(qsTr("Select text file"));
            if (path) {
                textArea.text = utility.loadText(path);
            }
        }
    }
}
