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

    buttonTexts: [qsTr("Ok"), qsTr("Cancel")]
    platformInverted: app.platformInverted
    privateCloseIcon: true

    signal entered(string text)
    onAccepted: root.entered(textField.text)

    onButtonClicked: {
        if (index === 0) {
            accept();
        } else if (index === 1) {
            reject();
        }
        root.destroy();
    }

    content: Column {
        width: parent.width
        height: textField.height + btnLongEdit.height
        TextField {
            id: textField
            platformInverted: root.platformInverted
            width: parent.width
        }
        Button {
            id: btnLongEdit
            text: qsTr("Long text edit...")
            platformInverted: root.platformInverted
            onClicked: {
                root.close();
                var page = openPage("EditTextPage.qml");
                page.showText(textField.text, root.entered);
            }
        }
    }

    function inputText(title, text, accept, reject) {
        root.titleText = title;
        textField.text = text;
        if (accept) {
            root.entered.connect(accept);
        }
        if (reject) {
            root.rejected.connect(reject);
        }
        root.open();
    }
}
