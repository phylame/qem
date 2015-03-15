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

#ifndef QEM_PMABUTILS_H
#define QEM_PMABUTILS_H

#include "book.h"


// import QuaZip
#ifndef QUAZIP_STATIC
# define QUAZIP_STATIC
#endif

class QuaZip;

QEM_BEGIN_NAMESPACE

namespace pmab
{
    class QEM_SHARED_EXPORT PMAB
    {
    private:
        Q_DISABLE_COPY(PMAB)
    public:
        /// Name of this book format.
        static const QString FORMAT_NAME;

        /** Content directory in PMAB */
        static QString TextDir;
        static QString ImageDir;
        static QString ExtraDir;

        /** Text encoding for chapter and item */
        static QByteArray TextEncoding;

        ///// XML format //////
        static QString XmlEncoding;
        static QString XmlIndent;
        static QString XmlLineFeed;

        // default output PMAB version
        static QString InitPbmVersion;
        static QString InitPbcVersion;

        /// Set one attribute in PMAB to Qem book.
        /**
     * \param book the Book instance
     * \param key name of the attribute
     * \param value string value from PBM document
     */
        static void setAttribute(Book &book, const QString &key, const QString &value);

        /// Test given ZIP is PMAB archive or not.
        static bool isPmab(QuaZip &zip);

        /// Qem Parser interface.
        static Book* parsePmab(QIODevice &device, const QVariantMap &args = QVariantMap(), QString *error = 0);

        static Book* parsePmab(QuaZip &zip, QString *error = 0);

        /// Qem Maker interface.
        static bool makePmab(const Book &book, QIODevice &device, const QVariantMap &args = QVariantMap(),
                             QString *error = 0);
    };
}   // pmab

QEM_END_NAMESPACE

#endif // QEM_PMABUTILS_H
