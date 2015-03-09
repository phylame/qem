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

#ifndef TESTATTRIBUTES_H
#define TESTATTRIBUTES_H

#include <QString>
#include <QVariant>
#include <QtTest/QtTest>


class TestAttributes : public QObject
{
    Q_OBJECT
public:
    TestAttributes();
signals:
    
public slots:
    void onChange(const QString &name, const QVariant &v);
    void onRemove(const QString &name, const QVariant &v);
private slots:
    void setAttribute();
    void getAttribute();
    void removeAttribute();

private:
    inline void reset()
    {
        m_name = "";
        m_value = QVariant();
    }

    QString m_name;
    QVariant m_value;
};

#endif // TESTATTRIBUTES_H
