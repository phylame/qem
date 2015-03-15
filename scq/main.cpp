/*
 * Copyright 2014-20-20 Peng Wan
 *
 * This file is part of SCQ.
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

#include <QCoreApplication>
#include <QTextStream>
#include <QTranslator>
#include <QFileInfo>
#include <QtDebug>
#include <QRegExp>
#include <QFile>
#include <QDir>
#include <qem.h>
#include <fileutils.h>
#include <filefactory.h>
#include "cli.h"

QEM_USE_NAMESPACE

// standard output and error
static QTextStream cout(stdout, QIODevice::WriteOnly);
static QTextStream cerr(stderr, QIODevice::WriteOnly);

static QTextStream& printError(const QString &s)
{
    cerr << QFileInfo(QCoreApplication::arguments()[0]).fileName() << ": " << s;
    return cerr;
}

static const QString ORDER_SEPARATOR("$");

static const QString OPTIONS("hvluf:o:c:p:j:V:P:M:S:");

static QVariantMap parseValues(const QStringList &values)
{
    QVariantMap result;
    foreach (const QString &s, values) {
        int index = s.indexOf('=');
        if (index < 0) {
            printError(QObject::tr("Except name=value pair")) << endl;
            QCoreApplication::exit(-1);
        }
        result.insert(s.left(index), s.mid(index+1));
    }
    return result;
}

static void showVersion()
{
    cout << "SCI for Qem " << QCoreApplication::applicationVersion()
        << " by " << QObject::tr("PW") << endl;
    cout << "Qem core: " << Qem::VERSION << endl;
    cout << QObject::tr("Copyright (C) 2014-2015 Peng Wan, PW") << endl;
}

static void showHelp()
{
    cout << QObject::tr("Usage: %1 [options] files...").arg(QFileInfo(QCoreApplication::arguments()[0]).fileName()) << endl;
    cout << QObject::tr("Options:") << endl;
    cout << QString(" -c %1").arg(QObject::tr("<format>"), -20) << QObject::tr("Convert file to specified format") << endl;
    cout << QString(" -f %1").arg(QObject::tr("<format>"), -20) << QObject::tr("Specify format of input file") << endl;
    cout << QString(" -h %1").arg("", -20) << QObject::tr("Print help message") << endl;
    cout << QString(" -j %1").arg(QObject::tr("<format>"), -20) << QObject::tr("Join input files to one file named name") << endl;
    cout << QString(" -l %1").arg("", -20) << QObject::tr("List supported formats") << endl;
    cout << QString(" -M %1").arg(QObject::tr("<name=value>"), -20) << QObject::tr("Set make arguments") << endl;
    cout << QString(" -o %1").arg(QObject::tr("<path>"), -20) << QObject::tr("Specify the output directory") << endl;
    cout << QString(" -p %1").arg(QObject::tr("<format>"), -20) << QObject::tr("Convert PMAB to specified format") << endl;
    cout << QString(" -P %1").arg(QObject::tr("<name=value>"), -20) << QObject::tr("Set parse arguments") << endl;
    cout << QString(" -S %1").arg(QObject::tr("<name=value>"), -20) << QObject::tr("Set properties to output book") << endl;
    cout << QString(" -u %1").arg("", -20) << QObject::tr("Convert file to PMAB") << endl;
    cout << QString(" -v %1").arg("", -20) << QObject::tr("Print version information") << endl;
    cout << QString(" -V %1").arg(QObject::tr("<name>"), -20) << QObject::tr("View properties of book or chapters") << endl;
    cout << QString("    %1").arg("", -18) << QString("all:    ") << QObject::tr("Print all properties(default)") << endl;
    cout << QString("    %1").arg("", -18) << QString("all_names:    ") << QObject::tr("Print available names") << endl;
    cout << QString("    %1").arg("", -18) << QString("items:    ") << QObject::tr("Print all items of book") << endl;
    cout << QString("    %1").arg("", -18) << QString("toc:    ") << QObject::tr("Print tree of TOC (table of contents)") << endl;
    cout << QString("    %1").arg("", -18) << QString("chapterN.N..N%1name:    ").arg(ORDER_SEPARATOR) <<
            QObject::tr("Print chapter properties") << endl;
    cout << QString("    %1").arg("", -18) << QString("title, author, genre, publisher, size...") << endl;
    cout << endl;
    cout << QObject::tr("The PMAB format is ") << "\"" << Qem::FORMAT_PMAB << "\"." << endl;
}

template <typename T>
static QList<T> map(const QList<T> &list, T (*func)(const T &v))
{
    Q_ASSERT(func != 0);
    QList<T> result(list);
    for (int i=0; i<result.size(); ++i) {
        result[i] = func(result[i]);
    }
    return result;
}

static QString toUpper(const QString &s)
{
    return s.toUpper();
}

static void showSupported()
{
    cout << QObject::tr("The following list contains supported formats") << endl;
    QStringList parser = map(Qem::getSupportedParser(), toUpper);
    const QString &in = QObject::tr("Input:");
    const QString &out = QObject::tr("Output:");
    int width = qMax(in.length(), out.length()) + 1;
    cout << QString(" %1").arg(in, -width) << parser.join(", ") << endl;
    QStringList maker = map(Qem::getSupportedMaker(), toUpper);
    cout << QString(" %1").arg(out, -width) << maker.join(", ") << endl;
}

static const QString DEFALUT_NAME("text");

typedef QMap<QString, QString> StringMap;
static void setProperties(Book *book, const QVariantMap &properties)
{
    for (QVariantMap::const_iterator i=properties.begin(); i != properties.end(); i++) {
        const QString &key = i.key();
        const QVariant &v = i.value();
        if (key == "cover") {
            FileObject *cover = FileFactory::getFile(v.toString(), "", book);
            if (0 == cover) {
                printError(QObject::tr("Not found image: ")) << v.toString() << endl;
            } else {
                book->setCover(cover);
            }
        } else if (key == "date") {
            QDate date = QDate::fromString(v.toString(), "yyyy-M-d");
            if (! date.isValid()) {
                printError(QObject::tr("Except date format \"yyyy-M-d\": ")) << v.toString() << endl;
            } else {
                book->setDate(date);
            }
        } else if (key == "intro") {
            book->setIntroText(v.toString());
        } else {
            book->setAttribute(key, v);
        }
    }
}

static Book* openBook(QFile &file, const QString &inFormat, const QVariantMap &inArgs,
            const QVariantMap &properties)
{
    if (! file.open(QFile::ReadOnly)) {
        printError(QObject::tr("Cannot open file: ")) << file.fileName() << endl;
        return 0;
    }
    QString fmt = inFormat.isEmpty() ? FileUtils::extensionName(file.fileName()) : inFormat;
    Book *book = Qem::readBook(file, fmt, inArgs);
    if (0 == book) {
        printError(QObject::tr("Cannot read book: ")) << file.fileName() << endl;
        file.close();
    } else {
        book->setAttribute("source_path", file.fileName());
        setProperties(book, properties);
    }
    return book;
}

static bool saveBook(const Book &book, const QString &output, const QString &format,
                     const QVariantMap &args, QString *writtenFile = 0)
{
    QFileInfo fileInfo(output);
    QString name;
    if (fileInfo.isDir()) {
        name = fileInfo.dir().dirName() + QDir::separator() + book.title() + "." + format;
    } else {
        name = output;
    }
    if (writtenFile != 0) {
        *writtenFile = name;
    }
    return Qem::writeBook(book, name, format, args);
}

static void convertBook(QFile &file, const QString &inFormat, const QVariantMap &inArgs,
            const QVariantMap &properties, const QString &output, const QString &outFormat,
            const QVariantMap &outArgs)
{
    Book *book = openBook(file, inFormat, inArgs, properties);
    if (0 == book) {
        return;
    }
    QString name;
    bool ret = saveBook(*book, output, outFormat, outArgs, &name);
    delete book;
    file.close();
    if (ret) {
        cout << name << endl;
    } else {
        printError(QObject::tr("Failed to write book: ")) << name << endl;
    }
}

static void unpackBook(QFile &file, const QString &inFormat, const QVariantMap &inArgs,
            const QVariantMap &properties, const QString &output, const QVariantMap &outArgs)
{
    convertBook(file, inFormat, inArgs, properties, output, Qem::FORMAT_PMAB, outArgs);
}

static void packBook(QFile &file, const QVariantMap &inArgs, const QVariantMap &properties,
            const QString &output, const QString &outFormat, const QVariantMap &outArgs)
{
    convertBook(file, Qem::FORMAT_PMAB, inArgs, properties, output, outFormat, outArgs);
}

static void walkTree(const Part &part, const QString &prefix, bool showProperties, bool showOrder) {
    cout << prefix;
    if (showProperties) {
        Qem::printProperties(part, " ", QStringList() << "title" << "cover", true, &cout);
    }
    int n = 1;
    for (Part::const_iterator i = part.begin(); i != part.end(); ++i) {
        QString s = prefix + "    ";
        if (showOrder) {
            s += QString().setNum(n++) + " ";
        }
        walkTree(**i, s, showProperties, showOrder);
    }
}

static void walkPart(const Part &part, const QString &path, bool showBookAttr = true,
                     bool showChapterAttr = true, bool showOrder = true)
{
    if (showBookAttr) {
        Qem::printProperties(part, "\n", part.attributeNames(), false, &cout);
    }
    cout << QObject::tr("Contents of ") << "\"" << part.title() << "\"";
    if (! path.isEmpty()) {
        cout << QObject::tr(" in ") << "\"" << path << "\"";
    }
    cout << endl;
    int n = 1;
    for (Part::const_iterator i=part.begin(); i != part.end(); ++i) {
        QString prefix;
        if (showOrder) {
            prefix = QString().setNum(n++) + " ";
        }
        walkTree(**i, prefix, showChapterAttr, showOrder);
    }
}

static void viewProperty(const Part &part, const QString &key)
{
    if (key == "text") {
        if (part.isSection()) {
            printError(QObject::tr("Given chapter is section:")) << part.title() << endl;
        } else {
            cout << part.content() << endl;
        }
    } else if (key == "all_names") {
        cout << part.title() << ": " << QStringList(part.attributeNames()).join(", ") << endl;
    } else if (key == "all") {
        Qem::printProperties(part, "\n", part.attributeNames(), false, &cout);
    } else if (key == "size") {
        cout << key << "=" << part.size() << endl;
    } else if (key == "items") {
        const Book &book = (Book&) part;
        foreach (const QString &name, book.itemNames()) {
            const QVariant &value = book.getItem(name);
            cout << "Item: name=\"" << name << "\", type=\"" << Qem::variantType(value) <<
                    "\", value=\"" << Qem::formatVariant(value) << "\"" << endl;
        }
    } else {
        cout << key << "=" << Qem::formatVariant(part.attribute(key)) << endl;
    }
}

static void viewPart(const Part &part, const QString &path, const QStringList &names);

static void splitOrder(const Part &owner, const QString &orders)
{
    QList<int> result;
    QStringList items = orders.split(ORDER_SEPARATOR, QString::SkipEmptyParts);
    const QStringList &ls = items.value(0).split(".");
    foreach (const QString &s, ls) {
        int n = s.toInt();
        if (0 == n) {
            printError(QObject::tr("Chapter order cannot be 0")) << endl;
            return;
        } else if (n > 0) {
            --n;    // begin from 1
        }
        result << n;
    }
    Part *part = owner.findPart(result);
    if (0 == part) {
        printError(QObject::tr("Not found chapter: ")) << items[0] << endl;
        return;
    }
    QStringList names;
    if (items.size() > 1) {
        names << items[1];
    } else {
        names << DEFALUT_NAME;
    }
    viewPart(*part, QString(), names);
}

void viewPart(const Part &part, const QString &path, const QStringList &names)
{
    QRegExp regex("chapter(\\-?[\\d]+\\.)*\\-?[\\d]*(\\$.*)?");
    foreach (const QString &name, names) {
        if ("toc" == name) {
            walkPart(part, path, false, true, true);
        } else if (regex.exactMatch(name)) {
            splitOrder(part, QString(name).replace("chapter", ""));
        } else {
            viewProperty(part, name);
        }
    }
}

static void viewBook(QFile &file, const QString &inFormat, QStringList names, const QVariantMap &inArgs,
                     const QVariantMap &properties)
{
    Book *book = openBook(file, inFormat, inArgs, properties);
    if (0 == book) {
        return;
    }
    viewPart(*book, file.fileName(), names);
    cout.flush();
    delete book;
}

static void joinBook(const QList<QString> &files, const QVariantMap &inArgs,
                     const QVariantMap &properties,const QString &output,
                     const QString &outFormat, const QVariantMap &outArgs)
{
    Book book;
    setProperties(&book, properties);
    QList<Book*> opened;
    foreach (const QString &name, files) {
        QFile *file = new QFile(name, &book);
        Book *sub = openBook(*file, QString(), inArgs, QVariantMap());
        if (sub != 0) {
            book.append(sub);
            opened.append(sub);
        }
    }
    QString name;
    bool ret = saveBook(book, output, outFormat, outArgs, &name);
    // destroy all opened sub-books
    foreach (Book *sub, opened) {
        delete sub;
    }
    if (ret) {
        cout << name << endl;
    } else {
        printError(QObject::tr("Failed to write book: ")) << name << endl;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("qem");
    app.setApplicationVersion(SCQ_VERSION);
    app.setOrganizationName("PW");

    QTranslator translator;
    translator.load(QLocale::system(), app.applicationName(), "_", app.applicationDirPath() +
                    QDir::separator() + "i18n");
    app.installTranslator(&translator);

    enum Command {Unpack, Pack, Convert, View, Join};
    Command cmd = View;
    QString inFormat, outFormat(Qem::FORMAT_PMAB);
    QVariantMap inArgs, outArgs;
    QVariantMap bookProperties;
    QString output(".");
    QStringList viewNames;
    viewNames << "all";

    ArgumentMap am;
    if (! parseArguments(app.arguments(), OPTIONS, am, &cerr, app.applicationName()+": ")) {
        return -1;
    }
    for (ArgumentMap::const_iterator i=am.begin(); i != am.end(); ++i) {
        const QString &opt = i.key();
        const QStringList &values = i.value();
        switch (opt.at(0).toLatin1()) {
        case 'u':
        {
            cmd = Unpack;
        }
            break;
        case 'p':
        {
            outFormat = values.last().toLower();
            if (! Qem::getSupportedMaker().contains(outFormat)) {
                printError(QObject::tr("Unsupported format: ")) << outFormat.toUpper() << endl;
                return -1;
            }
            cmd = Pack;
        }
            break;
        case 'c':
        {
            outFormat = values.last().toLower();
            if (! Qem::getSupportedMaker().contains(outFormat)) {
                printError(QObject::tr("Unsupported format: ")) << outFormat.toUpper() << endl;
                return -1;
            }
            cmd = Convert;
        }
            break;
        case 'f':
        {
            inFormat = values.last().toLower();
            if (! Qem::getSupportedParser().contains(inFormat)) {
                printError(QObject::tr("Unsupported format: ")) << inFormat.toUpper() << endl;
                return -1;
            }
        }
            break;
        case 'o':
        {
            output = values.last();
        }
            break;
        case 'P':
        {
            inArgs = parseValues(values);
        }
            break;
        case 'M':
        {
            outArgs = parseValues(values);
        }
            break;
        case 'S':
        {
            bookProperties = parseValues(values);
        }
            break;
        case 'j':
        {
            outFormat = values.last().toLower();
            if (! Qem::getSupportedMaker().contains(outFormat)) {
                printError(QObject::tr("Unsupported format: ")) << outFormat.toUpper() << endl;
                return -1;
            }
            cmd = Join;
        }
            break;
        case 'V':
        {
            viewNames = values;
            cmd = View;
        }
            break;
        case 'l':
        {
            showSupported();
            return 0;
        }
        case 'h':
        {
            showHelp();
            return 0;
        }
            break;
        case 'v':
        {
            showVersion();
            return 0;
        }
            break;
        }
    }
    QStringList files = am.value(OTHER_KEY);
    if (files.size() == 0) {
        printError(QObject::tr("Not input files")) << endl;
        return 0;
    }
    if (Join == cmd) {
        joinBook(files, inArgs, bookProperties, output, outFormat, outArgs);
        return 0;
    }
    foreach (const QString &name, files) {
        QFile file(name);
        if (! file.exists()) {
            printError(QObject::tr("Not such file or directory: ")) << name << endl;
            continue;
        }
        switch (cmd) {
        case Unpack:
            unpackBook(file, inFormat, inArgs, bookProperties, output, outArgs);
            break;
        case Pack:
            packBook(file, inArgs, bookProperties, output, outFormat, outArgs);
            break;
        case View:
            viewBook(file, inFormat, viewNames, inArgs, bookProperties);
            break;
        case Convert:
            convertBook(file, inFormat, inArgs, bookProperties, output, outFormat, outArgs);
            break;
        }
    }
    return 0;
}
