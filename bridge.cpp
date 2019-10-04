
#include "bridge.h"

configMessage::configMessage()
{
	myport = myroot = mydistance = myid = -1;
	return;
}

configMessage::configMessage(int a, int b, int c, int d)
{
	myroot = a, mydistance = b, myid = c, myport = d;
}

configMessage::configMessage(const configMessage &a)
{
	myroot = a.myroot, mydistance = a.mydistance, myid = a.myid, myport = a.myport;
	return;
}

configMessage& configMessage::operator=( const configMessage &a)
{
	myroot = a.myroot, mydistance = a.mydistance, myid = a.myid, myport = a.myport;
	return *this;
}

int configMessage::getRoot() const
{
	return myroot;
}

int configMessage::getDistance() const
{
	return mydistance;
}

int configMessage::getID() const
{
	return myid;
}

int configMessage::getPort() const
{
	return myport;
}

void configMessage::changePort(int p)
{
	myport = p;
}

bool configMessage::operator<(const configMessage &b) const
{
	//actually implements greater than since priorityQueue is a max priority queue
	if (b.myroot > myroot)
		return false;
	else if (b.myroot < myroot)
		return true;
	else if (b.mydistance > mydistance)
		return false;
	else if (b.mydistance < mydistance)
		return true;
	else if (b.myid > myid)
		return false;
	else if (b.myid < myid)
		return true;
	else if (b.myport > myport)
		return false;
	else if (b.myport < myport)
		return true;
	return true;			//randomly assing in case of equal configs
}

bool configMessage::operator==(configMessage &a)
{
	return myroot == a.myroot && mydistance == a.mydistance && myid == a.myid && myport == a.myport;
}

std::ostream& operator<<(std::ostream &os, const configMessage &msg)
{
	return os <<"(B"<< msg.getRoot() + 1 <<", "<< msg.getDistance()-1 <<", B"<< msg.getID() + 1<<")";
}

bridge::bridge(int assignedID)
{
	myPorts.clear();
	packetRecQueue = std::queue<int>();
	psender = std::queue<int>();
	preceiver = std::queue<int>();
	listOfReceivedConfigs = std::priority_queue<configMessage>();
	bridgeId = assignedID;
	bestReceivingPort = 1000;
	myStatus = configMessage(bridgeId, 0, -1, -1);
	traceOfNull = true;
	return;
}

void bridge::improve(configMessage rec)
{
	bool rootChange = false;
	if (myStatus < rec) //see the opposite definition of less than
	{
		//		std::cout<<"updates "<< bridgeId<< " " << rec << " " << myStatus<<"\n";
		rootChange = rec.getRoot() < myStatus.getRoot();
		myStatus = rec;
		bestReceivingPort = rec.getPort();
	}
	//	std::cout<<rootChange<<"hello\n"<<"\n";
	if (rootChange)
	{
		for (auto &i : myPorts)
		{
			i.second = true;//i.first != getRoot();//depends doesn't matters as such
		}
	}
	else if (rec.getRoot() > myStatus.getRoot());
	else if (rec.getDistance() -1 < myStatus.getDistance())
	{
		//		std::cout<<rec.getPort()<<"thisone"<<"\n";
		myPorts[rec.getPort()] = false;
	}
	else if (rec.getDistance() -1 == myStatus.getDistance() and rec.getID() < bridgeId)
	{
		//		std::cout<<rec.getPort()<<"thisone"<<"\n";
		myPorts[rec.getPort()] = false;
	}
	//	std::cout<<"for dps" << bridgeId << " " << numberOfDps()<<"\n";
	return;
}

configMessage bridge::makeSendingConfig()
{
	int x = 0;
	if (isRoot() || bestReceivingPort == 1100)
		x = 256;
	else if (bestReceivingPort == 1000)
		x = -1;
	else x = bestReceivingPort;
	int rr = myStatus.getRoot();
	int dd = myStatus.getDistance();
	//	std::cout<<"port "<<rr << " " << dd + 1 << " " << bridgeId<< " " <<x<<"\n";
	return configMessage(rr,  dd + 1, bridgeId, x);
}

void bridge::registerPort(int t)
{
	myPorts[t] = true;
	return;
}

void bridge::processReceivedList(bool trace, int time)
{
	bestReceivingPort = 1100;
	if (listOfReceivedConfigs.empty())
		bestReceivingPort -= 100;
	while(!listOfReceivedConfigs.empty())
	{
		configMessage mytemp(listOfReceivedConfigs.top());
		listOfReceivedConfigs.pop();
		if (trace)
		{
			std::cout<<time <<" B"<< bridgeId+1 <<" "<< 'r' <<" "<< mytemp<<"\n";
		}
		//		std::cout<<numberOfDps()<<" improev"<<"\n";
		improve(mytemp);
		//		std::cout<<myPorts.size()<<numberOfDps()<<"\n";
	}
	return;
}

