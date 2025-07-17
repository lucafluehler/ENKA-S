#pragma once

#include <QMainWindow>

#include "i_main_window_view.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public IMainWindowView {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void updateHomeScreen() override;

private:
    Ui::MainWindow *ui_;
};
