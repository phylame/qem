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

#include <formats/umd.h>
#include <utils.h>
#include <filefactory.h>
#include <QDate>
#include <QtDebug>
#include <QTextCodec>
#include <QByteArray>
#include <QStringList>
#include <QDataStream>

QEM_BEGIN_NAMESPACE

namespace umd
{
    const QString UMD::FORMAT_NAME("umd");

    /// File magic number of UMD book.
    static const quint32 FILE_HEADER        = 0xde9a9b89;

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

    /// Line separator used in UMD text chunk.
    static const QChar SYMBIAN_LINE_FEED(0x2029);

    /// Local line separator.
    static const QString LOCAL_LINE_FEED("\r\n");

    /// Returns UMD text codec.
    static inline QTextCodec* umdCodec()
    {
        // workaround for Symbian global static bug, tested on Nokia 603
        static QTextCodec *codec = QTextCodec::codecForName("UTF-16LE");
        Q_ASSERT(codec != 0);
        return codec;
    }

    /// Content block descriptor.
    struct ContentBlock
    {
        qint32 offset, length;
        inline ContentBlock() : offset(0), length(0)
        {}
        inline ContentBlock(qint32 offset, qint32 length) : offset(offset), length(length)
        {}
    };

    typedef QList<ContentBlock> BlockList;

    struct UmdParseData
    {
        Book *book;                                  // the Book
        QString *error;                              // receive error string
        UMD::UmdType umdType;                        // type of UMD content
        quint32 contentLength;                       // size of UMD content
        UMD::ImageFormat coverFormat;                // type of book cover image
        UMD::ImageFormat imageFormat;                // type of content images in comic UMD
        ref_ptr<BlockList> *blocks;           // all content block
        QMap<quint32, ChunkType> dataOwners;    // owner of data chunk

        inline UmdParseData() :
            book(0), error(0), contentLength(0), coverFormat(UMD::Jpg), imageFormat(UMD::Jpg),
            blocks(new ref_ptr<BlockList>(new BlockList(), true, 0))
        {}
    };

    QString UMD::getNameOfFormat(enum UMD::ImageFormat type)
    {
        switch (type) {
        case Bmp:
            return "bmp";
        case Gif:
            return "gif";
        case Jpg:
            return "jpg";
        default:
            return "jpg";
        }
    }

    enum UMD::ImageFormat UMD::getFormatOfName(QString format) {
        if ("bmp" == format) {
            return Bmp;
        } else if ("jpg" == format || "jpeg" == format) {
            return Jpg;
        } else if ("gif" == format) {
            return Gif;
        } else {
            return Jpg;
        }
    }

    bool UMD::isUMD(QDataStream &in)
    {
        quint32 magic;
        in >> magic;
        return magic == FILE_HEADER;
    }

    Book* UMD::parseUmd(QIODevice &device, const QVariantMap &args, QString *error)
    {
        Q_ASSERT(device.isReadable());
        QDataStream in(&device);
        return parseUmd(in, error);
    }

    // declare
    static bool readChunk(QDataStream &in, UmdParseData &umdData);
    static bool readData(QDataStream &in, UmdParseData &umdData);

    Book* UMD::parseUmd(QDataStream &in, QString *error)
    {
        if (0 == in.device()) {
            debug("No device set for QDataStream", error);
            return 0;
        }

        // UMD use little endian
        in.setByteOrder(QDataStream::LittleEndian);

        if (! isUMD(in)) {
            debug("Invalid UMD file: magic number", error);
            return 0;
        }

        UmdParseData *umdData = new UmdParseData;
        umdData->book = new Book;
        umdData->error = error;

        while (true) {
            if (in.atEnd()) {
                goto FINISHED;
            }
            quint8 sep;
            in >> sep;
            switch (sep) {
            case CHUNK_SEPARATOR:
            {
                readChunk(in, *umdData);
            }
                break;
            case DATA_SEPARATOR:
            {
                readData(in, *umdData);
            }
                break;
            default:
            {
                debug("Bad UMD file: unknown separator", error);
                goto ERROR;
            }
            }
        }
ERROR:
        delete umdData->book;
        delete umdData->blocks;
        delete umdData;
        return 0;
FINISHED:
        Book *book = umdData->book;
        delete umdData;
        return book;
    }

