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

#include <item.h>


Item::Type Item::nameType(const QString &s)
{
    if (s.compare("file") == 0) {
        return File;
    } else if (s.compare("number") == 0) {
        return Number;
    } else {
        return Text;
    }
}

QString Item::typeName(Item::Type type)
{
    switch (type) {
    case Text:
        return "Text";
    case Number:
        return "Number";
    case File:
        return "File";
    default:
        return QString();
    }
}
