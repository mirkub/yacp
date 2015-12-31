#include "configuration.h"
#include "ui_configuration.h"


#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>



configuration::configuration(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::configuration)
{
	ui->setupUi(this);

	// get list of NICs
	struct ifaddrs *ifaddr, *ifa;
	int family, s, n;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}

	for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
		if (ifa->ifa_addr == NULL) continue;

		family = ifa->ifa_addr->sa_family;

		if (family!=AF_INET) continue;

		s = getnameinfo(ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
						host, NI_MAXHOST,
						NULL, 0, NI_NUMERICHOST);
		if (s!=0) continue;
		char buf[200];
		sprintf(buf, "%s\t%s\n", ifa->ifa_name, host);
		ui->listWidget->addItem(buf);
	}

	freeifaddrs(ifaddr);
}

configuration::~configuration()
{
	delete ui;
}

void configuration::set_name(const std::string& name)
{
	ui->lineEdit->setText(QString::fromStdString(name));
}

std::string configuration::get_name() const
{
	return ui->lineEdit->text().toStdString();
}

std::string configuration::get_selected_nic() const
{
	QList<QListWidgetItem*> list = ui->listWidget->selectedItems();
	if (list.size()>=1)
	{
		return list[0]->text().toStdString();
	}
	return "";
}
