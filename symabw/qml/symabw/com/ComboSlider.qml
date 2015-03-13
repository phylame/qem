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


ListItem {
    id: root

    property string titlePrefix: ""

    property alias minimumValue: slider.minimumValue
    property alias maximumValue: slider.maximumValue
    property alias value: slider.value
    property alias stepSize: slider.stepSize
    property alias pressed: slider.pressed
    property alias valueIndicatorVisible: slider.valueIndicatorVisible

    platformInverted: settings.theme

    Column {
        anchors.fill: root
        Label {
            text: titlePrefix + slider.value
            anchors.left: parent.left
            anchors.leftMargin: 6
            platformInverted: root.platformInverted
        }

        Row {
            anchors {
                left: parent.left
                leftMargin: 6
                right: parent.right
                rightMargin: 6
            }

            spacing: 2
            Label {
                id: lbMin
                text: slider.minimumValue
                anchors.verticalCenter: parent.verticalCenter
                platformInverted: root.platformInverted
            }

            Slider {
                id: slider
                platformInverted: root.platformInverted
                width: parent.width - lbMin.width - lbMax.width
                anchors.margins: -1
            }

            Label {
                id: lbMax
                text: slider.maximumValue
                anchors.verticalCenter: parent.verticalCenter
                platformInverted: root.platformInverted
            }
        }
    }
}
