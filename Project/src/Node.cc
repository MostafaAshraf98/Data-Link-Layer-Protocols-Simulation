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

#include "Node.h"
#include "MyMessage_m.h"
#include <bits/stdc++.h>
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <string>
#include <fstream>
#include <bitset>
#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

Define_Module(Node);
string encodeData(string data, string key);
void ApplyByteStuffying(string &payload, string &newPayload,
                        vector<bitset<8>> &bitset_vec);
void CRCCallingFunction(string &newPayload, string &dataForCRC,
                        vector<bitset<8>> &bitset_vec, string &CRCCheckBits_string,
                        bits &CRCCheckBits_bits, string key);

//-------------------HAMMING CODE------------------
void ApplyModification(vector<bitset<12>> &bitset_vec)
{

    int changedLetter = rand() % (bitset_vec.size() - 1);
    int changedBit = rand() % 11;

    bitset_vec[changedLetter][changedBit] =
        ~bitset_vec[changedLetter][changedBit]; // toggle
    // 3ashan el debugging
    cout << "Applying modification in charachter number " << changedLetter << " in bit number " << changedBit << endl;
}
vector<bitset<8>> extractDataBits(vector<bitset<12>> bitset_vec)
{
    vector<bitset<8>> msg;
    for (int i = 0; i < bitset_vec.size(); i++)
    {
        bitset<12> HC = bitset_vec[i];
        bitset<8> data;
        data[0] = HC[2];
        data[1] = HC[4];
        data[2] = HC[5];
        data[3] = HC[6];
        data[4] = HC[8];
        data[5] = HC[9];
        data[6] = HC[10];
        data[7] = HC[11];
        msg.push_back(data);
    }
    return msg;
}
void Hamming(vector<bitset<12>> &bitset_vec)
{
    int errorPosition = -1;
    int charNumber = -1;
    for (int i = 0; i < bitset_vec.size(); i++) // applying hamming error to each charachter
    {

        bitset<12> HC = bitset_vec[i];
        // calculating the parity bits
        // p0-> 1,3-5-7-9-11
        HC[0] = HC[0] ^ HC[2] ^ HC[4] ^ HC[6] ^ HC[8] ^ HC[10];
        // p1-> 2,3- 6,7- 10,11
        HC[1] = HC[1] ^ HC[2] ^ HC[5] ^ HC[6] ^ HC[9] ^ HC[10];
        // p2-> 4,5,6,7- 12
        HC[3] = HC[3] ^ HC[4] ^ HC[5] ^ HC[6] ^ HC[11];
        // p3-> 8,9,10,11,12
        HC[7] = HC[7] ^ HC[8] ^ HC[9] ^ HC[10] ^ HC[11];

        int sum = 0;
        if (HC[0] == 1)
            sum += 1;
        if (HC[1] == 1)
            sum += 2;
        if (HC[3] == 1)
            sum += 4;
        if (HC[7] == 1)
            sum += 8;

        if (sum != 0)
        {
            errorPosition = sum - 1;
            charNumber = i;
            cout << "HAMMING:modification in charachter number " << charNumber << " in bit number " << errorPosition << endl;
            // toggeling the error bit
            HC[errorPosition] = HC[errorPosition] ^ 1;
        }
        bitset_vec[i] = HC;
    }
}

vector<bitset<12>> InitializeHamming(vector<bitset<8>> bitset_vec)
{
    vector<bitset<12>> hammingMsg;
    for (int i = 0; i < bitset_vec.size(); i++) // applying hamming error to each charachter
    {
        bitset<8> data = bitset_vec[i];
        bitset<12> HC;
        // filling the new bitset with the data
        HC[0] = 0;
        HC[1] = 0;
        HC[2] = data[0];
        HC[3] = 0;
        HC[4] = data[1];
        HC[5] = data[2];
        HC[6] = data[3];
        HC[7] = 0;
        HC[8] = data[4];
        HC[9] = data[5];
        HC[10] = data[6];
        HC[11] = data[7];

        // calculating the parity bits
        // p0-> 1,3-5-7-9-11
        HC[0] = HC[0] ^ HC[2] ^ HC[4] ^ HC[6] ^ HC[8] ^ HC[10];
        // p1-> 2,3- 6,7- 10,11
        HC[1] = HC[1] ^ HC[2] ^ HC[5] ^ HC[6] ^ HC[9] ^ HC[10];
        // p2-> 4,5,6,7- 12
        HC[3] = HC[3] ^ HC[4] ^ HC[5] ^ HC[6] ^ HC[11];
        // p3-> 8,9,10,11,12
        HC[7] = HC[7] ^ HC[8] ^ HC[9] ^ HC[10] ^ HC[11];

        hammingMsg.push_back(HC);
    }
    return hammingMsg;
}

//-------------------------------------------------------------------------

float roundoff(float value, unsigned char prec)
{
    float pow_10 = pow(10.0f, (float)prec);
    return round(value * pow_10) / pow_10;
}

