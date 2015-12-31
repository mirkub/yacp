#include "communicator.h"
#include <iostream>
#include <fstream>

using namespace std;


static void chat_actor(zsock_t *pipe, void *params)
{
	Communicator*  comm = (Communicator*)params;
	zyre_t *node = zyre_new (comm->get_name().c_str());
	if (!node)
		return;		 //  Could not create new node
	//zyre_set_verbose (node);  // uncomment to watch the events

	if (!comm->get_nic().empty())
	{
		zyre_set_interface(node, comm->get_nic().c_str());
	}

	zyre_start (node);
	zyre_join (node, "CHAT");
	zsock_signal (pipe, 0);	 //  Signal "ready" to caller

	bool terminated = false;
	zpoller_t *poller = zpoller_new (pipe, zyre_socket (node), NULL);
	while (!terminated) {
		void *which = zpoller_wait (poller, -1); // no timeout
		if (which == pipe)
		{
			zmsg_t *msg = zmsg_recv (which);
			if (!msg)
				break;		  //  Interrupted
			char *command = zmsg_popstr (msg);
			if (streq (command, "$TERM")) {
				terminated = true;
			}
			else
				if (streq (command, "SHOUT")) {
					char *string = zmsg_popstr (msg);
					zyre_shouts (node, "CHAT", "%s", string);
				}
				else {
					puts ("E: invalid message to actor");
					assert (false);
				}
			free (command);
			zmsg_destroy (&msg);
		}
		else if (which == zyre_socket (node))
		{
			zmsg_t *msg = zmsg_recv (which);
			char *event = zmsg_popstr (msg);
			char *peer = zmsg_popstr (msg);
			char *name = zmsg_popstr (msg);
			char *group = zmsg_popstr (msg);
			char *message = zmsg_popstr (msg);

			if (streq (event, "ENTER")) {
				comm->m_receiver->message_received(name, "has joined the chat");
			}
			else if (streq (event, "EXIT")) {
				comm->m_receiver->message_received(name, "has left the chat");
			}
			else if (streq (event, "SHOUT")) {
				comm->m_receiver->message_received(name, message);
				printf ("%s: %s\n", name, message);
			}
			else if (streq (event, "EVASIVE")) {
				printf ("%s is being evasive\n", name);
			}
			//printf ("Message from node\n");
			//printf ("event: %s peer: %s  name: %s\n  group: %s message: %s\n", event, peer, name, group, message);

			free (event);
			free (peer);
			free (name);
			free (group);
			free (message);
			zmsg_destroy (&msg);
		}
	}
	zpoller_destroy (&poller);

	// Notify peers that this peer is shutting down. Provide
	// a brief interval to ensure message is emitted.
	zyre_stop(node);
	zclock_sleep(100);

	zyre_destroy (&node);
}


Communicator::Communicator()
: m_receiver(0)
, m_name("anonymous")
{
	m_actor = zactor_new (chat_actor, (void*)this);
	assert (m_actor);
}

Communicator::~Communicator()
{
	zactor_destroy(&m_actor);
}



void Communicator::send_message(const std::string& msg)
{
	zstr_sendx (m_actor, "SHOUT", msg.c_str(), NULL);
}


bool Communicator::load_config()
{
	char *home = getenv ("HOME");
	char filename[200];
	if (home != NULL) {
		snprintf(filename, sizeof(filename), "%s/.yacp/properties", home);
		if (access(filename, F_OK))
		{
			cerr << "DEBUG: can't open configuration file " << filename << endl;
			return false;
		}
	}
	else
	{
		cerr << "HOME is unknown" << endl;
		return false;
	}
	ifstream cfile(filename);
	string line;
	while (cfile.good())
	{
		getline(cfile, line);

		const string name_str = "name = ";
		if (line.compare(0, name_str.length(), name_str) == 0)
		{
			set_name(line.substr(name_str.size()));
		}

		const string nic_str = "NIC = ";
		if (line.compare(0, nic_str.length(), nic_str) == 0)
		{
			set_nic(line.substr(nic_str.size()));
		}
	}
	return true;
}

bool Communicator::save_config()
{
	char *home = getenv ("HOME");
	char filename[200];
	if (home != NULL) {
		char path[200];
		snprintf(path, sizeof(path), "%s/.yacp", home);
		snprintf(filename, sizeof(filename), "%s/.yacp/properties", home);
		if (mkdir(path, 0700))
		{
			std::cerr << "WARN: mkdir("<<path<<") failed " << strerror(errno) << std::endl;
		}
	}
	else
	{
		cerr << "HOME is unknown" << endl;
		return false;
	}
	ofstream cfile;
	cfile.open(filename);
	cfile << "name = " << get_name() << endl;
	cfile << "NIC = " << get_nic() << endl;

	return true;
}


void Communicator::reset()
{
	zactor_destroy(&m_actor);
	m_actor = zactor_new (chat_actor, (void*)this);
	assert (m_actor);
}

