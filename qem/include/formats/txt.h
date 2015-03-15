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

#ifndef QEM_TXT_H
#define QEM_TXT_H

#include "book.h"

QEM_BEGIN_NAMESPACE

namespace txt
{
    class QEM_SHARED_EXPORT TXT
    {
    private:
        Q_DISABLE_COPY(TXT)
    public:
        /// Name of this book format.
        static const QString FORMAT_NAME;

        static QByteArray TextEncoding;
        static QString ParagraphHeader;
        static QString TextLineFeed;
        static QString ChapterRegex;

        static Book* parseTxt(QIODevice &device, const QVariantMap &args = QVariantMap(), QString *error = 0);

        static Book* parseTxt(QTextStream &in, const QString &title, const QString &chapterRegex,
                              QString *error = 0);

        static bool makeTxt(const Book &book, QIODevice &device, const QVariantMap &args = QVariantMap(),
                            QString *error = 0);

        static bool makeTxt(const Book &book, QTextStream &out, const QVariantMap &args = QVariantMap(),
                            QString *error = 0);

        static bool makeTxt(const Book &book, QTextStream &out, const QByteArray &encoding,
                            const QString &lineFeed, const QString &paraStart, bool skipEmptyLine = false,
                            QString *error = 0);
    };
}   // txt

QEM_END_NAMESPACE

#endif // QEM_TXT_H
