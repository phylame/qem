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

#ifndef IMABW_IMABW_H
#define IMABW_IMABW_H

#include <qem_global.h>
#include <QObject>
#include <qem.h>


#define IMABW_NAME                       QObject::tr("Imabw")
#define IMABW_VERSION                    "1.0.0"
#define IMABW_AUTHOR                     QObject::tr("PW")
#define IMABW_RIGHTS                     QObject::tr("(C) Copyright 2014-2015 PW")
#define IMABW_HOME                       QObject::tr("https://github.com/phylame/qem")
#define IMABW_VENDOR                     QObject::tr("PW")
#define IMABW_DESCRIPTION                QObject::tr("I aM A BookWorm")
#define IMABW_FEATURES                   QObject::tr("First unstable version")

namespace symabw {

class Imabw : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QString author READ author CONSTANT)
    Q_PROPERTY(QString home READ home CONSTANT)
    Q_PROPERTY(QString vendor READ vendor CONSTANT)
    Q_PROPERTY(QString rights READ rights CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString features READ features CONSTANT)
    Q_PROPERTY(QString qem READ qem CONSTANT)
public:
    Imabw() {}

    static inline QString name()
    {
        return IMABW_NAME;
    }

    static inline QString version()
    {
        return IMABW_VERSION;
    }

    static inline QString author()
    {
        return IMABW_AUTHOR;
    }

    static inline QString home()
    {
        return IMABW_HOME;
    }

    static inline QString vendor()
    {
        return IMABW_VENDOR;
    }

    static inline QString rights()
    {
        return IMABW_RIGHTS;
    }

    static inline QString description()
    {
        return IMABW_DESCRIPTION;
    }

    static inline QString features()
    {
        return IMABW_FEATURES;
    }

    static inline QString qem()
    {
        QString ver(QObject::tr(
                        "Pem for Qt %1 by %2\n"
                        "Supported formats:\n"
                        "Read: %3\n"
                        "Write: %4"
                        ));
        return ver.arg(Qem::VERSION, IMABW_VENDOR,
                    Qem::getSupportedParser().join(QObject::tr(", ")),
                    Qem::getSupportedMaker().join(QObject::tr(", ")));
    }
};

}   // end symabw

#endif // IMABW_IMABW_H
