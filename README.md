# Spanning-Tree-Protocol
The protocol followed by a distributed system of network bridges in an extended LAN to agree upon a spanning tree of the extended LAN. Originally given by Radia Perlman.


## Code Explanation:

1. **Class configMessage**

* Objects of this class represents the configuration messages received and sent by the bridges

* It contains four variables which represent the following values when a bridge sends the message: the bridgeId of the bridge which it assumes is the root, its distance from the root, its bridgeId, the port on which it is sending (which is the LAN name itself)


2. **Class Bridge**

* Any object of this class simulates a  bridge in the given network topology.

* It contains a map named myPorts whose key is the name of ports and value is a boolean function which indicates whether the port is a designated port or not.

* It has a configMessage object named myStatus which represents the current status of the bridge. The four variables of this configMessage object represent the bridgeId of the bridge which it assumes is the root, its distance from it, the next bridge towards the path to the bridge, and the port corresponding to that path which is in fact the root port, respectively.

* It contain various queues (priority_queue for receiving configs) to queue up the receiving config messages during spanning tree formation and to queue up the packets during packet transmission.

* It contains various public functions to allow spanningTree.cpp to access the values of private variables and some private functions which are kind of some helper functions used to aid the major functions in their execution.



## Algorithm Sketch and Method Design

### Spanning Tree Formation

1. Initially everyone assumes that it is the root and hence generates messages and releases them on the desired ports. These messages are then queued up in the receiving queue of the receiving bridges.

2. Now we check whether the spanning tree has converged or not. This is done as follows: we check whether every node has received the same root AND number of total designated ports is equal to the number of lans in the topology. Note that this checking condition is just to stop the protocol after the spanning tree has converged and hence avoid sending extra messages after that and doing useless calculations. This checking condition is implemented in the boolean function treeConverged in the spanningTree.cpp and if it is required to generate message till 100s then it is very easy to modify : just make the treeConverged function return (mytime < 100). mytime is the variable which stores the current time of the system. 

3. If the tree has not converged yet then first the receiving queue of all the bridges is processed and if any message is the queue is found to be better than the current configuration of the bridge then the status of the bridge gets updated. Further, this bridge then sends the message on all of its active ports (except the one on which it has receive any best message : which is the root port itself) which in turn are stored in another queue for maintaining the sending messages. One point to note is that if it has neither received any message nor it assumes itself to be the root, then it is not going to send any packet. Note that the queue used is a priority queue which sort the packets according to order which was specified in the problem statement.

4. Once all bridges have completed processing their receiving queue and added their sending messages to the sending queue, the sending queue then sends all the messages to the required bridges which are queued up in the receiving queue of all the bridges and then continues from step2.

5. If the tree has converged, all the ports information is printed following the output format.

### Forwarding Table Filling

1. The lan corresponding to the sending host is looked up from a map which stores this value. Then a queue is maintained which stores the message which needs to be send at the next time unit. All the bridges corresponding to this lan are queued up in the queue.

2. Then the function receivePacket in the bridge class is called for those bridges who are receiving the packet. This function then fills up the forwarding table appropriately and decides whether to send the packet forward or not and if yes, then which ports to send. This information is added in the queue for the next iteration. Note that the bridges do listen on the null port but they don’t react to packet in such cases. They just drop it.

3. This goes on until the queue becomes empty which effectively means that every bridge which might can receive the packet has received it.

4. Finally the table of every bridge is printed in the correct format.
