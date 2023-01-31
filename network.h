#include <bits/stdc++.h>
#include "variables.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_cdf.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <dirent.h>
#include <random>

using namespace std;


node* get_new_node(int id, long long int base_fee, long long int proportional_fee_const, long long int imbalance_fee_const);

channel* get_new_channel(int id, int direction1, int direction2, int node1, int node2, int capacity);

edge* get_new_edge(int id, int channel_id, int counter_edge_id, int from_node_id, int to_node_id, int balance);

vector<vector<int>> get_isolated_components(network *new_network);

network* initialize_network(network_params net_params);
