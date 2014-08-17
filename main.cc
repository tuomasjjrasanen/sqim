// sqim - simply qute image manager
// Copyright (C) 2013 Tuomas Räsänen <tuomasjjrasanen@tjjr.fi>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <QApplication>

#include "mainwindow.hh"

static QString mainInitialDir;
static bool mainRecursiveOpen;

static void mainPrintHelp()
{
    QTextStream cout(stdout);

    cout << "Usage: sqim [OPTIONS]" << endl;
    cout << "Usage: sqim [OPTIONS] [--] DIR" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << " -h, --help         display this help and exit" << endl;
    cout << " -r, --recursive    search DIR recursively" << endl;
    cout << endl;
    cout << "Parameters:" << endl;
    cout << " DIR                directory to search for images" << endl;
}

static void mainPrintError(QString message)
{
    QTextStream cerr(stderr);

    cerr << QString("error: %1").arg(message) << endl;
    cerr << "Try --help for more information." << endl;
}

static void mainParseArgs(QApplication &app)
{
    QStringList args = app.arguments();

    // Skip the first argument which is the program name in Linux.
    QStringList::ConstIterator i;
    for (i = args.constBegin() + 1; i != args.constEnd(); ++i) {
        QString arg = *i;

        if (arg == "--help" || arg == "-h") {
            mainPrintHelp();
            exit(0);
        } else if (arg == "--recursive" || arg == "-r") {
            mainRecursiveOpen = true;
            continue;
        } else if (arg == "--" || !arg.startsWith("-")) {
            // Option parsing stops, positional parameter parsing
            // starts.
            break;
        }

        mainPrintError(QString("unrecognized argument '%1'").arg(arg));
        exit(1);
    }

    if (i != args.constEnd()) {
        mainInitialDir = *i;
        ++i;
    }

    if (i != args.constEnd()) {
        mainPrintError("too many arguments");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    mainParseArgs(app);

    MainWindow w;

    if (!mainInitialDir.isEmpty())
        w.openDir(mainInitialDir, mainRecursiveOpen);

    w.show();

    return app.exec();
}
