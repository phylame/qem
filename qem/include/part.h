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

#ifndef QEM_PART_H
#define QEM_PART_H

#include <QList>
#include <QObject>
#include <QStringList>
#include "attributes.h"
#include "textobject.h"
#include "fileobject.h"


class PartPrivate;
class QTextStream;
class QIODevice;

class QEM_SHARED_EXPORT Part : public Attributes, public QList<Part*>
{
    Q_OBJECT
#ifdef QEM_QML_TARGET
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int depth READ depth)
    Q_PROPERTY(int size READ size NOTIFY sizeChanged)
    Q_PROPERTY(QString content READ content)
    Q_PROPERTY(QStringList lines READ lines)
#endif

private:
    PartPrivate *p;
protected:
    static const QString TITLE_KEY;
public:
    typedef void (*Cleaner)(Part &part, void *arg);
    typedef bool (*Filter)(const Part &part, void *arg);

    explicit Part(const QString &title = QString(), const QString &text =
            QString(), QObject *parent = 0);

    Part(const QString &title, FileObject *file, const QByteArray &codec =
            QByteArray(), QObject *parent = 0);

    Part(const QString &title, const TextObject &source, QObject *parent = 0);

    Part(const Part &other);

    virtual ~Part();

    virtual Part& operator =(const Part &other);

    virtual bool operator ==(const Part &other) const;

    QString title() const;
    void setTitle(const QString &title);

    virtual QString content() const;

    virtual QStringList lines(bool skipEmptyLine = false) const;

    QEM_INVOKABLE virtual qint64 writeTo(QTextStream &out, qint64 size = -1) const;

    QEM_INVOKABLE virtual qint64 writeTo(QIODevice &out, const QByteArray &encoding =
            QByteArray(), qint64 size = -1) const;

    QEM_INVOKABLE virtual QString text() const;

    QEM_INVOKABLE virtual void setText(const QString &text);

    QEM_INVOKABLE virtual FileObject* file() const;

    QEM_INVOKABLE virtual void setFile(FileObject *file, const QByteArray &codec =
            QByteArray());

    QEM_INVOKABLE virtual QByteArray codec() const;

    /// Returns depth of sub-parts tree.
    int depth() const;

    /// Return \c true if has sub-parts, otherwise \c false.
    QEM_INVOKABLE inline bool isSection() const
    {return size() != 0;}

    /// Create a part and set its parent to self.
    QEM_INVOKABLE Part* newPart(const QString &title, const QString &text = QString());

    /// Create a part and set its parent to self.
    QEM_INVOKABLE Part* newPart(const QString &title, FileObject *file,
                                const QByteArray &codec = QByteArray());

    /// Sets sub-part \a part at index \a i and set its parent to self.
    QEM_INVOKABLE void set(int i, Part* part);

    /// Gets a sub-part with index \a i.
    /** If index is invalid, return \a defaultValue */
    QEM_INVOKABLE Part* get(int i, Part* defaultValue = 0) const;

    /// Add a part and set its parent to self.
    QEM_INVOKABLE void add(Part* part);

    /// Remove part at index \a i.
    QEM_INVOKABLE void remove(int i);

    /// Insert \a part before index \a i and set its parent to self.
    QEM_INVOKABLE void put(int i, Part* part);

    /// Index sub-part by its \a title begin index \a from.
    /** Returns index in self or \c -1 if not found. */
    QEM_INVOKABLE int indexOf(const QString &title, int from = 0) const;

    /// Index sub-part with Filter begin index \a from.
    /** Returns index in self or \c -1 if not found.
     * \param filter Filter sub-parts, \see Filter.
     * \param arg Argument to \a filter, \see Filter.
     */
    QEM_INVOKABLE int indexOf(Filter filter, void *arg, int from = 0) const;

    /// Select sub-parts form self and its sub-parts if \a recursion is \c true.
    /**
     * \param result Recive all expected parts.
     * \param filter Filter sub-parts, \see Filter.
     * \param arg Argument to \a filter, \see Filter.
     * \param recursion If \c true search its sub-parts otherwith only search self.
     */
    QEM_INVOKABLE void select(QList<Part*> &result, Filter filter, void *arg,
                              bool recursion = false) const;

    /// Find sub-part from self and its sub-parts.
    /** Returns Part* if found otherwith \c 0.
     * \param orders Index list of the wanted part in part tree.
     * \param size Number of index in \a orders.
     */
    QEM_INVOKABLE Part* findPart(const int *orders, int size) const;

    /// Find sub-part from self and its sub-parts.
    /** Returns Part* if found otherwith \c 0.
     * \param orders Index list of the wanted part in part treee.
     * \param fromIndex start index in \a orders.
     */
    QEM_INVOKABLE Part* findPart(const QList<int> &orders, int fromIndex = 0) const;

    /// Register a clean work \a cleaner with argument \a arg.
    /** The \a cleaner will be called when destroy object. */
    void registerCleaner(Cleaner cleaner, void *arg);

    /// Remove one registered clean work.
    void removeCleaner(Cleaner cleaner);

    /// Call all clean works and clear cleaner list.
    void cleanup();

#ifdef QEM_QML_TARGET
signals:
    void titleChanged(const QString &title);
    void sizeChanged(int size);
protected slots:
    virtual void fireAttributeChange(const QString &name, const QVariant &value);
    virtual void fireAttributeRemove(const QString &name, const QVariant &value);
#endif
};


Q_DECLARE_METATYPE(Part)

#endif // QEM_PART_H
