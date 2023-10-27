#include <QApplication>
#include <QWidget>
#include <QWindow>
#include <QThread>
#include "Editor.hpp"
#include <core/Engine.hpp>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("org.R3.editor");
  QApplication::setApplicationVersion("1.0.0");
  QApplication::setOrganizationName("R3");
  QApplication::setApplicationDisplayName("R3");

  QThread* th = new QThread;
  Editor* editor = new Editor;
  editor->moveToThread(th);

  th->connect(th, &QThread::started, editor, &Editor::runEngine);
  th->connect(editor, &Editor::finished, th, &QThread::quit);
  th->connect(editor, &Editor::finished, editor, &Editor::deleteLater);
  th->connect(th, &QThread::finished, th, &QThread::deleteLater);
  th->start();

  return app.exec();
}
