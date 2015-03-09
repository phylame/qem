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

Item {
    id: root

    implicitWidth: itemGroup.implicitWidth
    implicitHeight: itemGroup.implicitHeight
    property variant arguments

//    ListView {
//        id: listView
//        clip: true
//        anchors.fill: parent
//        model: listModel
//        delegate: listDelegate
//    }
    ItemGroup {
        id: itemGroup
        anchors.top: root.top
        TextField {
            text: "Hello"
        }
    }

    ListModel {
        id: listModel
    }

    Component {
        id: listDelegate
        Row {
            width: parent.width
//            anchors.fill: listItem.padding
            TextField {
                platformInverted: app.platformInverted
                text: name
            }
            Label {
                text: ":"
                platformInverted: app.platformInverted
            }
            TextField {
                platformInverted: app.platformInverted
                text: value
            }
        }
    }

//    SectionScroller {
//        listView: listView
//        platformInverted: app.platformInverted
//    }
    function add() {
        listModel.append({"name": "", "value": ""});
    }

    function remove(index) {
        listModel.remove(index);
    }
}
