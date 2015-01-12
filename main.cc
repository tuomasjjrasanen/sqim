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
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>

#include "mainwindow.hh"

static void printHelp()
{
    QTextStream cout(stdout);

    cout << "Usage: sqim [OPTIONS] [DIR|FILE]..." << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << " -h, --help         display this help and exit" << endl;
    cout << " -r, --recursive    search DIR recursively" << endl;
    cout << "     --version      output version information and exit" << endl;
    cout << endl;
    cout << "Parameters:" << endl;
    cout << " DIR                directory to search for images" << endl;
    cout << " FILE               image to import" << endl;
}

static void printVersion()
{
    QTextStream cout(stdout);

    cout << QString::fromUtf8(
        "Simple Qt Image Manager " VERSION "\n"
        "Copyright © 2014 Tuomas Räsänen\n"
        "This program is free software: you can redistribute it and/or modify\n"
        "it under the terms of the GNU General Public License as published by\n"
        "the Free Software Foundation, either version 3 of the License, or\n"
        "(at your option) any later version.\n"
        "\n"
        "This program is distributed in the hope that it will be useful, but\n"
        "WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
        "\n"
        "See the <https://www.gnu.org/licenses/gpl-3.0.html>GNU\n"
        "General Public License for more details.\n");
}

static void printError(QString message)
{
    QTextStream cerr(stderr);

    cerr << QString("error: %1").arg(message) << endl;
    cerr << "Try --help for more information." << endl;
}

static QHash<QString, QVariant> parseArgs(QStringList args)
{
    QHash<QString, QVariant> options;

    options["recursive"] = false;

    // Skip the first argument which is the program name in Linux.
    args.takeFirst();

    while (!args.isEmpty()) {
        QString arg = args.first();

        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg == "--recursive" || arg == "-r") {
            options["recursive"] = true;
            args.takeFirst();
            continue;
        } else if (arg == "--" || !arg.startsWith("-")) {
            // Option parsing stops, positional parameter parsing
            // starts.
            break;
        }

        printError(QString("unrecognized argument '%1'").arg(arg));
        exit(1);
    }

    options["paths"] = args;

    return options;
}

static void prepareDatabase()
{
    QTextStream cerr(stdout);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QDir::home().mkdir(".sqim");
    db.setDatabaseName(QString("%1/%2/%3")
                       .arg(QDir::homePath())
                       .arg(".sqim")
                       .arg("db.sqlite3"));

    if (!db.open()) {
        // If the database cannot be opened, there's nothing to be done here.
        cerr << "error: failed to open database:"
             << db.lastError().databaseText() << endl;
        exit(1);
    }

    if (!db.tables().isEmpty()) {
        // Assume database is valid if it has tables. TODO: implement robust
        // database validation check.
        return;
    }

    if (!db.transaction()) {
        cerr << "error: failed to begin initialization transaction:"
             << db.lastError().databaseText() << endl;
        exit(1);
    }

    QSqlQuery query;

    if (!query.exec("CREATE TABLE Image ("
                    "  id INTEGER PRIMARY KEY,"
                    "  file_path TEXT NOT NULL,"
                    "  file_size INTEGER NOT NULL,"
                    "  mtime TEXT NOT NULL,"
                    "  pixel_width INTEGER NOT NULL,"
                    "  pixel_height INTEGER NOT NULL,"
                    "  exif_datetime TEXT NOT NULL,"
                    "  exif_orientation INTEGER NOT NULL,"
                    "  thumbnail_file_path TEXT NOT NULL,"
                    "  thumbnail_pixel_width INTEGER NOT NULL,"
                    "  thumbnail_pixel_height INTEGER NOT NULL,"
                    "  UNIQUE(file_path));")) {
        cerr << "error: failed to create Image table:"
             << query.lastError().databaseText() << endl;
        exit(1);
    }

    if (!query.exec("CREATE TABLE Tagging ("
                    "  id INTEGER PRIMARY KEY,"
                    "  file_path TEXT NOT NULL,"
                    "  tag TEXT NOT NULL,"
                    "  UNIQUE(file_path, tag));")) {
        cerr << "error: failed to create Tagging table:"
             << query.lastError().databaseText() << endl;
        exit(1);
    }

    if (!db.commit()) {
        cerr << "error: failed to commit the initial transaction:"
             << db.lastError().databaseText() << endl;
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationDomain("tjjr.fi");
    app.setApplicationName("sqim");

    QFile styleSheetFile(":sqim.qss");
    styleSheetFile.open(QFile::ReadOnly);
    app.setStyleSheet(styleSheetFile.readAll());

    QHash<QString, QVariant> options = parseArgs(app.arguments());

    prepareDatabase();

    MainWindow mainWindow;
    mainWindow.openPaths(options["paths"].toStringList(),
                         options["recursive"].toBool());
    mainWindow.show();

    return app.exec();
}
