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

#ifndef QEM_GLOBAL_H
#define QEM_GLOBAL_H

#ifndef QEM_VERSION
# define QEM_VERSION "1.3.1"
#endif

/**
  This is automatically defined when building a static library, but when
  including sources directly into a project, QEM_STATIC should
  be defined explicitly to avoid possible troubles with unnecessary
  importing/exporting.
  */
#if defined(QEM_STATIC)
#  define QEM_SHARED_EXPORT
#else
/**
 * To build DLL with MSVC, QEM_BUILD must be defined.
 * qglobal.h takes care of defining Q_DECL_* correctly for msvc/gcc.
 */
#if defined(QEM_BUILD)
#  define QEM_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define QEM_SHARED_EXPORT Q_DECL_IMPORT
#endif
#endif  // QEM_STATIC

#ifndef QEM_NAMESPACE   /* no namespace */

# define QEM_PREPEND_NAMESPACE(name) ::name
# define QEM_USE_NAMESPACE
# define QEM_USE_TYPE(name)
# define QEM_BEGIN_NAMESPACE
# define QEM_END_NAMESPACE
# define QEM_FORWARD_DECLARE_CLASS(name) class name;
# define QEM_FORWARD_DECLARE_STRUCT(name) struct name;

#else   /* using namespace */

# define QEM_PREPEND_NAMESPACE(name) ::QEM_NAMESPACE::name
# define QEM_USE_NAMESPACE using namespace ::QEM_NAMESPACE;
# define QEM_USE_TYPE(name) using QEM_PREPEND_NAMESPACE(name);
# define QEM_BEGIN_NAMESPACE namespace QEM_NAMESPACE {
# define QEM_END_NAMESPACE }
# define QEM_FORWARD_DECLARE_CLASS(name) \
    QEM_BEGIN_NAMESPACE class name; QEM_END_NAMESPACE \
    using QEM_PREPEND_NAMESPACE(name);
# define QEM_FORWARD_DECLARE_STRUCT(name) \
    QEM_BEGIN_NAMESPACE struct name; QEM_END_NAMESPACE \
    using QEM_PREPEND_NAMESPACE(name);

#endif  /* qem namespace */

#include <QtCore/qglobal.h>

// QML target config
#ifdef QEM_QML_TARGET
# include <QObject>
# define QEM_INVOKABLE Q_INVOKABLE
#else
# define QEM_INVOKABLE
#endif

QEM_BEGIN_NAMESPACE

/// Reference pointer.
template<class T> struct ref_ptr
{
    /// The pointer for data.
    T *data;
    /// Reference count of this object.
    size_t ref;
    /// Whether the data is deleted when delete ref_ptr.
    bool autoDelete;
    /// Constructs ref_ptr object associated with data \a p and \a autoDelete.
    inline ref_ptr(T *p, bool autoDelete, size_t ref = 1) :
        data(p), ref(ref), autoDelete(autoDelete)
    {}
    /// Destroys ref_ptr object and delete data if autoDelete is \c true.
    inline ~ref_ptr()
    {
        if (autoDelete) {
            delete data;
        }
    }
};

QEM_END_NAMESPACE

#endif // QEM_GLOBAL_H
