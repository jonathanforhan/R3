#include <QApplication>
#include <QWidget>
#include <QWindow>
#include "Editor.hpp"
#include <core/Engine.hpp>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("org.R3.editor");
  QApplication::setApplicationVersion("1.0.0");
  QApplication::setOrganizationName("R3");
  QApplication::setApplicationDisplayName("R3");
  QApplication::setFont(QFont{ "Consolas", 12 });

  R3::Engine& engine = R3::Engine::instance();
  engine.run();

  // Editor editor;
  /*
  auto* w = QWindow::fromWinId(winId);
  w->show();
  w->hide();
  QWidget* container = QWidget::createWindowContainer(w);
  container->show();
  */
  // editor.show();

  return app.exec();
}
