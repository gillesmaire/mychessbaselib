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
    QMap<QString,bool> mBaseInUse;
protected:
    void showEvent(QShowEvent *) override;
private slots: 
    void RemoveTestBase();
    void CreateDataBaseFromPGNFile();
    QMap<QString,int> ListScid5DataBase();


    void TestDuplicate();
};

#endif // MAINWINDOW_H
