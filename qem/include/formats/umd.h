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


class QEM_SHARED_EXPORT UMD
{
private:
    UMD();
public:
    /// Name of this book format.
    static const QString FORMAT_NAME;

    /// File magic number of UMD book.
    static const quint32 FILE_HEADER        = 0xde9a9b89;

    /// UMD book type.
    enum UmdType {Text = 1, Cartoon = 2, Comic = 3};

    // data chunk type id
    enum ChunkType {
        Invalid         = 0x0,
        Head            = 0x1,
        Title           = 0x2,
        Author          = 0x3,
        Year            = 0x4,
        Month           = 0x5,
        Day             = 0x6,
        Genre           = 0x7,
        Publisher       = 0x8,
        Vendor          = 0x9,
        ContentLength   = 0xB,
        ChapterOffset   = 0x83,
        ChapterTitle    = 0x84,
        ImageType       = 0xE,
        ContentID       = 0xA,
        LicenseKey      = 0xF1,
        ContentEnd      = 0x81,
        Cover           = 0x82,
        PageOggset      = 0x87,
        CdsKey          = 0xF0,
        End             = 0xC,
        ID_D            = 0xD,
        ID_85           = 0x85,
        ID_86           = 0x86
    };

    /// Chunk content type.
    enum ContentType {Single, Appended};

    /// Separator for chunk and data.
    static const int DATA_SEPARATOR         = 0x24;      // "$"

    /// Separator for chunk and chunk.
    static const int CHUNK_SEPARATOR      = 0x23;     // "#"

    /// Text buffer size, 32KB
    static const int BUFFER_SIZE = 0x8000;

    /// Image format.
    enum ImageFormat {Bmp = 0, Jpg = 1, Gif = 2};

    /// Returns name of image \a type.
    static QString getNameOfFormat(ImageFormat type);

    /// Returns image type by \a name.
    static ImageFormat getFormatOfName(QString format);

    /// Test content of \a in is UMD or not.
    static bool isUMD(QDataStream &in);

    /// Extract images in Cartoon UMD file.
    static bool extractImage(const QString &path, const QString &dir);
    static bool extractImage(QIODevice &device, const QString &dir);

    static QString makeCartoon(const Book &metadata, const QList<QString> &files);
    static QString makeCartoon(const Book &metadata, const QString &dir);

    /// Qem Parser interface.
    static Book* parseUmd(QIODevice &device, const QVariantMap &args = QVariantMap(), QString *error = 0);

    /// Read UMD book from QDataStream \c in.
    static Book* parseUmd(QDataStream &in, QString *error = 0);

    /// Qem Maker interface.
    static bool makeUmd(const Book &book, QIODevice &device, const QVariantMap &args = QVariantMap(),
                        QString *error = 0);

    /// Write \a book to QDateStream \a out.
    static bool makeUmd(const Book &book, QDataStream &out, UmdType type = Text, QString *error = 0);
};

#endif // QEM_UMD_H
