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

#include <attributes.h>
#include <fileobject.h>
#include <textobject.h>


void Attributes::setAttribute(const QString &name, const QVariant &value)
{
    const QVariant &old = m_attributes.value(name);
    if (old.canConvert<TextObject>() && value.canConvert<TextObject>()) {
        if (old.value<TextObject>() == value.value<TextObject>()) {
            return;
        }
    } else if (value == old) {
        return;
    }
    m_attributes.insert(name, value);
    emit attributeChanged(name, value);
}

void Attributes::removeAttribute(const QString &name)
{
    if (m_attributes.contains(name)) {
        emit attributeRemoved(name, m_attributes.take(name));
    }
}
