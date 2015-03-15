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

#ifndef QEM_UMD_H
#define QEM_UMD_H

#include "book.h"

QEM_BEGIN_NAMESPACE

namespace umd
{
    class QEM_SHARED_EXPORT UMD
    {
    private:
        Q_DISABLE_COPY(UMD)
    public:
        /// Name of this book format.
        static const QString FORMAT_NAME;

        /// UMD book type.
        enum UmdType {Text = 1, Cartoon = 2, Comic = 3};

        /// Image format.
        enum ImageFormat {Bmp = 0, Jpg = 1, Gif = 2};

        /// Returns name of image \a type.
        static QString getNameOfFormat(ImageFormat type);

        /// Returns image type by \a name.
        static ImageFormat getFormatOfName(QString format);

        /// Test content of \a in is UMD or not.
        static bool isUMD(QDataStream &in);

        /// Qem Parser interface.
        static Book* parseUmd(QIODevice &device, const QVariantMap &args = QVariantMap(), QString *error = 0);

        /// Read UMD book from QDataStream \c in.
        static Book* parseUmd(QDataStream &in, QString *error = 0);

        /// Qem Maker interface.
        static bool makeUmd(const Book &book, QIODevice &device, const QVariantMap &args = QVariantMap(),
                            QString *error = 0);

        /// Write \a book to QDateStream \a out.
        static bool makeUmd(const Book &book, QDataStream &out, UmdType type = Text, QString *error = 0);

        /// Extract images in Cartoon UMD file.
        static bool extractImage(const QString &path, const QString &dir);
        static bool extractImage(QIODevice &device, const QString &dir);

        static QString makeCartoon(const Attributes &metadata, const QList<QString> &files);
        static QString makeCartoon(const Attributes &metadata, const QString &dir);
    };
}   // umd

QEM_END_NAMESPACE

#endif // QEM_UMD_H
