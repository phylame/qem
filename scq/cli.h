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

#ifndef CLI_H
#define CLI_H

#include <QMap>
#include <QString>


class QStringList;
class QTextStream;

const QString OTHER_KEY("__OTHER__");

typedef QMap<QString, QStringList> ArgumentMap;

extern bool parseArguments(const QStringList arguments, const QString format, ArgumentMap &result,
                           QTextStream *cerr = 0, const QString &errorPrefix = QString());

extern QStringList glob(const QString &path);

#endif // CLI_H
