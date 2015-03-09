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

Page {
    id: root

    property string file: ""
    property string format: ""
    property variant args: null

    property bool modified: false

    tools: ToolBarLayout {
        TipToolButton {
            iconSource: isTop() ? getIcon("gfx/close") : "toolbar-back"
            tipText: isTop() ? qsTr("Exit") : qsTr("Back")
            onClicked: isTop() ? maybeSave(qsTr("Exit Imabw"), onExit) : onBack()
        }
        TipToolButton {
            iconSource: "toolbar-add"
            tipText: qsTr("Add chapter")
            enabled: hasBook()
            onClicked: addPart(parts.top(), false, -1)
        }
        TipToolButton {
            iconSource: getIcon("gfx/extension")
            tipText: qsTr("View properties")
            enabled: hasBook()
            onClicked: viewProperties(parts.top())
        }
        TipToolButton {
            iconSource: "toolbar-home"
            tipText: qsTr("Go to top chapter")
            enabled: ! isTop()
            onClicked: onHome()
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
                text: qsTr("File")
                platformInverted: mainMenu.platformInverted
                platformSubItemIndicator: true
                onClicked: menuFile.open()
            }

            MenuItem {
                text: qsTr("Book")
                platformInverted: mainMenu.platformInverted
                platformSubItemIndicator: true
                onClicked: menuBook.open()
            }

            MenuItem {
                text: qsTr("Settings")
                platformInverted: mainMenu.platformInverted
                onClicked: app.openPage("SettingsPage.qml")
            }

            MenuItem {
                text: qsTr("About")
                platformInverted: mainMenu.platformInverted
                onClicked: app.openPage("AboutPage.qml")
            }

            MenuItem {
                text: qsTr("Exit")
                platformInverted: mainMenu.platformInverted
                onClicked: maybeSave(qsTr("Exit Imabw"), onExit)
            }
        }
    }

    ContextMenu {
        id: menuFile
        platformInverted: app.platformInverted
        MenuLayout {
            MenuItem {
                text: qsTr("New")
                platformInverted: menuFile.platformInverted
                onClicked: maybeSave(qsTr("New file"), newBook)
            }
            MenuItem {
                text: qsTr("Open")
                platformInverted: menuFile.platformInverted
                onClicked: maybeSave(qsTr("Open file"), openBook)
            }
            MenuItem {
                text: qsTr("Save")
                platformInverted: menuFile.platformInverted
                enabled: hasBook()
                onClicked: saveBook()
            }
            MenuItem {
                text: qsTr("Save as")
                platformInverted: menuFile.platformInverted
                enabled: hasBook()
                onClicked: savePart(parts.bottom())
            }
            MenuItem {
                text: qsTr("Close")
                platformInverted: menuFile.platformInverted
                enabled: hasFile()
                onClicked: maybeSave(qsTr("Close file"), closeBook)
            }
            MenuItem {
                text: qsTr("Details")
                platformInverted: menuFile.platformInverted
                enabled: hasFile()
                onClicked: showDetails()
            }
        }
    }

    ContextMenu {
        id: menuBook
        platformInverted: app.platformInverted
        MenuLayout {
            MenuItem {
                text: qsTr("Extension")
                platformInverted: menuBook.platformInverted
            }
        }
    }

    PartListView {
        id: partView

        onTitleClicked: {
            if (parts.size > 0) {
                renamePart(parts.top(), -1)
            }
        }
        onClicked: {
            var part = partView.getSelectedPart();
            if (part.isSection()) {
                showPart(part);
            } else {
                editContent(part);
            }
        }
        onPressAndHold: partMenu.open()
    }

    Binding {
        target: partView
        property: "modified"
        value: modified
    }

    ContextMenu {
        id: partMenu
        platformInverted: app.platformInverted
        MenuLayout {
            MenuItem {
                text: qsTr("New")
                platformInverted: partMenu.platformInverted
                onClicked: addPart(partView.getSelectedPart(), settings.enterAfterAdd, partView.getSelectedIndex())
            }
            MenuItem {
                text: qsTr("Insert")
                platformInverted: partMenu.platformInverted
                onClicked: insertPart(parts.top(), partView.getSelectedIndex())
            }
            MenuItem {
                text: qsTr("Rename")
                platformInverted: partMenu.platformInverted
                onClicked: renamePart(partView.getSelectedPart(), partView.getSelectedIndex())
            }
//            MenuItem {
//                text: qsTr("Move")
//                platformInverted: partMenu.platformInverted
//                onClicked: movePart(parts.top(), partView.getSelectedIndex())
//            }
            MenuItem {
                text: qsTr("Save as")
                platformInverted: partMenu.platformInverted
                onClicked: savePart(partView.getSelectedPart())
            }
            MenuItem {
                text: qsTr("Delete")
                platformInverted: partMenu.platformInverted
                onClicked: deletePart(parts.top(), partView.getSelectedIndex())
            }
            MenuItem {
                text: qsTr("Properties")
                platformInverted: partMenu.platformInverted
                onClicked: viewProperties(partView.getSelectedPart())
            }
        }
    }

    PropertiesPage {

    }

    /// Returns \c true if has book.
    function hasBook() {
        // index 0 is book
        return parts.size > 0;
    }

    /// Returns \c true if opened book file.
    function hasFile() {
        return file !== "";
    }

    /// The top is book.
    function isTop() {
        return parts.size < 2;
    }

    function onExit() {
        Qt.quit();
    }

    /// resetBook then show \a book.
    function showBook(book) {
        resetBook();
        showPart(book);
    }

    /// Reset properties to default.
    function resetBook() {
        file = "";
        format = "";
        args = null;
        modified = false;

        var old = null;
        if (parts.size > 0) {
            old = parts.bottom();
        }
        parts.clear();
        if (old) {
            old.destroy();
        }
    }

    /// Create new book and show.
    function newBook() {
        showBook(utility.newBook(qsTr("Untitled"), "", root));
    }

    /// Open book file and show.
    function openBook() {
        var work = function(file, format, args) {
//            app.showIndicator();
            var book = utility.readBook(file, format, args);
            if (book) {
                showBook(book);
                root.file = file;
                root.format = format;
                root.args = args;
            } else {
                app.showBanner(qsTr("Open book failed: ")+file);
                console.debug("error: "+utility.errorString());
            }
//            app.hideIndicator();
        };
        var path = Script.selectBook(qsTr("Select book"), settings.lastDirectory);
        if (path) {
            settings.lastDirectory = utility.dirName(path);
        } else {
            return;
        }
        var page = app.openPage("OpenBookPage.qml");
        page.setOpenMethod(work, path);
    }

    function maybeSave(title, work) {
        if (! modified) {
            work();
        } else {
            var save = function() {
                saveBook();
                work();
            };
            var askDialog = app.createElement("com/AskSavingDialog.qml", root);
            askDialog.showAsking(title, qsTr("Save book modifications to file?"), save, work);
        }
    }

    /// Save book modifications to file.
    function saveBook() {
        savePart(parts.bottom(), file, format, args);
    }

    /// Close current book.
    function closeBook() {
        showBook(null);
    }

    /// Returns \c true if success.
    function savePart(part, file, format, args) {
        var work = function(path, format, args) {
            var save = function() {
                var dest, ow = false;
                if (utility.sameFile(root.file, path)) {
                    dest = path + ".tmp";
                    ow = true;
                } else {
                    dest = path;
                }
                if (utility.writeBook(part, dest, format, args)) {
                    if (part === parts.bottom()) {  // is book
                        root.file = path;
                        root.format = format;
                        root.args = args;
                        root.modified = false;
                    }
                    app.showBanner(qsTr("Saved to \"%1\"").arg(path));
                } else {
                    app.showBanner(qsTr("Failed to save \"%1\"").arg(part.title));
                    console.debug("Trace error: "+utility.errorString());
                }
            };
            if (utility.fileExists(path)) {
                app.showQuery(qsTr("Save %1").arg(part.title),
                              qsTr("File %1 exists, overwrite it?").arg(path), save);
            } else {
                save();
            }
        };
        if (file) {
            work(file, format, args);
        } else {
            var path = utility.selectFolder(qsTr("Select directory"), settings.lastDirectory);
            if (path) {
                settings.lastDirectory = path;
            } else {
                return;
            }
            var page = app.openPage("SaveBookPage.qml");
            page.setSaveMethod(work, part.title, path);
        }
    }

    /// Show book file details.
    function showDetails() {
        app.showMessage(qsTr("File details"), Script.fileDetails(file));
    }

    /// Push part to part stack if not equals \c 0 and show it.
    function showPart(part) {
        if (part) {
            parts.push(part);
        }
        partView.setOwner(part);
    }

    /// Create new part with \a title and \a owner.
    /** \param parent QObject parent for newed part. */
    function newPart(title, owner, parent) {
        if (title.length === 0) {
            app.showBanner(qsTr("Empty chapter title"));
            return null;
        }
        if (owner) {
            return owner.newChapter(title);
        } else {
            return utility.newChapter(title, "", parent);
        }
    }

    /// Add a chapter to \c owner.
    /**
     * \param enter If \c true enter to sub list view.
     * \param index The index of \a owner in list view.
     */
    function addPart(owner, enter, index) {
        var work = function() {
            var create = function(title) {
                var part = newPart(title, owner);
                modified = true;
                if (owner === parts.top()) {     // owner of PartListView
                    partView.appendPart(part);
                } else {    // add from ContextMenu
                    if (enter) {
                        showPart(owner);
                    } else if (index >= 0) {    // refresh owner state
                        partView.setPart(index, owner);
                    }
                }
            };
            app.inputText(qsTr("Chapter title"), qsTr("Chapter"), create);
        };
        if (! owner.isSection() && owner !== parts.bottom()) {
            app.showQuery(qsTr("New sub-chapter"),
                          qsTr("Text of \"%1\" will be ignored, continue?").arg(owner.title), work);
        } else {
            work();
        }
    }

    /// Create new part and insert to \a owner before \a index.
    function insertPart(owner, index) {
        var work = function(title) {
            var part = newPart(title, null, owner);
            owner.put(index, part);
            partView.insertPart(index, part);
            modified = true;
        };
        app.inputText("Chapter title", qsTr("Chapter"), work);
    }

    /// Rename \a part title.
    /**
     * \param index If > 0 refresh \a part in PartListView otherwise refresh title pane of PartListView.
     */
    function renamePart(part, index) {
        var work = function(title) {
            if (title !== old) {
                part.title = title;
                if (index >= 0) {
                    partView.setPart(index, part);
                }
                modified = true;
            }
        };
        var old = part.title;
        app.inputText(qsTr("Rename \"%1\"").arg(old), old, work);
    }

    function movePart(owner, index) {

    }

    /// Remove one part from \a owner at \a index then refresh PartListView.
    function deletePart(owner, index) {
        var work = function() {
            owner.remove(index);
            partView.removePart(index);
            modified = true;
        };
        var part = owner.get(index);
        if (part.isSection()) {
            app.showQuery(qsTr("Delete \"%1\"").arg(owner.get(index).title),
                qsTr("All sub-parts will be deleted, continue?"), work);
        } else {
            work();
        }
    }

    /// Edit part text content.
    function editContent(part) {
        var save = function(text) {
            if (utility.saveText(part, text)) {
                modified = true;
            } else {
                console.log("Failed to save text: "+utility.errorString());
            }
        };
        var page = openPage("EditTextPage.qml");
        page.showText(part.content, save);
    }

    /// View and edit part's properties.
    function viewProperties(part) {
        var notify = function() {
            modified = true;
        };
        var page = app.openPage("PropertiesPage.qml");
        page.setPart(part, notify);
    }

    /// Back to top part and set to PartListView.
    function onBack() {
        if (! isTop()) {
            var part = parts.pop();
            partView.setOwner(parts.top());
        }
    }

    /// Set book to PartListView.
    function onHome() {
        if (! isTop()) {
            var book = parts.bottom();
            parts.clear();
            showPart(book);
        }
    }

    Component.onCompleted: {
        newBook();
    }
}
