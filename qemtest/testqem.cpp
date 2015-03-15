/*
 * Copyright 2014 Peng Wan
 *
 * This file is part of Qem test suite.
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

#include <QCoreApplication>
#include <QtTest/QtTest>
#include "testattributes.h"
#include "testfileobject.h"
#include "testtextobject.h"
#include "testpart.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    int err = 0;
    {
        TestAttributes testAttributes;
        err = qMax(err, QTest::qExec(&testAttributes, app.arguments()));
    }
    {
        TestFileObject testFileObject;
        err = qMax(err, QTest::qExec(&testFileObject, app.arguments()));
    }
    {
        TestTextObject testTextObject;
        err = qMax(err, QTest::qExec(&testTextObject, app.arguments()));
    }
    {
        TestPart testPart;
        err = qMax(err, QTest::qExec(&testPart, app.arguments()));
    }
    if (err == 0) {
        qDebug("All tests executed successfully");
    } else {
        qWarning("There were errors in some of the tests above.");
    }
    return err;
}
