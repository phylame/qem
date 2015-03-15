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

#ifndef QEM_BOOK_H
#define QEM_BOOK_H

#include "chapter.h"
#include <QDate>
#include <QMap>

QEM_BEGIN_NAMESPACE

class QEM_SHARED_EXPORT Book : public Chapter
{
    Q_OBJECT
#ifdef QEM_QML_TARGET
    Q_PROPERTY(QString author READ author WRITE setAuthor NOTIFY authorChanged)
    Q_PROPERTY(QString genre READ genre WRITE setGenre NOTIFY genreChanged)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString subject READ subject WRITE setSubject NOTIFY subjectChanged)
    Q_PROPERTY(QDate date READ date WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(QString publisher READ publisher WRITE setPublisher NOTIFY publisherChanged)
    Q_PROPERTY(QString rights READ rights WRITE setRights NOTIFY rightsChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
#endif

protected:
    const static QString AUTHOR_KEY;
    const static QString GENRE_KEY;
    const static QString STATE_KEY;
    const static QString SUBJECT_KEY;
    const static QString DATE_KEY;
    const static QString PUBLISHER_KEY;
    const static QString RIGHTS_KEY;
    const static QString LANGUAGE_KEY;

    /// Set all attributes to defaule.
    void reset();
public:
    typedef QMap<QString, QVariant> ExtensionMap;

    explicit Book(const QString &title = QString(), const QString &author = QString(),
                  QObject *parent = 0);

    inline Book(const Part &part) :
        Chapter(part)
    {
        reset();
    }

    inline Book(const Chapter &chapter) :
        Chapter(chapter)
    {
        reset();
    }

    inline Book(const Book &other):
        Chapter(other), m_extensions(other.m_extensions)
    {}

    ~Book();

    QString author() const;
    void setAuthor(const QString &author);

    QString genre() const;
    void setGenre(const QString &genre);

    QString state() const;
    void setState(const QString &state);

    QString subject() const;
    void setSubject(const QString &subject);

    QDate date() const;
    void setDate(const QDate &date);

    QString publisher() const;
    void setPublisher(const QString &publisher);

    QString rights() const;
    void setRights(const QString &rights);

    QString language() const;
    void setLanguage(const QString &language);

    QEM_INVOKABLE inline void setItem(const QString &name, const QVariant &value)
    {
        m_extensions.insert(name, value);
    }

    QEM_INVOKABLE inline void setItem(const QString &name, const FileObject *file)
    {
        setItem(name, QVariant::fromValue(const_cast<FileObjectPointer>(file)));
    }

    QEM_INVOKABLE inline bool hasItem(const QString &name) const
    {
        return m_extensions.contains(name);
    }

    QEM_INVOKABLE QVariant getItem(const QString &name, const QVariant &defValue = QVariant()) const
    {
        return m_extensions.value(name, defValue);
    }

    QEM_INVOKABLE QVariant removeItem(const QString &name)
    {
        return m_extensions.take(name);
    }

    QEM_INVOKABLE inline void clearItems()
    {
        m_extensions.clear();
    }

    QEM_INVOKABLE inline QStringList itemNames() const
    {
        return QStringList(m_extensions.keys());
    }

    QEM_INVOKABLE inline int itemSize() const
    {
        return m_extensions.size();
    }

#ifdef QEM_QML_TARGET
signals:
    void authorChanged(const QString &author);
    void genreChanged(const QString &genre);
    void stateChanged(const QString &state);
    void subjectChanged(const QString &subject);
    void dateChanged(const QDate &data);
    void publisherChanged(const QString &publisher);
    void rightsChanged(const QString &rights);
    void languageChanged(const QString &language);
protected slots:
    virtual void fireAttributeChange(const QString &name, const QVariant &value);
    virtual void fireAttributeRemove(const QString &name);
#endif

private:
    ExtensionMap m_extensions;
};

QEM_END_NAMESPACE

Q_DECLARE_METATYPE(QEM_PREPEND_NAMESPACE(Book))

#endif // QEM_BOOK_H
