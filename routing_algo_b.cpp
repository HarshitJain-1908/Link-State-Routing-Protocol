#include "node_b.h"
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

int router_graph[100][100];
void printRT(vector<RoutingNode*> nd){
/*Print routing table entries*/
// for (int i = 0; i < nd.size(); i++) {
// 	  nd[i]->printEdges();
	// }
	for (int i = 0; i < nd.size(); i++) {
	  nd[i]->printTable();
	}
}

void constructGraph(vector<RoutingNode*> nd, vector<edge_info> graph){

    for (int i = 0; i < graph.size(); i++){
      edge_info e = graph[i];
      int u, v;
      for (int j = 0; j < nd.size(); j++){
        if (nd[j]->getName() == e.from) u = j;
        if (nd[j]->getName() == e.to) v = j;
      }
      router_graph[u][v] = e.cost; 
    }
    // for (int i = 0; i < nd.size(); i++){
    //   for (int j = 0; j < nd.size(); j++){
    //     cout<<router_graph[i][j]<<" ";
    //   }
    //   cout<<endl;
    // }
    // cout<<endl;
}

void routingAlgo(vector<RoutingNode*> nd, int noOfEdges){
 
  bool saturation=false;
 
  for (RoutingNode* node: nd){
      node->sendMsg(nd, noOfEdges);
  }
 
  /*Print routing table entries after routing algo converges */
  printf("Printing the routing tables after the convergence \n");
  printRT(nd);
}

int minDist(int dist[], bool visited[], int N)
{
 
    int min = INT_MAX, min_index;
 
    for (int v = 0; v < N; v++)
        if (visited[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;
 
    return min_index;
}
 

void RoutingNode::recvMsg(vector<RoutingNode*> nd, RoutingNode* n, vector<edge_info> edges, int noOfEdges) {
  
  for (edge_info e: edges) n->graph.push_back(e);

  if (n->graph.size() == noOfEdges) { //start running dijkstra's algorithm
    constructGraph(nd, n->getGraph());

    int src;
    for (int i = 0; i < nd.size(); i++){
      if (nd[i]->getName() == n->getName()) src = i;
    }
    int N = nd.size();
    int dist[N], parent[N];
    bool visited[N]; 
    for (int i = 0; i < N; i++)
        dist[i] = INT_MAX, visited[i] = false;

    dist[src] = 0;
    parent[src] = -1;

    for (int i = 0; i < N - 1; i++) {
        
      int u = minDist(dist, visited, N);
      visited[u] = true;
      for (int v = 0; v < N; v++){
          
        if (!visited[v] && router_graph[u][v]
            && dist[u] != INT_MAX
            && dist[u] + router_graph[u][v] < dist[v]){
              dist[v] = dist[u] + router_graph[u][v];
              parent[v] = u;
            }
      }
    }

    // cout<<"Shortest distance from "<<src<<" :"<<endl;
    // for (int i = 0; i < V; i++){
    //   cout<<dist[i]<<" ";
    // }
    // cout<<endl<<endl;
    // cout<<nd[src]->getName()<<" Parent's array:"<<endl;
    // for (int i = 0; i < V; i++){
    //   cout<<parent[i]<<" ";
    // }
    // cout<<endl<<endl;

    //creating routing table of this router
    string destip, srcip, nexthopip;
    int dest, cost;
    for (int i = 0; i < nd.size(); i++){
      if (nd[i] != this){
        for (int k = 0; k < nd[i]->interfaces.size(); k++){

          destip = nd[i]->interfaces[k].first.getip();
          dest = i;

          while (parent[dest] != src){
            dest = parent[dest];
          }
          for (int j = 0; j < interfaces.size(); j++){
            if (interfaces[j].second == nd[dest]){
              nexthopip = interfaces[j].first.getConnectedIp();
              srcip = interfaces[j].first.getip();
              cost = dist[i];
              break;
            }
          }
          this->addTblEntry(destip, nexthopip, srcip, cost);
        }
      }
    }
  }
}