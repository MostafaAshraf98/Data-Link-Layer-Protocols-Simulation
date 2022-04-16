//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef __PROJECT_NODE_H_
#define __PROJECT_NODE_H_

#include "MyMessage_m.h"
#include <omnetpp.h>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <set>

using namespace std;
using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{
private:
    ifstream infile;
    ofstream outfile;
    int startTime = -1;
    bool peerNodeFinished = false;
    bool currentNodeFinished = false;

    // dool eli lihom 3elaka beli and mestanih meno
    int nextRequiredFrame = 0; // = the seqNum of the last received frame+1 % maxSeq from my Peer
    vector<int> inWindowBuffer;
    vector<bool> receivedInWindowBuffer;
    bool isFirstNack = true;

    // dool eli lihom 3elaka b ana bab3at eih
    int ptr1 = 0;
    int currentFrameIndex = 0;
    int ptr2 = 3;
    vector<pair<string, MyMessage_Base *>> allMessagesQueue; // the int is for the error type as implemented in phase 1 and the second is the message itself with the payload and the modification if any exists
    vector<bool> allMessagesTimers;

    // dool eli lihom 3elaka bel final output stats
    int totalSuccessfulTransmissions = 0;
    int totalTransmissions = 0;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void sendMessage(int i);
    virtual void ApplyModification(vector<bitset<8>> &bitset_vec);
    virtual void SendOriginalMessage(MyMessage_Base *myMsg);
    virtual void SendDelayedMessage(MyMessage_Base *myMsg);
    virtual void SendDuplicatedMessage(MyMessage_Base *myMsg);
    virtual void SendDuplicatedAndDelayedMessage(MyMessage_Base *myMsg);
    virtual void SendLostMessage(MyMessage_Base *myMsg);
    virtual void SendDuplicatedAndLostMessage(MyMessage_Base *myMsg);
    virtual void GenerateAllMessagesQueue();
    virtual bool isInsideOutBuf(int piggyBackingId);
    virtual int incSeq(int &x);
    virtual void shiftInWindowBuffer();
    virtual int getElementIndexInArray(int val);
    virtual bool ThereIsAMissingFrame();
    virtual void PrintInWindowBuffer();
    virtual int incSeqWindow(int x);
    //    virtual bool NodeFinished();
};

#endif