int Node::incSeq(int &x)
{
    // x = x + 1 % (par("MAX_SEQ").intValue() + 1);
    return x = (x + 1) % (8); // it is in the range between 0 and 7 because we calculate remainder 7+1
}

int Node::incSeqWindow(int x)
{
    // x = x + 1 % (par("MAX_SEQ").intValue() + 1);
    return x = (x + 1) % (8); // it is in the range between 0 and 7 because we calculate remainder 7+1
}

// window is between the seq number a and c.
// a is considered earlier than c
// window is circular
// checks if b is within the window
static bool between(int a, int b, int c)
{
    // return ((a <= b) && (b <= c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a));
    return ((a <= b) && (b <= c));
}

void Node::initialize()
{
    remove("../simulations/pair01.txt");
    remove("../simulations/pair23.txt");
    remove("../simulations/pair45.txt");
    //-----------OPENING THE CORRESPONDING OUTPUT FILE-----------------
    int currentNode = getIndex();
    int peerNode;
    string outputFile = "pair";

    if (currentNode % 2 == 0)
    {
        peerNode = currentNode + 1;
        outputFile += to_string(currentNode) + to_string(peerNode);
    }
    else
    {
        peerNode = currentNode - 1;
        outputFile += to_string(peerNode) + to_string(currentNode);
    }

    outfile.open("../simulations/" + outputFile + ".txt", std::ios::app);
    if (!outfile.is_open())
    {
        cout << "Unable to open the output file" << endl;
        return;
    }
    for (int i = 0; i < 4; i++)
    {
        inWindowBuffer.push_back(i);
        receivedInWindowBuffer.push_back(false);
    }
}

void split_str(std::string const &str, const char delim,
               std::vector<std::string> &out)
{
    // create a stream from the string
    std::stringstream s(str);

    std::string s2;
    while (std::getline(s, s2, delim))
    {
        out.push_back(s2); // store the string in s2
    }
}

string XORAndShift(string first, string second)
{

    string final = ""; // the string where the XORed and shifted results will be, its size will be size of original strings-1

    for (int i = 1; i < second.length(); i++) // we start XORing from the second element in the strings, hence the shift
    {
        if (first[i] == second[i])
            final += "0";
        else
            final += "1";
    }
    return final;
}

string CalculatingCRC(string data, string generator)
{

    int generatorLen = generator.length(); // number of bits that we will be working with at a time
    // int dataLen = message.length();// data length

    // shifting the message by (generator length -1) =>this is the number of bits of the CRC remainder
    string message = (data + string(generatorLen - 1, '0'));
    int dataLen = message.length();                       // data length
    string workingBits = message.substr(0, generatorLen); // these are the bits that will be xored with the generator at a time

    while (generatorLen < dataLen)
    {
        if (workingBits[0] == '1') // if the first bit is 1 then we will xor the data with the generator and shift
        {
            // after the xor and shift is done we pull down another bit drom the original data
            workingBits = XORAndShift(generator, workingBits) + message[generatorLen];
        }
        else // if the first bit is zero the algorithm itself xors data with 0 and shifts-> this is equivilent to shifting only
        {
            // after the xor and shift is done we pull down another bit drom the original data
            workingBits = XORAndShift(string(generatorLen, '0'), workingBits) + message[generatorLen];
        }

        generatorLen += 1;
    }

    // to not have a string subscript out of range the last bits are done with the same technique but out of the loop
    if (workingBits[0] == '1')
        workingBits = XORAndShift(generator, workingBits);
    else
        workingBits = XORAndShift(string(generatorLen, '0'), workingBits);

    return workingBits; // the last bits after all the Xoring is the modulo2 division remainder
}

void ApplyByteStuffying(string &payload, string &newPayload,
                        vector<bitset<8>> &bitset_vec)
{
    for (int i = 0; i < payload.size(); i++)
    {
        if (payload[i] == '/' || payload[i] == '$')
            newPayload += '/';
        newPayload += payload[i];
    }
    newPayload += '$';

    // convert the payload chars (letters) to bitsets used in the modification error type later
    for (int i = 0; i < newPayload.size(); i++)
    {
        bitset<8> currentChar(newPayload[i]);
        bitset_vec.push_back(currentChar);
    }
}

void CRCCallingFunction(string &newPayload, string &dataForCRC,
                        vector<bitset<8>> &bitset_vec, string &CRCCheckBits_string,
                        bits &CRCCheckBits_bits, string key)
{
    for (int i = 0; i < newPayload.size(); i++)
    {
        dataForCRC += bitset_vec[i].to_string();
    }
    CRCCheckBits_string = CalculatingCRC(dataForCRC, key);
    bits temp(CRCCheckBits_string);
    CRCCheckBits_bits = temp;
}

void Node::ApplyModification(vector<bitset<8>> &bitset_vec)
{
    int changedLetter = rand() % (bitset_vec.size() - 1);
    int changedBit = rand() % 7;

    bitset_vec[changedLetter][changedBit] =
        ~bitset_vec[changedLetter][changedBit]; // toggle
}

