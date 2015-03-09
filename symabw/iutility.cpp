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

#include "iutility.h"
#include <qem.h>
#include <fileutils.h>
#include <filefactory.h>
#include <QUrl>
#include <QFile>
#include <QtDebug>
#include <QFileInfo>
#include <QClipboard>
#include <QFileDialog>
#include <QApplication>
#include <QTemporaryFile>
#include <QDesktopServices>


namespace symabw {

QString IUtility::selectFile(const QString &caption, const QString &dir, const QString &filter,
                             QString *selectedFilter)
{
    return QFileDialog::getOpenFileName(0, caption, dir, filter, selectedFilter, 0);
}

QString IUtility::selectFolder(const QString &caption, const QString &dir)
{
    return QFileDialog::getExistingDirectory( 0, caption, dir, QFileDialog::ShowDirsOnly );
}

bool IUtility::openInSystem(const QString &url)
{
    return QDesktopServices::openUrl(QUrl(url));
}

void IUtility::copyToClipbord(const QString &text)
{
    qApp->clipboard()->setText(text);
}

QString IUtility::baseName(const QString &name)
{
    return QFileInfo(name).baseName();
}

QString IUtility::fileName(const QString &name)
{
    return QFileInfo(name).fileName();
}

QString IUtility::dirName(const QString &name)
{
    return QFileInfo(name).dir().path();
}

QString IUtility::extensionName(const QString &name)
{
    return FileUtils::extensionName(name);
}

qint64 IUtility::fileSize(const QString &name)
{
    return QFileInfo(name).size();
}

QDateTime IUtility::modifiedDate(const QString &name)
{
    return QFileInfo(name).lastModified();
}

bool IUtility::fileExists(const QString &name)
{
    return QFileInfo(name).exists();
}

bool IUtility::sameFile(const QString &file1, const QString &file2)
{
    return QFileInfo(file1) == QFileInfo(file2);
}

bool IUtility::renameFile(const QString &orig, const QString &dest)
{
    return QFile::rename(orig, dest);
}

bool IUtility::deleteFile(const QString &name)
{
    return QFile::remove(name);
}

bool IUtility::copyFile(const QString &src, const QString &dest)
{
    return QFile::copy(src, dest);
}

void IUtility::setCover(Book *book, const QString &path)
{
    Q_ASSERT(book != 0);
    FileObject* cover = FileFactory::getFileObject(path, "", book);
    if (cover != 0) {
        book->setCover(cover);
    } else {
        m_error = "Cannot load cover: "+path;
    }
}

Book* IUtility::newBook(const QString &title, const QString &author, QObject *parent)
{
    if (0 == parent) {
        parent = this;
    }
    Book *book = new Book(title, author, parent);
    Q_ASSERT(book != 0);
    setCover(book, ":/bookCover");
    book->setRights(Imabw::rights());
    book->setLanguage(QLocale::system().name());
    return book;
}

QString IUtility::pmab()
{
    return Qem::FORMAT_PMAB;
}

QStringList IUtility::getSupportedParser()
{
    return Qem::getSupportedParser();
}

QStringList IUtility::getSupportedMaker()
{
    return Qem::getSupportedMaker();
}

Book* IUtility::readBook(const QString &name, const QString &format, const QVariantMap &args)
{
    return Qem::readBook(name, format, args, &m_error);
}

bool IUtility::writeBook(QObject *part, const QString &name, const QString &format,
                const QVariantMap &args)
{
    Chapter *p = dynamic_cast<Chapter*>(part);
    if (p) {
        return Qem::writeBook(Book(*p), name, format, args, &m_error);
    } else {
        m_error = "Not a Book pointer";
        return false;
    }
}

bool IUtility::saveText(Part *part, const QString &text)
{
    Q_ASSERT(part != 0);
    QTemporaryFile *file = new QTemporaryFile(part);
    if (!file->open()) {
        m_error = file->errorString();
        delete file;
        return false;
    }
    QTextStream out(file);
    out.setCodec("UTF-16LE");
    out << text;
    out.flush();
    file->close();
    FileObject *fb = FileFactory::getFileObject(file->fileName(), "text/plain", part);
    if (0 == fb) {
        delete file;
        return false;
    }
    part->setFile(fb, "UTF-16lE");
    return true;
}

QString IUtility::loadText(const QString &path, const QByteArray &codec)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        return QString();
    }
    QTextStream in(&file);
    if (!codec.isEmpty()) {
        in.setCodec(codec.constData());
    }
    const QString &rev = in.readAll();
    file.close();
    return rev;
}

QString IUtility::formatVariant(const QVariant &v)
{
    return Qem::formatVariant(v);
}

QString IUtility::variantType(const QVariant &v)
{
    if (v.canConvert<QObject*>()) {
        QObject *obj = v.value<QObject*>();
        if (0 == obj) {
            return QString();
        } else {
            return obj->metaObject()->className();
        }
    } else {
        return v.typeName();
    }
}

FileObject* IUtility::getFileObject(const QString &name, const QString &mime, QObject *parent)
{
    return FileFactory::getFileObject(name, mime, parent);
}

FileObject* IUtility::getFileObject(const QString &name, QIODevice *device, qint64 offset,
            qint64 size, const QString &mime, QObject *parent)
{
    return FileFactory::getFileObject(name, device, offset, size, mime, parent);
}

FileObject* IUtility::getFileObject(QuaZip *zip, const QString &name, const QString &mime, QObject *parent)
{
    return FileFactory::getFileObject(zip, name, mime, parent);
}

QString IUtility::cacheFileObject(FileObject *fb)
{
    if (0 == fb) {
        return QString();
    }
    QTemporaryFile *file = new QTemporaryFile(fb);
    if (!file->open()) {
        m_error = file->errorString();
        delete file;
        return QString();
    }
    fb->copyTo(*file);
    file->close();
    return file->fileName();
}

}   // end symabw
