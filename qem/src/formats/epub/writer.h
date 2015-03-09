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

#ifndef QEM_EPUB_WRITER_H
#define QEM_EPUB_WRITER_H

#include <QList>
#include <QString>


class Book;
class QuaZip;
class QuaZipFile;
struct EpubMakeConfig;
class FileObject;
class QIODevice;
class QXmlStreamWriter;

class EpubWriter
{
public:
    inline EpubWriter(const Book &book, QuaZip &zip, const EpubMakeConfig &config, QString *error) :
        book(const_cast<Book*>(&book)), zip(&zip), config(const_cast<EpubMakeConfig*>(&config)), error(error)
    {}
    virtual bool make() = 0;
protected:
    /// Returns file name in OPS.
    QString opsPath(QString name);

    /// Write FileObject to EPUB archive named \a entryName.
    bool writeToEpub(FileObject &fb, QuaZip &zip, const QString &entryName);

    bool writeToEpub(QIODevice &device, QuaZip &zip, const QString &entryName);

    /// Write MIME type file to EPUB.
    bool writeMt();

    struct ManifestItem {
        QString id;
        QString href;
        QString mediaType;
        inline ManifestItem(const QString &id, const QString &href, const QString &mediaType) :
            id(id), href(href), mediaType(mediaType)
        {}
    };
    inline void addManifestItem(const QString &id, const QString &href, const QString &mediaType)
    {
        manifestItems.append(ManifestItem(id, href, mediaType));
    }

    struct SpineItem {
        QString idref;
        bool linear;
        QString property;
        inline SpineItem(const QString &idref, bool linear, const QString &property) :
            idref(idref), linear(linear), property(property)
        {}
    };
    inline void addSpineItem(const QString &idref, bool linear, const QString &property = QString())
    {
        spineItems.append(SpineItem(idref, linear, property));
    }

    struct GuideItem {
        QString href;
        QString type;
        QString title;
        inline GuideItem(const QString &href, const QString &type, const QString &title) :
            href(href), type(type), title(title)
        {}
    };
    inline void addGuideItem(const QString &href, const QString &type, const QString &title)
    {
        guideItems.append(GuideItem(href, type, title));
    }

    bool writeContainer(const QString &opfPath);

    QuaZipFile* initOpsEntry(const QString &entryName, QString *opsPath = 0);

    void writeNcxHead(QXmlStreamWriter &xml, const QString &bookID, int depth);

    void writeNcxHeadMeta(QXmlStreamWriter &xml, const QString &name, const QString &content);

    /// Write NCX document version 2005-1.
    bool writeNCX_2005_1(QXmlStreamWriter &xml, const QString &bookID);

    /// Write NCX document.
    virtual bool writeNCX(const QString &bookID);

    /// Write OPF document.
    virtual bool writeOPF(const QString &bookID, QString &opfPath) = 0;

    // write <html>, <body>
    void writeHtmlStart(QXmlStreamWriter &xml, const QString &title, const QString &cssHref);
    // write </body>, </html>
    void writeHtmlEnd(QXmlStreamWriter &xml);

    void writeHtmlPara(QXmlStreamWriter &xml, const QStringList &lines);

    bool writeCoverPage(const QString &title, const QString &href, const QString &img);

    bool writeIntroPage(const QString &href);

    bool writeInfoPage(const QString &href);

    bool writeTocPage(const QString &href);

    static QString getBookID(const Book &book);

protected:
    Book *book;
    QuaZip *zip;
    EpubMakeConfig *config;
    QString *error;
    QList<ManifestItem> manifestItems;
    QList<SpineItem> spineItems;
    QList<GuideItem> guideItems;
    QString cover, css;
};

class EpubWriterV2 : public EpubWriter
{
public:
    inline EpubWriterV2(const Book &book, QuaZip &zip, const EpubMakeConfig &config, QString *error) :
        EpubWriter(book, zip, config, error)
    {}
    virtual bool make();
protected:
    virtual bool writeOPF(const QString &bookID, QString &opfPath);

};

#endif
