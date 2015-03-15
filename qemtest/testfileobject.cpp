/*
 * Copyright 2014 Peng Wan
 *
 * This file is part of Qem test suite.
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

#include "testfileobject.h"
#include <filefactory.h>
#include <QDir>
#include <QFile>
#include <QBuffer>
#include <QDataStream>
#include <quazipfile.h>
#include <quazipnewinfo.h>

QEM_USE_NAMESPACE


TestFileObject::TestFileObject(){
}

#define READ_ALL(file, buf) \
do {\
    QIODevice *dev = file->openDevice();\
    QVERIFY(dev != 0);\
    buf = dev->readAll();\
} while(0)

void TestFileObject::testNormalFile()
{
    QDir curDir;
    QString name("temp.txt");
    QFile file(name);
    QVERIFY(file.open(QFile::WriteOnly));
    const QByteArray data("write some bytes");
    file.write(data);
    file.close();
    FileObject *bf = FileFactory::getFile(name, 0);
    QVERIFY(bf != 0);
    QCOMPARE(bf->name(), name);
    QCOMPARE(bf->mime(), QString("text/plain"));
    QByteArray buf;
    READ_ALL(bf, buf);
    QCOMPARE(buf, data);
    QBuffer buffer;
    QVERIFY(buffer.open(QBuffer::ReadWrite));
    qint64 n = bf->copyTo(buffer);
    QVERIFY(n == data.size());
    QVERIFY(buffer.seek(0));
    buf = buffer.readAll();
    QCOMPARE(buf, data);
    buffer.close();
    delete bf;
    curDir.remove(file.fileName());
}

void TestFileObject::testPartFile()
{
    QDir curDir;
    QString name("temp.txt");
    QFile file(name);
    QVERIFY(file.open(QFile::ReadWrite));
    QByteArray data("write some bytes");
    file.write(data);
    QVERIFY(file.seek(0));
    FileObject *bf = FileFactory::getFile(file.fileName(), &file, 6, 4, 0);
    QVERIFY(bf != 0);
    QByteArray buf;
    READ_ALL(bf, buf);
    qDebug() << buf.size();
    QCOMPARE(buf, QByteArray("some"));
    QBuffer buffer;
    QVERIFY(buffer.open(QBuffer::ReadWrite));
    bf->copyTo(buffer);
    QVERIFY(buffer.seek(0));
    buf = buffer.readAll();
    QCOMPARE(buf, QByteArray("some"));
    buffer.close();
    delete bf;
    file.close();
    curDir.remove(file.fileName());
}

void TestFileObject::testZipFile()
{
    QDir curDir;
    QuaZip zip("tmp.zip");
    QVERIFY(zip.open(QuaZip::mdCreate));
    QuaZipFile file(&zip);
    QuaZipNewInfo zipInfo("A.txt");
    QVERIFY(file.open(QuaZipFile::WriteOnly, zipInfo));
    file.write("Hellow");
    zip.close();
    QVERIFY(zip.open(QuaZip::mdUnzip));
    FileObject *fb = FileFactory::getFile(&zip, "A.txt");
    QVERIFY(fb != 0);
    QByteArray ba;
    READ_ALL(fb, ba);
    QCOMPARE(ba, QByteArray("Hellow"));
    delete fb;
    zip.close();
    curDir.remove("tmp.zip");
}
