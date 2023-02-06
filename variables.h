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
  double faulty_node_probability;
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
    // Status:
    // 0 if not started
    // 1 if cancelled
    // 2 if complete
};

// structure to store the path chosen by a transaction.
// A complete path consists of vexctor of `path_var`
struct path_var{
  int txn_id; // Transaction id which this intermediate node belongs to.
  int node_id;
  long long int time_taken; // Represents the time taken to reach at this node.
  int fee_taken; // Represents the fee taken by this node.
  // For sender and receiver node, fee taken is 0.

  edge* outgoing_edge;

  long long int amount_passed; // Represents the amount passing through this outgoing edge.
};

#endif

