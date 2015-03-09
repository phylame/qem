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
import "js/main.js" as JS

PageStackWindow {
    id: app

    showStatusBar: settings.statusBar
    showToolBar: true
    platformInverted: settings.theme

    Image {
        source: settings.background.length > 0 ? "file:///"+settings.background : ""
        parent: pageStack
        width: screen.width
        height: screen.height
        fillMode: Image.PreserveAspectCrop
        visible: settings.background.length > 0
        asynchronous: true
        opacity: settings.theme ? 0.7 : 0.5
    }

    initialPage: MainPage {

    }

    ToolTip {
        id: toolTip
        visible: false
        platformInverted: app.platformInverted
    }

    /// Get icon by platformInverted.
    function getIcon(icon, ext) {
        if (! ext)
            ext = "svg";
        if (! settings.theme)
            return icon + "." + ext;
        else
            return icon + "_inverted." + ext;
    }

    /// Create a QML object from QML file.
    function createElement(url, parent, properties) {
        var component = Qt.createComponent(url);
        if (component.status === Component.Ready) {
            if (properties) {
                return component.createObject(parent, properties);
            } else {
                return component.createObject(parent);
            }
        } else {
            return null;
        }
    }

    /// Push a page to page stack
    function openPage(url, properties) {
        var page = createElement(url, app, properties);
        if (page !== null) {
            pageStack.push(page);
        }
        return page;
    }

    /// Pop page from page stack
    function closePage(page) {
        pageStack.pop(page);
        if (page) {
            page.destroy();
        }
    }

    /// Display tool tip for \a target with \a text.
    function showTip(target, text) {
        toolTip.target = target
        toolTip.text = text
        toolTip.show()
    }

    /// Hide shown tool tip.
    function hideTip() {
        toolTip.hide();
    }

    InfoBanner {
        id: banner
        timeout: 1000
        platformInverted: app.platformInverted
    }

    /// Display a banner with \a text.
    function showBanner(text) {
        banner.text = text;
        banner.open();
    }

    BusyIndicator {
        id: indicator
        visible: false
        platformInverted: app.platformInverted
        width: 100
        height: 100
        anchors.verticalCenterOffset: -20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

    /// Show and run indicator.
    function showIndicator() {
        indicator.running = true;
        indicator.visible = true;
    }

    /// Stop and hide indicator.
    function hideIndicator() {
        indicator.visible = false;
        indicator.running = false;
    }

    property QtObject messageDialog: null;
    /// Show message in dialog.
    function showMessage(title, text) {
        if (! app.messageDialog) {
            app.messageDialog = createElement("com/MessageDialog.qml", app);
        }
        app.messageDialog.showMessage(title, text);
    }

    /// Show query message dialog.
    function showQuery(title, text, accept, reject) {
        var dialog = createElement("com/MyQueryDialog.qml", app);
        dialog.showQuery(title, text, accept, reject);
    }

    /// Show input dialog
    function inputText(title, text, accept, reject) {
        var dialog = createElement("com/InputDialog.qml", app);
        dialog.inputText(title, text, accept, reject);
    }

    Component.onCompleted: {
        JS.imabw = imabw;
        JS.utility = utility;
        JS.settings = settings;
    }
}
