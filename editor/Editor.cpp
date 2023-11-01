#include "Editor.hpp"
#include <QApplication>
#include <QCloseEvent>
#include <QMessageBox>
#include <QScreen>
#include <QWindow>
#include <core/Engine.cpp>

Editor::Editor(QWidget* parent)
    : QMainWindow(parent) {
    QRect rect{QGuiApplication::primaryScreen()->geometry()};
    qint32 w = rect.width() * 0.75;
    qint32 h = rect.height() * 0.75;
    qint32 x = (rect.width() - w) / 2;
    qint32 y = (rect.height() - h) / 2;
    setGeometry(x, y, w, h);
}

Editor::~Editor() {}

void Editor::closeEvent(QCloseEvent* event) {
    QMessageBox::StandardButton e_quit =
        QMessageBox::question(this, "R3", tr("Are you sure you want to quit?\n"),
                              QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

    if (e_quit != QMessageBox::Yes) {
        event->ignore();
        return;
    }

    event->accept();
    R3::Engine::instance()->window.kill();
    emit finished();
}

int Editor::runEngine() {
    R3::Engine* engine = R3::Engine::instance();

    WId win_id = reinterpret_cast<WId>(engine->window.native_id());
    QWindow* win{QWindow::fromWinId(win_id)};
    QWidget* container = QWidget::createWindowContainer(win);

    setCentralWidget(container);
    show();

    return engine->run();
}