void Node::SendOriginalMessage(MyMessage_Base *myMsg)
{

    float header = simTime().dbl();
    myMsg->setHeaderSendTime(header);
    send(myMsg, "out");
    // Timeout
    SendLostMessage(myMsg);
}

void Node::SendDelayedMessage(MyMessage_Base *myMsg)
{
    float header = simTime().dbl();
    float delayErr = par("delay").doubleValue();
    myMsg->setHeaderSendTime(header);
    sendDelayed(myMsg, delayErr, "out"); // receiver is sending

    // Timeout
    SendLostMessage(myMsg);
}

void Node::SendDuplicatedMessage(MyMessage_Base *myMsg)
{
    float delayDup = 0.01;
    SendOriginalMessage(myMsg);
    MyMessage_Base *temp = myMsg->dup();
    auto totalTime = simTime().dbl() + delayDup;
    float header = totalTime;
    temp->setHeaderSendTime(header);
    sendDelayed(temp, delayDup, "out"); // sending the duplicate message
    MyMessage_Base *temp2 = temp->dup();
    totalTransmissions++;
    scheduleAt(totalTime - 0.001, temp2); // for logging the duplicate message

    // Timeout
    SendLostMessage(myMsg);
}

void Node::SendDuplicatedAndDelayedMessage(MyMessage_Base *myMsg)
{
    float delayDup = 0.01;
    float delayErr = par("delay").doubleValue();
    MyMessage_Base *temp = myMsg->dup();
    float header = simTime().dbl();
    temp->setHeaderSendTime(header);
    sendDelayed(temp, delayErr, "out"); // for sending delayed

    MyMessage_Base *temp2 = temp->dup();
    float totalTime = simTime().dbl() + delayDup;
    header = totalTime;
    temp2->setHeaderSendTime(header);
    totalTransmissions++;
    sendDelayed(temp2, delayDup + delayErr, "out"); // for sending duplicate
    MyMessage_Base *temp3 = temp->dup();
    scheduleAt(totalTime - 0.001, temp3); // for logging duplicate

    // Timeout
    SendLostMessage(myMsg);
}

void Node::SendLostMessage(MyMessage_Base *myMsg)
{
    float TimeOutInterval = par("timeOutInterval").doubleValue();
    MyMessage_Base *timoutMessage = myMsg->dup();
    timoutMessage->setM_Type(5);
    timoutMessage->setHeaderSendTime(currentFrameIndex); // this is for identification when receiving the self sent message to retrieve the whether the timer is still set or not
    allMessagesTimers[currentFrameIndex] = true;

    scheduleAt(simTime() + TimeOutInterval, timoutMessage);
}

void Node::SendDuplicatedAndLostMessage(MyMessage_Base *myMsg)
{
    // Timeout
    SendLostMessage(myMsg);

    float delayDup = 0.01;
    auto totalTime = simTime().dbl() + delayDup;
    myMsg->setM_Type(2);
    float header = totalTime;
    myMsg->setHeaderSendTime(header);
    totalTransmissions++;
    scheduleAt(totalTime, myMsg); // for the duplication log file output
}

void Node::PrintInWindowBuffer()
{
    cout << "The elements in the InWindowBuffer are: [";
    for (int i = 0; i < 4; i++)
        cout
            << inWindowBuffer[i] << ", ";
    cout << "]" << endl;
}
// bool Node::NodeFinished()
//{
//     return (getIndex() % 2 == 0 && evenNodeFinished) || (getIndex() % 2 == 1 && oddNodeFinished);
// }

void Node::sendMessage(int msgNum)

