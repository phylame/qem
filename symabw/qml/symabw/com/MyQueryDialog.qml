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

QueryDialog {
    id: root

    acceptButtonText: qsTr("Ok")
    rejectButtonText: qsTr("Cancel")
    platformInverted: app.platformInverted
    privateCloseIcon: true

    function showQuery(title, text, accept, reject) {
        root.titleText = title;
        root.message = text.concat("\n");
        if (accept) {
            var work = function() {
                accept();
                root.destroy();
            };
            root.accepted.connect(work);
        }
        if (reject) {
            work = function() {
                reject();
                root.destroy();
            }
            root.rejected.connect(work);
        }
        root.open();
    }
}
