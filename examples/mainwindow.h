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
    static void createDir();
    static QString exampleDir();
private:
    Ui::MainWindow *ui;
protected:
    void showEvent(QShowEvent *) override;
private slots: 
    void RemoveTestBase();
    void CreateDataBaseFromPGNFile();

};

#endif // MAINWINDOW_H