{
    //----------------------VARIABLES------------------------------

    // Setting the piggyBacking
    int piggybackingId;
    int piggybackingAck;
    if (ThereIsAMissingFrame()) // if there is a missing message
    {
        // ACK Or NACK? law ACK fa da ma3nah dont slide law Nack da ma3nah eni ba2olo yeb3ato tani
        if (isFirstNack) // it is the first frame after the missing one
        {
            piggybackingAck = 1; // send nack
            isFirstNack = false;
        }
        else
            piggybackingAck = 0; // send ack

        piggybackingId = inWindowBuffer[0];
    }
    else
    {
        piggybackingAck = 0;
        piggybackingId = nextRequiredFrame;
    }

    // if this node finished sending all its data
    if (currentNodeFinished) // Reply with the Ack or Nack only
    {
        MyMessage_Base *onlyAckMsg = new MyMessage_Base();
        onlyAckMsg->setHeaderSendTime(simTime().dbl());
        onlyAckMsg->setM_Type(0);
        onlyAckMsg->setPiggybackingAck(piggybackingAck);
        onlyAckMsg->setPiggybackingId(piggybackingId);
        outfile << "- node" << getIndex() << " sending Ack only message at time: " << roundoff(simTime().dbl(), 2) << " with piggybacking ";
        if (onlyAckMsg->getPiggybackingAck() == 0)
            outfile << "Ack number ";
        else
            outfile << "Nack number";
        outfile << onlyAckMsg->getPiggybackingId() << endl;
        send(onlyAckMsg, "out");
        return;
    }

    int seqNum = allMessagesQueue[msgNum].second->getHeaderSeqNum();
    int NodeId = getIndex();
    string errorType = allMessagesQueue[msgNum].first;

    bool isModified = false;
    if (errorType[0] == '1')
        isModified == true;
    errorType.erase(0, 1); // removed the first five chars representing the error type
    int errInt = stoi(errorType);

    MyMessage_Base *myMsg = allMessagesQueue[msgNum].second;
    myMsg->setPiggybackingAck(piggybackingAck);
    myMsg->setPiggybackingId(piggybackingId);
    string finalMessage = myMsg->getM_Payload();
    float delayDup = 0.01;
    float delayError = par("delay").doubleValue();
    totalTransmissions++;

    switch (errInt)
    {
    case 0: // original message
        // cout << "Send original message with payload" << myMsg->getM_Payload()
        //      << endl;
        outfile << "- node" << getIndex() << " sends message with id=" << seqNum
                << " and content= " << finalMessage << " at "
                << roundoff(simTime().dbl(), 2);
        if (isModified)
            outfile << " with modification";
        outfile << ", and piggybacking ";
        if (myMsg->getPiggybackingAck() == 0)
            outfile << "Ack number ";
        else
            outfile << "Nack number";
        outfile << myMsg->getPiggybackingId() << endl;

        SendOriginalMessage(myMsg);
        break;
    case 1: // Delayed
        // cout << "Sending Delayed Message with payload" << myMsg->getM_Payload()
        //      << endl;
        outfile << "- node" << NodeId << " sends message with id=" << seqNum
                << " and content= " << finalMessage << " at "
                << roundoff(simTime().dbl(), 2);
        if (isModified)
            outfile << " with modification";

        outfile << ", and piggybacking ";
        if (myMsg->getPiggybackingAck() == 0)
            outfile << "Ack number ";
        else
            outfile << "Nack number";
        outfile << myMsg->getPiggybackingId() << endl;

        SendDelayedMessage(myMsg);
        break;
    case 10: // Duplication
        // cout << "Sending Duplicated Message with payload"
        //      << myMsg->getM_Payload() << endl;
        outfile << "- node" << NodeId << " sends message with id=" << seqNum
                << " and content= " << finalMessage << " at "
                << roundoff(simTime().dbl(), 2);
        if (isModified)
            outfile << " with modification";

        outfile << ", and piggybacking ";
        if (myMsg->getPiggybackingAck() == 0)
            outfile << "Ack number ";
        else
            outfile << "Nack number";
        outfile << myMsg->getPiggybackingId() << endl;
        SendDuplicatedMessage(myMsg);
        break;
    case 11: // duplication and Delay
        // cout << "Sending duplicated and delayed message with payload"
        //      << myMsg->getM_Payload() << endl;
        outfile << "- node" << NodeId << " sends message with id=" << seqNum
                << " and content= " << finalMessage << " at "
                << roundoff(simTime().dbl(), 2);
        if (isModified)
            outfile << " with modification";

        outfile << ", and piggybacking ";
        if (myMsg->getPiggybackingAck() == 0)
            outfile << "Ack number ";
        else
            outfile << "Nack number";
        outfile << myMsg->getPiggybackingId() << endl;
        SendDuplicatedAndDelayedMessage(myMsg);
        break;
    case 100: // Lost
        // cout << "Sending lost message with payload" << myMsg->getM_Payload()
        //      << endl;
        outfile << "- node" << NodeId << " sends message with id=" << seqNum
                << " and content= " << finalMessage << " at "
                << roundoff(simTime().dbl(), 2) << " with loss";
        if (isModified)
            outfile << " and with modification";

        outfile << ", and piggybacking ";
        if (myMsg->getPiggybackingAck() == 0)
            outfile << "Ack number ";
        else
            outfile << "Nack number";
        outfile << myMsg->getPiggybackingId() << endl;

        SendLostMessage(myMsg);
        break;
    case 101: // lost and delayed --> Lost
        // cout << "Sending lost and delayed message with payload"
        //      << myMsg->getM_Payload() << endl;
        outfile << "- node" << NodeId << " sends message with id=" << seqNum
                << " and content= " << finalMessage << " at "
                << roundoff(simTime().dbl(), 2) << " with loss";
        if (isModified)
            outfile << " and with modification";

        outfile << ", and piggybacking ";
        if (myMsg->getPiggybackingAck() == 0)
            outfile << "Ack number ";
        else
            outfile << "Nack number";
        outfile << myMsg->getPiggybackingId() << endl;
        SendLostMessage(myMsg);
        break;
    case 110: // duplication then lost
        // cout << "Sending lost and duplicated message with payload"
        //      << myMsg->getM_Payload() << endl;
        outfile << "- node" << NodeId << " sends message with id=" << seqNum
                << " and content= " << finalMessage << " at "
                << roundoff(simTime().dbl(), 2) << " with loss";
        if (isModified)
            outfile << " and with modification";

        outfile << ", and piggybacking ";
        if (myMsg->getPiggybackingAck() == 0)
            outfile << "Ack number ";
        else
            outfile << "Nack number";
        outfile << myMsg->getPiggybackingId() << endl;
        SendDuplicatedAndLostMessage(myMsg);

        break;
    case 111: // duplication and delay and lost --> duplication and lost
        // cout << "Sending duplicated and lost and delayed message with payload"
        //      << myMsg->getM_Payload() << endl;
        outfile << "- node" << NodeId << " sends message with id=" << seqNum
                << " and content= " << finalMessage << " at "
                << roundoff(simTime().dbl(), 2) << " with loss";
        if (isModified)
            outfile << " and with modification";

        outfile << ", and piggybacking ";
        if (myMsg->getPiggybackingAck() == 0)
            outfile << "Ack number ";
        else
            outfile << "Nack number";
        outfile << myMsg->getPiggybackingId() << endl;
        SendDuplicatedAndLostMessage(myMsg);
        break;
    default:
        break;
    }
    if (currentFrameIndex == allMessagesQueue.size() - 1)
    {
        outfile << "- node" << getIndex() << " finished" << endl;
        currentNodeFinished = true;
        MyMessage_Base *FinishedMsg = new MyMessage_Base();
        FinishedMsg->setM_Type(30);
        FinishedMsg->setPiggybackingAck(totalTransmissions);
        FinishedMsg->setPiggybackingId(totalSuccessfulTransmissions);
        send(FinishedMsg, "out");
    }
}

