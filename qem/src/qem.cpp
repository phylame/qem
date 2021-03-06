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

#include <qem.h>
#include <utils.h>
#include <fileutils.h>
#include <formats/all.h>
#include <QDate>
#include <QFile>
#include <QtDebug>
#include <QTextStream>
#include <QStringList>

QEM_BEGIN_NAMESPACE

const QString Qem::VERSION(QEM_VERSION " (built " __DATE__ ")");

const QString Qem::FORMAT_PMAB("pmab");

QString Qem::variantType(const QVariant &v)
{
    QString name;
    switch (v.type()) {
    case QVariant::String:
    case QVariant::Char:
        name = "str";
        break;
    case QVariant::Date:
        name = "date";
        break;
    case QVariant::Time:
        name = "time";
        break;
    case QVariant::DateTime:
        name = "datetime";
        break;
    case QVariant::Int:
        name = "int";
        break;
    case QVariant::UInt:
        name = "uint";
        break;
    case QVariant::Bool:
        name = "bool";
        break;
    case QVariant::ByteArray:
        name = "bytes";
        break;
    case QVariant::Double:
        name = "real";
        break;
    default:
    {
        if (v.canConvert<FileObjectPointer>()) {
            name = "file";
        } else if (v.canConvert<TextObject>()) {
            name = "text";
        } else if (v.canConvert<QObject*>()) {
            name = v.value<QObject*>()->metaObject()->className();
        }
    }
        break;
    }
    return name;
}

QString Qem::formatVariant(const QVariant &value)
{
    QString s;
    if (value.isNull()) {
        return s;
    }
    switch (value.type()) {
    case QVariant::String:
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::Bool:
    {
        s = value.toString();
    }
        break;
    case QVariant::Date:
    {
        s = value.toDate().toString("yyyy-M-d");
    }
        break;
    case QVariant::Time:
    {
        s = value.toTime().toString("H:m:s");
    }
        break;
    case QVariant::DateTime:
    {
        s = value.toDateTime().toString("yyyy-M-d H:m:s");
    }
        break;
    default:
    {
        if (value.canConvert<FileObjectPointer>()) {
            FileObjectPointer p = value.value<FileObjectPointer>();
            if (p != 0) {
                s = p->name();
            }
        } else if (value.canConvert<TextObject>()) {
            s = value.value<TextObject>().text();
        } else if (value.canConvert<QObject*>()) {     // convert by class name
            QObject *obj = value.value<QObject*>();
            const char *className = obj->metaObject()->className();
            if (QString("FileObject") == className) {       // from QML invoke
                FileObject *file = dynamic_cast<FileObject*>(obj);
                if (file != 0) {
                    s = file->name();
                }
            }
        }
    }
        break;
    }
    return s;
}

void Qem::printProperties(const Part &part, QString sep, QStringList names, bool showBrackets,
                          QTextStream *out)
{
    QTextStream orig(stdout, QIODevice::WriteOnly), *cout = out;
    if (0 == cout) {
        cout = &orig;
    }
    QStringList sp;
    for (QStringList::const_iterator i = names.constBegin(); i != names.constEnd(); ++i) {
        const QString &key = *i;
        const QVariant &v = part.attribute(key);
        if (v.isNull()) {
            continue;
        }
        QString s = formatVariant(v);
        if (! s.isEmpty()) {
            sp << key + "=\"" + s + "\"";
        }
    }
    if (showBrackets) {
        *cout << "<" << sp.join(sep) << ">";
    } else {
        *cout << sp.join(sep);
    }
    *cout << endl;
}

void Qem::walkPart(Part &part, Walker walker)
{
    Q_ASSERT(walker != 0);
    if (! walker(part)) {
        return;
    }
    for (int ix = 0; ix < part.size(); ++ix) {
        Part *p = part.get(ix);
        Q_ASSERT(p != 0);
        walkPart(*p, walker);
    }
}

// parse and maker
struct BookDesc
{
    QString m_name;
    Qem::Parser m_parser;
    Qem::Maker m_maker;

    BookDesc() {}
    inline BookDesc(const QString &name, Qem::Parser parser, Qem::Maker maker) :
        m_name(name), m_parser(parser), m_maker(maker)
    {}

    BookDesc& operator =(const BookDesc &o)
    {
        this->m_name = o.m_name;
        this->m_parser = o.m_parser;
        this->m_maker = o.m_maker;
        return *this;
    }
};

static QMap<QString, BookDesc> _books;
static bool _isRegisteredInner = false;

static void registerInner()
{
    using txt::TXT;
    _books.insert(TXT::FORMAT_NAME, BookDesc(TXT::FORMAT_NAME, TXT::parseTxt, TXT::makeTxt));
    using umd::UMD;
    _books.insert(UMD::FORMAT_NAME, BookDesc(UMD::FORMAT_NAME, UMD::parseUmd, 0));
    using jar::JAR;
    _books.insert(JAR::FORMAT_NAME, BookDesc(JAR::FORMAT_NAME, JAR::parseJar, 0));
    using pmab::PMAB;
    _books.insert(PMAB::FORMAT_NAME, BookDesc(PMAB::FORMAT_NAME, PMAB::parsePmab, 0));
    using epub::EPUB;
    _books.insert(EPUB::FORMAT_NAME, BookDesc(EPUB::FORMAT_NAME, 0, EPUB::makeEpub));
    _isRegisteredInner = true;
}

