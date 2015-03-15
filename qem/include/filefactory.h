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

#ifndef QEM_FILEFACTORY_H
#define QEM_FILEFACTORY_H

#include "fileobject.h"


// import QuaZip
#ifndef QUAZIP_STATIC
# define QUAZIP_STATIC
#endif

class QuaZip;
class QIODevice;

QEM_BEGIN_NAMESPACE

class QEM_SHARED_EXPORT FileFactory
{
private:
    FileFactory();
public:
    static FileObject* getFile(const QString &name, const QString &mime = QString(),
                               QObject *parent = 0);

    static FileObject* getFile(const QString &name, QIODevice *device,
                               qint64 offset, qint64 size,
                               const QString &mime = QString(),
                              QObject *parent = 0);

    static FileObject* getFile(QuaZip *zip, const QString &name, const QString &mime = QString(),
                               QObject *parent = 0);

};

QEM_END_NAMESPACE

#endif // QEM_FILEFACTORY_H