void Node::GenerateAllMessagesQueue()
{
    string line; // Reads line by line from the input file
    int msgSeqNum = 0;
    while (getline(infile, line))
    {
        istringstream iss(line); // i string stream, used from streaming
        string errorType;        // For modification, Loss, Duplication, Delay
        iss >> errorType;

        int NodeId = getIndex();
        string payload = line.substr(5, line.size() - 5); // removed the first five chars representing the error type
        vector<bitset<8>> bitset_vec;                     // serves as a vector of  bitsets of payload characters
        string newPayload = "$";                          // the new payload starts with the flag the add the old payload with the byte stuffing
        string dataForCRC;                                // the message as binary string representation to calculate the CRC checksum
        string key = "1001";                              // generator function
        string CRCCheckBits_string;
        bits CRCCheckBits_bits;
        bool isModified = false;

        float delayError = par("delay").doubleValue();
        float delayDup = 0.01;

        //--------------------BYTE STUFFING-------------------------
        ApplyByteStuffying(payload, newPayload, bitset_vec);

        //------------------------CRC--------------------------
        CRCCallingFunction(newPayload, dataForCRC, bitset_vec,
                           CRCCheckBits_string, CRCCheckBits_bits, key);

        if (errorType[0] != '1') // if not modified
            totalSuccessfulTransmissions++;

        //---------------------------MODIFICATION ERROR---------------------------------

        if (errorType[0] == '1')
        { // there is a modification
            isModified = true;
            ApplyModification(bitset_vec);
        }

        //------------------FINAL STRING MESSAGE--------------
        string finalMessage; // the final string message of the modification

        for (int i = 0; i < bitset_vec.size(); i++)
        {
            finalMessage += (char)bitset_vec[i].to_ulong();
        }
        MyMessage_Base *myMsg = new MyMessage_Base();
        myMsg->setM_Payload(finalMessage.c_str());
        myMsg->setTrailer(CRCCheckBits_bits);
        myMsg->setM_Type(2); // type is Data
        myMsg->setHeaderSeqNum(msgSeqNum);
        //-------------------------REST OF ERRORS----------------------------------

        allMessagesQueue.push_back(make_pair(errorType, myMsg));
        allMessagesTimers.push_back(false);
        incSeq(msgSeqNum);
    }
}

int Node::getElementIndexInArray(int val)
{
    for (int i = 0; i < 4; i++)
    {
        if (inWindowBuffer[i] == val)
            return i;
    }
    return -1;
}

void Node::shiftInWindowBuffer()
{
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (receivedInWindowBuffer[i] == false)
            break;
        count++;
    }
    for (int i = 0; i < count; i++)
    {
        isFirstNack = true;
        inWindowBuffer.erase(inWindowBuffer.begin());
        receivedInWindowBuffer.erase(receivedInWindowBuffer.begin());
        inWindowBuffer.push_back(incSeqWindow(inWindowBuffer[inWindowBuffer.size() - 1]));
        receivedInWindowBuffer.push_back(false);
    }
}

bool Node::ThereIsAMissingFrame()
{
    for (int i = 1; i < 4; i++)
        if (receivedInWindowBuffer[i] == true)
            return true;

    return false;
}
bool Node::isInsideOutBuf(int piggyBackingId)
{
    for (int i = ptr1; i <= ptr2; i++)
    {
        if (allMessagesQueue[ptr1].second->getHeaderSeqNum() == piggyBackingId)
            return true;
    }
    return false;
}

