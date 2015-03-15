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

#include <chapter.h>
#include <QtDebug>

QEM_BEGIN_NAMESPACE

const QString Chapter::COVER_KEY("cover");
const QString Chapter::INTRO_KEY("intro");

Chapter::Chapter(const QString &title, const QString &text, const FileObject *cover,
                 const TextObject &intro, QObject *parent) :
    Part(title, text, parent)
{
    setCover(cover);
    setIntro(intro);
}

Chapter::Chapter(const QString &title, FileObject *file, const QByteArray &codec,
                 const FileObject *cover, const TextObject &intro, QObject *parent) :
    Part(title, file, codec, parent)
{
    setCover(cover);
    setIntro(intro);
}

Chapter::Chapter(const QString &title, const TextObject &source, const FileObject *cover,
                 const TextObject &intro, QObject *parent) :
    Part(title, source, parent)
{
    setCover(cover);
    setIntro(intro);
}

FileObject* Chapter::cover() const
{
    return FileObject::fromQVariant(attribute(COVER_KEY));
}
void Chapter::setCover(const FileObject *cover)
{
    setAttribute(COVER_KEY, QVariant::fromValue(const_cast<FileObjectPointer>(cover)));
}

TextObject Chapter::intro() const
{
    return TextObject::fromQVariant(attribute(INTRO_KEY));
}
void Chapter::setIntro(const TextObject &intro)
{
    setAttribute(INTRO_KEY, QVariant::fromValue(intro));
}

Chapter* Chapter::newChapter(const QString &title, const QString &text)
{
    Chapter *chapter = new Chapter(title, text, 0, TextObject(), this);
    Q_ASSERT(chapter != 0);
    append(chapter);
    return chapter;
}

Chapter* Chapter::newChapter(const QString &title, FileObject *file, const QByteArray &codec)
{
    Chapter *chapter = new Chapter(title, file, codec, 0, TextObject(), this);
    Q_ASSERT(chapter != 0);
    append(chapter);
    return chapter;
}

#ifdef QEM_QML_TARGET
void Chapter::fireAttributeChange(const QString &name, const QVariant &value)
{
    if (COVER_KEY == name) {
        emit coverChanged(value.value<FileObjectPointer>());
    } else if (INTRO_KEY == name) {
        emit introChanged(value.value<TextObject>());
    } else {
        Part::fireAttributeChange(name, value);
    }
}

void Chapter::fireAttributeRemove(const QString &name)
{
    if (COVER_KEY == name) {
        emit coverChanged(0);
    } else if (INTRO_KEY == name) {
        emit introChanged(TextObject());
    } else {
        Part::fireAttributeRemove(name);
    }
}

#endif  // QEM_QML_TARGET

QEM_END_NAMESPACE
