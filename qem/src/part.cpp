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

#include <part.h>
#include <QtDebug>


class PartPrivate
{
    friend class Part;
    // instance count
    static int objectCount;
    int id;
    typedef QMap<Part::Cleaner, void*> CleanerList;

    CleanerList cleaners;
    TextObject source;

    inline PartPrivate(const QString &text) :
        id(++objectCount), source(TextObject(text))
    {}
    inline PartPrivate(FileObject *file, const QByteArray codec) :
        id(++objectCount), source(TextObject(file, codec))
    {}
    inline PartPrivate(const TextObject &source) :
        id(++objectCount), source(source)
    {}
    inline PartPrivate(const PartPrivate &other) :
        cleaners(other.cleaners), source(other.source)
    {}
    inline PartPrivate& operator =(const PartPrivate &other)
    {
        cleaners = other.cleaners;
        source = other.source;
        return *this;
    }
};

int PartPrivate::objectCount = 0;

const QString Part::TITLE_KEY("title");

Part::Part(const QString &title, const QString &text, QObject *parent) :
    Attributes(parent), p(new PartPrivate(text))
{
    setTitle(title);
}

Part::Part(const QString &title, FileObject *file, const QByteArray &codec, QObject *parent) :
    Attributes(parent), p(new PartPrivate(file, codec))
{
    setTitle(title);
}

Part::Part(const QString &title, const TextObject &source, QObject *parent) :
    Attributes(parent), p(new PartPrivate(source))
{
    setTitle(title);
}

Part::Part(const Part &other) :
    Attributes(other), QList<Part*>(other), p(new PartPrivate(*other.p))
{}

Part::~Part()
{
    cleanup();
    delete p;
}

Part& Part::operator =(const Part &other)
{
    Attributes::operator =(other);
    QList<Part*>::operator =(other);
    *p = *other.p;
    return *this;
}

bool Part::operator ==(const Part &other) const
{
    return p->id == other.p->id;
}

QString Part::title() const
{
    const QVariant &v = attribute(TITLE_KEY);
    Q_ASSERT(v.type() == QVariant::String);
    return v.toString();
}

void Part::setTitle(const QString &title)
{
    setAttribute(TITLE_KEY, title);
}

QString Part::content() const
{
    return p->source.text();
}

QStringList Part::lines(bool skipEmptyLine) const
{
    return p->source.lines(skipEmptyLine);
}

qint64 Part::writeTo(QTextStream &out, qint64 size) const
{
    return p->source.writeTo(out, size);
}

qint64 Part::writeTo(QIODevice &out, const QByteArray &encoding, qint64 size) const
{
    return p->source.writeTo(out, encoding, size);
}

QString Part::text() const
{
    return p->source.raw();
}
void Part::setText(const QString &text)
{
    p->source.setRaw(text);
}

FileObject* Part::file() const
{
    return p->source.file();
}
void Part::setFile(FileObject *file, const QByteArray &codec)
{
    p->source.setFile(file, codec);
}

QByteArray Part::codec() const
{
    return p->source.codec();
}

int Part::depth() const
{
    if (0 == size()) {
        return 0;
    }
    int myDepth = 0, maxDepth = 0;
    for (int ix = 0; ix < size(); ++ix) {
        const Part *p = get(ix);
        Q_ASSERT(p != 0);
        myDepth = p->depth();
        if (myDepth > maxDepth) {
            maxDepth = myDepth;
        }
    }
    return maxDepth + 1;
}

Part* Part::newPart(const QString &title, const QString &text)
{
    Part *part = new Part(title, text, this);
    Q_ASSERT(part != 0);
    append(part);
    return part;
}

Part* Part::newPart(const QString &title, FileObject *file, const QByteArray &codec)
{
    Part *part = new Part(title, file, codec, this);
    Q_ASSERT(part != 0);
    append(part);
    return part;
}

void Part::set(int i, Part* part)
{
    Q_ASSERT(part != 0);
    replace(i, part);
}

Part* Part::get(int i, Part* defaultValue) const
{
    return value(i, defaultValue);
}

