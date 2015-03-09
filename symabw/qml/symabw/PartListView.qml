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

Item {
    id: root
    anchors.fill: parent

    signal titleClicked
    signal clicked
    signal pressAndHold

    property IPart owner: null

    property bool modified: false

    property int increment: 20

    property int topIndex: 0
    // 1 to move up, 2 to move bottom
    property int moveDirection: 0

    SectionHeader {
        id: header
        text: {
            if (! root.owner) {
                return "";
            }
            var s = root.modified ? "*" + root.owner.title : root.owner.title;
            return s + qsTr(", ") + root.owner.size;
        }

        textAlignment: Text.AlignLeft
        anchors.top: parent.top
        anchors.topMargin: 0

        MouseArea {
            anchors.fill: parent
            onClicked: root.titleClicked()
        }
    }

    ListView {
        id: listView
        anchors.fill: parent
        anchors.topMargin: header.height
        focus: true
        clip: true
        interactive: listModel.count != 0
        model: listModel
        delegate: listDelegate

        onContentYChanged: {
            if (listView.atYBeginning) {
                root.moveDirection = 1
            } else if (listView.atYEnd) {
                root.moveDirection = 2
            }
        }
        onMovementStarted: {
            root.moveDirection = 0
        }
        onMovementEnded: {
            listView.currentIndex = -1;
            if (1 === root.moveDirection) {
                internal.loadMoreTop();
            } else if (2 === root.moveDirection) {
                internal.loadMoveBottom();
            }
        }
    }

    Component {
        id: listDelegate
        ListItem {
            id: listItem
            subItemIndicator: section
            platformInverted: app.platformInverted
            Column {
                anchors.fill: listItem.padding
                ListItemText {
                    platformInverted: app.platformInverted
                    mode: listItem.mode
                    role: "Title"
                    text: title
                }
                ListItemText {
                    platformInverted: app.platformInverted
                    mode: listItem.mode
                    role: "Subtitle"
                    elide: Text.ElideLeft
                    maximumLineCount: 1
                    text: intro
                }
            }
            onClicked: root.clicked()
            onPressAndHold: root.pressAndHold()
        }
    }

    ListModel {
        id: listModel
    }

    ScrollDecorator {
        flickableItem: listView
        platformInverted: app.platformInverted
    }

    /// Returns selected index.
    function getSelectedIndex() {
        if (listView.currentIndex < 0) {
            return listView.currentIndex;
        } else {
            return listView.currentIndex + topIndex;
        }
    }

    /// Returns selected part.
    function getSelectedPart() {
        if (listView.currentIndex < 0) {
            return null;
        } else {
            return owner.get(topIndex+listView.currentIndex);
        }
    }

    function setOwner(part) {
        listModel.clear();
        topIndex = 0;
        root.owner = part;
        if (part) {
            internal.generateList(root.owner, 0, Math.min(settings.maxShownParts, part.size));
        }
    }

    /// Convert \a part to ListElement.
    function toItem(part) {
        return {"title": part.title, "intro": part.introContent(), "section": part.isSection()};
    }

    /// Append one \a part.
    /** The \a part should be added to owner first. */
    function appendPart(part) {
        listModel.append(toItem(part));
        if (listModel.count > settings.maxShownParts) {
            internal.removeBottom(listModel.count-settings.maxShownParts);
        }
    }

    /// Insert \a part before \a index.
    /** The \a part should be added to owner first. */
    function insertPart(index, part) {
        listModel.insert(index-topIndex, toItem(part));
        if (listModel.count > settings.maxShownParts) {
            internal.removeBottom(listModel.count-settings.maxShownParts);
        }
    }

    /// Show \a part at \a index.
    /** The \a part should be added to owner first. */
    function setPart(index, part) {
        listModel.set(index-topIndex, toItem(part));
    }

    /// Remove one shown part at \a index.
    function removePart(index) {
        listModel.remove(index-topIndex);
        if (listModel.count < settings.maxShownParts) {   // increase
            var bottomIndex = topIndex + Math.min(settings.maxShownParts-1, listModel.count-1);
            internal.increaseBottom(Math.min(settings.maxShownParts-listModel.count,
                                             owner.size-1-bottomIndex));
        }
    }

    /// Move \a count shown sub-parts from \a orig to \a dest.
    function movePart(orig, dest, count) {
        listModel.move(orig, dest, count);
    }

    /// Clear all shown sub-parts.
    function clearParts() {
        listModel.clear();
    }

    QtObject {
        id: internal

        function generateList(part, begin, count) {
            for (var i = 0; i < count; ++i) {
                listModel.append(toItem(part.get(begin+i)));
            }
        }

        function refreshList(maxShownParts) {
            generateList(root.owner, topIndex, Math.min(maxShownParts, root.owner.size-topIndex-1));
        }

        function loadMoreTop() {
            if (0 === root.topIndex) {   // no more in top
                return;
            }
            var inc = Math.min(root.increment, root.topIndex);
            increaseTop(inc);
            removeBottom(inc);
        }

        function loadMoveBottom() {
            var bottomIndex = root.topIndex + Math.min(settings.maxShownParts-1, listModel.count-1);
            if (root.owner.size - 1 === bottomIndex) {   // no more in bottom
                return;
            }
            var inc = Math.min(root.increment, root.owner.size-bottomIndex-1);
            increaseBottom(inc);
            removeTop(inc);
        }

        /// Remove \a count items at begin.
        function removeTop(count) {
            for (var i = 0; i < count; ++i) {
                listModel.remove(0);
            }
            root.topIndex += count;
        }

        /// Remove \a count items at end.
        function removeBottom(count) {
            for (var i = 0; i < count; ++i) {
                listModel.remove(listModel.count - 1);
            }
        }

        /// Increase \a count items to begin.
        function increaseTop(count) {
            for (var i = 1; i <= count; ++i) {
                listModel.insert(0, toItem(owner.get(topIndex-i)));
            }
            root.topIndex -= count;
        }

        /// Increase \a count items to end.
        function increaseBottom(count) {
            var bottomIndex = root.topIndex + Math.min(settings.maxShownParts-1, listModel.count-1);
            for (var i = 1; i <= count; ++i) {
                listModel.append(toItem(owner.get(bottomIndex+i)));
            }
        }
    }

    Component.onCompleted: {
        settings.maxShownPartsChanged.connect(internal.refreshList);
    }
}
