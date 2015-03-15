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

#include <formats/txt.h>
#include <qem.h>
#include <utils.h>
#include <fileutils.h>
#include <textobject.h>
#include <filefactory.h>
#include <QFile>
#include <QtDebug>
#include <QRegExp>
#include <QTextCodec>
#include <QStringList>
#include <QTextStream>
#include <QTemporaryFile>

QEM_BEGIN_NAMESPACE

namespace txt
{
    const QString TXT::FORMAT_NAME("txt");
    QByteArray TXT::TextEncoding("GB18030");
    QString TXT::ParagraphHeader(QObject::tr("    "));
    QString TXT::TextLineFeed("\r\n");
    QString TXT::ChapterRegex(QObject::tr("^Part\\s+[\\d]+[\\r\\n]{1,2}$"));

    Book* TXT::parseTxt(QIODevice &device, const QVariantMap &args, QString *error)
    {
        Q_ASSERT(device.isReadable());
        QString title, regex;
        QByteArray codec;
        if (! args.isEmpty()) {
            QVariant v = args["text_encoding"];
            if (! v.isNull()) {
                if (QVariant::String == v.type()) {
                    codec = v.toString().toLatin1();
                } else if (QVariant::ByteArray == v.type()) {
                    codec = v.toByteArray();
                } else {
                    qWarning() << "\"text_encoding\" expect QString or QByteArray";
                }
            }
            v = args["chapter_pattern"];
            if (! v.isNull() && QVariant::String == v.type()) {
                regex = v.toString();
            }
            v = args["text_book_title"];
            if (! v.isNull() && QVariant::String == v.type()) {
                title = v.toString();
            }
        }
        if (regex.isEmpty()) {
            regex = ChapterRegex;
        }
        QTextStream in(&device);
        if (! codec.isEmpty()) {
            in.setCodec(codec.constData());
        }
        return parseTxt(in, title, regex, error);
    }

    Book* TXT::parseTxt(QTextStream &in, const QString &title, const QString &chapterRegex, QString *error)
    {
        QRegExp regex(chapterRegex);
        if (!regex.isValid()) {
            debug("Invalid chapter regex: "+chapterRegex, error);
            return 0;
        }
        regex.setMinimal(true);
        Book *book = new Book(title);

        const char *TEMP_TEXT_ENCODING("UTF-16LE");
        QTemporaryFile *tmpFile = new QTemporaryFile(book);
        tmpFile->open();
        QTextStream out(tmpFile);
        out.setCodec(TEMP_TEXT_ENCODING);
        FileUtils::copy(in, out);
        in.seek(0);

        const QString &raw = in.readAll();
        int index = 0;
        QList<int> offsets;
        QList<QString> titles;
        while (index < raw.length()) {
            index = regex.indexIn(raw, index);
            if (index < 0) {
                break;
            }
            offsets << index;
            titles << regex.cap(0);
            index += regex.matchedLength();
        }
        offsets << raw.length();
        QList<int>::const_iterator offsetIter = offsets.begin();
        QList<QString>::const_iterator titleIter = titles.begin();
        int start = *offsetIter++;

        FileObject *file = FileFactory::getFile("text_head", tmpFile, 0, start*2, "", book);
        if (file != 0) {
            book->setItem("text_head", file);
        } else {
            qWarning() << "Cannot create FileObject for text_head";
        }

        while (titleIter != titles.end()) {
            const QString &title = *titleIter++;
            start += title.length();
            int end = *offsetIter++;
            int length = end - start;
            Chapter *chapter = book->newChapter(title.trimmed());
            FileObject *file = FileFactory::getFile(QString("chapter%1").arg(book->size()), tmpFile,
                                                    start*2, length*2, "", chapter);
            if (file != 0) {
                chapter->setFile(file, TEMP_TEXT_ENCODING);
            }
            start = end;
        }
        return book;
    }

    bool TXT::makeTxt(const Book &book, QIODevice &device, const QVariantMap &args, QString *error)
    {
        Q_ASSERT(device.isWritable());
        QTextStream out(&device);
        return makeTxt(book, out, args, error);
    }

    bool TXT::makeTxt(const Book &book, QTextStream &out, const QVariantMap &args, QString *error)
    {
        QByteArray encoding(TextEncoding);
        QString lineFeed = TextLineFeed, paraStart = ParagraphHeader;
        bool skipEmptyLine = false;
        if (args.size() != 0) {
            QVariant v = args.value("text_encoding");
            if (! v.isNull()) {
                if (v.canConvert<QString>()) {
                    encoding = v.toString().toLocal8Bit();
                } else {
                    qWarning() << "\"text_encoding\" require QString";
                }
            }
            v = args.value("text_linefeed");
            if (! v.isNull()) {
                if (v.canConvert<QString>()) {
                    lineFeed = v.toString();
                } else {
                    qWarning() << "\"text_linefeed\" require QString";
                }
            }
            v = args.value("paragraph_head");
            if (! v.isNull()) {
                if (v.canConvert<QString>()) {
                    paraStart = v.toString();
                } else {
                    qWarning() << "\"paragraph_head\" require QString";
                }
            }
            v = args.value("skip_empty_line");
            if (! v.isNull()) {
                skipEmptyLine = true;
            }
        }
        return makeTxt(book, out, encoding, lineFeed, paraStart, skipEmptyLine, error);
    }

    static void writePart(const Part *part, QTextStream &out, const QString &lineFeed,
                          const QString &paraStart, bool skipEmptyLine);

    bool TXT::makeTxt(const Book &book, QTextStream &out, const QByteArray &encoding,
                      const QString &lineFeed, const QString &paraStart, bool skipEmptyLine, QString *error)
    {
        out.setCodec(encoding.constData());
        out << book.title() << lineFeed;
        const QString &author = book.author();
        if (!author.isEmpty()) {
            out << author << lineFeed;
        }
        const QStringList &lines = book.intro().lines(skipEmptyLine);
        foreach (const QString &line, lines) {
            if (! line.isEmpty()) {
                out << paraStart << line.trimmed();
            }
            out  << lineFeed;
        }
        out.flush();
        for (int ix = 0; ix < book.size(); ++ix) {
            const Part *p = book.at(ix);
            Q_ASSERT(p != 0);
            writePart(p, out, lineFeed, paraStart, skipEmptyLine);
        }
        return true;
    }

    static void writePart(const Part *part, QTextStream &out, const QString &lineFeed,
                          const QString &paraStart, bool skipEmptyLine)
    {
        Q_ASSERT(part != 0);
        out << lineFeed << part->title() << lineFeed;
        if (! part->isSection()) {  // no sub items
            const QStringList &lines = part->lines();
            QStringList::const_iterator i = lines.constBegin();
            for (; i != lines.constEnd(); ++i) {
                const QString &line = (*i).trimmed();
                if (line.isEmpty() && skipEmptyLine) {
                    continue;
                }
                out << paraStart << line;
                out << lineFeed;
            }
            out.flush();
        } else {
            for (int ix = 0; ix < part->size(); ++ix) {
                const Part *p = part->at(ix);
                Q_ASSERT(p != 0);
                writePart(p, out, lineFeed, paraStart, skipEmptyLine);
            }
        }
    }
}   // txt

QEM_END_NAMESPACE
