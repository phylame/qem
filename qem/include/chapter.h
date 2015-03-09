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

#ifndef QEM_CHAPTER_H
#define QEM_CHAPTER_H

#include "part.h"


class QEM_SHARED_EXPORT Chapter : public Part
{
    Q_OBJECT
#ifdef QEM_QML_TARGET
    Q_PROPERTY(FileObjectPointer cover READ cover WRITE setCover NOTIFY coverChanged)
    Q_PROPERTY(TextObject intro READ intro WRITE setIntro NOTIFY introChanged)
#endif

protected:
    static const QString COVER_KEY;
    static const QString INTRO_KEY;
public:
    explicit Chapter(const QString &title = QString(), const QString &text = QString(),
            const FileObject *cover = 0, const TextObject &intro = TextObject(), QObject *parent = 0);

    Chapter(const QString &title, FileObject *file, const QByteArray &codec = QByteArray(),
            const FileObject *cover = 0, const TextObject &intro = TextObject(), QObject *parent = 0);

    Chapter(const QString &title, const TextObject &source, const FileObject *cover = 0,
            const TextObject &intro = TextObject(), QObject *parent = 0);

    inline Chapter(const Part &part) :
        Part(part)
    {}

    inline Chapter(const Chapter &other) :
        Part(other)
    {
        setCover(other.cover());
        setIntro(other.intro());
    }

    FileObject* cover() const;
    void setCover(const FileObject *cover);

    TextObject intro() const;
    void setIntro(const TextObject &intro);

#ifdef QEM_QML_TARGET
    Q_INVOKABLE
#endif
    inline QString introContent() const
    {
        return intro().text();
    }

#ifdef QEM_QML_TARGET
    Q_INVOKABLE
#endif
    inline QString introText() const
    {
        return intro().raw();
    }

#ifdef QEM_QML_TARGET
    Q_INVOKABLE
#endif
    inline void setIntroText(const QString &s)
    {
        setIntro(TextObject(s));
    }

#ifdef QEM_QML_TARGET
    Q_INVOKABLE
#endif
    inline FileObject* introFile() const
    {
        return intro().file();
    }

#ifdef QEM_QML_TARGET
    Q_INVOKABLE
#endif
    inline void setIntroFile(FileObject *file, const QByteArray &codec = QByteArray())
    {
        setIntro(TextObject(file, codec));
    }

#ifdef QEM_QML_TARGET
    Q_INVOKABLE
#endif
    Chapter* newChapter(const QString &title, const QString &text = QString());

#ifdef QEM_QML_TARGET
    Q_INVOKABLE
#endif
    Chapter* newChapter(const QString &title, FileObject *file, const QByteArray &codec = QByteArray());

#ifdef QEM_QML_TARGET
signals:
    void coverChanged(FileObjectPointer cover);
    void introChanged(const TextObject &intro);
protected slots:
    virtual void fireAttributeChange(const QString &name, const QVariant &value);
    virtual void fireAttributeRemove(const QString &name, const QVariant &value);
#endif

};


Q_DECLARE_METATYPE(Chapter)

#endif // QEM_CHAPTER_H
