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

    buttonTexts: [qsTr("Ok")]
    platformInverted: app.platformInverted
    privateCloseIcon: true

    content: Label {
        id: label
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 0
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        platformInverted: root.platformInverted
    }

    function showMessage(title, text) {
        root.titleText = title;
        label.text = text;
        root.open();
    }
}