// parser
void Qem::registerParser(const QString &format, Qem::Parser parser)
{
    if (! _isRegisteredInner) {
        registerInner();
    }
    if (0 == parser) {
        return;
    }
    if (! _books.contains(format)) {
        _books.insert(format, BookDesc(format, parser, 0));
    } else {
        BookDesc b = _books.value(format);
        b.m_parser = parser;
    }
}

bool Qem::hasParser(const QString &format)
{
    if (! _isRegisteredInner) {
        registerInner();
    }
    if (! _books.contains(format)) {
        return false;
    }
    return _books.value(format).m_parser != 0;
}

QStringList Qem::getSupportedParser()
{
    if (! _isRegisteredInner) {
        registerInner();
    }
    QStringList rev;
    for (QMap<QString, BookDesc>::const_iterator i = _books.constBegin(); i != _books.constEnd(); ++i) {
        const BookDesc &b = *i;
        if (b.m_parser != 0) {
            rev << b.m_name;
        }
    }
    return rev;
}

Qem::Parser Qem::getParser(const QString &format)
{
    if (! _isRegisteredInner) {
        registerInner();
    }
    if (! _books.contains(format)) {
        return 0;
    }
    return _books.value(format).m_parser;
}

// maker
void Qem::registerMaker(const QString &format, Qem::Maker maker)
{
    if (! _isRegisteredInner) {
        registerInner();
    }
    if (0 == maker) {
        return;
    }
    if (! _books.contains(format)) {
        _books.insert(format, BookDesc(format, 0, maker));
    } else {
        BookDesc b = _books.value(format);
        b.m_maker = maker;
    }
}

bool Qem::hasMaker(const QString &format)
{
    if (! _isRegisteredInner) {
        registerInner();
    }
    if (! _books.contains(format)) {
        return false;
    }
    return _books.value(format).m_maker != 0;
}

QStringList Qem::getSupportedMaker()
{
    if (! _isRegisteredInner) {
        registerInner();
    }
    QStringList rev;
    for (QMap<QString, BookDesc>::const_iterator i = _books.constBegin(); i != _books.constEnd(); ++i) {
        const BookDesc &b = *i;
        if (b.m_maker != 0) {
            rev << b.m_name;
        }
    }
    return rev;
}

Qem::Maker Qem::getMaker(const QString &format)
{
    if (! _isRegisteredInner) {
        registerInner();
    }
    if (! _books.contains(format)) {
        return 0;
    }
    return _books.value(format).m_maker;
}

static bool openReadDevice(QIODevice &device)
{
    if (device.isOpen()) {
        if (device.isReadable()) {
            return true;
        } else {
            return false;
        }
    } else {
        return device.open(QIODevice::ReadOnly);
    }
}

static bool openWriteDevice(QIODevice &device)
{
    if (device.isOpen()) {
        if (device.isWritable()) {
            return true;
        } else {
            return false;
        }
    } else {
        return device.open(QIODevice::WriteOnly);
    }
}

Book* Qem::readBook(const QString &name, const QString &format, const QVariantMap &args, QString *error)
{
    QFile *file = new QFile(name);
    if (! file->exists()) {
        debug("Not such file or directory: "+name, error);
        return 0;
    }
    QString fmt;
    if (format.isEmpty()) {
        fmt = FileUtils::extensionName(name);
    } else {
        fmt = format;
    }
    Book *book = readBook(*file, fmt, args, error);
    if (book != 0) {
        book->setAttribute("source_path", name);
        file->setParent(book);
    } else {
        file->close();
        delete file;
    }
    return book;
}

Book* Qem::readBook(QIODevice &device, const QString &format, const QVariantMap &args, QString *error)
{
    Parser parser = getParser(format);
    if (0 == parser) {
        debug("Not found parser for: "+format, error);
        return 0;
    }
    // open the device if necessary
    if (! openReadDevice(device)) {
        debug("Cannot open device for read", error);
        return 0;
    }
    Book *book = parser(device, args, error);
    if (book != 0) {
        book->setAttribute("source_format", format);
    }
    return book;
}

bool Qem::writeBook(const Book &book, const QString &name, const QString &format,
                const QVariantMap &args, QString *error)
{
    QFile file(name);
    QString fmt;
    if (format.isEmpty()) {
        fmt = FileUtils::extensionName(name);
    } else {
        fmt = format;
    }
    bool rev = writeBook(book, file, fmt, args, error);
    if (file.isOpen()) {
        file.close();
    }
    return rev;
}

bool Qem::writeBook(const Book &book, QIODevice &device, const QString &format,
                const QVariantMap &args, QString *error)
{
    Maker maker = getMaker(format);
    if (0 == maker) {
        debug("Not found maker for: "+format, error);
        return false;
    }
    // open the device if necessary
    if (! openWriteDevice(device)) {
        debug("Cannot open device for write", error);
        return 0;
    }
    return maker(book, device, args, error);
}

bool Qem::convertBook(QIODevice &in, QString inFormat, const QVariantMap &parseArgs,
                      QIODevice &out, QString outFormat, const QVariantMap &makeArgs) {
    Book *book = readBook(in, inFormat, parseArgs, 0);
    if (0 == book) {
        return false;
    }
    bool ret = writeBook(*book, out, outFormat, makeArgs, 0);
    delete book;
    return ret;
}

QEM_END_NAMESPACE
