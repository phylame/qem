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

#ifndef QEM_FILEUTILS_H
#define QEM_FILEUTILS_H

#include "qem_global.h"
#include <QByteArray>


// import QuaZip
#ifndef QUAZIP_STATIC
# define QUAZIP_STATIC
#endif

class QuaZip;
class QIODevice;
class QDataStream;
class QTextStream;

QEM_BEGIN_NAMESPACE

class FileObject;

/// Utility class for file operations.
/**
 * This class contains a number of useful static functions to perform
 * file operations.
 **/
class QEM_SHARED_EXPORT FileUtils
{
private:
    FileUtils();
public:
    /// Get extension of file name.
    /** The return not contains the separator. */
    static QString extensionName(const QString &name);

    /// Get MIME type of file.
    /**
     * This function detect the MIME by the extensin name of \a name.
     * Returns MIME string or  QString() if MIME is unknown.
     */
    static QString mimeType(const QString &name);

    /// Copy \a size bytes data from \a in to \a out.
    /** Returns copied bytes number.
     * \param in The readable input QIODevice.
     * \param out The writeable output QIODevice.
     * \param size number of bytes to copy, if \a size < 0 copy all available data.
     */
    static qint64 copy(QIODevice &in, QIODevice &out, qint64 size = -1);

    /// Copy \a size bytes data from \a in to \a out.
    /** Returns copied bytes number or \c -1 if occurs errors.
     * \param in The readable input QIODevice.
     * \param out The writeable output QDataStream.
     * \param size number of bytes to copy, if \a size < 0 copy all available data.
     */
    static qint64 copy(QIODevice &in, QDataStream &out, qint64 size = -1);

    /// Copy \a size bytes data from \a in to \a out.
    /** Returns copied bytes number or \c -1 if occurs errors.
     * \param in The readable input QDataStream.
     * \param out The writeable output QIODevice.
     * \param size number of bytes to copy, if 0 copy all available data.
     */
    static qint64 copy(QDataStream &in, QIODevice &out, qint64 size = -1);

    /// Copy \a size bytes data from \a in to \a out.
    /** Returns copied bytes number or \c -1 if occurs errors.
     * \param in The readable input QDataStream.
     * \param out The writeable output QDataStream.
     * \param size number of bytes to copy, if \a size < 0 copy all available data.
     */
    static qint64 copy(QDataStream &in, QDataStream &out, qint64 size = -1);

    /// Copy \a size characters from \a in to \a out.
    /** Returns copied characters number or \c -1 if occurs errors.
     * \param in The readable input device.
     * \param out The writeable output device.
     * \param size number of bytes to copy, if \a size < 0 copy all available data.
     */
    static qint64 copy(QTextStream &in, QTextStream &out, qint64 size = -1);

    /// Read bytes from ZIP archive.
    static QByteArray readZipData(QuaZip &zip, const QString &entryName,
                                  const char *password = 0);

    /// Write bytes to ZIP archive.
    static bool writeZipData(QuaZip &zip, const QString &entryName, const QByteArray &data,
                             const char *password = 0);

    static bool writeToZip(FileObject &fb, QuaZip &zip, const QString &entryName,
                           const char *password = 0);

    static bool writeToZip(QIODevice &device, QuaZip &zip, const QString &entryName,
                           const char *password = 0);

    static QString readZipText(QuaZip &zip, const QString &entryName,
                               const QByteArray &codec = QByteArray(),
                               const char *password = 0);

    static bool writeZipText(QuaZip &zip, const QString &entryName,
                             const QString &text,
                             const QByteArray &codec = QByteArray(),
                             const char *password = 0);
};

QEM_END_NAMESPACE

#endif // QEM_FILEUTILS_H
