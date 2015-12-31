#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "communicator.h"
#include "configuration.h"

#include <QtCore>
#include <QMessageBox>

#include "zyre.h"
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	m_communicator=new Communicator();
	if (!m_communicator->load_config())
	{
		exec_config_dialog();
	}
	m_communicator->register_message_receiver(this);
}

MainWindow::~MainWindow()
{
    delete ui;
	delete m_communicator;
}

void MainWindow::on_pushButton_clicked()
{
//    QMessageBox::information(this, "title", ui->lineEdit->text());
    QString text = ui->lineEdit->text();
	m_communicator->send_message(text.toStdString());

	ui->textEdit->setTextColor(QColor(150,50,0));
	QString qname = QString::fromStdString(m_communicator->get_name()+": ");
	ui->textEdit->append(qname);
	ui->textEdit->setTextColor(QColor(0,0,0));
	ui->textEdit->insertPlainText(text);
    ui->lineEdit->clear();
}

void MainWindow::on_lineEdit_returnPressed()
{
    on_pushButton_clicked();
}

void MainWindow::message_received(const std::string& name, const std::string& msg)
{
	QString qmsg = QString::fromStdString(msg);
	QString qname = QString::fromStdString(name+": ");
	ui->textEdit->setTextColor(QColor(0,150,50));
	ui->textEdit->append(qname);
	ui->textEdit->setTextColor(QColor(0,0,0));
	ui->textEdit->insertPlainText(qmsg);
}


void MainWindow::on_toolButton_clicked()
{
	exec_config_dialog();
}

void MainWindow::exec_config_dialog()
{
	configuration c;
	c.set_name(m_communicator->get_name());
	if (c.exec())
	{
		m_communicator->set_name(c.get_name());
		string selected_nic = c.get_selected_nic();
		size_t tpos = selected_nic.find("\t");
		if (tpos!=string::npos)
		{
			selected_nic.resize(tpos);
		}
		m_communicator->set_nic(selected_nic);
		m_communicator->save_config();
		m_communicator->reset();
	}
}
