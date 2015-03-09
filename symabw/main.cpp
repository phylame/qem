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

#include <QTranslator>
#include <QApplication>
#include <QtDeclarative>
#include <QSplashScreen>
#include "qmlapplicationviewer.h"
#include <qem.h>
#include "imabw.h"
#include "iutility.h"
#include "isettings.h"
#include "ipartstack.h"


Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    app->setApplicationName(symabw::Imabw::name());
    app->setApplicationVersion(symabw::Imabw::version());
    app->setOrganizationName(symabw::Imabw::vendor());

    // splash screen
    QSplashScreen splash(QPixmap(":/splash"));
    splash.showFullScreen();

    // settings
    symabw::ISettings settings;
    // utility
    symabw::IUtility utility;
    // part stack
    symabw::IPartStack partStack;
    // Imabw version
    symabw::Imabw imabw;

    QTranslator translator;
    const QString &trans = "imabw_"+settings.language();
    if (translator.load(trans, ":/i18n")) {
        app->installTranslator(&translator);
    } else {
        qDebug() << "Failed to load translation:" << trans;
    }

    qmlRegisterUncreatableType<FileObject>("pw.symbian.imabw", 1, 0, "IFile",
                                           "Abstract class cannot be instantiated.");
    qmlRegisterType<TextObject>("pw.symbian.imabw", 1, 0, "IText");
    qmlRegisterType<Item>("pw.symbian.imabw", 1, 0, "IItem");
    qmlRegisterType<Part>("pw.symbian.imabw", 1, 0, "IPart");
    qmlRegisterType<Chapter>("pw.symbian.imabw", 1, 0, "IChapter");
    qmlRegisterType<Book>("pw.symbian.imabw", 1, 0, "IBook");

    QmlApplicationViewer viewer;
    QDeclarativeContext* context = viewer.rootContext();
    context->setContextProperty("settings", &settings);
    context->setContextProperty("utility", &utility);
    context->setContextProperty("parts", &partStack);
    context->setContextProperty("imabw", &imabw);

    viewer.setMainQmlFile(QLatin1String("qml/symabw/main.qml"));
    viewer.showExpanded();

    splash.finish(&viewer);
    return app->exec();
}
