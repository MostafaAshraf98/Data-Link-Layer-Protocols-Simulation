package project;

@license(LGPL);
//
// TODO documentation
//

import ned.IdealChannel;

network Network
{
    parameters:
        int n = default(6);
        @display("bgb=797,473");
    submodules:
        nodes[n]: Node ;

        coordinator: Coordinator {
            @display("p=615,217");
        }
    connections:
        for i=0..sizeof(nodes)-1 {
            nodes[i].inCord <-- IdealChannel <-- coordinator.outs++;
        }
        for i=0..sizeof(nodes)-1, if i%2==0 {
            nodes[i].out --> IdealChannel --> nodes[i+1].inNode;
            nodes[i+1].out --> IdealChannel --> nodes[i].inNode;
        }
}
