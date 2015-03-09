/*
 * Copyright 2014 Peng Wan
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

#include <formats/jar.h>
#include <formats/utils.h>
#include <filefactory.h>
#include <QtDebug>
#include <QTextCodec>
#include <QDataStream>
#include <quazipfile.h>


const QString JAR::FORMAT_NAME("jar");
const QByteArray JAR::HEAD_ENCODING("UTF-8");
const QByteArray JAR::TEXT_ENCODING("UTF-16LE");

static void deleteQuaZip(Part &part, void *arg)
{
    QuaZip *zip = static_cast<QuaZip*>(arg);
    delete zip;
}

Book* JAR::parseJar(QIODevice &device, const QVariantMap &args, QString *error)
{
    Q_ASSERT(device.isReadable());
    QuaZip *zip = new QuaZip(&device);
    if (! zip->open(QuaZip::mdUnzip)) {
        FormatsUtility::debug("Cannot open JAR archive", error);
        delete zip;
        return 0;
    }
    Book *book = parseJar(*zip, args, error);
    if (0 == book) {
        delete zip;
    } else {
        book->registerCleaner(deleteQuaZip, zip);
    }
    return book;
}

static bool readNavi(QuaZip &zip, Book &book, QString *error);

Book* JAR::parseJar(QuaZip &zip, const QVariantMap &args, QString *error)
{
    Book *book = new Book;
    if (! readNavi(zip, *book, error)) {
        delete book;
        return 0;
    } else {
        return book;
    }
}

static bool readNavi(QuaZip &zip, Book &book, QString *error)
{
    if (! zip.setCurrentFile("0")) {
        FormatsUtility::debug("Not found file in JAR: 0", error);
        return false;
    }
    QuaZipFile file(&zip);
    if (! file.open(QuaZipFile::ReadOnly)) {
        FormatsUtility::debug("Cannot open device with read-only", error);
        return false;
    }
    QDataStream in(&file);
    in.setByteOrder(QDataStream::BigEndian);
    quint32 n32;
    in >> n32;
    if (JAR::FILE_HEADER != n32) {
        FormatsUtility::debug("Unsupported JAR book: magic number", error);
        return false;
    }
    quint8 n8;
    in >> n8;
    QTextCodec *headCodec = QTextCodec::codecForName(JAR::HEAD_ENCODING);
    char *buf = new char[n8];
    if (in.readRawData(buf, n8) != n8) {
        FormatsUtility::debug("Bad JAR book file: title", error);
        delete []buf;
        return false;
    }
    book.setTitle(headCodec->toUnicode(buf, n8));
    delete []buf;
    quint16 n16;
    in >> n16;
    buf = new char[n16];
    if (in.readRawData(buf, n16) != n16) {
        FormatsUtility::debug("Bad JAR book file: chapter count", error);
        delete []buf;
        return false;
    }
    bool ok;
    int chapterCount = headCodec->toUnicode(buf, n16).toInt(&ok);
    if (! ok) {
        FormatsUtility::debug("Bad JAR book file: chapter title", error);
        delete []buf;
        return false;
    }
    delete []buf;
    for (int i=0; i<chapterCount; ++i) {
        in >> n16;
        buf = new char[n16];
        if (in.readRawData(buf, n16) != n16) {
            FormatsUtility::debug("Bad JAR book file: chapter entry", error);
            delete []buf;
            return false;
        }
        const QString &s = headCodec->toUnicode(buf, n16);
        delete []buf;
        QStringList items = s.split(",");
        if (items.size() < 3) {
            FormatsUtility::debug("Bad JAR book file: chapter item", error);
            return false;
        }
        Chapter *chapter = book.newChapter(items[2]);
        FileObject *file = FileFactory::getFileObject(&zip, items[0], "", chapter);
        if (0 == file) {
            qWarning() << "Bad JAR book file: not found chapter:" << items[0];
            continue;
        }
        chapter->setFile(file, JAR::TEXT_ENCODING);
    }
    return true;
}

bool JAR::makeJar(const Book &book, QIODevice &device, const QVariantMap &args, QString *error)
{
    Q_ASSERT(device.isWritable());
    FormatsUtility::debug("Under development", error);
    return false;
}