    bool UMD::makeUmd(const Book &book, QIODevice &device, const QVariantMap &args, QString *error)
    {
        Q_ASSERT(device.isWritable());
        debug("UMD maker is under development", error);
        return false;
    }

    bool UMD::makeUmd(const Book &book, QDataStream &out, UMD::UmdType type, QString *error)
    {
        debug("UMD maker is under development", error);
        return false;
    }

    /// UMD specified chapter for parsing.
    class UmdChapter : public Chapter
    {
    public:
        UmdChapter(const QString &title, ref_ptr<BlockList> *blocks, QIODevice *file,
                   qint32 offset, qint32 length, QObject *parent = 0);

        inline ~UmdChapter()
        {
            // reduce reference of content blocks
            if (0 == --m_blocks->ref) {
                delete m_blocks;
            }
        }

        inline void setText(const QString &text)
        {
            Chapter::setText(text);
            m_fromUmd = false;
        }

        inline void setFile(FileObject *file, const QByteArray &codec = QByteArray())
        {
            Chapter::setFile(file, codec);
            m_fromUmd = false;
        }

        QString content() const;

        QStringList lines(bool skipEmptyLine = true) const;

        qint64 writeTo(QTextStream &out, qint64 size = -1) const;

        qint64 writeTo(QIODevice &out, const QByteArray &encoding = QByteArray(), qint64 size = -1) const;

        inline qint32& textOffset()
        {
            return m_offset;
        }

        inline qint32& textLength()
        {
            return m_length;
        }
    private:
        QString rawText() const;
    private:
        ref_ptr<BlockList> *m_blocks;
        mutable QIODevice *m_file;
        qint32 m_offset, m_length;
        bool m_fromUmd;
    };

    UmdChapter::UmdChapter(const QString &title, ref_ptr<BlockList> *blocks, QIODevice *file,
                           qint32 offset, qint32 length, QObject *parent):
        Chapter(title, "", 0, TextObject(), parent), m_blocks(blocks), m_file(file),
        m_offset(offset), m_length(length), m_fromUmd(true)
    {}

    static void makeUint32(quint32 x, char *b)
    {
        b[0] = (char)(x >> 24);
        b[1] = (char)(x >> 16);
        b[2] = (char)(x >> 8);
        b[3] = (char)x;
    }

    QString UmdChapter::rawText() const
    {
        qint32 index = m_offset / BUFFER_SIZE;
        qint32 start = m_offset % BUFFER_SIZE;
        qint32 length = -start;
        QByteArray data;
        do {
            const ContentBlock &block = m_blocks->data->at(index++);
            m_file->seek(block.offset);
            char *bytes = new char[block.length+4];    // 4 bytes header and data
            makeUint32(block.length, bytes);
            quint64 n = m_file->read(bytes+4, block.length);
            QByteArray res = qUncompress(reinterpret_cast<const uchar*>(bytes), n+4);
            delete []bytes;
            length += res.size();
            data.append(res);
            if (m_length < length) {
                return umdCodec()->toUnicode(data.mid(start, m_length));
            }
        } while (true);
    }

    QString UmdChapter::content() const
    {
        if (! m_fromUmd) {
            return Chapter::content();
        } else {
            return rawText().replace(SYMBIAN_LINE_FEED, LOCAL_LINE_FEED);
        }
    }

