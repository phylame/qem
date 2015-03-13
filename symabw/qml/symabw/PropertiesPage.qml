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
import "js/main.js" as Script

Page {
    id: page

    property IPart part
    signal changed

    tools: ToolBarLayout {
        BackToolButton {}

        TipToolButton {
            iconSource: "toolbar-add"
            tipText: qsTr("Add one attribute")
            onClicked: internal.addAttribute()
        }
    }

    Rectangle {
        id: titlePane
        width: parent.width
        height: tools.height
        color: "#e88821"

        Text {
            color: "#ffffff"
            text: part ? part.title : ""
            font.pixelSize: titlePane.height * 0.5
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    ListView {
        id: listView
        clip: true
        anchors.fill: parent
        anchors.topMargin: titlePane.height
        model: propertiesModel
        delegate: listDelegate
    }

    ListModel {
        id: propertiesModel
    }

    Component {
        id: listDelegate
        ListItem {
            id: listItem
            platformInverted: app.platformInverted
            Column {
                anchors.fill: listItem.padding
                ListItemText {
                    platformInverted: app.platformInverted
                    mode: listItem.mode
                    font.capitalization: Font.Capitalize
                    role: "Title"
                    text: name
                }
                ListItemText {
                    platformInverted: app.platformInverted
                    mode: listItem.mode
                    role: "Subtitle"
                    elide: Text.ElideLeft
                    maximumLineCount: 1
                    text: value
                }
            }
            onClicked: internal.editAttribute(listView.currentIndex)
            onPressAndHold: propMenu.open()
        }
    }

    ContextMenu {
        id: propMenu
        platformInverted: app.platformInverted
        MenuLayout {
            MenuItem {
                text: qsTr("Remove")
                platformInverted: propMenu.platformInverted
                onClicked: internal.removeAttribute(listView.currentIndex)
            }
        }
    }

    ScrollDecorator {
        flickableItem: listView
        platformInverted: app.platformInverted
    }

    function setPart(part, changed) {
        if (changed) {
            page.changed.connect(changed);
        }
        page.part = part;
        var names = part.names();
        for (var ix = 0; ix < names.length; ++ix) {
            var name = names[ix];
            if ("title" === name || name.match("source_.*")) {     // ignore title
                continue;
            }
            var value = utility.formatVariant(part.attribute(name));
            if (value) {
                propertiesModel.append({"name": Script.propertyName(name), "value": value, "key": name});
            }
        }
    }

    QtObject {
        id: internal

        function addAttribute() {
            var work = function(text) {
                if (text) {
                    page.part.setAttribute(text, "");
                    propertiesModel.append({"name": Script.propertyName(text), "value": "", "key": text});
                }
            };
            app.inputText(qsTr("Attribute key"), "", work);
        }

        function editAttribute(index) {
            var key = propertiesModel.get(index).key;
            if (! key) {
                return;
            }
            var value = page.part.attribute(key);
            if ("cover" === key) {
                var path = utility.cacheFileObject(value);
                if (path) {
                    app.openPage("ImageViewer.qml", {"imageUrl": path});
                } else {
                    app.showBanner(qsTr("Cannot cache cover file"));
                }
            } else if ("intro" === key) {
                var p = app.openPage("EditTextPage.qml");
                if (!p) {
                    console.log("cannot open EditTextPage.qml");
                } else {
                    var work = function(text) {
                        page.part.setIntroText(text);
                        page.changed();
                    };
                    p.showText(utility.formatVariant(value), work);
                }
            } else if ("source_path" === key || "source_format" === key) {
                // cannot modified
            } else {
                var type = utility.variantType(value);
                if ("QString" === type) {
                    var old = utility.formatVariant(value);
                    var work = function(text) {
                        if (text !== old) {
                            page.part.setAttribute(key, text);
                            propertiesModel.get(index).value = text;
                            page.changed();
                        }
                    };
                    app.inputText(qsTr("New value"), utility.formatVariant(value), work);
                } else if (type.indexOf("QDate") === 0) {
                    datePicker.selectDate(qsTr("Select date"), value);
                }
                else {
                    console.debug("type: "+type)
                }
            }
        }

        function removeAttribute(index) {
            var key = propertiesModel.get(index).key;
            if (key) {
                page.part.setAttribute(key, null);
                propertiesModel.remove(index);
                page.changed();
            }
        }
    }
    DatePickerDialog {
        id: datePicker
        platformInverted: app.platformInverted
        acceptButtonText: qsTr("Select")
        rejectButtonText: qsTr("Cancel")

        function selectDate(title, date, accept) {
            datePicker.titleText = title;
            var d = utility.formatVariant(date);
//            datePicker.year = date.year;
//            datePicker.month = date.month;
//            datePicker.day = date.day;
            if (accept) {
                datePicker.accepted.connect(accept);
            }
            datePicker.open();
        }
    }
}
