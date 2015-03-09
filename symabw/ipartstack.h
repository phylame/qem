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

#ifndef IMABW_IPARTSTACK_H
#define IMABW_IPARTSTACK_H

#include <QStack>
#include <QObject>


class Part;

namespace symabw {

class IPartStack : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int size READ size NOTIFY sizeChanged)
public:
    explicit IPartStack(QObject *parent = 0) :
        QObject(parent)
    {}
    
    inline int size() const
    {
        return m_stack.size();
    }

    Q_INVOKABLE inline Part* pop()
    {
        Part *p = m_stack.pop();
        emit sizeChanged(size());
        return p;
    }

    Q_INVOKABLE inline void push(Part *part)
    {
        m_stack.push(part);
        emit sizeChanged(size());
    }

    Q_INVOKABLE inline void clear()
    {
        m_stack.clear();
        emit sizeChanged(size());
    }

    Q_INVOKABLE inline Part* top() const
    {
        return m_stack.top();
    }

    Q_INVOKABLE inline Part* bottom()
    {
        return m_stack.data()[0];
    }


signals:
    void sizeChanged(int size);
public slots:
private:
    QStack<Part*> m_stack;
};

}   // end symabw

#endif // IMABW_IPARTSTACK_H
