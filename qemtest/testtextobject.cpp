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

#include <QDir>
#include <QFile>
#include <QBuffer>
#include <QTextStream>
#include "textobject.h"
#include "filefactory.h"
#include "testtextobject.h"


void TestTextObject::newTextObject()
{
    const QString s("Hello");
    TextObject to(s);
    QCOMPARE(to.raw(), s);
    QCOMPARE(to.text(), s);
    QFile file("tmp.txt");
    QVERIFY(file.open(QFile::WriteOnly));
    file.write("this is a string");
    file.close();
    FileObject *fb = FileFactory::getFileObject("tmp.txt", "", &to);
    QVERIFY(fb != 0);
    TextObject to1(fb);
    QVERIFY(to1.file() != 0);
    QCOMPARE(to1.text(), QString("this is a string"));
    delete fb;
}

void TestTextObject::setText()
{
    const QString s("Hello");
    TextObject to;
    QVERIFY(to.text().isEmpty());
    to.setRaw(s);
    QVERIFY(to.file() == 0);
    QCOMPARE(to.raw(), s);
    QCOMPARE(to.text(), s);
    to.setRaw("Welcom to use Qem\rEnjoy!\n\r\nABC");
    QVERIFY(to.lines(QString::KeepEmptyParts).size() == 4);
}

void TestTextObject::setFile()
{
    TextObject to1;
    FileObject *fb = FileFactory::getFileObject("tmp.txt", "", &to1);
    to1.setFile(fb);
    QVERIFY(to1.file() != 0);
    QCOMPARE(to1.text(), QString("this is a string"));
    delete fb;
    QDir().remove("tmp.txt");
}

void TestTextObject::testWrite()
{
    TextObject to("Hello world!");
    QBuffer buffer;
    QVERIFY(buffer.open(QBuffer::ReadWrite));
    to.writeTo(buffer, 0, 5);
    buffer.seek(0);
    QCOMPARE(buffer.readAll(), QByteArray("Hello"));
    buffer.seek(0);
    to.writeTo(buffer, 0, -1);
    buffer.seek(0);
    QCOMPARE(buffer.readAll(), QByteArray("Hello world!"));
    QBuffer buf;
    QVERIFY(buf.open(QBuffer::ReadWrite));
    QTextStream ts(&buf);
    to.writeTo(ts, 5);
    buf.seek(0);
    QCOMPARE(ts.readAll(), QString("Hello"));
    buf.seek(0);
    to.writeTo(ts, -1);
    buf.seek(0);
    QCOMPARE(ts.readAll(), QString("Hello world!"));
}