    QStringList UmdChapter::lines(bool skipEmptyLine) const
    {
        if (! m_fromUmd) {
            return Chapter::lines(skipEmptyLine);
        } else {
            const QString &s = rawText();
            return s.split(SYMBIAN_LINE_FEED, QString::SkipEmptyParts);
        }
    }

    qint64 UmdChapter::writeTo(QTextStream &out, qint64 size) const
    {
        if (! m_fromUmd) {
            return Chapter::writeTo(out, size);
        } else {
            const QString &s = content();
            if (size < 0) {
                size = s.length();
            }
            out << s.left(size);
            return size;
        }
    }

    qint64 UmdChapter::writeTo(QIODevice &out, const QByteArray &encoding, qint64 size) const
    {
        if (! m_fromUmd) {
            return Chapter::writeTo(out, encoding, size);
        } else {
            QTextCodec *ts = QTextCodec::codecForName(encoding);
            if (0 == ts) {
                qWarning() << "Not found codec:" << encoding;
                return -1;
            }
            const QString &s = content();
            if (size < 0) {
                size = s.length();
            }
            out.write(ts->fromUnicode(s.left(size)));
            return size;
        }
    }

    // declare
    static QString readString(QDataStream &in, int size, bool *ok);

    /// Read a UMD chunk.
    static bool readChunk(QDataStream &in, UmdParseData &umdData)
    {
        Book *book = umdData.book;

        quint16 chunkId;
        quint8 type, length;
        in >> chunkId >> type >> length;
        length -= 5;

        ChunkType chunkType = ChunkType(chunkId);

        switch (chunkType) {
        case Head:
        {
            quint8 type;
            in >> type;
            umdData.umdType = UMD::UmdType(type);
            in.skipRawData(length-1);
        }
            break;
        case Title:
        {
            book->setTitle(readString(in, length, 0));
        }
            break;
        case Author:
        {
            book->setAuthor(readString(in, length, 0));
        }
            break;
        case Year:
        {
            int year = readString(in, length, 0).toInt();
            const QDate &date = book->date();
            book->setDate(QDate(year, date.month(), date.day()));
        }
            break;
        case Month:
        {
            int month = readString(in, length, 0).toInt();
            const QDate &date = book->date();
            book->setDate(QDate(date.year(), month, date.day()));
        }
            break;
        case Day:
        {
            int day = readString(in, length, 0).toInt();
            const QDate &date = book->date();
            book->setDate(QDate(date.year(), date.month(), day));
        }
            break;
        case Genre:
        {
            book->setGenre(readString(in, length, 0));
        }
            break;
        case Publisher:
        {
            book->setPublisher(readString(in, length, 0));
        }
            break;
        case Vendor:
        {
            book->setAttribute("vendor", readString(in, length, 0));
        }
            break;
        case ContentLength:
        {
            in >> umdData.contentLength;
            in.skipRawData(length-4);
        }
            break;
        case ChapterOffset:
        {
            quint32 check;
            in >> check;
            umdData.dataOwners.insert(check, chunkType);
            in.skipRawData(length-4);
        }
            break;
        case ChapterTitle:
        {
            quint32 check;
            in >> check;
            umdData.dataOwners.insert(check, chunkType);
            in.skipRawData(length-4);
        }
            break;
        case ContentID:
        {
            quint32 bookID;
            in >> bookID;
            book->setAttribute("content_id", bookID);
            in.skipRawData(length-4);
        }
            break;
        case CdsKey:
        {
            char *bytes = new char[length];
            if (in.readRawData(bytes, length) != length) {
                delete bytes;
                debug("Bad UMD file: read CDS key", umdData.error);
                return false;
            }
            QByteArray key(bytes);
            delete bytes;
            book->setProperty("cds_key", key);
        }
            break;
        case LicenseKey:
        {
            char *bytes = new char[length];
            if (in.readRawData(bytes, length) != length) {
                delete bytes;
                debug("Bad UMD file: read license key", umdData.error);
                return false;
            }
            QByteArray key(bytes);
            delete bytes;
            book->setProperty("license_key", key);
        }
            break;
        case ContentEnd:
        {
            quint32 check;
            in >> check;
            umdData.dataOwners.insert(check, chunkType);
            in.skipRawData(length-4);
        }
            break;
        case Cover:
        {
            quint8 format;
            in >> format;
            umdData.coverFormat = UMD::ImageFormat(format);
            quint32 check;
            in >> check;
            umdData.dataOwners.insert(check, chunkType);
            in.skipRawData(length-1-4);
        }
            break;
        case PageOggset:
        {
            // ignore page information
            in.skipRawData(2);
            quint32 check;
            in >> check;
            umdData.dataOwners.insert(check, chunkType);
            in.skipRawData(length-2-4);
        }
            break;
        case End:
        {
            quint32 pos;
            in >> pos;
            if (pos != in.device()->pos()) {
                debug("Bad UMD file: end position", umdData.error);
                return false;
            }
        }
            break;
        case 0x85:
        {
            quint32 check;
            in >> check;
            umdData.dataOwners.insert(check, chunkType);
            in.skipRawData(length-4);
        }
            break;
        case 0x86:
        {
            quint32 check;
            in >> check;
            umdData.dataOwners.insert(check, chunkType);
            in.skipRawData(length-4);
        }
            break;
        case ImageType:
        {
            quint8 format;
            in >> format;
            umdData.imageFormat = UMD::ImageFormat(format);
            in.skipRawData(length - 1);
        }
            break;
        case 0xD:
        {
            // ignore unknown data
            in.skipRawData(length);
        }
            break;
        default:
        {
            // ignore unknown data
            in.skipRawData(length);
        }
            break;
        }
        return true;
    }