void Node::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
    if (currentNodeFinished && peerNodeFinished)
        return;

    cGate *arrivalGate = msg->getArrivalGate();
    MyMessage_Base *receivedMessage = dynamic_cast<MyMessage_Base *>(msg);
    int currentNode = getIndex();
    cout << "-----------------------------Current node is " << getIndex()
         << "-------------------------" << endl;

    /*------------------RECEIVING SELF SENT MESSAGE-------------------*/
    if (msg->isSelfMessage())
    {
        cout << "S->Received self message" << endl;

        //---------TIMEOUT---------
        if (receivedMessage != NULL && receivedMessage->getM_Type() == 5) // this type is specialized for timeout self sent messages
        {
            cout << "received Timer" << endl;
            if (allMessagesTimers[receivedMessage->getHeaderSendTime()])
            {
                cout << "TimeOut" << endl;
                outfile << "- node" << getIndex() << " timeout for message id="
                        << receivedMessage->getHeaderSeqNum() << " at "
                        << roundoff(simTime().dbl(), 2) << endl;

                cout << "The last message header is: "
                     << receivedMessage->getHeaderSeqNum() << endl;
                cout << "The last message payload is: "
                     << receivedMessage->getM_Payload() << endl;

                outfile << "- node" << getIndex() << " sends message with id="
                        << receivedMessage->getHeaderSeqNum()
                        << " and content= " << receivedMessage->getM_Payload()
                        << " at " << roundoff(simTime().dbl(), 2)
                        << ", and piggybacking ";
                if (receivedMessage->getPiggybackingAck() == 0)
                    outfile << "Ack number ";
                else
                    outfile << "Nack number";
                outfile << receivedMessage->getPiggybackingId() << endl;

                totalTransmissions++;
                receivedMessage->setHeaderSendTime(simTime().dbl());
                receivedMessage->setM_Type(2);
                cout << "Sending Last Message Due to Timeout" << endl;
                send(receivedMessage, "out");
            }
            return;
        }
        /*-------------------------Selective Repeat Protocol / the message sent by the coordinator---------------------*/
        if (strcmp(msg->getName(), "Send Next") == 0)
        {
            cout << "selective repeat protocol self message" << endl;
            cout << "Frame window (" << ptr1 << ", " << currentFrameIndex << ", " << ptr2 << " )" << endl;
            if (between(ptr1, currentFrameIndex, ptr2) || currentNodeFinished) // only sends the next frame if it is within the window
            {
                cout << "the currentFrameIndex is inside the window" << endl;
                sendMessage(currentFrameIndex);
                currentFrameIndex++;
                scheduleAt(simTime() + 0.05, new cMessage("Send Next")); // to send the next frame after 0.01 second
            }
            return;
        }
        //-----------------Logging Duplicated Message---------------------
        cout << "Logging duplicated message" << endl;
        MyMessage_Base *receivedMessage = dynamic_cast<MyMessage_Base *>(msg);
        outfile << "- node" << currentNode << " sends message with id="
                << receivedMessage->getHeaderSeqNum() << " and content= "
                << receivedMessage->getM_Payload() << " at "
                << roundoff(simTime().dbl(), 2) << " with Duplication,"
                << ", and piggybacking ";
        if (receivedMessage->getPiggybackingAck() == 0)
            outfile << "Ack number ";
        else
            outfile << "Nack number";
        outfile << receivedMessage->getPiggybackingId() << endl;
        return;
    }
    //-----------------RECEIVING MESSAGE FROM THE COORDINATOR----------------------
    if (arrivalGate == gate("inCord"))
    {

        cout << "Message is received from coordinator" << endl;
        string inputFileName;
        string receivedMessage = msg->getName();
        vector<string> result;
        const char delim = '/';
        split_str(receivedMessage, delim, result);
        inputFileName = result[0];
        infile.open("../simulations/" + inputFileName, ifstream::in);
        if (!infile)
            cout << "input file was not opened" << endl;

        GenerateAllMessagesQueue();
        if (result.size() == 2)
        {
            startTime = stof(result[1]);
            // serves as a delay/sleep until the starting time arrives
            scheduleAt(simTime() + stof(result[1]), new cMessage("Send Next"));
        }
        return;
    }
    //------------------RECEIVING MESSAGE FROM PEER--------------------
    else
    {
        if (receivedMessage->getM_Type() == 30)
        {
            peerNodeFinished = true;
            totalTransmissions += receivedMessage->getPiggybackingAck();
            totalSuccessfulTransmissions += receivedMessage->getPiggybackingId();
        }
        int receivedSeqNum;
        if (receivedMessage->getM_Type() == 2) // the message also contains data
        {
            /*For debugging and console printing */
            cout << "Node " << getIndex()
                 << " Received message from peer with seqNum: "
                 << receivedMessage->getHeaderSeqNum()
                 << " and sent at time: "
                 << receivedMessage->getHeaderSendTime() << " and type: "
                 << receivedMessage->getM_Type() << " and payload: "
                 << receivedMessage->getM_Payload()
                 << endl;

            // The received message is just logged and not used anythere as it is passed to the network layer (we wont use the In_buffer)
            // timedOut = false;
            string payLoad = receivedMessage->getM_Payload();
            string dataForCRC;
            string key = "1001";
            vector<bitset<8>> bitset_vec;
            vector<string> headerVec;
            receivedSeqNum = receivedMessage->getHeaderSeqNum();
            float sendingTime = receivedMessage->getHeaderSendTime();

            // convert the payload chars (letters) to bitsets used in the modification error type later
            for (int i = 0; i < payLoad.size(); i++)
            {
                bitset<8> currentChar(payLoad[i]);
                bitset_vec.push_back(currentChar);
            }

            //-------------CRC--------------
            for (int i = 0; i < payLoad.size(); i++)
            {
                dataForCRC += bitset_vec[i].to_string();
            }
            string CRCCheckBits_string = CalculatingCRC(dataForCRC, key);

            bits temp(CRCCheckBits_string);
            bits CRCCheckBits_bits = temp;

            if (CRCCheckBits_bits == receivedMessage->getTrailer())
            { // No modification occured

                if (roundoff(simTime().dbl(), 2) >= roundoff(sendingTime + par("delay").doubleValue(),
                                                             2))
                { // No modification but  delay
                    outfile << "- node" << currentNode
                            << " received message with id=" << receivedSeqNum
                            << " and content= "
                            << receivedMessage->getM_Payload() << " at "
                            << roundoff(simTime().dbl(), 2) << " with delay";
                }
                else
                { // No delay, No modification
                    outfile << "- node" << currentNode
                            << " received message with id=" << receivedSeqNum
                            << " and content= " << receivedMessage->getM_Payload()
                            << " at " << roundoff(simTime().dbl(), 2);
                }
            }
            else
            { // A modification occured
                if (simTime() >= sendingTime + par("delay").doubleValue())
                { // modification and delay
                    outfile << "- node" << currentNode
                            << " received message with id=" << receivedSeqNum
                            << " and content= " << receivedMessage->getM_Payload()
                            << " at " << roundoff(simTime().dbl(), 2)
                            << " with modification and delay";
                }
                else
                { // modification and no delay
                    outfile << "- node" << currentNode
                            << " received message with id=" << receivedSeqNum
                            << " and content= " << receivedMessage->getM_Payload()
                            << " at " << roundoff(simTime().dbl(), 2)
                            << " with modification";
                }
            }
            outfile << ", and piggybacking ";
            if (receivedMessage->getPiggybackingAck() == 0)
                outfile << "Ack number ";
            else
                outfile << "Nack number";
            outfile << receivedMessage->getPiggybackingId() << endl;

            if (getElementIndexInArray(receivedSeqNum) == -1 || receivedInWindowBuffer[getElementIndexInArray(receivedSeqNum)] == true)
            { // This message is duplicated
                PrintInWindowBuffer();
                cout << "R->Duplication: The received sequence number is: "
                     << receivedSeqNum << " The nextRequiredFrame is: "
                     << nextRequiredFrame << endl;
                cout << "R->Receiver drops message with id= " << receivedSeqNum
                     << endl;
                outfile << "- node" << currentNode << " drops message with id="
                        << receivedSeqNum << endl;
                return;
            }

            receivedInWindowBuffer[getElementIndexInArray(receivedSeqNum)] = true;
            shiftInWindowBuffer();
            nextRequiredFrame = inWindowBuffer[0];
        }
        else // the message only contains piggybacking data
        {
            outfile << "- node" << currentNode
                    << " received message with no data at " << roundoff(simTime().dbl(), 2)
                    << ", and piggybacking ";
            if (receivedMessage->getPiggybackingAck() == 0)
                outfile << "Ack number ";
            else
                outfile << "Nack number";
            outfile << receivedMessage->getPiggybackingId() << endl;
        }
        //----------Based on the piggyBack Ack/Nack -->slide the window or resend the message--------
        int piggyBackingAck = receivedMessage->getPiggybackingAck();
        int piggyBackingId = receivedMessage->getPiggybackingId();
        cout << "The piggyBackingAck: " << piggyBackingAck << endl;
        cout << "The piggybackingId: " << piggyBackingId << endl;

        // if you receive ACK
        // slide the window
        if (piggyBackingAck == 0) // ACK
        {
            // Make sure that the window starts with the frame that has the exact seq number as the piggybackingId
            // if (isInsideOutBuf(piggyBackingAck))
            // {
            bool reInitiate = false;
            if (currentFrameIndex > ptr2)
                reInitiate = true;
            while (true)
            {
                // shift the window one step right
                if (ptr2 >= allMessagesQueue.size()) // if ptr2 reaches the end
                    ptr2 = allMessagesQueue.size() - 1;
                if (ptr1 >= allMessagesQueue.size()) //  all the data messages are sent
                {
                    ptr1 = allMessagesQueue.size() - 1;
                    break;
                }
                if (allMessagesQueue[ptr1].second->getHeaderSeqNum() == piggyBackingId) //  the window starts with the frame that has the exact seq number as the piggybackingId
                    break;
                cout << "Node " << getIndex() << " Closing timer of the frame with seq number " << allMessagesQueue[ptr1].second->getHeaderSeqNum() << endl;
                allMessagesTimers[ptr1] = false; // close the timer
                ptr1++;
                ptr2++;
            }
            if (reInitiate && !currentNodeFinished)
                scheduleAt(simTime() + 0.05, new cMessage("Send Next")); // to send the next frame after 0.01 second
            // }
        }
        else // if you receive NACK
        {
            cout << "Node " << getIndex() << " received Nack with lost frame number " << receivedMessage->getPiggybackingId() << endl;
            // resend the frame with this seq number and no sliding
            for (int i = ptr1; i < ptr2; i++) // loop over the current window frames and see which has this seq number and resend it
            {
                // resend all the messages before this nack id
                if (allMessagesQueue[i].second->getHeaderSeqNum() != piggyBackingId)
                    continue;
                MyMessage_Base *myMsg = allMessagesQueue[i].second->dup();
                myMsg->setHeaderSendTime(simTime().dbl());
                myMsg->setM_Type(2);

                if (ThereIsAMissingFrame()) // if there is a missing message
                {
                    // ACK Or NACK? law ACK fa da ma3nah dont slide law Nack da ma3nah eni ba2olo yeb3ato tani
                    if (isFirstNack) // it is the first frame after the missing one
                    {
                        myMsg->setPiggybackingAck(1); // send nack
                        isFirstNack = false;
                    }
                    else
                        myMsg->setPiggybackingAck(0); // send ack

                    myMsg->setPiggybackingId(inWindowBuffer[0]);
                }
                else
                {
                    myMsg->setPiggybackingAck(0);
                    myMsg->setPiggybackingId(nextRequiredFrame);
                }
                outfile << "- node" << getIndex() << " resending lost message with id=" << myMsg->getHeaderSeqNum()
                        << " and content= " << myMsg->getM_Payload() << " at "
                        << roundoff(simTime().dbl(), 2);
                outfile << ", and piggybacking ";
                if (myMsg->getPiggybackingAck() == 0)
                    outfile << "Ack number ";
                else
                    outfile << "Nack number";
                outfile << myMsg->getPiggybackingId() << endl;
                send(myMsg, "out");
                break;
            }
        }
        if (currentNodeFinished && peerNodeFinished)
        {
            cout << "###################BOTH NODES FINISHED#########################" << endl;
            //-----------------------------------------
            int piggybackingAck;
            int piggybackingId;
            if (ThereIsAMissingFrame()) // if there is a missing message
            {
                // ACK Or NACK? law ACK fa da ma3nah dont slide law Nack da ma3nah eni ba2olo yeb3ato tani
                if (isFirstNack) // it is the first frame after the missing one
                {
                    piggybackingAck = 1; // send nack
                    isFirstNack = false;
                }
                else
                    piggybackingAck = 0; // send ack

                piggybackingId = inWindowBuffer[0];
            }
            else
            {
                piggybackingAck = 0;
                piggybackingId = nextRequiredFrame;
            }

            // if this node finished sending all its data
            if (currentNodeFinished) // Reply with the Ack or Nack only
            {
                MyMessage_Base *onlyAckMsg = new MyMessage_Base();
                onlyAckMsg->setHeaderSendTime(simTime().dbl());
                onlyAckMsg->setM_Type(0);
                onlyAckMsg->setPiggybackingAck(piggybackingAck);
                onlyAckMsg->setPiggybackingId(piggybackingId);
                outfile << "- node" << getIndex() << " sending Ack only message with piggybacking ";
                if (onlyAckMsg->getPiggybackingAck() == 0)
                    outfile << "Ack number ";
                else
                    outfile << "Nack number";
                outfile << onlyAckMsg->getPiggybackingId() << endl;
                send(onlyAckMsg, "out");
            }
            //--------------------------------------------------------------------
            outfile << "----------------------" << endl;
            outfile << "- node" << getIndex() << " end of input file" << endl;
            outfile << "- total transmission time= "
                    << roundoff(simTime().dbl(), 2) - startTime << endl;
            outfile << "- total number of transmissions= " << totalTransmissions
                    << endl;
            outfile << "- the network throughput= "
                    << roundoff(
                           (totalSuccessfulTransmissions / (simTime().dbl() - startTime)), 2)
                    << endl;

            outfile.close();
            return;
        }

        if (startTime == -1) // the non starting node starts sending with the first received message from peer
        {
            startTime = 1;
            scheduleAt(simTime(), new cMessage("Send Next")); // to send the next frame after 0.01 second
        }
    }
}
