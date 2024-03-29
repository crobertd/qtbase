/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QFile>
#include <QDir>

#include <iostream>

static const char usage1[] =
"\nTests various file functionality in Qt\n\n"
"Usage: ";
static const char usage2[] =" [KEYWORD] [ARGUMENTS]\n\n"
"Keywords: ls  FILES             list file information\n"
"          mv  SOURCE TARGET     rename files using QFile::rename\n"
"          cp  SOURCE TARGET     copy files using QFile::copy\n"
"          rm  FILE              remove file using QFile::remove\n"
"          rmr DIR               remove directory recursively\n"
"                                using QDir::removeRecursively\n";

static int ls(int argCount, char **args)
{
    for (int i = 0 ; i < argCount; ++i) {
        const QFileInfo fi(QString::fromLocal8Bit(args[i]));
        std::cout << QDir::toNativeSeparators(fi.absoluteFilePath()).toStdString() << ' ' << fi.size();
        if (fi.exists())
            std::cout << " [exists]";
        if (fi.isFile())
            std::cout << " [file]";
        if (fi.isSymLink()) {
            std::cout << " [symlink to "
                      << QDir::toNativeSeparators(fi.symLinkTarget()).toStdString() << ']';
        }
        if (fi.isDir())
            std::cout << " [dir]";
        std::cout << std::endl;
    }
    return 0;
}

static int mv(const char *sourceFileName, const char *targetFileName)
{
    QFile sourceFile(QString::fromLocal8Bit(sourceFileName));
    if (!sourceFile.rename(QString::fromLocal8Bit(targetFileName))) {
        qWarning().nospace() << sourceFile.errorString();
        return -1;
    }
    return 0;
}

static int cp(const char *sourceFileName, const char *targetFileName)
{
    QFile sourceFile(QString::fromLocal8Bit(sourceFileName));
    if (!sourceFile.copy(QString::fromLocal8Bit(targetFileName))) {
        qWarning().nospace() << sourceFile.errorString();
        return -1;
    }
    return 0;
}

static int rm(const char *fileName)
{
    QFile file(QString::fromLocal8Bit(fileName));
    if (!file.remove()) {
        qWarning().nospace() << file.errorString();
        return -1;
    }
    return 0;
}

static int rmr(const char *dirName)
{
    QDir dir(QString::fromLocal8Bit(dirName));
    if (!dir.removeRecursively()) {
        qWarning().nospace() << "Failed to remove " << dir.absolutePath();
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Q_UNUSED(a)
    if (argc >= 3 && !qstrcmp(argv[1], "ls"))
        return ls(argc -2, argv + 2);

    if (argc == 4 && !qstrcmp(argv[1], "mv"))
        return mv(argv[2], argv[3]);

    if (argc == 4 && !qstrcmp(argv[1], "cp"))
        return cp(argv[2], argv[3]);

    if (argc == 3 && !qstrcmp(argv[1], "rm"))
        return rm(argv[2]);

    if (argc == 3 && !qstrcmp(argv[1], "rmr"))
        return rmr(argv[2]);

    std::cerr << usage1 << argv[0] << usage2;
    return 0;
}
