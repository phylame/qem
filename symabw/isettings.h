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

#ifndef IMABW_ISETTINGS_H
#define IMABW_ISETTINGS_H

#include <QLocale>
#include <QSettings>


namespace symabw {

class ISettings : public QSettings
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(bool theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(bool statusBar READ statusBar WRITE setStatusBar NOTIFY statusBarChanged)
    Q_PROPERTY(QString background READ background WRITE setBackground NOTIFY backgroundChanged)
    Q_PROPERTY(QString lastDirectory READ lastDirectory WRITE setLastDirectory)
    Q_PROPERTY(bool enterAfterAdd READ enterAfterAdd WRITE setEnterAfterAdd NOTIFY enterAfterAddChanged)
    Q_PROPERTY(int maxShownParts READ maxShownParts WRITE setMaxShownParts NOTIFY maxShownPartsChanged)
private:
    static const QString LANGUAGE_KEY;
    static const QString LAST_DIRECTORY_KEY;
    static const QString THEME_KEY;
    static const QString STATUS_BAR_KEY;
    static const QString BACKGROUND_KEY;
public:
    explicit ISettings(QObject *parent = 0) :
        QSettings(parent)
    {}

    /// Language
    inline QString language() const
    {
        return value(LANGUAGE_KEY, QLocale::system().name()).toString();
    }
    inline void setLanguage(const QString &name)
    {
        if (name != language()) {
            setValue(LANGUAGE_KEY, name);
            emit languageChanged(name);
        }
    }
    
    // true is white theme, false is black
    inline bool theme() const
    {
        return value(THEME_KEY, false).toBool();
    }
    inline void setTheme(bool whiteTheme)
    {
        if (whiteTheme != theme()) {
            setValue(THEME_KEY, whiteTheme);
            emit themeChanged(whiteTheme);
        }
    }

    /// true to show status bar, false not show
    inline bool statusBar() const
    {
        return value(STATUS_BAR_KEY, false).toBool();
    }
    inline void setStatusBar(bool enable)
    {
        if (enable != statusBar()) {
            setValue(STATUS_BAR_KEY, enable);
            emit statusBarChanged(enable);
        }
    }


    /// Path of background image
    inline QString background() const
    {
        return value(BACKGROUND_KEY).toString();
    }
    inline void setBackground(const QString &path)
    {
        if (path != background()) {
            setValue(BACKGROUND_KEY, path);
            emit backgroundChanged(path);
        }
    }

    /// Return last access directory.
    inline QString lastDirectory() const
    {
        return value(LAST_DIRECTORY_KEY, "").toString();
    }
    inline void setLastDirectory(const QString &path)
    {
        setValue(LAST_DIRECTORY_KEY, path);
    }

    /// Returns \c true if enter part when add to list/
    inline bool enterAfterAdd() const
    {
        return value("Operations/EnterAfterAdd", false).toBool();
    }
    inline void setEnterAfterAdd(bool enable)
    {
        if (enterAfterAdd() != enable) {
            setValue("Operations/EnterAfterAdd", enable);
            emit enterAfterAddChanged(enable);
        }
    }

    /// Maximum number of shown parts in PartListView.
    inline int maxShownParts() const
    {
        return value("Operations/MaxShownParts", 40).toInt();
    }
    inline void setMaxShownParts(int n)
    {
        if (maxShownParts() != n) {
            setValue("Operations/MaxShownParts", n);
            emit maxShownPartsChanged(n);
        }
    }

    /// Reset all settings to default
    Q_INVOKABLE inline void reset()
    {
        setTheme(false);
        setStatusBar(false);
        setBackground("");
        setEnterAfterAdd(false);
        setMaxShownParts(40);
    }

signals:
    void languageChanged(const QString &name);
    void themeChanged(bool whiteTheme);
    void statusBarChanged(bool enable);
    void backgroundChanged(const QString &path);
    void enterAfterAddChanged(bool enable);
    void maxShownPartsChanged(int n);
public slots:
    
};

}   // end symabw

#endif // IMABW_ISETTINGS_H
