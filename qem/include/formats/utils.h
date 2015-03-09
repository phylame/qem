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

#ifndef QEM_FORMATS_UTILS_H
#define QEM_FORMATS_UTILS_H

#include <qem_global.h>


class QDebug;
class QString;
class QVariant;

class QEM_SHARED_EXPORT FormatsUtility
{
private:
    FormatsUtility();
public:
    /// Print debug message \a msg and copy to \a error if not \c 0.
    static QDebug& debug(const QString &msg, QString *error = 0);
};
#endif // QEM_FORMATS_UTILS_H
