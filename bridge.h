
#include <iostream>
#include <vector>
#include <iterator>
#include <queue>
#include <algorithm>
#include <map>

#ifndef _BRIDGE_AND_CONFIG_MESSAGE_
#define _BRIDGE_AND_CONFIG_MESSAGE_

class configMessage
{
	int myroot, mydistance, myid, myport;
	
	public:
	configMessage();
	configMessage(int a, int b, int c, int d);
	configMessage(const configMessage &a);
	configMessage& operator=(const configMessage &a);
	int getRoot() const;
	int getDistance() const;
	int getID() const;
	int getPort() const;
	void changePort(int p);
	bool operator<( const configMessage &b) const;
	bool operator==(configMessage &a);
};

std::ostream& operator<<(std::ostream &os, const configMessage &msg);

class bridge
{
	std::map<int, bool> myPorts;
	std::map<int , bool> sendingPorts;
	std::priority_queue<configMessage> listOfReceivedConfigs;
	std::queue<int> packetRecQueue, psender, preceiver;
	std::map<int, int> forwardingTable;
	configMessage myStatus;		//root bridge, distance from it, next hop, rootport
	int src, dest;
	int bridgeId;
	int bestReceivingPort;
	void improve(configMessage rec);
	configMessage makeSendingConfig();
	bool isNullPort(int pport);
	void resetSendingPorts();
	bool traceOfNull;

	public:
	bridge(int myid);
	void registerPort(int t);
	void processReceivedList(bool trace, int mytime);
	configMessage getConfig(bool trace, int mytime);
	bool isRoot();
	int getRoot();
	int getRootPort();
	bool askForDP(int pport);
	int numberOfDps();
	void receiveConfig(configMessage msg);
	void receivePacket(int pport, int sender, int receiver, int ssrc, int ddest);
	bool processPacket(bool trace, int time);
	bool askToSend(int pport);
	void printPorts();
	void printTable();
};

#endif




