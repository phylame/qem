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

#include <book.h>


const QString Book::AUTHOR_KEY("author");
const QString Book::GENRE_KEY("genre");
const QString Book::STATE_KEY("state");
const QString Book::SUBJECT_KEY("subject");
const QString Book::DATE_KEY("date");
const QString Book::PUBLISHER_KEY("publisher");
const QString Book::RIGHTS_KEY("rights");
const QString Book::LANGUAGE_KEY("language");

Book::Book(const QString &title, const QString &author, QObject *parent) :
    Chapter(title, "", 0, TextObject(), parent)
{
    reset();
    setAuthor(author);
}

Book::~Book()
{
    clearItems();
}

void Book::reset()
{
    setAuthor("");
    setGenre("");
    setState("");
    setSubject("");
    setDate(QDate::currentDate());
    setPublisher("");
    setRights("");
    setLanguage("");
}

QString Book::author() const
{
    const QVariant & v = attribute(AUTHOR_KEY);
    Q_ASSERT(v.type() == QVariant::String);
    return v.toString();
}
void Book::setAuthor(const QString &author)
{
    setAttribute(AUTHOR_KEY, author);
}

QString Book::genre() const
{
    const QVariant & v = attribute(GENRE_KEY);
    Q_ASSERT(v.type() == QVariant::String);
    return v.toString();
}
void Book::setGenre(const QString &genre)
{
    setAttribute(GENRE_KEY, genre);
}

QString Book::state() const
{
    const QVariant & v = attribute(STATE_KEY);
    Q_ASSERT(v.type() == QVariant::String);
    return v.toString();
}
void Book::setState(const QString &state)
{
    setAttribute(STATE_KEY, state);
}

QString Book::subject() const
{
    const QVariant & v = attribute(SUBJECT_KEY);
    Q_ASSERT(v.type() == QVariant::String);
    return v.toString();
}
void Book::setSubject(const QString &subject)
{
    setAttribute(SUBJECT_KEY, subject);
}

QDate Book::date() const
{
    return attribute(DATE_KEY).toDate();
}
void Book::setDate(const QDate &date)
{
    setAttribute(DATE_KEY, date);
}

QString Book::publisher() const
{
    const QVariant & v = attribute(PUBLISHER_KEY);
    Q_ASSERT(v.type() == QVariant::String);
    return v.toString();
}
void Book::setPublisher(const QString &publisher)
{
    setAttribute(PUBLISHER_KEY, publisher);
}

QString Book::rights() const
{
    const QVariant & v = attribute(RIGHTS_KEY);
    Q_ASSERT(v.type() == QVariant::String);
    return v.toString();
}
void Book::setRights(const QString &rights)
{
    setAttribute(RIGHTS_KEY, rights);
}

QString Book::language() const
{
    const QVariant & v = attribute(LANGUAGE_KEY);
    Q_ASSERT(v.type() == QVariant::String);
    return v.toString();
}
void Book::setLanguage(const QString &language)
{
    setAttribute(LANGUAGE_KEY, language);
}

#ifdef QEM_QML_TARGET
void Book::fireAttributeChange(const QString &name, const QVariant &value)
{
    if (AUTHOR_KEY == name) {
        emit authorChanged(value.toString());
    } else if (GENRE_KEY == name) {
        emit genreChanged(value.toString());
    } else if (STATE_KEY == name) {
        emit stateChanged(value.toString());
    } else if (SUBJECT_KEY == name) {
        emit subjectChanged(value.toString());
    } else if (DATE_KEY == name) {
        emit dateChanged(value.toDate());
    } else if (PUBLISHER_KEY == name) {
        emit publisherChanged(value.toString());
    } else if (RIGHTS_KEY == name) {
        emit rightsChanged(value.toString());
    } else if (LANGUAGE_KEY == name) {
        emit languageChanged(value.toString());
    } else {
        Chapter::fireAttributeRemove(name, value);
    }
}

void Book::fireAttributeRemove(const QString &name, const QVariant &value)
{
    if (AUTHOR_KEY == name) {
        emit authorChanged(QString());
    } else if (GENRE_KEY == name) {
        emit genreChanged(QString());
    } else if (STATE_KEY == name) {
        emit stateChanged(QString());
    } else if (SUBJECT_KEY == name) {
        emit subjectChanged(QString());
    } else if (DATE_KEY == name) {
        emit dateChanged(QDate());
    } else if (PUBLISHER_KEY == name) {
        emit publisherChanged(QString());
    } else if (RIGHTS_KEY == name) {
        emit rightsChanged(QString());
    } else if (LANGUAGE_KEY == name) {
        emit languageChanged(QString());
    } else {
        Chapter::fireAttributeRemove(name, value);
    }
}

#endif
