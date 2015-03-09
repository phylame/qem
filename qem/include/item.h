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

#ifndef QEM_ITEM_H
#define QEM_ITEM_H

#include <QObject>
#include <QVariant>
#include "qem_global.h"


class QEM_SHARED_EXPORT Item : public QObject
{
    Q_OBJECT
    Q_ENUMS(Type)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(Type type READ type WRITE setType)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    enum Type {File, Text, Number};
    static Type nameType(const QString &s);
    static QString typeName(Type type);
public:
    explicit inline Item(const QString &name = QString(), Type type = Text,
                const QVariant &value = QVariant(), QObject *parent = 0) :
        QObject(parent), m_name(name), m_type(type), m_value(value)
    {}

    inline Item(const Item &other) :
        QObject(0), m_name(other.m_name), m_type(other.m_type), m_value(other.m_value)
    {}

    inline Item& operator =(const Item &other)
    {
        m_name = other.m_name;
        m_type = other.m_type;
        m_value = other.m_value;
        return *this;
    }

    inline const QString& name() const
    {
        return m_name;
    }
    inline void setName(const QString &name)
    {
        m_name = name;
    }

    inline Type type() const
    {
        return m_type;
    }
    inline void setType(Type type)
    {
        m_type = type;
    }

    inline const QVariant& value() const
    {
        return m_value;
    }
    inline void setValue(const QVariant &value)
    {
        m_value = value;
    }

private:
    QString m_name;
    Type m_type;
    QVariant m_value;
};


Q_DECLARE_METATYPE(Item)

#endif // QEM_ITEM_H
