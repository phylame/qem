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

#ifndef QEM_TEXTOBJECT_H
#define QEM_TEXTOBJECT_H

#include "fileobject.h"
#include <QObject>
#include <QMetaType>
#include <QStringList>


class TextObjectPrivate;
class QTextStream;
class QIODevice;
class QVariant;

class QEM_SHARED_EXPORT TextObject : public QObject
{
    Q_OBJECT
#ifdef QEM_QML_TARGET
    Q_PROPERTY(QString text READ text)
    Q_PROPERTY(QStringList lines READ lines)
#endif

private:
    TextObjectPrivate *p;
public:
    /// Get TextObject from QVariant \a v.
    /**
     * \param ok Save conver state.
     */
    static TextObject fromQVariant(const QVariant &v, bool *ok = 0);

    TextObject(const QString &s = QString(), QObject *parent = 0);

    explicit TextObject(FileObject *file, const QByteArray &codec = QByteArray(), QObject *parent = 0);

    TextObject(const TextObject &other);

    ~TextObject();

    TextObject& operator =(const TextObject &other);

    bool operator ==(const TextObject &other) const;

    /// Returns text content.
    QString text() const;

    /// Returns lines of text content split by line feed.
    QStringList lines(bool skipEmptyLine = false) const;

    /// Write \a size chararcters text content to QTextStream \a out.
    /** Returns copied characters number or \c -1 if occurs errors. */
#ifdef QEM_QML_TARGET
    Q_INVOKABLE
#endif
    qint64 writeTo(QTextStream &out, qint64 size = -1) const;

    /// Write \a size characters to QIODevice \a out with codec \a encoding.
    /** Returns copied characters number or \c -1 if occurs errors. */
#ifdef QEM_QML_TARGET
    Q_INVOKABLE
#endif
    qint64 writeTo(QIODevice &out, const QByteArray &encoding = QByteArray(), qint64 size = -1) const;

#ifdef QEM_QML_TARGET
    Q_INVOKABLE
#endif
    const QString& raw() const;

#ifdef QEM_QML_TARGET
    Q_INVOKABLE
#endif
    void setRaw(const QString &s);

#ifdef QEM_QML_TARGET
    Q_INVOKABLE
#endif
    FileObject* file() const;

#ifdef QEM_QML_TARGET
    Q_INVOKABLE
#endif
    QByteArray codec() const;

#ifdef QEM_QML_TARGET
    Q_INVOKABLE
#endif
    void setFile(FileObject *file, const QByteArray &codec = QByteArray());
};


Q_DECLARE_METATYPE(TextObject)

#endif // QEM_TEXTOBJECT_H
