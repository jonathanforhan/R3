#pragma once
#include <QMainWindow>

class Editor : public QMainWindow {
  Q_OBJECT

public:
  Editor(QWidget* parent = nullptr);
  ~Editor();

  void closeEvent(QCloseEvent* event) override;

  int runEngine();

signals:
  void finished();
};
