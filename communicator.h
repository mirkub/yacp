#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <zyre.h>
#include <string>

class Communicator_receiver
{
public:
	virtual void message_received(const std::string&, const std::string&) = 0;
};

class Communicator
{
public:
    Communicator();
	~Communicator();

	std::string get_name() { return m_name; }
	void set_name(const std::string& name) { m_name = name; }

	void set_nic(const std::string& nic) { m_nic = nic; }
	std::string get_nic() const { return m_nic; }

	void register_message_receiver(Communicator_receiver* r) { m_receiver=r; }
	void send_message(const std::string&);

	bool load_config();
	bool save_config();

	void reset();

	Communicator_receiver* m_receiver;

private:
	zactor_t* m_actor;
	std::string m_name;
	std::string m_nic;
};

#endif // COMMUNICATOR_H
