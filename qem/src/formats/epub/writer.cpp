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

#include "writer.h"
#include <utils.h>
#include <fileutils.h>
#include <formats/epub.h>
#include <QUuid>
#include <QtDebug>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QXmlStreamWriter>
#include <quazipfile.h>

QEM_BEGIN_NAMESPACE

namespace epub
{

inline QString EpubWriter::opsPath(QString name)
{
    return config->opsDir + "/" + name;
}

inline bool EpubWriter::writeToEpub(FileObject &fb, QuaZip &zip, const QString &entryName)
{
    return FileUtils::writeToZip(fb, zip, opsPath(entryName));
}

inline bool EpubWriter::writeToEpub(QIODevice &device, QuaZip &zip, const QString &entryName)
{
    return FileUtils::writeToZip(device, zip, opsPath(entryName));
}

QuaZipFile* EpubWriter::initOpsEntry(const QString &entryName, QString *opsPath)
{
    QuaZipFile *file = new QuaZipFile(zip, book);
    QString zipPath(this->opsPath(entryName));
    if (opsPath != 0) {
        *opsPath = zipPath;
    }
    if (!file->open(QuaZipFile::WriteOnly, QuaZipNewInfo(zipPath))) {
        debug("Cannot write " + entryName, error);
        delete file;
        return 0;
    }
    return file;
}

inline bool EpubWriter::writeMt()
{
    if (!FileUtils::writeZipData(*zip, EPUB::MIMETYPE_FILE, EPUB::MT_EPUB)) {
        debug("Cannot write mimetype to EPUB", error);
        return false;
    }
    return true;
}

bool EpubWriter::writeContainer(const QString &opfPath)
{
    QuaZipFile zipFile(zip, book);
    if (!zipFile.open(QuaZipFile::WriteOnly, QuaZipNewInfo(EPUB::CONTAINER_FILE))) {
        debug("Cannot write " + EPUB::CONTAINER_FILE, error);
        return false;
    }
    QXmlStreamWriter xml(&zipFile);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("container");
    xml.writeAttribute("xmlns", EPUB::CONTAINER_XML_NS);
    xml.writeAttribute("version", EPUB::CONTAINER_VERSION);
    xml.writeStartElement("rootfiles");
    xml.writeEmptyElement("rootfile");
    xml.writeAttribute("full-path", opfPath);
    xml.writeAttribute("media-type", EPUB::MT_OPF);
    xml.writeEndElement();      // rootfiles
    xml.writeEndElement();      // container
    xml.writeEndDocument();
    zipFile.close();
    return true;
}

void EpubWriter::writeNcxHead(QXmlStreamWriter &xml, const QString &bookID, int depth)
{
    xml.writeStartElement("head");
    writeNcxHeadMeta(xml, "dtb:uid", bookID);
    writeNcxHeadMeta(xml, "dtb:depth", QString("%1").arg(depth));
    int pageCount = book->attribute("total_page_count", 0).toInt();
    int pageNumber = book->attribute("max_page_number", 0).toInt();
    writeNcxHeadMeta(xml, "dtb:totalPageCount", QString("%1").arg(pageCount));
    writeNcxHeadMeta(xml, "dtb:maxPageNumber", QString("%1").arg(pageNumber));
    xml.writeEndElement();
}

void EpubWriter::writeNcxHeadMeta(QXmlStreamWriter &xml, const QString &name, const QString &content)
{
    xml.writeEmptyElement("meta");
    xml.writeAttribute("name", name);
    xml.writeAttribute("content", content);
}

bool EpubWriter::writeNCX_2005_1(QXmlStreamWriter &xml, const QString &bookID)
{
    int depth = book->depth();
    xml.writeStartElement("ncx");
    xml.writeAttribute("xmlns", EPUB::NCX_XML_NS);
    xml.writeAttribute("version", EPUB::NCX_VERSION);
    // head
    writeNcxHead(xml, bookID, depth);
    // docTitle
    xml.writeStartElement("docTitle");
    xml.writeTextElement("text", book->title());
    xml.writeEndElement();
    // docAuthor
    xml.writeStartElement("docAuthor");
    xml.writeTextElement("text", book->author());
    xml.writeEndElement();
    // navMap
    int chapterCount = 0, playOrder = 1;
    xml.writeStartElement("navMap");
    QString href;
    // cover
    if (!cover.isEmpty()) {
        href = QString("%1/%2").arg(config->textDir, EPUB::CoverPageFileName);
        if (!writeCoverPage(EPUB::CoverPageTitle, href, cover)) {
            debug("Cannot write book cover page", error);
            return false;
        }
        addSpineItem(EPUB::CoverPageFileId, true, EPUB::DUOKAN_FULL_SCREEN);
        addGuideItem(href, "cover", EPUB::CoverPageTitle);
    }
    // intro
    href = QString("%1/%2").arg(config->textDir, EPUB::IntroPageFileName);
    if (writeIntroPage(href)) {
//        mknvp(navMap, Epub.IntroPageFileId, Epub.IntroPageTitle, href);
//        addSpineItem(Epub.IntroPageFileId, true, null);
    }
    // info
    // toc
    // text
    xml.writeEndElement();          // </ncx>
    return true;
}

bool EpubWriter::writeNCX(const QString &bookID)
{
    QTemporaryFile ncxFile;
    if (! ncxFile.open()) {
        debug("Cannot write NCX file", error);
        return false;
    }
    QXmlStreamWriter xml(&ncxFile);
    if (!config->xmlEncoding.isEmpty()) {
        xml.setCodec(config->xmlEncoding.constData());
    }
    xml.setAutoFormatting(true);
    xml.writeStartDocument("1.0");
    xml.writeDTD(QString("<!DOCTYPE %1 PUBLIC \"%2\" \"%3\">").arg(EPUB::NCX_XML_DT,
                    EPUB::NCX_DT_ID, EPUB::NCX_DT_URI));
    xml.writeCharacters("\n");
    if (!writeNCX_2005_1(xml, bookID)) {
        ncxFile.close();
        return false;
    }
    xml.writeEndDocument();
    ncxFile.seek(0);
    writeToEpub(ncxFile, *zip, EPUB::NcxFileName);
    ncxFile.close();
    return true;
}

QString EpubWriter::getBookID(const Book &book)
{
    QString bookID;
    if (book.hasAttribute("isbn")) {
        bookID = book.attribute("isbn").toString();
    } else if (book.hasAttribute("uuid")) {
        bookID = book.attribute("uuid").toString();
        if (!bookID.isEmpty() && !bookID.startsWith("urn:")) {
            bookID.prepend("urn:");
        }
    }
    if (bookID.isEmpty()) {
        bookID = QUuid::createUuid().toString();
        bookID = "urn:" + bookID.mid(1, bookID.length() - 2);
    }
    return bookID;
}

void EpubWriter::writeHtmlStart(QXmlStreamWriter &xml, const QString &title, const QString &cssHref)
{
    xml.writeDTD(QString("<!DOCTYPE %1 PUBLIC \"%2\" \"%3\">").arg(EPUB::XHTML_DT,
                    EPUB::XHTML_DT_ID, EPUB::XHTML_DT_URI));
    xml.writeStartElement("html");
    xml.writeAttribute("xmlns", EPUB::XHTML_NS);
    xml.writeAttribute("xml:lang", book->language());
    xml.writeStartElement("head");
    if (!cssHref.isEmpty()) {
        xml.writeEmptyElement("link");
        xml.writeAttribute("rel", "stylesheet");
        xml.writeAttribute("type", EPUB::MT_CSS);
        xml.writeAttribute("href", cssHref);
    }
    xml.writeTextElement("title", title);
    xml.writeEndElement();                      // head
    xml.writeStartElement("body");
}

void EpubWriter::writeHtmlEnd(QXmlStreamWriter &xml)
{
    xml.writeEndElement();  // </body>
    xml.writeEndElement();  // </html>
}

void EpubWriter::writeHtmlPara(QXmlStreamWriter &xml, const QStringList &lines)
{
    foreach (const QString &line, lines) {
        xml.writeTextElement("p", line.trimmed());
    }
}

bool EpubWriter::writeCoverPage(const QString &title, const QString &href, const QString &img)
{
    QuaZipFile *htmlFile = initOpsEntry(href);
    if (0 == htmlFile) {
        return false;
    }
    QXmlStreamWriter xml(htmlFile);
    if (!config->xmlEncoding.isEmpty()) {
        xml.setCodec(config->xmlEncoding.constData());
    }
    xml.setAutoFormatting(true);
    writeHtmlStart(xml, title, css);

    xml.writeStartElement("div");
    xml.writeAttribute("class", config->coverStyle);
    xml.writeEmptyElement("img");
    xml.writeAttribute("src", "../" + img);
    xml.writeEndElement();      // </div>

    writeHtmlEnd(xml);
    delete htmlFile;
    return true;
}

bool EpubWriter::writeIntroPage(const QString &href)
{
    QuaZipFile *htmlFile = initOpsEntry(href);
    if (0 == htmlFile) {
        return false;
    }
    QXmlStreamWriter xml(htmlFile);
    if (!config->xmlEncoding.isEmpty()) {
        xml.setCodec(config->xmlEncoding.constData());
    }
    xml.setAutoFormatting(true);
    writeHtmlStart(xml, EPUB::IntroPageTitle, css);

    xml.writeStartElement("div");
    xml.writeAttribute("class", config->introTitleStyle);
    xml.writeTextElement("h1", book->title());
    xml.writeEndElement();          // </div>

    xml.writeStartElement("div");
    xml.writeAttribute("class", config->introContentStyle);
    xml.writeTextElement("h3", QObject::tr("Intro"));
    writeHtmlPara(xml, book->intro().lines(true));
    xml.writeEndElement();          // </div>

    writeHtmlEnd(xml);
    delete htmlFile;
    return true;
}

bool EpubWriter::writeInfoPage(const QString &href)
{
    QuaZipFile *htmlFile = initOpsEntry(href);
    if (0 == htmlFile) {
        return false;
    }
    QXmlStreamWriter xml(htmlFile);
    if (!config->xmlEncoding.isEmpty()) {
        xml.setCodec(config->xmlEncoding.constData());
    }
    xml.setAutoFormatting(true);
    writeHtmlStart(xml, EPUB::InfoPageTitle, css);



    writeHtmlEnd(xml);
    delete htmlFile;
    return true;
}

bool EpubWriter::writeTocPage(const QString &href)
{
    return false;
}

bool EpubWriterV2::make()
{
    const QString &bookID = getBookID(*book);
    // write cover
    FileObject *fb = book->cover();
    if (fb != 0) {
        cover = QString("%1/%2").arg(config->imageDir, QFileInfo(fb->name()).fileName());
        if (!writeToEpub(*fb, *zip, cover)) {
            qWarning() << "Cannot write cover image:" << fb->name();
            cover.clear();
        } else {
            addManifestItem(EPUB::CoverFileID, cover, fb->mime());
        }
    }
    // main CSS
    QFile cssFile(":/mainCSS");
    if (cssFile.exists() && cssFile.open(QFile::ReadOnly)) {
        css = QString("%1/%2").arg(config->styleDir, EPUB::CssFileName);
        if (!writeToEpub(cssFile, *zip, css)) {
            qWarning() << "Cannot write CSS file:" << cssFile.fileName();
            css.clear();
        } else {
            addManifestItem(EPUB::CssFileID, css, EPUB::MT_CSS);
        }
        cssFile.close();
    }
    if (!writeNCX(bookID)) {
        return false;
    }
    QString opfPath;
    if (!writeOPF(bookID, opfPath)) {
        return false;
    }
    if (!writeContainer(opfPath)) {
        return false;
    }
    if (!writeMt()) {
        return false;
    }
    return true;
}

bool EpubWriterV2::writeOPF(const QString &bookID, QString &opfPath)
{
    QuaZipFile *zipFile = initOpsEntry(EPUB::OpfFileName, &opfPath);
    if (0 == zipFile) {
        return false;
    }
    QXmlStreamWriter xml(zipFile);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("package");
    xml.writeAttribute("xmlns", EPUB::OPF_XML_NS);
    xml.writeAttribute("version", EPUB::OPF_VERSION_2);
    xml.writeAttribute("unique-identifier", EPUB::IdentifierName);

    // metadata
    // manifest
    // spine
    // guide

    xml.writeEndElement();          // package
    xml.writeEndDocument();
    delete zipFile;
    return true;
}

}   // epub

QEM_END_NAMESPACE
