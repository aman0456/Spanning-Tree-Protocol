
#define rep(i, a, b) for (int i = a; i < b; ++i)
#define repr(i,a,b) for (int i = a; i > b; --i)
#define null NULL
#define pb push_back
#define pf push_front
#define mp make_pair
#define fi first
#define se second
#define ALL(V) V.begin(), V.end()
#define sz(V) (int)V.size()
#define _ <<" "<<

#include <iostream>
#include "bridge.h"
#include <sstream>
#include <stdio.h>
#include <bitset>
#include <bitset>
#include <vector>
#include <iterator>
#include <cstring>
#include <string>
#include <algorithm>
#include <cassert>

using namespace std;

/* important variables:
 * time				: stores the current time
 * trace			: stores whether trace has to be shown or not
 * numBridges 		: stores the number of bridges present
 * portsNum 		: stores the number of ports in total (of every bridge)
 * rootsNum			: store the total number of roots (this decreases to from numBridges to 1 as simulation goes on)
 * dPortsNum		: stores the total number of designated ports present (decreses from portsNum to lanNum as simulation goes on)
 * lanNum			: stores the total number of LANs
 * presentLan		: stores whether this lan is present or not
 * switches			: stores the instances of bridges
 * connectedLans	: stores the connected LANs for every bridge
 * listLan			: stores the connected bridges for every LAN
 */

int mytime;
bool trace;
int numBridges, portsNum, rootsNum, dPortsNum, lanNum, packetNum;
int sender, receiver;
bitset<300> presentLan;
vector<bridge> switches;
vector<vector<char> > connectedLans;
vector<vector<int> > listLan;
vector<vector<int> > hostList;
map<int, char> lanOfHost;
string forInput;					//temporary string to take input
vector<configMessage> sendingMessages;
queue<pair<int, int> > packetQueue;

void takeInput()
{
	//takes input and initializes variables
	cin>>trace;
	cin>>numBridges;
	presentLan.reset();
	mytime = portsNum = dPortsNum = lanNum =  0;
	rootsNum = -1;
	connectedLans.resize(numBridges);
	listLan.resize(300);			// 300 (slightly larger than 256) to avoid off-by-one erros
	rep(i, 0, numBridges)
	{
		switches.pb(bridge(i));
		cin>>forInput;
		getline(cin, forInput);			
		char t;
		//string stream for taking inputs
		stringstream ss;
		ss<<forInput;
		while(ss>>t)
		{
			portsNum++;
			dPortsNum++;
			if (!presentLan[t])
			{
				lanNum++;
				presentLan[t] = true;
			}
			switches[i].registerPort(t);
			connectedLans[i].pb(t);
			listLan[t].pb(i);
		}
	}
	return;
}

bool treeConverged()
{
	//checks whether tree has converged or not
	//	return mytime == 100;
	//	cout<<";alfja;ljfl;dsj" << dPortsNum<<"\n";
	return (rootsNum == 0 and dPortsNum == lanNum); // or mytime == 100;
}

void showTree()
{
	//prints the output (not the trace)
	rep(i, 0, numBridges)
	{
		cout<<"B"<<i+1<<": ";
		switches[i].printPorts();
		cout<<"\n";
	}
	return;
}

void updateReceivedList(int ind, configMessage msg)
{
	int ports = msg.getPort();
	//	if (mytime == 5 && ports == 106 ) cout<<"Asdfaklg pvfh p rhvfqleaf\n\n\n\n";
	if (ports == -1) return;
	rep(i, 0, sz(connectedLans[ind]))
	{
		int u = connectedLans[ind][i];
		//	if (mytime == 5 && ports == 106 and ind == 11) cout<<"aaaAsdfaklg pvfh p rhvfqleaf\n\n\n\n";
		if (u != ports )//&& switches[ind].askForDP(u))
		{
			//	if (mytime == 5 && ports == 106 and ind == 11) cout<<"dfaklg pvfh p rhvfqleaf\n\n\n\n"<<u << " " << endl;
			rep(j, 0, sz(listLan[u])){
				int v = listLan[u][j];
				if (v == ind) continue;
				else 
				{
					msg.changePort(u);
					//	if (mytime == 5 && ports == 106 and ind == 11) cout<<"dfaklg pvfh p rhvfqleaf\n\n\n\n"<<u << " " << v<<endl;
					switches[v].receiveConfig(msg);
				}
			}
		}
	}
	return;
}

void simulateForNextSec()
{
	int tempForRoots = 0, tempForDps = 0;
	sendingMessages.clear();
	rep(i, 0, numBridges)
	{
		switches[i].processReceivedList(trace, mytime);
		sendingMessages.push_back(switches[i].getConfig(trace, mytime));
		tempForRoots += switches[i].getRoot();
		tempForDps += switches[i].numberOfDps();
	}
	rep(i, 0, numBridges)
		updateReceivedList(i, sendingMessages[i]);
	rootsNum = tempForRoots, dPortsNum = tempForDps, mytime++;
	return;
}

void makeTree()
{
	takeInput();

	while(!treeConverged())
	{
		simulateForNextSec();
	}
	showTree();
	return;
}

void takeSecondInput()
{
	hostList.clear();lanOfHost.clear();
	hostList.resize(256, vector<int>());
	rep(i, 0, lanNum)
	{
		getline(cin, forInput);
		char lanny,tempy;int hosty;
		stringstream ss(forInput);
		ss>>lanny;
		ss>>tempy;
		while(ss>>tempy && ss >> hosty)
		{
//			cout<<tempy<<" " << hosty<< " " << lanny<<"\n";
			hostList[lanny].push_back(hosty);
			lanOfHost[hosty] = lanny;
		}
	}
	cin>>packetNum;
	return;
}

void simulatePacket()
{
	while(!packetQueue.empty())
	{		
		mytime++;
		while(!packetQueue.empty())
		{
			pair<int, int> mytemp = packetQueue.front();
			packetQueue.pop();
			rep(i, 0, sz(listLan[mytemp.se]))
			{
				int u = listLan[mytemp.se][i];
				if (u != mytemp.fi)	switches[u].receivePacket(mytemp.se, sender, receiver, lanOfHost[sender], lanOfHost[receiver]);
			}
		}
		rep(i, 0, numBridges)
		{
			if (switches[i].processPacket(trace, mytime))
			{
				rep(j, 0, sz(connectedLans[i]))
				{
					int u = connectedLans[i][j];
					if (switches[i].askToSend(u)) packetQueue.push(mp(i, u));
				}
			}
		}
	}
}

void printTables()
{
	rep(i, 0, numBridges)
	{
		cout<<"B"<<i+1<<":\n";
		cout<<"HOST ID | FORWARDING PORT"<<"\n";
		switches[i].printTable();
	}
	return;
}

void exchangePackets()
{
	takeSecondInput();
	getline(cin, forInput);
	rep(i, 0, packetNum)
	{
		mytime = 100* i + 100;
		getline(cin, forInput);
		stringstream ss(forInput);
		char c;
		ss>>c>> sender >>c>> receiver ;
//		cout<<lanOfHost[sender]<<"\n";
		packetQueue.push(mp(-1, lanOfHost[sender]));
		simulatePacket();
		printTables();
		cout<<"\n";
	}
	return;
}

int main()
{
	ios_base::sync_with_stdio(false);
	cin.tie(null); cout.tie(null);
	makeTree();
	if (mytime > 100)
		cout<<"Spanning Tree construction took more than 100s"<<"\n";
	exchangePackets();
	return 0;
}
