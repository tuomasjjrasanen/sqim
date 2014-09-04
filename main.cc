// SQIM - Simple Qt Image Manager
// Copyright (C) 2014 Tuomas Räsänen <tuomasjjrasanen@tjjr.fi>

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

static QStringList mainInitialPaths;
static bool mainRecursiveOpen;

static void mainPrintHelp()
{
    QTextStream cout(stdout);

    cout << "Usage: sqim [OPTIONS] [DIR|FILE]..." << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << " -h, --help         display this help and exit" << endl;
    cout << " -r, --recursive    search DIR recursively" << endl;
    cout << endl;
    cout << "Parameters:" << endl;
    cout << " DIR                directory to search for images" << endl;
    cout << " FILE               image to import" << endl;
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
    int i = 1;
    while (i != args.length()) {
        QString arg = args.at(i);
        ++i;

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
    mainInitialPaths = args.mid(i - 1);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    mainParseArgs(app);

    MainWindow w;

    w.openPaths(mainInitialPaths, mainRecursiveOpen);

    w.show();

    return app.exec();
}
