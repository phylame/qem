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

QEM_BEGIN_NAMESPACE

namespace jar
{
    class QEM_SHARED_EXPORT JAR
    {
    private:
        JAR();
    public:
        /// Name of this book format.
        static const QString FORMAT_NAME;

        static Book* parseJar(QIODevice &device, const QVariantMap &args = QVariantMap(), QString *error = 0);

        static Book* parseJar(QuaZip &zip, const QVariantMap &args = QVariantMap(), QString *error = 0);

        static bool makeJar(const Book &book, QIODevice &device, const QVariantMap &args = QVariantMap(),
                            QString *error = 0);
    };
}   // jar

QEM_END_NAMESPACE

#endif // QEM_JAR_H
