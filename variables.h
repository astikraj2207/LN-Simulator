#ifndef VARIABLES_H
#define VARIABLES_H

#include <bits/stdc++.h>
using namespace std;


// Rem to assign values
struct network_fee{
  long long int base_fee;
  long long int proportional_fee_const;
  long long int imbalance_fee_const;
  long long int time_fee_const;
};

struct input_averages{
  double average;
  double percentile_90th;
  double percentile_50th;
  double percentile_10th;
};

struct network_params{
  long n_nodes;
  input_averages degree;
  input_averages capacity;
  network_fee fees_lower_limit;
  network_fee fees_upper_limit;
  int num_of_txn;
  int txn_fee_upper_limit;
};

struct channel{
    int id;
    int node1_id;
    int node2_id;
    int edge1_id;
    int edge2_id;
    int capacity;
    unsigned int is_closed;
};

struct edge{
    int id;
    int channel_id;
    int from_node_id;
    int to_node_id;
    int counter_edge_id;
    long long int balance;
    unsigned int is_closed;
    int tot_flows;
};


struct node{
    int id;
    vector<edge*> open_edges; // Stores id of the edges.
    network_fee* node_fee; // Remaining.......
    long long int fee_collected;
};


struct graph_channel {
  int node1_id;
  int node2_id;
};


struct network {
  map<int,node*> nodes; // maps id to the pointer of node
  map<int,channel*> channels;
  map<int,edge*> edges;
};

struct transaction{
    int sender_id;
    int receiver_id;
    int amount;
    int fee;
    int status;
    int path_length;
    vector<pair<int, edge*>> txn_path;
    // Status:
    // 0 if ongoing
    // 1 if not complete
    // 2 if req is invalid as receiver doesn't exist
    // 3 if req cannot be full-filled due to channel capacity
    // 4 if ongoing
    // 5 if complete
};

#endif

