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

#include <formats/epub.h>
#include <formats/utils.h>
#include <quazipfile.h>
#include <QtDebug>
#include "epub/writer.h"


const QString EPUB::FORMAT_NAME("epub");
const QString EPUB::MIMETYPE_FILE("mimetype");
const QByteArray EPUB::MT_EPUB("application/epub+zip");

// container.xml
const QString EPUB::CONTAINER_FILE("META-INF/container.xml");
const QString EPUB::CONTAINER_XML_NS("urn:oasis:names:tc:opendocument:xmlns:container");
const QString EPUB::CONTAINER_VERSION("1.0");

QString EPUB::OpfFileName("content.opf");
const QString EPUB::MT_OPF("application/oebps-package+xml");
const QString EPUB::OPF_XML_NS("http://www.idpf.org/2007/opf");
const QString EPUB::OPF_VERSION_2("2.0");

QString EPUB::NcxFileName("toc.ncx");
QString EPUB::NcxFileId("nav");
const QString EPUB::MT_NCX("application/x-dtbncx+xml");
const QString EPUB::NCX_XML_DT("ncx");
const QString EPUB::NCX_DT_ID("-//NISO//DTD ncx 2005-1//EN");
const QString EPUB::NCX_DT_URI("http://www.daisy.org/z3986/2005/ncx-2005-1.dtd");
const QString EPUB::NCX_XML_NS("http://www.daisy.org/z3986/2005/ncx/");
const QString EPUB::NCX_VERSION("2005-1");

QString EPUB::IdentifierName("book_id");
QString EPUB::CoverFileID("cover-image");

QString EPUB::CssFileName("style.css");
QString EPUB::CssFileID("css");
const QString EPUB::MT_CSS("text/css");

const QString EPUB::MT_XHTML("application/xhtml+xml");
const QString EPUB::XHTML_NS("http://www.w3.org/1999/xhtml");
const QString EPUB::XHTML_DT("html");
const QString EPUB::XHTML_DT_ID("-//W3C//DTD XHTML 1.1//EN");
const QString EPUB::XHTML_DT_URI("http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd");

QString EPUB::CoverPageTitle(QObject::tr("Book cover"));
QString EPUB::CoverPageFileName("cover.xhtml");
QString EPUB::CoverPageFileId("cover-page");

QString EPUB::IntroPageTitle(QObject::tr("Intro"));
QString EPUB::IntroPageFileName("intro.xhtml");

QString EPUB::InfoPageTitle(QObject::tr("Book details"));

QString EPUB::TocPageTitle(QObject::tr("Contents"));

const QString EPUB::DUOKAN_FULL_SCREEN("duokan-page-fullscreen");

const EpubMakeConfig EPUB::DefaultMakeConfig("OEBPS", "Text", "Images", "Styles", "UTF-8");

bool EPUB::makeEpub(const Book &book, QIODevice &device, const QVariantMap &args, QString *error)
{
    QString version("2");
    QByteArray nameCodec;
    EpubMakeConfig config = DefaultMakeConfig;
    if (!args.isEmpty()) {
        QVariant v = args["epub_version"];
        if (!v.isNull() && QVariant::String == v.type()) {
            version = v.toString();
        } else {
            qWarning() << "Invalid epub_version, expect string";
        }
        v = args["compression_method"];
        if (!v.isNull()) {
            bool ok;
            config.compressionMethod = v.toInt(&ok);
            if (!ok) {
                config.compressionMethod = Z_DEFLATED;
            } else {
                qWarning() << "Invalid compression_method string, expect int or string";
            }
        }
        v = args["compression_level"];
        if (!v.isNull()) {
            bool ok;
            config.compressionLevel = v.toInt(&ok);
            if (!ok) {
                config.compressionLevel = Z_DEFAULT_COMPRESSION;
            } else {
                qWarning() << "Invalid compression_method string, expect int or string";
            }
        }
        v = args["zip_name_encoding"];
        if (QVariant::String == v) {
            nameCodec = v.toString().toLatin1();
        } else if (QVariant::ByteArray == v) {
            nameCodec = v.toByteArray();
        } else if (!v.isNull()) {
            qWarning() << "zip_name_encoding require string or byte array";
        }
        v = args["xml_encoding"];
        if (QVariant::String == v) {
            config.xmlEncoding = v.toString().toLatin1();
        } else if (QVariant::ByteArray == v) {
            config.xmlEncoding = v.toByteArray();
        } else if (!v.isNull()) {
            qWarning() << "xml_encoding require string or byte array";
        }
        v = args["html_encoding"];
        if (QVariant::String == v) {
            config.htmlEncoding = v.toString().toLatin1();
        } else if (QVariant::ByteArray == v) {
            config.htmlEncoding = v.toByteArray();
        } else if (!v.isNull()) {
            qWarning() << "html_encoding require string or byte array";
        }
    }
    QuaZip zip(&device);
    if (!zip.open(QuaZip::mdCreate)) {
        FormatsUtility::debug("Cannot create EPUB archive", error);
        return false;
    }
    if (!nameCodec.isEmpty()) {
        zip.setFileNameCodec(nameCodec.constData());
    }
    return makeEpub(book, zip, version, config, error);
}

bool EPUB::makeEpub(const Book &book, QuaZip &zip, const QString &version,
                    const EpubMakeConfig &config, QString *error)
{
    EpubWriter *writer = 0;
    if (version.startsWith("2")) {
        writer = new EpubWriterV2(book, zip, config, error);
    } else {
        FormatsUtility::debug("Unsupported ePub version: " + version, error);
        return false;
    }
    bool ret = writer->make();
    delete writer;
    return ret;
}