void Part::add(Part *const part)
{
    Q_ASSERT(part != 0);
    append(part);
#ifdef QEM_QML_TARGET
    emit sizeChanged(size());
#endif
}

void Part::remove(int i)
{
#ifdef QEM_QML_TARGET
    int n = size();
#endif
    removeAt(i);
#ifdef QEM_QML_TARGET
    if (n != size()) {
        emit sizeChanged(size());
    }
#endif
}

void Part::put(int i, Part *part)
{
    Q_ASSERT(part != 0);
    insert(i, part);
#ifdef QEM_QML_TARGET
    emit sizeChanged(size());
#endif
}

int Part::indexOf(const QString &title, int from) const
{
    Q_ASSERT(from >= 0 && from < size());
    int index = 0;
    for (int ix = from; ix < size(); ++ix) {
        const Part *p = get(ix);
        Q_ASSERT(p != 0);
        if (title == p->title()) {
            return index;
        }
        ++index;
    }
    return -1;
}

int Part::indexOf(Part::Filter filter, void *arg, int from) const
{
    Q_ASSERT(from >= 0 && from < size());
    int index = 0;
    for (int ix = from; ix < size(); ++ix) {
        const Part *p = get(ix);
        Q_ASSERT(p != 0);
        if (filter(*p, arg)) {
            return index;
        }
        ++index;
    }
    return -1;
}

void Part::select(QList<Part*> &result, Part::Filter filter, void *arg, bool recursion) const
{
    Q_ASSERT(filter != 0);
    for (int ix = 0; ix < size(); ++ix) {
        Part *p = get(ix);
        Q_ASSERT(p != 0);
        if (filter(*p, arg)) {
            result.append(p);
        }
        if (p->isSection() && recursion) {
            p->select(result, filter, arg, recursion);
        }
    }
}

Part* Part::findPart(const int *orders, int size) const
{
    Q_ASSERT(orders != 0);
    if (size <= 0) {
        qWarning() << "size must more than 0";
        return 0;
    }
    int index = *orders;
    if (index >= this->size()) {
        qWarning() << "Index out of range:" << index << this->size() << title();
        return 0;
    }
    Part *part;
    if (index < 0) {
        part = get(this->size()+index, 0);
    } else {
        part = get(index, 0);
    }
    if (1 == size) {   // last one
        return part;
    } else {
        Q_ASSERT(part != 0);
        return part->findPart(orders+1, size-1);
    }
}

Part* Part::findPart(const QList<int> &orders, int fromIndex) const
{
    if (fromIndex < 0 || fromIndex >= orders.size()) {
        qWarning() << "fromIndex out of range:" << fromIndex << title();
        return 0;
    }
    int orig = orders[fromIndex];
    int index = orig;
    if (index < 0) {
        index = this->size()+index;
    }
    if (index < 0 || index >= this->size()) {
        qWarning() << "Index out of range:" << orig << "in" << title();
        return 0;
    }
    Part *part = get(index, 0);
    if (fromIndex == orders.size()-1) {   // last one
        return part;
    } else {
        Q_ASSERT(part != 0);
        return part->findPart(orders, fromIndex+1);
    }
}

void Part::registerCleaner(Part::Cleaner cleaner, void *arg)
{
    Q_ASSERT(cleaner != 0);
    p->cleaners.insert(cleaner, arg);
}

void Part::removeCleaner(Part::Cleaner cleaner)
{
    Q_ASSERT(cleaner != 0);
    p->cleaners.remove(cleaner);
}

void Part::cleanup()
{
    PartPrivate::CleanerList::const_iterator i = p->cleaners.constBegin();
    for (; i != p->cleaners.constEnd(); ++i) {
        i.key()(*this, i.value());
    }
    p->cleaners.clear();
}

#ifdef QEM_QML_TARGET
void Part::fireAttributeChange(const QString &name, const QVariant &value)
{
    if (TITLE_KEY == name) {
        emit titleChanged(value.toString());
    }
}

void Part::fireAttributeRemove(const QString &name, const QVariant &value)
{
    if (TITLE_KEY == name) {
        emit titleChanged(QString());
    }
}

#endif
