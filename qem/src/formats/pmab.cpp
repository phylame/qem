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

#include <formats/pmab.h>
#include <utils.h>
#include <fileutils.h>
#include <filefactory.h>
#include <QStack>
#include <QtDebug>
#include <QXmlStreamReader>
#include <quazipfile.h>

QEM_BEGIN_NAMESPACE

namespace pmab
{
    const QString PMAB::FORMAT_NAME("pmab");

    ///// MIME type for PMAB /////
    static const QString MIMETYPE_FILE("mimetype");
    static const QByteArray MT_PMAB("application/pmab+zip");

    ///// PBM(PMAB Book Metadata) /////
    static const QString PBM_FILE("book.xml");
    static const QString PBM_XML_NS("http://phylame.pw/format/pmab/pbm");
    static const QString PBM_XML_ROOT_NAME_V1("package");
    static const QString PBM_XML_ROOT_NAME_V2("pbm");

    ///// PBC(PMAB Book Content) /////
    static const QString PBC_FILE("content.xml");
    static const QString PBC_XML_NS("http://phylame.pw/format/pmab/pbc");
    static const QString PBC_XML_ROOT_NAME_V1("container");
    static const QString PBC_XML_ROOT_NAME_V2("pbc");

    /** Content directory in PMAB */
    QString PMAB::TextDir("Text");
    QString PMAB::ImageDir("Images");
    QString PMAB::ExtraDir("Extras");

    /** Text encoding for chapter and item */
    QByteArray PMAB::TextEncoding("UTF-8");

    /** Date format in PBM, used in PMAB 2.x */
    static const QString DATE_FORMAT("yyyy-M-d H:m:s");

    ///// XML format //////
    QString PMAB::XmlEncoding("UTF-8");
    QString PMAB::XmlIndent("\t");
    QString PMAB::XmlLineFeed("\r\n");

    // default output PMAB version
    QString PMAB::InitPbmVersion("2.0");
    QString PMAB::InitPbcVersion("2.0");

    bool PMAB::isPmab(QuaZip &zip)
    {
        return MT_PMAB == FileUtils::readZipData(zip, MIMETYPE_FILE).trimmed();
    }

    static void deleteQuaZip(Part &part, void *arg)
    {
        QuaZip *zip = static_cast<QuaZip*>(arg);
        delete zip;
    }

    Book* PMAB::parsePmab(QIODevice &device, const QVariantMap &args, QString *error)
    {
        Q_ASSERT(device.isReadable());
        QuaZip *zip = new QuaZip(&device);
        QByteArray nameCodec;
        if (! args.isEmpty()) {
            QVariant v = args.value("pmab_name_encoding");
            if (! v.isNull()) {
                if (QVariant::String == v.type()) {
                    nameCodec = v.toString().toLatin1();
                } else if (QVariant::ByteArray == v.type()) {
                    nameCodec = v.toByteArray();
                } else {
                    qWarning() << "\"pmab_name_encoding\" expect QString or QByteArray";
                }
            }
        }
        if (!nameCodec.isEmpty()) {
            zip->setFileNameCodec(nameCodec.constData());
        }
        if (!zip->open(QuaZip::mdUnzip)) {
            debug("Cannot open PMAB archive", error);
            delete zip;
            return 0;
        }
        Book *book = parsePmab(*zip, error);
        if (0 == book) {
            delete zip;
        } else {
            book->registerCleaner(deleteQuaZip, zip);
        }
        return book;
    }

    static bool readPBM(QuaZip &zip, Book &book, QString *error);
    static bool readPBC(QuaZip &zip, Book &book, QString *error);

    Book* PMAB::parsePmab(QuaZip &zip, QString *error)
    {
        if (! isPmab(zip)) {
            debug("Not PMAB archive", error);
            return 0;
        }
        Book *book = new Book;
        if (! readPBM(zip, *book, error)) {
            delete book;
            return 0;
        }
        if (! readPBC(zip, *book, error)) {
            delete book;
            return 0;
        }
        return book;
    }

    static bool readPBM_V2(QXmlStreamReader &xml, QuaZip &zip, Book &book, QString *error);

    static bool readPBM(QuaZip &zip, Book &book, QString *error)
    {
        if (! zip.setCurrentFile(PBM_FILE)) {
            debug("Not found PBM file: " + PBM_FILE, error);
            return false;
        }
        QuaZipFile zipFile(&zip);
        if (!zipFile.open(QuaZipFile::ReadOnly)) {
            debug("Cannot open " + PBM_FILE + " for reading", error);
            return false;
        }
        QXmlStreamReader xml(&zipFile);

        while (!xml.atEnd()) {
            switch (xml.readNext()) {
            case QXmlStreamReader::StartElement:
            {
                if ("pbm" == xml.name()) {  // root
                    const QStringRef &version = xml.attributes().value("version");
                    if (QString("2.0") == version) {
                        return readPBM_V2(xml, zip, book, error);
                    } else {
                        debug("Unsupported PBM version:" + version.toString(), error);
                        return false;
                    }
                }
            }
                break;
            case QXmlStreamReader::DTD:
            {
                if (xml.dtdName() != "pbm") {
                    debug("Invalid PBM document", error);
                    return false;
                }
            }
                break;
            }
        }
        if (xml.hasError()) {
            debug("PBM error: " + xml.errorString(), error);
        }
        return false;
    }

