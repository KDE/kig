/*
    This file is part of Kig, a KDE program for Interactive Geometry.
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kig.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QLibrary>

#include <KAboutData>
#include <KCrash>
#include <KPluginMetaData>

#include "aboutdata.h"

static int convertToNative(const QUrl &file, const QByteArray &outfile)
{
    KPluginMetaData libraryLoader(QStringLiteral("kf" QT_STRINGIFY(QT_VERSION_MAJOR)) + QStringLiteral("/parts/kigpart"));
    QLibrary library(libraryLoader.fileName());
    int (*converterfunction)(const QUrl &, const QByteArray &);
    converterfunction = (int (*)(const QUrl &, const QByteArray &))library.resolve("convertToNative");
    if (!converterfunction) {
        qCritical() << "Error: broken Kig installation: different library and application version !";
        return -1;
    }
    return (*converterfunction)(file, outfile);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("kig");
    KAboutData about = kigAboutData("kig");
    KCrash::initialize();
    QCommandLineParser parser;
    QCommandLineOption convertToNativeOption(
        QStringList() << QStringLiteral("c") << QStringLiteral("convert-to-native"),
        i18n("Do not show a GUI. Convert the specified file to the native Kig format. Output goes to stdout unless --outfile is specified."));
    QCommandLineOption outfileOption(QStringList() << QStringLiteral("o") << QStringLiteral("outfile"),
                                     i18n("File to output the created native file to. '-' means output to stdout. Default is stdout as well."),
                                     QStringLiteral("file"));

    QCoreApplication::setApplicationName(QStringLiteral("kig"));
    QCoreApplication::setApplicationVersion(KIG_VERSION_STRING);
    QCoreApplication::setOrganizationDomain(i18n("kde.org"));
    KAboutData::setApplicationData(about);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("kig")));

    about.setupCommandLine(&parser);
    parser.addOption(convertToNativeOption);
    parser.addOption(outfileOption);
    parser.addPositionalArgument(QStringLiteral("URL"), i18n("Document to open"));
    parser.process(app);
    about.processCommandLine(&parser);

    QStringList urls = parser.positionalArguments();

    if (parser.isSet(QStringLiteral("convert-to-native"))) {
        QString outfile = parser.value(QStringLiteral("outfile"));
        if (outfile.isNull())
            outfile = '-';
        if (urls.isEmpty()) {
            qCritical() << "Error: --convert-to-native specified without a file to convert.";
            return -1;
        }
        if (urls.count() > 1) {
            qCritical() << "Error: --convert-to-native specified with more than one file to convert.";
            return -1;
        }
        return convertToNative(QUrl::fromLocalFile(urls[0]), outfile.toLocal8Bit());
    } else {
        if (parser.isSet(QStringLiteral("outfile"))) {
            qCritical() << "Error: --outfile specified without convert-to-native.";
            return -1;
        }

        // see if we are starting with session management
        if (app.isSessionRestored()) {
            kRestoreMainWindows<Kig>();
        }

        Kig *widget = new Kig;
        widget->show();

        if (!urls.isEmpty()) {
            widget->load(QUrl::fromLocalFile(urls[0]));
        }

        return app.exec();
    }
}
