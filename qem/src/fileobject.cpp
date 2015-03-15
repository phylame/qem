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

#include <fileobject.h>
#include <fileutils.h>
#include <QtDebug>
#include <QVariant>
#include <QIODevice>
#include <QByteArray>

QEM_BEGIN_NAMESPACE

FileObject* FileObject::fromQVariant(const QVariant &v, bool *ok)
{
    if (v.canConvert<FileObjectPointer>()) {
        if (ok != 0) {
            *ok = true;
        }
        return v.value<FileObjectPointer>();
    } else {
        if (ok != 0) {
            *ok = false;
        }
        return 0;
    }
}

QByteArray FileObject::readAll()
{
    QIODevice* in = openDevice();
    Q_ASSERT(in != 0);
    const QByteArray &ba = in->readAll();
    delete in;
    reset();
    return ba;
}

qint64 FileObject::copyTo(QIODevice &out, qint64 size)
{
    QIODevice* in = openDevice();
    Q_ASSERT(in != 0);
    qint64 total = FileUtils::copy(*in, out, size);
    delete in;
    reset();
    return total;
}

QEM_END_NAMESPACE
