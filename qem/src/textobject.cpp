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

#include <fileutils.h>
#include <textobject.h>
#include <QRegExp>
#include <QtDebug>
#include <QVariant>
#include <QIODevice>
#include <QTextStream>


class TextObjectPrivate
{
    friend class TextObject;
private:
    enum Source {Text, File};
    QString raw;
    FileObject *file;
    QByteArray codec;
    Source source;

    inline TextObjectPrivate(const QString &s) :
        raw(s), file(0), source(Text) {}

    inline TextObjectPrivate(FileObject *file, const QByteArray &codec) :
        file(file), source(File)
    {
        if (! codec.isEmpty()) {
            this->codec = codec;
        }
    }
    inline TextObjectPrivate(const TextObjectPrivate &other) :
        raw(other.raw), file(other.file), codec(other.codec), source(other.source)
    {}

    inline TextObjectPrivate& operator =(const TextObjectPrivate &other)
    {
        raw = other.raw;
        file = other.file;
        codec = other.codec;
        source = other.source;
        return *this;
    }

    inline QTextStream* openStream()
    {
        Q_ASSERT(file != 0);
        QIODevice *dev = file->openDevice();
        Q_ASSERT(dev != 0);
        QTextStream *in = new QTextStream(dev);
        if (! codec.isEmpty()) {
            in->setCodec(codec.constData());
        }
        return in;
    }
};

TextObject TextObject::fromQVariant(const QVariant &v, bool *ok)
{
    if (v.canConvert<TextObject>()) {
        if (ok != 0) {
            *ok = true;
        }
        return v.value<TextObject>();
    } else {
        if (ok != 0) {
            *ok = false;
        }
        return TextObject();
    }
}

TextObject::TextObject(const QString &s, QObject *parent) :
    QObject(parent), p(new TextObjectPrivate(s))
{}

TextObject::TextObject(FileObject *file, const QByteArray &codec, QObject *parent) :
    QObject(parent), p(new TextObjectPrivate(file, codec))
{}

TextObject::TextObject(const TextObject &other) :
    QObject(other.parent()), p(new TextObjectPrivate(*other.p))
{}

TextObject::~TextObject()
{
    delete p;
}

TextObject& TextObject::operator =(const TextObject &other)
{
    *p = *other.p;
    return *this;
}

bool TextObject::operator ==(const TextObject &other) const
{
    if (p->source != other.p->source) {
        return false;
    }
    switch (p->source) {
    case TextObjectPrivate::Text:
        return p->raw == other.p->raw;
    case TextObjectPrivate::File:
        return p->file == other.p->file;
    default:
        return false;
    }
}

QString TextObject::text() const
{
    switch (p->source) {
    case TextObjectPrivate::Text:
    {
        return p->raw;
    }
        break;
    case TextObjectPrivate::File:
    {
        QTextStream *in = p->openStream();
        const QString &str = in->readAll();
        delete in->device();
        delete in;
        p->file->reset();
        return str;
    }
        break;
    default:
        return QString();
    }
}

QStringList TextObject::lines(bool skipEmptyLine) const
{

    return text().split(QRegExp("(\\r\\n|\\r|\\n)"),
                        skipEmptyLine ? QString::SkipEmptyParts : QString::KeepEmptyParts);
}

qint64 TextObject::writeTo(QTextStream &out, qint64 size) const
{
    quint64 total = -1;
    switch (p->source) {
    case TextObjectPrivate::Text:
    {
        if (size < 0) {
            total = p->raw.size();
        } else {
            total = size;
        }
        out << p->raw.left(total);
    }
        break;
    case TextObjectPrivate::File:
    {
        QTextStream *in = p->openStream();
        total = FileUtils::copy(*in, out, size);
        delete in->device();
        delete in;
        p->file->reset();
    }
        break;
    }
    out.flush();
    return total;
}

qint64 TextObject::writeTo(QIODevice &out, const QByteArray &encoding, qint64 size) const
{
    QTextStream stream(&out);
    if (! encoding.isEmpty()) {
        stream.setCodec(encoding.constData());
    }
    return writeTo(stream, size);
}

const QString& TextObject::raw() const
{
    return p->raw;
}

void TextObject::setRaw(const QString &s)
{
    p->source = TextObjectPrivate::Text;
    p->raw = s;
}

FileObject* TextObject::file() const
{
    return p->file;
}

QByteArray TextObject::codec() const
{
    return p->codec;
}

void TextObject::setFile(FileObject *file, const QByteArray &codec)
{
    Q_ASSERT(file != 0);
    p->source = TextObjectPrivate::File;
    if (p->file == file) {
        return;
    }
    p->file = file;
    if (! codec.isEmpty()) {
        p->codec = codec;
    }
}
