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
import "js/main.js" as Script

Page {
    id: page;

    property url imageUrl;

    tools: ToolBarLayout {
        BackToolButton {}

        TipToolButton {
            iconSource: "toolbar-add"
            tipText: qsTr("Load image")
            onClicked: {
                var path = Script.selectImage(qsTr("Select image"));
                if (path) {
                    page.imageUrl = path;
                }
            }
        }

        TipToolButton {
            iconSource: "toolbar-delete"
            tipText: qsTr("Remove image")
            onClicked: page.imageUrl = ""
        }

        TipToolButton {
            iconSource: app.getIcon("gfx/save")
            tipText: qsTr("Save image");
            onClicked: {
                var dir = utility.selectFolder(qsTr("Select directory"));
                if (!dir) {
                    return;
                }
                var path = dir + "/" + utility.fileName(imageUrl);
                if (utility.copyFile(imageUrl, path)) {
                    app.showBanner(qsTr("Saved image to %1").arg(path));
                }
            }
        }
    }

    Flickable {
        id: imageFlickable
        anchors.fill: parent
        contentWidth: imageContainer.width; contentHeight: imageContainer.height
        clip: true
        onHeightChanged: if (imagePreview.status === Image.Ready) imagePreview.fitToScreen()

        Item {
            id: imageContainer
            width: Math.max(imagePreview.width * imagePreview.scale, imageFlickable.width)
            height: Math.max(imagePreview.height * imagePreview.scale, imageFlickable.height)

            Image {
                id: imagePreview

                property real prevScale

                function fitToScreen() {
                    scale = Math.min(imageFlickable.width / width, imageFlickable.height / height, 1)
                    pinchArea.minScale = scale
                    prevScale = scale
                }

                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                cache: false
                asynchronous: true
                source: imageUrl
                sourceSize.height: 1000;
                smooth: !imageFlickable.moving

                onStatusChanged: {
                    if (status == Image.Ready) {
                        fitToScreen()
                        loadedAnimation.start()
                    }
                }

                NumberAnimation {
                    id: loadedAnimation
                    target: imagePreview
                    property: "opacity"
                    duration: 250
                    from: 0; to: 1
                    easing.type: Easing.InOutQuad
                }

                onScaleChanged: {
                    if ((width * scale) > imageFlickable.width) {
                        var xoff = (imageFlickable.width / 2 + imageFlickable.contentX) * scale / prevScale;
                        imageFlickable.contentX = xoff - imageFlickable.width / 2
                    }
                    if ((height * scale) > imageFlickable.height) {
                        var yoff = (imageFlickable.height / 2 + imageFlickable.contentY) * scale / prevScale;
                        imageFlickable.contentY = yoff - imageFlickable.height / 2
                    }
                    prevScale = scale
                }
            }
        }

        PinchArea {
            id: pinchArea

            property real minScale: 1.0
            property real maxScale: 3.0

            anchors.fill: parent
            enabled: imagePreview.status === Image.Ready
            pinch.target: imagePreview
            pinch.minimumScale: minScale * 0.5 // This is to create "bounce back effect"
            pinch.maximumScale: maxScale * 1.5 // when over zoomed

            onPinchFinished: {
                imageFlickable.returnToBounds()
                if (imagePreview.scale < pinchArea.minScale) {
                    bounceBackAnimation.to = pinchArea.minScale
                    bounceBackAnimation.start()
                }
                else if (imagePreview.scale > pinchArea.maxScale) {
                    bounceBackAnimation.to = pinchArea.maxScale
                    bounceBackAnimation.start()
                }
            }

            NumberAnimation {
                id: bounceBackAnimation
                target: imagePreview
                duration: 250
                property: "scale"
                from: imagePreview.scale
            }
        }
        MouseArea {
            id: mouseArea;
            anchors.fill: parent;
            enabled: imagePreview.status === Image.Ready;
            onDoubleClicked: {
                if (imagePreview.scale > pinchArea.minScale){
                    bounceBackAnimation.to = pinchArea.minScale
                    bounceBackAnimation.start()
                } else {
                    bounceBackAnimation.to = pinchArea.maxScale
                    bounceBackAnimation.start()
                }
            }
        }
    }

    Loader {
        anchors.centerIn: parent
        sourceComponent: {
            switch (imagePreview.status) {
            case Image.Loading:
                return loadingIndicator
            case Image.Error:
                return failedLoading
            default:
                return undefined
            }
        }

        Component {
            id: loadingIndicator

            Item {
                height: childrenRect.height
                width: page.width

                BusyIndicator {
                    id: imageLoadingIndicator
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: platformStyle.graphicSizeLarge; width: platformStyle.graphicSizeLarge
                    running: true
                    platformInverted: app.platformInverted;
                }

                Label {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        top: imageLoadingIndicator.bottom;
                        topMargin: platformStyle.paddingLarge
                    }
                    platformInverted: app.platformInverted
                    text: qsTr("Loading image...%1").arg(Math.round(imagePreview.progress*100) + "%")
                }
            }
        }

        Component {
            id: failedLoading
            Label {
                text: qsTr("Error loading image")
                platformInverted: app.platformInverted
            }
        }
    }

    ScrollDecorator { platformInverted: app.platformInverted; flickableItem: imageFlickable }

    // For keypad
    onStatusChanged: {
        if (status === PageStatus.Active){
            page.forceActiveFocus();
        }
    }
}
