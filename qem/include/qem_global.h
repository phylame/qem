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

#include <QtCore/qglobal.h>


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

#ifndef QEM_VERSION
# define QEM_VERSION "1.2.5"
#endif

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

#endif // QEM_GLOBAL_H
