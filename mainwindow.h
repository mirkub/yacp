#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "communicator.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow, public Communicator_receiver
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_lineEdit_returnPressed();

	void on_toolButton_clicked();

private:
    Ui::MainWindow *ui;
	Communicator* m_communicator;
	virtual void message_received(const std::string&, const std::string&);
	void exec_config_dialog();
};

#endif // MAINWINDOW_H
