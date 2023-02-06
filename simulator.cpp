#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <dirent.h>
#include <sstream>

#include <gsl/gsl_rng.h>
#include <random>

#include "variables.h"
#include "network.h"
#include "transactions.h"


using namespace std;
void print_network_params(const network_params& net_params ){
  cout<<"No. of nodes: "<<net_params.n_nodes<<"\n";
  cout<<"No. of avg degree: "<<net_params.degree.average<<"\n";
  cout<<"No. of degree 90th percentile: "<<net_params.degree.percentile_90th<<"\n";
  cout<<"No. of degree 50th percentile: "<<net_params.degree.percentile_50th<<"\n";
  cout<<"No. of degree 10th percentile: "<<net_params.degree.percentile_10th<<"\n";
  cout<<"No. of avg capacity: "<<net_params.capacity.average<<"\n";
  cout<<"No. of capacity 90th percentile: "<<net_params.capacity.percentile_90th<<"\n";
  cout<<"No. of capacity 50th percentile: "<<net_params.capacity.percentile_50th<<"\n";
  cout<<"No. of capacity 10th percentile: "<<net_params.capacity.percentile_10th<<"\n";
}


/*
degree_average=7.2
degree_90th_percentile=15
degree_50th_percentile=2
degree_10th_percentile=1
*/


void read_input(network_params* net_params){

  net_params->n_nodes = 0;
  net_params->degree.average = 0;
  net_params->degree.percentile_90th = 0;
  net_params->degree.percentile_50th = 0;
  net_params->degree.percentile_10th = 0;
  net_params->capacity.average = 0;
  net_params->capacity.percentile_90th = 0;
  net_params->capacity.percentile_50th = 0;
  net_params->capacity.percentile_10th = 0;
  net_params->fees_lower_limit.base_fee = 0;
  net_params->fees_upper_limit.base_fee = 0;
  net_params->fees_lower_limit.proportional_fee_const = 0;
  net_params->fees_upper_limit.proportional_fee_const = 0;
  net_params->fees_lower_limit.imbalance_fee_const = 0;
  net_params->fees_upper_limit.imbalance_fee_const = 0;

  string line;
  ifstream input_file ("input.txt");
  if (input_file.is_open())
  {
    string key;
    while(getline(input_file, key, '=') )
    {
      string value;
      if(getline(input_file, value) ) 
        if(key == "n_nodes")
          net_params->n_nodes = stoi(value);
        else if(key == "degree_average")
          net_params->degree.average = stoi(value);
        else if(key == "degree_90th_percentile")
          net_params->degree.percentile_90th = stoi(value);
        else if(key == "degree_50th_percentile")
          net_params->degree.percentile_50th = stoi(value);
        else if(key == "degree_10th_percentile")
          net_params->degree.percentile_10th = stoi(value);
        else if(key == "capacity_average")
          net_params->capacity.average = stoi(value);
        else if(key == "capacity_90th_percentile")
          net_params->capacity.percentile_90th = stoi(value);
        else if(key == "capacity_50th_percentile")
          net_params->capacity.percentile_50th = stoi(value);
        else if(key == "capacity_10th_percentile")
          net_params->capacity.percentile_10th = stoi(value);
        else if(key == "base_fee_lower")
          net_params->fees_lower_limit.base_fee = stol(value);
        else if(key == "base_fee_upper")
          net_params->fees_upper_limit.base_fee = stol(value);
        else if(key == "proportional_fee_const_lower")
          net_params->fees_lower_limit.proportional_fee_const = stol(value);
        else if(key == "proportional_fee_const_upper")
          net_params->fees_upper_limit.proportional_fee_const = stol(value);
          else if(key == "imbalance_fee_const_lower")
          net_params->fees_lower_limit.imbalance_fee_const = stol(value);
        else if(key == "imbalance_fee_const_upper")
          net_params->fees_upper_limit.imbalance_fee_const = stol(value);
        else if(key == "num_of_txn")
          net_params->num_of_txn = stoi(value);
        else if(key == "txn_fee_upper_limit")
          net_params->txn_fee_upper_limit = stol(value);
        else if(key == "faulty_node_probability")
          net_params-> faulty_node_probability = stod(value);
    }
    
    input_file.close();

  cout<<"Avg degree: "<<net_params->degree.average<<" 90th percentile: "<<net_params->degree.percentile_90th<<"\n";
  }
  else cout << "Unable to open file"; 
  // print_network_params(*net_params);
}

void print_network_stats(network* net){
  cout<<"Total nodes: "<< net->nodes.size()<<"\n";
  // for(auto x: net->nodes){
  //   cout<<"ID: "<<x.first<<"  ";
  //   cout<<"Number of edges: "<< x.second.open_edges.size()<<"\n";
  // }
  cout<<"Total channels: "<< net->channels.size()<<"\n";
  for(auto x: net->channels){
    // x is iterator in map
    cout<<(x.second)->node1_id<<" "<<(x.second)->node2_id<<"\n";
    // cout<<(x.second)->node1_id<<" "<<(x.second)->node2_id<<" Amt:  "<<(x.second)->capacity<<"\n";
  }
}

int main(int argc, char *argv[]) {
  
  network_params net_params;
  
  network *new_network;
  long n_nodes, n_edges;

  read_input(&net_params);

  printf("NETWORK INITIALIZATION\n");
  new_network = initialize_network(net_params);

  print_network_stats(new_network);

  n_nodes = new_network->nodes.size();
  n_edges = new_network->edges.size();

  process_payments(new_network, net_params);

  return 0;
}
