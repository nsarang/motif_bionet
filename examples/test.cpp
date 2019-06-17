//
//  Copyright © 2017 Nima Sarang. All rights reserved.
//


#include <iostream>
#include "Graph.h"

using namespace std;


int main()
{
    Graph G, P, H;

    G.addEdge({"1", "2"});
    G.addEdge({"3", "2"});
    G.addEdge({"1", "3"});
    G.addEdge({"3", "4"});
    G.addEdge({"5", "1"});
    G.addEdge({"1", "6"});
    G.addEdge({"6", "7"});
    G.addEdge({"6", "8"});


    P.addEdge({"a", "g"});
    P.addEdge({"a", "f"});
    P.addEdge({"a", "b"});
    P.addEdge({"h", "b"});
    P.addEdge({"b", "e"});
    P.addEdge({"e", "d"});
    P.addEdge({"c", "b"});
    P.addEdge({"c", "e"});


    H.addEdge({"1", "2"});
    H.addEdge({"3", "2"});
    H.addEdge({"1", "3"});
    H.addEdge({"3", "4"});


    cerr << "G & P are isomorphic:  " << boolalpha << G.isIsomorphic(P) << "\n";
    cerr << "G & H have intersection:  " << boolalpha << G.doesIntersect(H) << "\n";
    cerr << "G & H have explicit partly intersection:  " << boolalpha << G.doesIntersect(H, true) << "\n\n";


    Graph A, B;

    A.addEdge({"1", "2"});
    A.addEdge({"1", "3"});
    A.addEdge({"1", "4"});
    A.addEdge({"5", "4"});
    A.addEdge({"5", "6"});


    B.addEdge({"1", "2"});
    B.addEdge({"1", "3"});
    B.addEdge({"1", "4"});
    B.addEdge({"5", "4"});
    B.addEdge({"3", "6"});

    cerr << "A & B are isomorphic:  " << boolalpha << A.isIsomorphic(B) << "\n";
    cerr << "A & B have intersection:  " << boolalpha << B.doesIntersect(B) << "\n";
    cerr << "A & B have explicit partly intersection:  " << boolalpha << A.doesIntersect(B, true) << "\n\n";


    A.join(B);
    cerr << A;
}