    /// Read chunk string data.
    static QString readString(QDataStream &in, int size, bool *ok)
    {
        char *bytes = new char[size];
        QString str;
        if (in.readRawData(bytes, size) != size) {  // error
            if (ok != 0) {
                *ok = false;
            }
        } else {
            if (ok != 0) {
                *ok = true;
            }
            str = umdCodec()->toUnicode(bytes, size);
        }
        delete []bytes;
        return str;
    }

    // declare
    static void skipBlock(QDataStream &in);
    static void readContent(QDataStream &in, UmdParseData &umdData);
    static void readChapterOffsets(QDataStream &in, UmdParseData &umdData);
    static void readChapterTitles(QDataStream &in, UmdParseData &umdData);
    static void readContentEnd(QDataStream &in, UmdParseData &umdData);
    static void readBookCover(QDataStream &in, UmdParseData &umdData);
    static void readPageOffsets(QDataStream &in, UmdParseData &umdData);

    /// Read a UMD data block.
    static bool readData(QDataStream &in, UmdParseData &umdData)
    {
        quint32 check;
        in >> check;
        ChunkType chunkType = umdData.dataOwners.value(check, Invalid);

        switch (chunkType) {
        case Invalid:
        {
            readContent(in, umdData);
        }
            break;
        case ChapterOffset:
        {
            readChapterOffsets(in, umdData);
            umdData.dataOwners.remove(check);
        }
            break;
        case ChapterTitle:
        {
            readChapterTitles(in, umdData);
            umdData.dataOwners.remove(check);
        }
            break;
        case ContentEnd:
        {
            readContentEnd(in, umdData);
            umdData.dataOwners.remove(check);
        }
            break;
        case Cover:
        {
            readBookCover(in, umdData);
            umdData.dataOwners.remove(check);
        }
            break;
        case PageOggset:
        {
            readPageOffsets(in, umdData);
            umdData.dataOwners.remove(check);
        }
            break;
        default:
        {
            skipBlock(in);
            umdData.dataOwners.remove(check);
        }
            break;
        }
        return true;
    }

    /// Skip data block.
    static inline void skipBlock(QDataStream &in)
    {
        quint32 length;
        in >> length;
        length -= 9;
        in.skipRawData(length);
    }

