#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Progress;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
private slots: 
     void Test();
    void RemoveTestBase();
    void refredProgressBar(int value, int total, int type);
};

#endif // MAINWINDOW_H
