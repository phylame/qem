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

#ifndef QEM_ATTRIBUTES_H
#define QEM_ATTRIBUTES_H

#include "qem_global.h"
#include <QMap>
#include <QObject>
#include <QVariant>
#include <QStringList>

QEM_BEGIN_NAMESPACE

class QEM_SHARED_EXPORT Attributes : public QObject
{
    Q_OBJECT
public:
    inline explicit Attributes(QObject *parent = 0) :
        QObject(parent)
    {}

    inline Attributes(const Attributes &o) :
        QObject(o.parent()), m_attributes(o.m_attributes)
    {}

    inline Attributes& operator= (const Attributes &other)
    {
        m_attributes = other.m_attributes;
        return *this;
    }

    /// Returns number of attributes.
    QEM_INVOKABLE inline int attributeCount() const
    {
        return m_attributes.size();
    }

    /// Returns names of all attributes.
    QEM_INVOKABLE inline QStringList attributeNames() const
    {
        return QStringList(m_attributes.keys());
    }

    /// Returns \c true if contains attribute named \a name.
    QEM_INVOKABLE inline bool hasAttribute(const QString &name) const
    {
        return m_attributes.contains(name);
    }

    /// Get attribute value by \a name.
    QEM_INVOKABLE inline QVariant attribute(const QString &name,
                const QVariant &defaultValue = QVariant()) const
    {
        return m_attributes.value(name, defaultValue);
    }

    /// Set attribute \a value named \a name.
    QEM_INVOKABLE void setAttribute(const QString &name, const QVariant &value);

    /// Remove one attribute named \a name.
    QEM_INVOKABLE void removeAttribute(const QString &name);

signals:
    void attributeChanged(const QString &name, const QVariant &value);
    void attributeRemoved(const QString &name);

private:
    QVariantMap m_attributes;
};

QEM_END_NAMESPACE

Q_DECLARE_METATYPE(QEM_PREPEND_NAMESPACE(Attributes))

#endif // QEM_ATTRIBUTES_H
