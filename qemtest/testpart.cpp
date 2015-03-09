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

#include <QString>
#include <QBuffer>
#include <QTextStream>
#include "testpart.h"
#include "book.h"


TestPart::TestPart()
{
}

void TestPart::newPart()
{
    Book book("Example", "PW");
    QCOMPARE(book.title(), QString("Example"));
    QCOMPARE(book.author(), QString("PW"));
    QVERIFY(book.cover() == 0);
    QVERIFY(! book.date().isNull());
    QVERIFY(book.size() == 0);
    QVERIFY(! book.isSection());
    QCOMPARE(book.text(), QString(""));
    QVERIFY(book.file() == 0);
    Part p1("Part 1", "Hello");
    p1.newPart("Part 1.1");
    p1.newPart("Part 1.2");
    Part p2(p1);
    p1.setText("World");
    QCOMPARE(p2.title(), p1.title());
    QCOMPARE(p2.content(), QString("Hello"));
    QVERIFY(p2.size() == 2);
}

void TestPart::addPart()
{
    Book book("Example", "PW");
    Part *p1 = book.newPart("Part 1", "Hello");
    QVERIFY(p1 != 0);
    QVERIFY(p1->parent() == &book);
    QVERIFY(book.size() == 1);
    QCOMPARE(p1->title(), QString("Part 1"));
    QCOMPARE(p1->text(), QString("Hello"));

    Part *p2 = new Part("Part 2", "World");
    book.append(p2);
    QVERIFY(p2 != 0);
    QVERIFY(p2->parent() != &book);
    QVERIFY(book.size() == 2);
    QCOMPARE(p2->title(), QString("Part 2"));
    QCOMPARE(p2->text(), QString("World"));

    *p2 = *p1;
    p1->setText("ABC");
    QVERIFY(p2->text() != "ABC");
    delete p2;
}

void TestPart::removePart()
{
    Book book("Example", "PW");
    Part *p;
    for (int i=0; i<10; ++i) {
        p = book.newPart(QString("Part %1").arg(i+1), "Hello");
    }
    QVERIFY(book.size() == 10);
    book.removeAt(-1);
    QVERIFY(book.size() == 10);
    book.removeAt(0);
    QVERIFY(book.size() == 9);
    QVERIFY(book.removeOne(p));
    QVERIFY(book.size() == 8);
}

static bool selectPart3(const Part &p, void *arg)
{
    return p.title().startsWith("Part 3");
}

void TestPart::indexPart()
{
    Book book("Example", "PW");
    book.newPart("Part 1", "Hello");
    book.newPart("Part 2", "Hello");
    Part *p3 = book.newPart("Part 3", "Hello");
    p3->newPart("Part 3.1");
    Part *pp = p3->newPart("Part 3.2");
    pp->newPart("Part 3.2.1");
    pp->newPart("Part 3.2.2");
    p3->newPart("Part 3.3");
    book.newPart("Part 4", "Hello");
    QVERIFY(book.indexOf("Part 1") == 0);
    QVERIFY(book.indexOf("Part 4") == 3);
    QVERIFY(book.indexOf("Part 5") == -1);
    QVERIFY(book.indexOf(selectPart3, 0) == 2);
    QList<Part*> ls;
    book.select(ls, selectPart3, 0, true);
    QVERIFY(ls.size() == 6);
}

void TestPart::getPart()
{
    Book book("Example", "PW");
    book.newPart("Part 1", "Hello");
    book.newPart("Part 2", "Hello");

    Part *p3 = book.newPart("Part 3", "Hello");
    p3->newPart("Part 3.1");

    Part *pp = p3->newPart("Part 3.2");

    pp->newPart("Part 3.2.1");
    pp->newPart("Part 3.2.2");

    p3->newPart("Part 3.3");

    book.newPart("Part 4", "Hello");
    QVERIFY(book.size() == 4);
    QVERIFY(p3->size() == 3);
    QVERIFY(pp->size() == 2);
    int orders[] = {2, 1, 1};
    Part *p = book.findPart(orders, 3);
    QVERIFY(p != 0);
    QCOMPARE(p->title(), QString("Part 3.2.2"));
}

void TestPart::modifyContent()
{
    Part p("Part", "ABC\nDEF");
    QCOMPARE(p.content(), QString("ABC\nDEF"));
    QVERIFY(p.lines().size() == 2);
    p.setText("Just a string");
    QCOMPARE(p.content(), QString("Just a string"));
    QVERIFY(p.lines().size() == 1);
    QVERIFY(p.file() == 0);
}

void TestPart::writeText()
{
    Part p("Part", "ABC\nDEF");
    QBuffer buffer;
    buffer.open(QBuffer::WriteOnly);
    p.writeTo(buffer, 0, 3);
    QCOMPARE(buffer.data(), QByteArray("ABC"));
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QTextStream ts(&buf);
    p.writeTo(ts, 4);
    QCOMPARE(buf.data(), QByteArray("ABC\n"));
}
