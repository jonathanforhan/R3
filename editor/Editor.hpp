#pragma once
#include <QMainWindow>

class Editor : public QMainWindow {
  Q_OBJECT

public:
  Editor(QWidget* parent = nullptr);
  ~Editor();

protected:
  QSize sizeHint() const override;
};
