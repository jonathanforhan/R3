#include "Editor.hpp"
#include <core/Engine.cpp>
#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QMessageBox>
#include <QCloseEvent>

Editor::Editor(QWidget* parent)
    : QMainWindow(parent) {}

Editor::~Editor() {}

void Editor::closeEvent(QCloseEvent* event) {
  QMessageBox::StandardButton bQuit =
      QMessageBox::question(this, "R3", tr("Are you sure you want to quit?\n"),
                            QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

  if (bQuit != QMessageBox::Yes) {
    event->ignore();
  } else {
    event->accept();
    R3::Engine::instance()->window.kill();
    emit finished();
  }
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

QSize Editor::sizeHint() const {
  QRect rect{window()->windowHandle()->screen()->geometry()};
  return QSize{rect.width(), rect.height()} * 0.75;
}
