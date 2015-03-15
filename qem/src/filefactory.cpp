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

#include <filefactory.h>
#include <fileutils.h>
#include <QFile>
#include <QBuffer>
#include <QtDebug>
#include <QIODevice>
#include <quazipfile.h>

QEM_BEGIN_NAMESPACE

namespace file_object_impl {

    // *********************
    // ** NormalFile
    // *********************

    class NormalFile : public FileObject
    {
    public:
        static NormalFile* createObject(const QString &path, const QString &mime = QString(),
                                        QObject *parent = 0);
        inline QString name() const
        {
            return m_path;
        }
        QIODevice* openDevice();

        void reset() {}
    private:
        inline NormalFile(const QString &path, const QString &mime, QObject *parent) :
            FileObject(mime, parent), m_path(path)
        {}
    private:
        QString m_path;
    };

    NormalFile* NormalFile::createObject(const QString &path, const QString &mime,
                                         QObject *parent)
    {
        QFile file(path);
        if (! file.exists()) {
            qWarning() << "file" << path << "not exists";
            return 0;
        }
        return new NormalFile(path, mime, parent);
    }

    QIODevice* NormalFile::openDevice()
    {
        QFile *file = new QFile(m_path);
        if (! file->open(QFile::ReadOnly)) {
            qDebug() << "open file failed";
            return 0;
        }
        return file;
    }


    // *********************
    // ** AreaFile
    // *********************

    class AreaFile : public FileObject
    {
    public:
        static AreaFile* createObject(const QString &name, QIODevice *device, qint64 offset,
                                      qint64 size, const QString &mime = QString(), QObject *parent = 0);
        inline QString name() const
        {
            return m_name;
        }
        QIODevice *openDevice();
        inline void reset()
        {
            if (m_oldOffset >= 0) {
                m_device->seek(m_oldOffset);
            }
        }

    private:
        inline AreaFile(const QString &name, QIODevice *device, qint64 offset, qint64 size,
                        const QString &mime, QObject *parent) :
            FileObject(mime, parent), m_name(name), m_device(device), m_oldOffset(-1),
            m_offset(offset), m_size(size)
        {}
    private:
        QString m_name;
        QIODevice *m_device;
        qint64 m_oldOffset, m_offset, m_size;
        QList<QDataStream*> m_opened;
    };

    AreaFile* AreaFile::createObject(const QString &name, QIODevice *device, qint64 offset,
                                     qint64 size, const QString &mime, QObject *parent)
    {
        Q_ASSERT(device != 0);
        if (device->isSequential()) {
            qWarning() << "device is sequential";
            return 0;
        }
        if (offset < 0 || offset >= device->size()) {
            qWarning() << "invalid offset: " << offset << ", device size:" << device->size();
            return 0;
        }
        qint64 available = device->size() - offset;
        if (size > available ) {
            qWarning() << "size more than file length, available:" << available << ", wanted:" << size;
            return 0;
        }
        return new AreaFile(name, device, offset, size, mime, parent);
    }

    QIODevice* AreaFile::openDevice()
    {
        Q_ASSERT(m_device != 0);
        m_oldOffset = m_device->pos();
        if (! m_device->seek(m_offset)) {
            qWarning() << "Cannot seek IO device";
            return 0;
        }
        QBuffer *buffer = new QBuffer();
        if (! buffer->open(QBuffer::ReadWrite)) {
            qWarning() << "Cannot open QBuffer with ReadWrite";
            delete buffer;
            return 0;
        }
        FileUtils::copy(*m_device, *buffer, m_size);
        buffer->seek(0);
        return buffer;
    }


    // *********************
    // ** ZipFile
    // *********************

    class ZipFile : public FileObject
    {
    public:
        static ZipFile* createObject(QuaZip *zip, const QString &name, const QString &mime,
                                     QObject *parent = 0);
        inline QString name() const
        {
            return m_name;
        }
        QIODevice *openDevice();
        inline void reset()
        {
            // reset to old entry
            if (! m_last.isEmpty()) {
                m_zip->setCurrentFile(m_last);
            }
        }

    private:
        ZipFile(QuaZip *zip, const QString &name, const QString &mime, QObject *parent) :
            FileObject(mime, parent), m_zip(zip), m_name(name)
        {}
    private:
        QuaZip *m_zip;
        QString m_last, m_name;
    };

    ZipFile* ZipFile::createObject(QuaZip *zip, const QString &name, const QString &mime, QObject *parent)
    {
        Q_ASSERT(zip != 0);
        const QString &old = zip->getCurrentFileName();
        if (! zip->setCurrentFile(name)) {
            qWarning() << "Not found file in ZIP:" << name;
            return 0;
        }
        zip->setCurrentFile(old);
        return new ZipFile(zip, name, mime, parent);
    }

    QIODevice* ZipFile::openDevice()
    {
        m_last = m_zip->getCurrentFileName();
        if (!m_zip->setCurrentFile(m_name)) {
            return 0;
        }
        QuaZipFile *file = new QuaZipFile(m_zip);
        if (!file->open(QuaZipFile::ReadOnly)) {
            delete file;
            return 0;
        }
        return file;
    }

}   // end file_object_impl

FileObject* FileFactory::getFile(const QString &name, const QString &mime, QObject *parent)
{
    return file_object_impl::NormalFile::createObject(name,
                                                      mime.isEmpty() ? FileUtils::mimeType(name) : mime, parent);
}

FileObject* FileFactory::getFile(const QString &name, QIODevice *device, qint64 offset, qint64 size,
                                 const QString &mime, QObject *parent)
{
    return file_object_impl::AreaFile::createObject(name, device, offset, size,
                                                    mime.isEmpty() ? FileUtils::mimeType(name) : mime, parent);
}

FileObject* FileFactory::getFile(QuaZip *zip, const QString &name, const QString &mime, QObject *parent)
{
    return file_object_impl::ZipFile::createObject(zip, name,
                                                   mime.isEmpty() ? FileUtils::mimeType(name) : mime, parent);
}

QEM_END_NAMESPACE
