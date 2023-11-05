#pragma once
#include <QMainWindow>

class Editor : public QMainWindow {
    Q_OBJECT

public:
    Editor(QWidget* parent = nullptr);
    ~Editor();

    void createDockUI();

    void closeEvent(QCloseEvent* event) override;

    void runEngine();

signals:
    void finished();

public:
    QWidget* dockWidget;
};