    typedef QMap<QString, QString> QStringMap;

    static QStringMap toMap(const QXmlStreamAttributes &attributes)
    {
        QStringMap result;
        for (int i = 0; i < attributes.size(); ++i) {
            result.insert(attributes.at(i).name().toString(), attributes.at(i).value().toString());
        }
        return result;
    }

    static bool readPBC_V2(QXmlStreamReader &xml, QuaZip &zip, Book &book, QString *error);

    static bool readPBC(QuaZip &zip, Book &book, QString *error)
    {
        if (! zip.setCurrentFile(PBC_FILE)) {
            debug("Not found PBC file: " + PBC_FILE, error);
            return false;
        }
        QuaZipFile zipFile(&zip);
        QXmlStreamReader xml(&zipFile);
        if (!zipFile.open(QuaZipFile::ReadOnly)) {
            debug("Cannot open " + PBC_FILE + " for reading", error);
            return false;
        }

        while (!xml.atEnd()) {
            switch (xml.readNext()) {
            case QXmlStreamReader::StartElement:
            {
                const QString &version = xml.attributes().value("version").toString();
                if ("2.0" == version) {
                    return readPBC_V2(xml, zip, book, error);
                } else {
                    debug("Unsupported PBC version:" + version, error);
                    return false;
                }
            }
                break;
            case QXmlStreamReader::DTD:
            {
                if (xml.dtdName() != "pbc") {
                    debug("Invalid PBC document", error);
                    return false;
                }
            }
                break;
            }
        }
        if (xml.hasError()) {
            debug("PBC error: " + xml.errorString(), error);
        }
        return false;
    }

    static bool readPBM_V2(QXmlStreamReader &xml, QuaZip &zip, Book &book, QString *error)
    {
        int index = 0, count = -1;
        QString text;
        QStringMap attrs;
        while (!xml.atEnd()) {
            switch (xml.readNext()) {
            case QXmlStreamReader::StartElement:
            {
                if ("attr" == xml.name()) {
                    if (count < 0 || index < count) {
                        attrs = toMap(xml.attributes());
                    }
                } else if ("item" == xml.name()) {
                    if (count < 0 || index < count) {
                        const QString &name = xml.attributes().value("name").toString();
                        const QString &type = xml.attributes().value("type").toString().toLower();
                        if (name.isEmpty()) {
                            qWarning() << "Item name is empty";
                        } else {
                            if (type == "file") {
                                // process in object
                                attrs = toMap(xml.attributes());
                            } else if (type == "number") {
                                bool ok;
                                int n = xml.attributes().value("value").toString().toInt(&ok);
                                if (ok) {
                                    book.setItem(name, n);
                                } else {
                                    qWarning() << "Invalid number";
                                }
                            } else {
                                book.setItem(name, xml.attributes().value("value").toString());
                            }
                        }
                    }
                } else if ("object" == xml.name()) {
                    if (count < 0 || index < count) {
                        attrs.unite(toMap(xml.attributes()));
                        const QString &name = attrs["name"], &href = attrs["href"];
                        if (href.isEmpty()) {
                            qWarning() << "href is empty";
                        } else {
                            FileObject *file = FileFactory::getFile(&zip, href, attrs["media-type"], &book);
                            if (file != 0) {
                                book.setItem(name, file);
                            }
                        }
                    }
                } else if ("metadata" == xml.name() || "extension" == xml.name()) {
                    const QString &s = xml.attributes().value("count").toString();
                    bool ok;
                    count = s.toInt(&ok);
                    if (! ok) {
                        qWarning() << "Except integer value of \"count\":" << s;
                        count = -1;
                    }
                    index = 0;
                }
            }
                break;
            case QXmlStreamReader::Characters:
            {
                text.append(xml.text());
            }
                break;
            case QXmlStreamReader::EndElement:
            {
                if ("attr" == xml.name()) {
                    if (count < 0 || index < count) {
                        const QString &name = attrs["name"];
                        if (name.isEmpty()) {
                            qWarning() << "Empty \"name\" attribute of attr node";
                        } else {
                            const QString &value = text.trimmed();
                            if (name == "cover") {
                                FileObject *cover = FileFactory::getFile(&zip, value,
                                                                         attrs["media-type"], &book);
                                book.setCover(cover);
                            } else if ("date" == name) {
                                QDateTime datetime = QDateTime::fromString(value, DATE_FORMAT);
                                if (datetime.isValid()) {
                                    book.setDate(datetime.date());
                                } else {
                                    qWarning() << "Invalid date format:" << value << ", expect" << DATE_FORMAT;
                                }
                            } else if ("intro" == name) {
                                book.setIntroText(value);
                            } else {
                                book.setAttribute(name, value);
                            }
                        }
                    }
                    ++index;
                } else if ("item" == xml.name()) {
                    ++index;
                }
                text.clear();
            }
                break;
            }
        }
        if (xml.hasError()) {
            debug("PBM error: " + xml.errorString(), error);
            return false;
        }
        return true;
    }