configMessage bridge::getConfig(bool trace, int time)
{
	//	std::cout<<numberOfDps()<<"\n";
	if (trace and numberOfDps() and (isRoot() or bestReceivingPort != 1000))
		std::cout<<time <<" "<< "B" << bridgeId+1 <<" "<< 's' <<" (B"<< myStatus.getRoot() + 1 <<", "<< myStatus.getDistance() <<", B"<< bridgeId + 1<<")\n";
	return makeSendingConfig();
}

bool bridge::isRoot()
{
	return myStatus.getRoot() == bridgeId;
}

int bridge::getRoot()
{
	return myStatus.getRoot();
}

bool bridge::askForDP(int pport)
{
	if (myPorts.find(pport) == myPorts.end())
		return false;
	else return myPorts[pport];
}

int bridge::getRootPort()
{
	return myStatus.getPort();
}

int bridge::numberOfDps()
{
	int cnt = 0;
	for (auto i : myPorts)
	{
		cnt += i.second;
		//		std::cout<<" "<<cnt<<" "<<i.second << " ";
	}
	return cnt;
}

void bridge::receiveConfig(configMessage msg)
{
	return listOfReceivedConfigs.push(msg);
}

void bridge::printPorts()
{
	int rrr = 0;
	for (auto i : myPorts)
	{
		if (rrr != 0)std::cout<<" ";
		rrr++;
		char name = char(i.first);
		bool isDP = i.second;
		std::cout<<name<<"-"<<(name == myStatus.getPort() ? "RP" : (isDP ? "DP" : "NP"));
	}
	return;
}

void bridge::receivePacket(int pport, int sender, int receiver, int ssrc, int ddest)
{
//	std::cout<<"hellop"<<"\n";
	if (isNullPort(pport))
		return;
//	std::cout<<"helloj"<<"\n";
	packetRecQueue.push(pport);
	psender.push(sender);
	preceiver.push(receiver);
	src = ssrc;
	dest = ddest;
	return;
}

bool bridge::processPacket(bool trace, int time)
{
//	std::cout<<"hello"<<"\n";
	resetSendingPorts();
	if (packetRecQueue.empty()) return false;
//	std::cout<<"helloa"<<"\n";
	int cnt = 0;int szi = (int) packetRecQueue.size();
	for (int i = 0; i < szi ; i++)
	{
		int sender = psender.front(), receiver = preceiver.front(), pport = packetRecQueue.front();
		packetRecQueue.pop();psender.pop();preceiver.pop();
		if (!isNullPort(pport))
		{
			cnt++;
			packetRecQueue.push(pport);psender.push(sender);preceiver.push(receiver);
			//print statements
			if (trace) std::cout<< time << " B"<<bridgeId+1 << " r " << char(src)<< "->" << char(dest)<<"\n";
		}
		else if (trace && traceOfNull)
			std::cout<< time << " B"<<bridgeId+1 << " r " << char(src)<< "->" << char(dest)<<"\n";

	}
//	std::cout<<cnt<<"\n";
	if (cnt == 0)
		return false;
	if (cnt > 1)
	{
		std::cout<<"Something is fishy about the spanning tree: I am bridge "<<bridgeId+1<<" and I just received the packet from two valid ports of mine and I am not passing both."<<"\n";
		std::cout << " one from " << char(packetRecQueue.front()) << " and other from ";
		packetRecQueue.pop();
		std::cout<<char(packetRecQueue.front())<<"\n";
		return false;
	}
//	std::cout<<cnt<<"\n";
	int sender = psender.front(), receiver = preceiver.front(), pport = packetRecQueue.front();
	packetRecQueue.pop();psender.pop();preceiver.pop();
	forwardingTable[sender] = pport;
	if (forwardingTable.find(receiver) != forwardingTable.end())
	{
		int xport = forwardingTable[receiver];
		if (xport == pport) return false;
		else 
		{
			sendingPorts[xport] = true;
			//print statements
			//cout<< " snd to only xport"
			if (trace) std::cout<< time << " B"<<bridgeId+1 << " s " << char(src)<< "->" << char(dest)<<"\n";
			return true;
		}
	}
	cnt = 0;
	for (auto i : myPorts)
	{
		sendingPorts[i.first] = (!isNullPort(i.first)) and i.first != pport;
		cnt += sendingPorts[i.first];
		if (sendingPorts[i.first])
		{
			//print statements
			//cout<< " snd to only xport"
		}
	}
	if (cnt > 0 && trace) std::cout<< time << " B"<<bridgeId+1 << " s " << char(src)<< "->" << char(dest)<<"\n";
	return cnt > 0;
}
	
void bridge::resetSendingPorts()
{
	for(auto i : myPorts)
		sendingPorts[i.first] = false;
	return;
}

bool bridge::askToSend(int pport)
{
	return sendingPorts[pport];
}

bool bridge::isNullPort(int pport)
{
	return !(askForDP(pport) || getRootPort() == pport);
}

void bridge::printTable()
{
	for (auto i : forwardingTable)
	{
		std::cout<<"H"<<i.first<< " | " << char(i.second)<<"\n";
	}
	return;
}

