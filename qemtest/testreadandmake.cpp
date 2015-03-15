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

#include "test"
#include <QFile>
#include <QMap>
#include <QDir>
#include "qem.h"
#include "textobject.h"
#include "testreadandmake.h"


void TestReadAndMake::testMakeTxt()
{
    QDir curDir;
    Book book("Example", "PW");
    book.setIntro(TextObject("The intro\r\rX\nY\r\nZ"));
    book.newChapter("Chapter 1", "Hello\n\nQem");
    book.newChapter("Chapter 2", "World");
    QVariantMap args;
    args.insert("text_linefeed", "\n");
    args.insert("paragraph_head", "  ");
    args.insert("skip_empty_line", "");
    QVERIFY(Qem::writeBook(book, "ex.txt", "txt", args));
    QFile file("ex.txt");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray ba = file.readAll();
    QString s(ba);
    qDebug() << s;
    file.close();
    curDir.remove(file.fileName());
}

void TestReadAndMake::testMakeUmd()
{

}

void TestReadAndMake::testReadTxt()
{
}

void TestReadAndMake::testReadUmd()
{

}