    static bool readPBC_V2(QXmlStreamReader &xml, QuaZip &zip, Book &book, QString *error)
    {
        QStack<int> indexs, counts;
        QStack<Chapter*> chapters;
        QString text;
        while (!xml.atEnd()) {
            switch (xml.readNext()) {
            case QXmlStreamReader::StartElement:
            {
                if ("chapter" == xml.name()) {
                    if (counts.top() < 0 || indexs.top() < counts.top()) {
                        const QString &href = xml.attributes().value("href").toString(), &encoding =
                                xml.attributes().value("encoding").toString();
                        const QString &count = xml.attributes().value("count").toString();
                        int n = -1;
                        if (! count.isEmpty()) {
                            bool ok;
                            n = count.toInt(&ok);
                            if (! ok) {
                                qWarning() << "Except integer value of \"count\":" << count;
                                n = -1;
                            }
                        }
                        counts.push(n);
                        indexs.push(0);
                        chapters.push(chapters.top()->newChapter(""));
                        if (! href.isEmpty()) {     // not section
                            FileObject *file = FileFactory::getFile(&zip, href, "", chapters.top());
                            if (file != 0) {
                                chapters.top()->setFile(file, encoding.toLatin1());
                            }
                        }
                        ++indexs.top();
                    }
                } else if ("cover" == xml.name()) {
                    if (counts.top() < 0 || indexs.top() < counts.top()) {
                        Q_ASSERT(chapters.top() != 0);
                        const QString &href = xml.attributes().value("href").toString(), &mime =
                                xml.attributes().value("media-type").toString();
                        if (href.isEmpty() || mime.isEmpty()) {
                            qWarning() << "\"href\" and \"media-type\" of \"cover\" cannot be empty";
                        } else {
                            FileObject *file = FileFactory::getFile(&zip, href, mime, chapters.top());
                            if (file != 0) {
                                chapters.top()->setCover(file);
                            }
                        }
                    }
                } else if ("intro" == xml.name()) {
                    if (counts.top() < 0 || indexs.top() < counts.top()) {
                        Q_ASSERT(chapters.top() != 0);
                        const QString &href = xml.attributes().value("href").toString(), &encoding =
                                xml.attributes().value("encoding").toString();
                        if (href.isEmpty() || encoding.isEmpty()) {
                            qWarning() << "\"href\" and \"media-type\" of \"intro\" cannot be empty";
                        } else {
                            FileObject *file = FileFactory::getFile(&zip, href, "", chapters.top());
                            if (file != 0) {
                                chapters.top()->setIntroFile(file, encoding.toLatin1());
                            }
                        }
                    }
                } else if ("contents" == xml.name()) {
                    const QString &count = xml.attributes().value("count").toString();
                    int n = -1;
                    if (! count.isEmpty()) {
                        bool ok;
                        n = count.toInt(&ok);
                        if (! ok) {
                            qWarning() << "Except integer value of \"count\":" << count;
                            n = -1;
                        }
                    }
                    counts.push(n);
                    indexs.push(0);
                    chapters.push(&book);
                }
            }
                break;
            case QXmlStreamReader::Characters:
            {
                text.append(xml.text());
            }
                break;
            case QXmlStreamReader::EndElement:
            {
                if ("chapter" == xml.name()) {
                    if (counts.top() < 0 || indexs.top() < counts.top()) {
                        indexs.pop();
                        counts.pop();
                        chapters.pop();
                    }
                } else if ("title" == xml.name()) {
                    if (counts.top() < 0 || indexs.top() < counts.top()) {
                        Q_ASSERT(chapters.top() != 0);
                        chapters.top()->setTitle(text.trimmed());
                    }
                }
                text.clear();
            }
                break;
            }
        }
        if (xml.hasError()) {
            debug("PBC error: " + xml.errorString(), error);
            return false;
        }
        return true;
    }

    bool PMAB::makePmab(const Book &book, QIODevice &device, const QVariantMap &args, QString *error)
    {
        Q_ASSERT(device.isWritable());
        qDebug() << book.title() << args;
        debug("PMAB maker is under development", error);
        return false;
    }
}   // pmab

QEM_END_NAMESPACE
