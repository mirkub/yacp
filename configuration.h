#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QDialog>

namespace Ui {
class configuration;
}

class configuration : public QDialog
{
	Q_OBJECT

public:
	explicit configuration(QWidget *parent = 0);
	~configuration();

	std::string get_name() const;
	std::string get_selected_nic() const;
	void set_name(const std::string& name);

private:
	Ui::configuration *ui;
};

#endif // CONFIGURATION_H
