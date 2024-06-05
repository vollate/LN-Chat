// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <random>

#include "ClientController.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    ClientController clientController;
    engine.rootContext()->setContextProperty("clientController", &clientController);

    ClientManager clientManager;
    engine.rootContext()->setContextProperty("clientManager", &clientManager);

    ServerManager serverManager;
    engine.rootContext()->setContextProperty("serverManager", &serverManager);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if(!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
    engine.addImportPath(":/");

    engine.load(url);

    if(engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}