    /// Read chapter offsets.
    static void readChapterOffsets(QDataStream &in, UmdParseData &umdData)
    {
        Book *book = umdData.book;

        quint32 length, last = 0;
        in >> length;
        length -= 9;
        length /= 4;

        UmdChapter *chapter = 0;
        for (quint32 ix = 0; ix < length; ++ix) {
            quint32 offset;
            in >> offset;

            chapter = dynamic_cast<UmdChapter*>(book->value(ix, 0));
            if (0 == chapter) {     // not created
                chapter = new UmdChapter("", umdData.blocks, in.device(), offset, 0, umdData.book);
                ++umdData.blocks->ref;
                umdData.book->append(chapter);
            } else {
                chapter->textOffset() = offset;
            }
            // first is different
            if (ix > 0) {
                quint32 size = offset - last;
                UmdChapter *prev = dynamic_cast<UmdChapter*>(book->at(ix-1));
                prev->textLength() = size;
            }
            last = offset;
        }
        if (umdData.contentLength > 0 && chapter != 0) {    // have content length read
            chapter->textLength() = umdData.contentLength - last;
        }
    }

    /// Read chapter titles.
    static void readChapterTitles(QDataStream &in, UmdParseData &umdData)
    {
        Book *book = umdData.book;

        quint32 length;
        in >> length;
        length -= 9;
        const qint64 end = in.device()->pos() + length;

        int ix = 0;     // chapter index in book
        while (in.device()->pos() < end) {
            quint8 size;
            in >> size;
            char *bytes = new char[size];
            in.readRawData(bytes, size);
            const QString &title = umdCodec()->toUnicode(bytes, size);
            delete []bytes;
            UmdChapter *chapter = dynamic_cast<UmdChapter*>(book->value(ix++, 0));
            if (0 == chapter) {     // not created
                chapter = new UmdChapter(title, umdData.blocks, in.device(), 0, 0, umdData.book);
                ++umdData.blocks->ref;
                umdData.book->append(chapter);
            } else {
                chapter->setTitle(title);
            }
        }
    }

    /// Read content block.
    static void readContent(QDataStream &in, UmdParseData &umdData)
    {
        quint32 offset, length;
        in >> length;
        length -= 9;
        offset = in.device()->pos();
        Book *book = umdData.book;

        switch (umdData.umdType) {
        case UMD::Text:
        {
            umdData.blocks->data->append(ContentBlock(in.device()->pos(), length));
        }
            break;
        case UMD::Cartoon:
        {
            // add image to book extension
            QString name = QString("comic_%1.%2").arg(book->itemSize()+1).arg(
                        UMD::getNameOfFormat(umdData.imageFormat));
            FileObject *image = FileFactory::getFile(name, in.device(), offset, length, QString(),
                                                     book);
            book->setItem(image->name(), image);
        }
            break;
        case UMD::Comic:
        {
        }
            break;
        }
        in.skipRawData(length);
    }

    /// Read content end.
    static void readContentEnd(QDataStream &in, UmdParseData &umdData)
    {
        skipBlock(in);
    }

    /// Read book cover.
    static void readBookCover(QDataStream &in, UmdParseData &umdData)
    {
        quint32 length;
        in >> length;
        length -= 9;
        qint64 pos = in.device()->pos();
        FileObject *cover = FileFactory::getFile(QString("cover.%1").arg(
                                                     UMD::getNameOfFormat(umdData.coverFormat)),
                                                 in.device(), pos, length, QString(), umdData.book);
        umdData.book->setCover(cover);
        in.skipRawData(length);
    }

    /// Read page offsets.
    static void readPageOffsets(QDataStream &in, UmdParseData &umdData)
    {
        // ignore page offsets
        skipBlock(in);
    }
}   // umd

QEM_END_NAMESPACE
