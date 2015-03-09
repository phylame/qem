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

#include <fileutils.h>
#include <fileobject.h>
#include <QMap>
#include <QString>
#include <QtDebug>
#include <QIODevice>
#include <QDataStream>
#include <QTextStream>
#include <QTextCodec>
#include <quazipfile.h>
#include <quazipnewinfo.h>


QString FileUtils::extensionName(const QString &name)
{
    int index = name.lastIndexOf(".");
    if (index >= 0) {
        return name.right(name.length() - index - 1);
    } else {
        return "";
    }
}

static QMap<QString, QString> _mimeTypeMap;

static void initMimeMap()
{
    _mimeTypeMap.insert("jpe", "image/jpeg");
    _mimeTypeMap.insert("jpg", "image/jpeg");
    _mimeTypeMap.insert("jpeg", "image/jpeg");
    _mimeTypeMap.insert("png", "image/png");
    _mimeTypeMap.insert("gif", "image/gif");
    _mimeTypeMap.insert("bmp", "image/bmp");
    _mimeTypeMap.insert("txt", "text/plain");
    _mimeTypeMap.insert("zip", "application/zip");
    _mimeTypeMap.insert("svg", "image/svg+xml");
    _mimeTypeMap.insert("ico", "image/x-icon");
    _mimeTypeMap.insert("htm", "text/html");
    _mimeTypeMap.insert("html", "text/html");
    _mimeTypeMap.insert("css", "text/css");
    _mimeTypeMap.insert("xml", "text/xml");
    _mimeTypeMap.insert("csv", "text/csv");
    _mimeTypeMap.insert("pmab", "application/pmab+zip");
    _mimeTypeMap.insert("epub", "application/epub+zip");
}

QString FileUtils::mimeType(const QString &name)
{
    QString ext = extensionName(name);
    if (ext.isEmpty()) {
        return "";
    }
    if (_mimeTypeMap.size() == 0) {
        initMimeMap();
    }
    return _mimeTypeMap.value(ext, "");
}

static const qint64 BUFFER_SIZE = 4096;

qint64 FileUtils::copy(QIODevice &in, QIODevice &out, qint64 size)
{
    Q_ASSERT(in.isReadable() && out.isWritable());
    char buf[BUFFER_SIZE];
    qint64 n, total = 0;
    while ((n=in.read(buf, BUFFER_SIZE)) > 0) {
        total += n;
        if (size < 0) {     // copy all data
            out.write(buf, n);
        } else {
            if (total < size) {
                out.write(buf, n);
            } else {
                out.write(buf, n - (total - size));
                total = size;
                break;
            }
        }
    }
    return total;
}

qint64 FileUtils::copy(QIODevice &in, QDataStream &out, qint64 size)
{
    Q_ASSERT(in.isReadable());
    char buf[BUFFER_SIZE];
    qint64 n, total = 0;
    while ((n=in.read(buf, BUFFER_SIZE)) > 0) {
        total += n;
        if (size < 0) {     // copy all data
            out.writeRawData(buf, n);
        } else {
            if (total < size) {
                out.writeRawData(buf, n);
            } else {
                out.writeRawData(buf, n - (total - size));
                total = size;
                break;
            }
        }
    }
    return total;
}

qint64 FileUtils::copy(QDataStream &in, QIODevice &out, qint64 size)
{
    if (in.atEnd() && size > 0) {
        qWarning() << "Input is at end and the required > 0";
        return -1;
    }
    Q_ASSERT(out.isWritable());
    char buf[BUFFER_SIZE];
    qint64 n, total = 0;
    while ((n=in.readRawData(buf, BUFFER_SIZE)) > 0) {
        total += n;
        if (size < 0) {     // copy all data
            out.write(buf, n);
        } else {
            if (total < size) {
                out.write(buf, n);
            } else {
                out.write(buf, n - (total - size));
                total = size;
                break;
            }
        }
    }
    return total;
}

