/*
 * Copyright 2014 Peng Wan
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

#ifndef QEM_JAR_H
#define QEM_JAR_H

#include "book.h"


// import QuaZip
#ifndef QUAZIP_STATIC
# define QUAZIP_STATIC
#endif

class QuaZip;

class QEM_SHARED_EXPORT JAR
{
private:
    JAR();
public:
    /// Name of this book format.
    static const QString FORMAT_NAME;

    /// File magic number of JAR book.
    static const quint32 FILE_HEADER = 0x00013000;

    static const QByteArray TEXT_ENCODING;
    static const QByteArray HEAD_ENCODING;

    static Book* parseJar(QIODevice &device, const QVariantMap &args = QVariantMap(), QString *error = 0);

    static Book* parseJar(QuaZip &zip, const QVariantMap &args = QVariantMap(), QString *error = 0);

    static bool makeJar(const Book &book, QIODevice &device, const QVariantMap &args = QVariantMap(),
                        QString *error = 0);
};

#endif // QEM_JAR_H
