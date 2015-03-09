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

#ifndef QEM_H
#define QEM_H

#include "book.h"


class QVariant;
class QIODevice;

class QEM_SHARED_EXPORT Qem
{
private:
    Qem();
public:
    /// Version of Qem
    static const QString VERSION;

    static const QString FORMAT_PMAB;

    typedef Book* (*Parser)(QIODevice &device, const QVariantMap &args, QString *error);
    typedef bool (*Maker)(const Book &book, QIODevice &device, const QVariantMap &args, QString *error);

    // paser book file
    static void registerBookParser(const QString &format, Parser parser);
    static bool hasBookParser(const QString &format);
    static QStringList getSupportedParser();
    static Parser getBookParser(const QString &format);

    // make book file
    static void registerBookMaker(const QString &format, Maker maker);
    static bool hasBookMaker(const QString &format);
    static QStringList getSupportedMaker();
    static Maker getBookMaker(const QString &format);

    /** The caller should delete returned Book. */
    static Book* readBook(const QString &name, const QString &format = QString(),
                   const QVariantMap &args = QVariantMap(), QString *error = 0);

    /** The caller should delete returned Book. */
    static Book* readBook(QIODevice &device, const QString &format = QString(),
                   const QVariantMap &args = QVariantMap(), QString *error = 0);

    static bool writeBook(const Book &book, const QString &name, const QString &format = QString(),
                    const QVariantMap &args = QVariantMap(), QString *error = 0);
    static bool writeBook(const Book &book, QIODevice &device, const QString &format = QString(),
                    const QVariantMap &args = QVariantMap(), QString *error = 0);

    static bool convertBook(QIODevice &in, QString inFormat, const QVariantMap &parseArgs,
                            QIODevice &out, QString outFormat, const QVariantMap &makeArgs);

    static QString formatVariant(const QVariant &v);

    static void printProperties(const Part &part, QString sep, QStringList names,
                                          bool showBrackets = true, QTextStream *out = 0);

    // return false to stop walking
    typedef bool (*Walker)(Part &p);

    static void walkPart(Part &part, Walker walker);

};

#endif // QEM_H
