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

.pragma library

var imabw;
var utility;
var settings;


/// Select image file.
function selectImage(title, dir) {
    return utility.selectFile(title, dir, qsTr("Images (*.jpg *.jpeg *.png *.bmp);; All files (*.*)"));
}

/// Select book file.
function selectBook(title, dir) {
    var formats = utility.getSupportedParser();
    var filter = qsTr("Books ") + "(*." + formats.join(" *.") + ");;" + qsTr("All files (*.*)");
    return utility.selectFile(title, dir, filter);
}

/// Select text file.
function selectText(title, dir) {
    return utility.selectFile(title, dir, qsTr("Text (*.txt);; All files (*.*)"));
}

/// Open book
function openBook(dir)
{
    var path = selectBook(qsTr("Select book"), dir);
    if (! path) {
        return null;
    } else {
        return utility.readBook(path);
    }
}

/// Returns property readable name
function propertyName(key) {
    if ("author" === key) {
        return qsTr("Author");
    } else if ("intro" === key) {
        return qsTr("Intro");
    } else if ("cover" === key) {
        return qsTr("Cover");
    } else if ("genre" === key) {
        return qsTr("Genre");
    } else if ("state" === key) {
        return qsTr("State");
    } else if ("subject" === key) {
        return qsTr("Subject");
    } else if ("date" === key) {
        return qsTr("Date");
    } else if ("publisher" === key) {
        return qsTr("Publisher");
    } else if ("rights" === key) {
        return qsTr("Rights");
    } else if ("language" === key) {
        return qsTr("Language");
    } else if ("source_path" === key) {
        return qsTr("File");
    } else if ("source_format" == key) {
        return qsTr("Format");
    } else if ("vendor" === key) {
        return qsTr("Vendor");
    } else {
        return key;
    }
}

function formatSize(size) {
    var text;
    if (size > 1024){
        text = size / 1024 / 1024;
        text = Math.round(text * 100) /100;
        text += " Mb";
    } else {
        text = size + " Kb";
    }
    return text;
}

function fileDetails(name) {
    var text = qsTr("Name: ") + utility.baseName(name) + "\n" +
            qsTr("Path: ") + utility.dirName(name) + "\n" +
            qsTr("Format: ") + utility.extensionName(name) + "\n" +
            qsTr("Size: ") + formatSize(utility.fileSize(name)) + "\n" +
            qsTr("Date: ") + Qt.formatDateTime(utility.modifiedDate(name), "yyyy-M-d H:m:s");
    return text;
}
