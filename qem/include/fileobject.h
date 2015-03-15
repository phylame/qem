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

#ifndef QEM_FILEOBJECT_H
#define QEM_FILEOBJECT_H

#include "qem_global.h"
#include <QObject>
#include <QMetaType>

class QIODevice;

QEM_BEGIN_NAMESPACE

/// Provides readable file content.
/** \class FileObject fileobject.h <qem/fileobject.h>
 * This class provides common ways to read data from file. The data of FileObject
 * is always readable.
 *
 * This class is an abstract class and at least name() and openDevice() should be
 * implemented.
 **/
class QEM_SHARED_EXPORT FileObject : public QObject
{
    Q_OBJECT
private:
    Q_DISABLE_COPY(FileObject)
public:
    /// Gets FileObject from QVariant \a v.
    /**
     * \param ok Save conver state.
     */
    static FileObject* fromQVariant(const QVariant &v, bool *ok = 0);

    /// Constructs FileObject object associated with MIME type \a mime.
    explicit inline FileObject(const QString &mime = QString(), QObject *parent = 0) :
        QObject(parent), m_mime(mime)
    {}

    /// Destroys FileObject object.
    inline virtual ~FileObject()
    {}

    /// Returns the name of the object.
    /** The name is commonly base name of a file. */
    virtual QString name() const = 0;

    /// Returns the MIME type of file content.
    inline virtual QString mime() const
    {return m_mime;}

    /// Returns new IO device of file content.
    /** The caller need delete the pointer after using */
    virtual QIODevice* openDevice() = 0;

    /// Resets to default state.
    /** The method should be called after deleting the pointer from openDevice(). */
    virtual void reset() = 0;

    /// Returns number of available bytes in file content.
    /**
     * @return number of avaliable bytes or \c -1 if unknown.
     */
    virtual qint64 available()
    { return -1; }

    /// Reads all available data from the file content.
    /** This method use openDevice(), so make sure openDevice() return availabe IO device. */
    virtual QByteArray readAll();

    /// Copies \a size bytes to IO device \a out.
    /** The IO device must be writable, otherwise errors will occur when copying.
     * If \a size < 0, all bytes will be copied to \a out.
     *
     * This method use openDevice(), so make sure openDevice() return availabe IO device.
     * Returns copied bytes number or \c -1 if occurs errors.
     **/
    virtual qint64 copyTo(QIODevice &out, qint64 size = -1);
private:
    QString m_mime;
};

typedef FileObject * FileObjectPointer;

QEM_END_NAMESPACE

Q_DECLARE_METATYPE(QEM_PREPEND_NAMESPACE(FileObjectPointer))

#endif // QEM_FILEOBJECT_H
