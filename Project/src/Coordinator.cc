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

#include "Coordinator.h"
#include<iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <fstream>

using namespace std;

Define_Module(Coordinator);

void Coordinator::initialize() {

    //--------------REMOVE ALL EXISTING OUTPUT FILES-----------------
    remove("../simulations/pair01.txt");
    remove("../simulations/pair23.txt");
    remove("../simulations/pair45.txt");

    ifstream infile;
    infile.open("../simulations/coordinator.txt",ifstream::in);
    if (!infile.is_open()) {
       cout << "Could not open file "<<endl;

    }
    string line;
    while (getline(infile, line)) {
        istringstream iss(line);
        int NodeId; // the first node
        int startingTime;
        string inputFileName; // the input file name
        string Starting;
        iss >> NodeId;
        iss >> inputFileName;
        if (iss >> Starting) // if this Node is the starting node
        {

            iss >> startingTime;
            string sentMessage = inputFileName + '/'
                    + to_string(startingTime);
            cMessage *msg = new cMessage(sentMessage.c_str());
            send(msg, "outs", NodeId);
            cout << "input file name is: " << inputFileName << " start time: " << startingTime << endl;

        } else {
            string sentMessage = inputFileName;
            cMessage *msg = new cMessage(sentMessage.c_str());
            send(msg, "outs", NodeId);
        }

    }
    infile.close();
}

void Coordinator::handleMessage(cMessage *msg) {
    // TODO - Generated method body
}
