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

#include "cli.h"
#include <QRegExp>
#include <QStringList>
#include <QTextStream>


bool parseArguments(const QStringList arguments, const QString format, ArgumentMap &result,
                    QTextStream *cerr, const QString &errorPrefix)
{
    for (int i=1; i<arguments.size(); ++i) {
        const QString &opt = arguments.at(i);
        if (opt.startsWith("--")) {
            if (cerr != 0) {
                *cerr << errorPrefix << QObject::tr("Unrecognized option: ") << opt << endl;
            }
            return false;
        } else if (opt.startsWith("-")) {
            QString key = opt.mid(1, 1);
            int index = format.indexOf(key);
            if (-1 == index) {
                if (cerr != 0) {
                    *cerr << errorPrefix << QObject::tr("Unrecognized option: ") << opt << endl;
                }
                return false;
            }
            QStringList values = result.value(key);
            if (index != format.size() - 1 && format.at(index+1) == ':') {    // not last and has arg
                if (opt.length() > 2) {     // appended value
                    values.append(opt.mid(2));
                } else {
                    if (i == arguments.size() - 1) {
                        if (cerr != 0) {
                            *cerr << errorPrefix << QObject::tr("Missing argument for option: ") << opt << endl;
                        }
                        return false;
                    }
                    values.append(arguments.at(++i));
                }
            }
            result.insert(key, values);
        } else {
            QStringList values = result.value(OTHER_KEY);
            values.append(opt);
            result.insert(OTHER_KEY, values);
        }
    }
    return true;
}

static bool hasMagic(const QString &s);

QStringList glob(const QString &path)
{
    if (! hasMagic(path)) {
        return QStringList() << path;
    }
    return QStringList();
}

static QRegExp MAGIC_CHECK("([*?[])");

static bool hasMagic(const QString &s)
{
    return MAGIC_CHECK.indexIn(s) != 0;
}
