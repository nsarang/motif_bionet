//============================================================================
// Name        : MotifDiscovery.h
// Author      : Nima Sarang
// Version     :
// Copyright   :
// Description :
//============================================================================


#ifndef __MOTIFDISCOVERY_H
#define __MOTIFDISCOVERY_H

#include "Graph.h"


template<class T1, class T2>
int intersectionSize(set<T1> list1, T2 list2);

void getAllSubgraphs_Isomorphic_to_BasicPatterns(Graph &G, vector<set<Graph>> &vecBPSf1);

void extract_maxDisjointSubgraphs_PerBasicPattern(Graph &G, vector<set<Graph>> &vecBPSf1, vector<set<Graph>> &vecBPS);

void extractMaxDisjointSubgraphsPerPattern(vector<set<Graph>> &NS, vector<set<Graph>> &CS);



#endif /* __MOTIFDISCOVERY_H */
