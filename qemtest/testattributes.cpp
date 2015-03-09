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

#include "testattributes.h"
#include "attributes.h"


TestAttributes::TestAttributes()
{
}

void TestAttributes::setAttribute()
{
    Attributes attr;
    connect(&attr, SIGNAL(attributeChanged(QString,QVariant)), this, SLOT(onChange(QString,QVariant)));
    connect(&attr, SIGNAL(attributeRemoved(QString,QVariant)), this, SLOT(onRemove(QString,QVariant)));
    QVERIFY(attr.attributeCount() == 0);
    attr.setAttribute("Name", "PW");
    QVERIFY(attr.attributeCount() == 1);
    QCOMPARE(m_name, QString("Name"));
    QVERIFY(m_value.type() == QVariant::String);
    QCOMPARE(m_value.toString(), QString("PW"));
}

void TestAttributes::getAttribute()
{
    Attributes attr;
    connect(&attr, SIGNAL(attributeChanged(QString,QVariant)), this, SLOT(onChange(QString,QVariant)));
    connect(&attr, SIGNAL(attributeRemoved(QString,QVariant)), this, SLOT(onRemove(QString,QVariant)));
    QVERIFY(attr.attributeCount() == 0);
    attr.setAttribute("Name", "PW");
    QVERIFY(attr.attributeCount() == 1);
    QCOMPARE(attr.attribute("Name").toString(), QString("PW"));
}

void TestAttributes::removeAttribute()
{
    Attributes attr;
    connect(&attr, SIGNAL(attributeChanged(QString,QVariant)), this, SLOT(onChange(QString,QVariant)));
    connect(&attr, SIGNAL(attributeRemoved(QString,QVariant)), this, SLOT(onRemove(QString,QVariant)));
    QVERIFY(attr.attributeCount() == 0);
    attr.setAttribute("Name", "PW");
    QVERIFY(attr.attributeCount() == 1);
    reset();
    attr.removeAttribute("Name");
    QCOMPARE(m_name, QString("Name"));
    QVERIFY(m_value.type() == QVariant::String);
    QCOMPARE(m_value.toString(), QString("PW"));
    QVERIFY(attr.attributeCount() == 0);
}

void TestAttributes::onChange(const QString &name, const QVariant &v)
{
    qDebug() << "change attr";
    m_name = name;
    m_value = v;
}

void TestAttributes::onRemove(const QString &name, const QVariant &v)
{
    qDebug() << "remove attr";
    m_name = name;
    m_value = v;
}