qint64 FileUtils::copy(QDataStream &in, QDataStream &out, qint64 size)
{
    if (in.atEnd() && size > 0) {
        qWarning() << "Input is at end and the required > 0";
        return -1;
    }
    char buf[BUFFER_SIZE];
    qint64 n, total = 0;
    while ((n=in.readRawData(buf, BUFFER_SIZE)) > 0) {
        total += n;
        if (size < 0) {     // copy all data
            out.writeRawData(buf, n);
        } else {
            if (total < size) {
                out.writeRawData(buf, n);
            } else {
                out.writeRawData(buf, n - (total - size));
                total = size;
                break;
            }
        }
    }
    return total;
}

qint64 FileUtils::copy(QTextStream &in, QTextStream &out, qint64 size)
{
    if (in.atEnd() && size > 0) {
        qWarning() << "Input is at end and the required > 0";
        return -1;
    }
    qint64 total = 0;
    QString s;
    while ((s = in.read(BUFFER_SIZE)).length() > 0) {
        total += s.length();
        if (size < 0) {     // copy all data
            out << s;
        } else {
            if (total < size) {
                out << s;
            } else {
                out << s.left(s.length() - (total - size));
                total = size;
                break;
            }
        }
    }
    out.flush();
    return total;
}

QByteArray FileUtils::readZipData(QuaZip &zip, const QString &entryName, const char *password)
{
    if (!zip.setCurrentFile(entryName)) {
        qWarning() << "Not found entry in ZIP:" << entryName;
        return QByteArray();
    }
    QuaZipFile file(&zip);
    if (!file.open(QuaZipFile::ReadOnly, password)) {
        qWarning() << "Cannot open" << entryName << "for reading";
        return QByteArray();
    }
    const QByteArray &data = file.readAll();
    file.close();
    return data;
}

bool FileUtils::writeZipData(QuaZip &zip, const QString &entryName, const QByteArray &data,
                             const char *password)
{
    Q_ASSERT(zip.isOpen());
    QuaZipFile zipFile(&zip);
    if (!zipFile.open(QuaZipFile::WriteOnly, QuaZipNewInfo(entryName), password)) {
        qWarning() << "Cannot open QuaZipFile for writing";
        return false;
    }
    zipFile.write(data);
    zipFile.close();
    return true;
}

bool FileUtils::writeToZip(FileObject &fb, QuaZip &zip, const QString &entryName, const char *password)
{
    QIODevice *dev = fb.openDevice();
    bool ret = writeToZip(*dev, zip, entryName, password);
    fb.reset();
    delete dev;
    return ret;
}

bool FileUtils::writeToZip(QIODevice &device, QuaZip &zip, const QString &entryName, const char *password)
{
    Q_ASSERT(zip.isOpen());
    QuaZipFile zipFile(&zip);
    if (!zipFile.open(QuaZipFile::WriteOnly, QuaZipNewInfo(entryName), password)) {
        qWarning() << "Cannot open QuaZipFile for writing";
        return false;
    }
    copy(device, zipFile);
    zipFile.close();
    return true;
}

QString FileUtils::readZipText(QuaZip &zip, const QString &entryName, const QByteArray &codec,
                               const char *password)
{
    const QByteArray &data = readZipData(zip, entryName, password);
    QTextCodec *tc = QTextCodec::codecForName(codec);
    if (0 == tc) {
        qWarning() << "Not found codec:" << codec;
        return QString();
    }
    return tc->toUnicode(data);
}

bool FileUtils::writeZipText(QuaZip &zip, const QString &entryName, const QString &text,
                             const QByteArray &codec, const char *password)
{
    QTextCodec *tc = QTextCodec::codecForName(codec);
    if (0 == tc) {
        qWarning() << "Not found codec:" << codec;
        return false;
    }
    return writeZipData(zip, entryName, tc->fromUnicode(text), password);
}
