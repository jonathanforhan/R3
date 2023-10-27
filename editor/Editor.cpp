#include "Editor.hpp"
#include <QApplication>
#include <QScreen>

Editor::Editor(QWidget* parent)
    : QMainWindow(parent) {}

Editor::~Editor() {}

QSize Editor::sizeHint() const {
  QRect rect{QApplication::primaryScreen()->geometry()};
  return QSize{rect.width(), rect.height()} * 0.75;
}
