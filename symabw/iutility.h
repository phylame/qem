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

#ifndef IMABW_UTILITY_H
#define IMABW_UTILITY_H

#include <book.h>
#include <QObject>
#include <QLocale>
#include <QDateTime>
#include "imabw.h"


class Book;
class QuaZip;

namespace symabw {

class IUtility : public QObject
{
    Q_OBJECT
public:
    explicit IUtility(QObject *parent = 0) :
        QObject(parent)
    {}

    Q_INVOKABLE inline QString errorString() const
    {
        return m_error;
    }

    Q_INVOKABLE QString selectFile(const QString & caption, const QString & dir = QString(),
                const QString & filter = QString(), QString * selectedFilter = 0);

    Q_INVOKABLE QString selectFolder(const QString & caption, const QString & dir = QString());

    Q_INVOKABLE bool openInSystem(const QString &url);

    Q_INVOKABLE void copyToClipbord(const QString &text);

    Q_INVOKABLE QString baseName(const QString& name);

    Q_INVOKABLE QString fileName(const QString &name);

    Q_INVOKABLE QString dirName(const QString& name);

    Q_INVOKABLE QString extensionName(const QString &name);

    Q_INVOKABLE qint64 fileSize(const QString &name);

    Q_INVOKABLE bool fileExists(const QString &name);

    Q_INVOKABLE bool sameFile(const QString &file1, const QString &file2);

    Q_INVOKABLE bool renameFile(const QString &orig, const QString &dest);

    Q_INVOKABLE bool deleteFile(const QString &name);

    Q_INVOKABLE bool copyFile(const QString &src, const QString &dest);

    Q_INVOKABLE QDateTime modifiedDate(const QString &name);

    Q_INVOKABLE void setCover(Book *book, const QString &path);

    Q_INVOKABLE inline Chapter* newChapter(const QString &title, const QString &text = QString(),
                                    QObject *parent = 0)
    {
        if (0 == parent) {
            parent = this;
        }
        return new Chapter(title, text, 0, TextObject(), parent);
    }

    Q_INVOKABLE Book* newBook(const QString &title, const QString &author = QString(),
                                     QObject *parent = 0);

    /// Returns current PMAB format.
    Q_INVOKABLE QString pmab();

    Q_INVOKABLE QStringList getSupportedParser();

    Q_INVOKABLE QStringList getSupportedMaker();

    Q_INVOKABLE Book* readBook(const QString &name, const QString &format = QString(),
                   const QVariantMap &args = QVariantMap());

    Q_INVOKABLE bool writeBook(QObject *part, const QString &name, const QString &format = QString(),
                    const QVariantMap &args = QVariantMap());

    /// Save text to temporary file and offer to part's content.
    Q_INVOKABLE bool saveText(Part *part, const QString &text);

    Q_INVOKABLE QString loadText(const QString &path, const QByteArray &codec = QByteArray());

    Q_INVOKABLE QString formatVariant(const QVariant &v);

    /// Retuens type name of QVariant \a v.
    Q_INVOKABLE QString variantType(const QVariant &v);

//    Q_INVOKABLE inline FileObject* fileObject(void *p)
//    {
//        return (FileObject*) p;
//    }

    Q_INVOKABLE FileObject* getFileObject(const QString &name, const QString &mime = QString(),
                QObject *parent = 0);

    Q_INVOKABLE FileObject* getFileObject(const QString &name, QIODevice *device, qint64 offset,
                qint64 size, const QString &mime = QString(), QObject *parent = 0);

    Q_INVOKABLE FileObject* getFileObject(QuaZip *zip, const QString &name, const QString &mime = QString(),
                QObject *parent = 0);

    Q_INVOKABLE QString cacheFileObject(FileObject *fb);
private:
    QString m_error;
};

}   // end symabw

#endif // IMABW_UTILITY_H
