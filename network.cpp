#include <bits/stdc++.h>
#include "variables.h"
#include "network.h"
#include <gsl/gsl_math.h>
#include <gsl/gsl_rng.h>
#include <time.h>
#include <random>

using namespace std;

std::random_device rd;


// srand(time(0));

node* get_new_node(int id,  long long int base_fee, long long int proportional_fee_const, long long int imbalance_fee_const)
{
    node* new_node = new node();
    new_node->id = id;
    network_fee* fee = new network_fee();
    new_node->node_fee= fee;
    new_node->fee_collected = 0;
    new_node->node_fee->base_fee = base_fee;
    new_node->node_fee->proportional_fee_const = proportional_fee_const;
    new_node->node_fee->imbalance_fee_const = imbalance_fee_const;
    return new_node;
}

channel* get_new_channel(int id, int direction1, int direction2, int node1, int node2, int capacity)
{
    channel* new_channel;
    new_channel = new channel();
    new_channel->id = id;
    new_channel->edge1_id = direction1;
    new_channel->edge2_id = direction2;
    new_channel->node1_id = node1;
    new_channel->node2_id = node2;
    new_channel->capacity = capacity;
    new_channel->is_closed = 0;
    return new_channel;
}

edge* get_new_edge(int id, int channel_id, int counter_edge_id, int from_node_id, int to_node_id, int balance){
    edge* new_edge;
    new_edge = new edge();
    new_edge->id = id;
    new_edge->channel_id = channel_id;
    new_edge->from_node_id = from_node_id;
    new_edge->to_node_id = to_node_id;
    new_edge->counter_edge_id = counter_edge_id;
    new_edge->balance = balance;
    new_edge->is_closed = 0;
    new_edge->tot_flows = 0;
    return new_edge;
}

vector<int> get_random_elements(vector<int>& inp, int k){
    random_shuffle(inp.begin(),inp.end());
    vector<int> res;
    for(int i=0;i<inp.size() && i<k;i++)res.push_back(inp[i]);
    return res;
}

// Assigns value of capacity based on normal distribution continously to vector elements.
void generate_random_channel_capacity(const network_params& net_param, vector<long long int>& cap){
    int sz = cap.size()-1;
    double standard_dev=2.0;
    // Assign capacities to cap vector elements based on the values of top 90 percentile,
    // 50 percentile etc. These values will then be shuffled and assigned to capacity values of each edge.
    std::mt19937 gen1{rd()};
    std::uniform_real_distribution<double> d1(net_param.capacity.percentile_90th, 7500);
    // std::normal_distribution<> d1{net_param.capacity.percentile_90th,standard_dev};
    for(int i=1;i<=sz/10;i++){
        cap[i] = d1(gen1); 
    }
    // double average_capacity_50th_percentile = ((5.0 * net_param.capacity.percentile_50th)
    // - net_param.capacity.percentile_90th)/(4);
    // std::normal_distribution<> d2{average_capacity_50th_percentile,standard_dev};
    std::mt19937 gen2{rd()};
    std::uniform_real_distribution<double> d2(net_param.capacity.percentile_50th, net_param.capacity.percentile_90th);
    for(int i=sz/10+1;i<=sz/2;i++){
        cap[i] = d2(gen2);
    }
    // double average_capacity_10th_percentile = ((9.0 * net_param.capacity.percentile_10th)
    // - (5.0*net_param.capacity.percentile_50th))/(4);
    std::mt19937 gen3{rd()};
    // std::normal_distribution<> d3{average_capacity_10th_percentile,standard_dev};
    std::uniform_real_distribution<double> d3(net_param.capacity.percentile_10th, net_param.capacity.percentile_50th);
    for(int i=sz/2+1;10*i<=sz*9;i++){
        cap[i] = d3(gen3);
    }
    // double average_capacity_rem = (10.0*(net_param.capacity.average)
    // - (9.0*net_param.capacity.percentile_10th));
    std::mt19937 gen4{rd()};
    // std::normal_distribution<> d4{average_capacity_rem,standard_dev};
    std::uniform_real_distribution<double> d4(100, net_param.capacity.percentile_10th);
    for(int i=(sz)*(0.9)+1;i<=sz;i++){
        cap[i] = d4(gen4);
    }
    // Shuffle the input vector.
    // auto rng = default_random_engine {};
    random_shuffle(cap.begin(), cap.end());
}

void generate_random_channel(channel* channel_data, network* network, int capacity) {
    int edge1_balance, edge2_balance;
    channel* channel1 = new channel();
    // capacity = fabs(mean_channel_capacity + gsl_ran_ugaussian(random_generator)); 
    channel1 = get_new_channel(channel_data->id, channel_data->edge1_id, channel_data->edge2_id, channel_data->node1_id, channel_data->node2_id, capacity);
    edge* edge1 = new edge();
    edge* edge2= new edge();

    edge1_balance = capacity/2;
    edge2_balance = capacity - edge1_balance;
    //multiplied by 1000 to convert satoshi to millisatoshi
    // edge1_balance*=1000;
    // edge2_balance*=1000;


    edge1 = get_new_edge(channel_data->edge1_id, channel_data->id, channel_data->edge2_id, channel_data->node1_id, channel_data->node2_id, edge1_balance);
    edge2 = get_new_edge(channel_data->edge2_id, channel_data->id, channel_data->edge1_id, channel_data->node2_id, channel_data->node1_id, edge2_balance);

    // network->channels.push_back(channel1);
    // network->edges.push_back(edge1);
    // network->edges.push_back(edge2);
    network->channels[channel1->id] = channel1;
    network->edges[edge1->id] = edge1;
    network->edges[edge2->id] = edge2;

    auto it = network->nodes.find(channel_data->node1_id);
    if(it!=network->nodes.end()){
        (network->nodes[channel_data->node1_id])->open_edges.push_back((edge1));
    }else{
        cout<<"Couldn't find the node with id: "<<channel_data->node1_id<<"\n";
    }
    it = network->nodes.find(channel_data->node2_id);
    if(it!=network->nodes.end()){
        (network->nodes[channel_data->node2_id])->open_edges.push_back((edge2));
    }else{
        cout<<"Couldn't find the node with id: "<<channel_data->node2_id<<"\n";
    }
}


network* generate_random_network(network_params net_param){
    int node_id_counter=0, id, channel_id_counter=0, tot_nodes,
    tot_channels, node_to_connect_id, edge_id_counter=0;
    vector<double> probability_per_node(net_param.n_nodes+3,0.0);
    vector<int> channels_per_node(net_param.n_nodes+3,0);
    network* new_network;
    channel* new_channel = new channel();
    new_network = new network();

    std::random_device rd{};
    std::mt19937 gen{rd()};
    double standard_dev=1.0;
    // Assign degrees to each node based on the values of top 90 percentile,
    // 50 percentile etc.
    std::uniform_real_distribution<double> dist1(net_param.degree.percentile_90th, 20.0);
    // std::normal_distribution<> d1{net_param.degree.percentile_90th,standard_dev};
    for(int i=1;i<=net_param.n_nodes/10;i++){
        channels_per_node[i] = dist1(gen);
        // cout<<channels_per_node[i]<<"  ";
    }
    // std::normal_distribution<> d2{average_degree_50th_percentile,standard_dev};
    std::uniform_real_distribution<double> dist2(net_param.degree.percentile_50th, net_param.degree.percentile_90th);
    for(int i=net_param.n_nodes/10+1;i<=net_param.n_nodes/2;i++){
        channels_per_node[i] = dist2(gen);
    }
    // std::normal_distribution<> d3{average_degree_10th_percentile,standard_dev};
    std::uniform_real_distribution<double> dist3(net_param.degree.percentile_10th, net_param.degree.percentile_50th);
    for(int i=net_param.n_nodes/2+1;10*i<=net_param.n_nodes*9;i++){
        channels_per_node[i] = dist3(gen);
    }
    std::uniform_real_distribution<double> dist4(1.0, net_param.degree.percentile_10th);
    for(int i=(net_param.n_nodes)*(0.9)+1;i<=net_param.n_nodes;i++){
        channels_per_node[i] = dist4(gen);
    }

    long long int total_edges_in_network=0;
    for(int i=1;i<=net_param.n_nodes;i++){
        total_edges_in_network+=channels_per_node[i];
        // cout<<channels_per_node[i]<<" ";
        // if(i%10==0)cout<<"\n";
    }

    vector<long long int> channel_capacity_random_value(total_edges_in_network/2+100,net_param.capacity.average);
    generate_random_channel_capacity(net_param,channel_capacity_random_value);
    // for(auto a: channel_capacity_random_value){
    //     cout<<a<<"\n";
    // }
    int channel_cap_rand_value_ind=0;

    
    // Open channels such that the degree of each node is 
    // same as above assigned.
    vector<int> rem_channels_per_node = channels_per_node;

    default_random_engine generator1, generator2, generator3;
    uniform_int_distribution<int> distribution1(net_param.fees_lower_limit.base_fee,net_param.fees_upper_limit.base_fee);
    uniform_int_distribution<int> distribution2(net_param.fees_lower_limit.proportional_fee_const,net_param.fees_upper_limit.proportional_fee_const);
    uniform_int_distribution<int> distribution3(net_param.fees_lower_limit.imbalance_fee_const,net_param.fees_upper_limit.imbalance_fee_const);
    for(int i=1;i<=net_param.n_nodes;i++){
        int base_fee_val = distribution1(generator1);
        int proportional_fee_val = distribution2(generator2);
        int imbalance_fee_const = distribution3(generator3);
        node* node_ = get_new_node(i, base_fee_val, proportional_fee_val, imbalance_fee_const);
        new_network->nodes[i] = node_; // map node id to the node pointer.
    }
    // for each node
    for(int i=1;i<=net_param.n_nodes;i++){
        vector<int> available_nodes; // Stores node id of the node with which a channel can be created.
        node* node_one = new_network->nodes[i];
        for(int j=i+1;j<=net_param.n_nodes;j++){
            // if the node still requires some edge to reach its degree,push its id in the above vector
            if(rem_channels_per_node[j]>0){
                available_nodes.push_back(j);
            }
        }
        // the available_nodes vector contains the id, now choose the required number of nodes and connect the edge, reduce the number of remaining edges for those nodes
        // the random selection can be done in O(k) 
        
        vector<int> channel_connecting_nodes=get_random_elements(available_nodes,rem_channels_per_node[i]);

        // Reduce the number of remaining channels to be connected for those nodes with which ith node is forming a channel.
        for(const auto& nodev: channel_connecting_nodes){
            rem_channels_per_node[nodev]--;
        }
        rem_channels_per_node[i]=0; // The i'th node has been fully connected.
        // generate channel
        for(const auto& node_two: channel_connecting_nodes){
            new_channel->id = channel_id_counter;
            new_channel->edge1_id = edge_id_counter;
            new_channel->edge2_id = edge_id_counter+1;
            new_channel->node1_id = node_one->id;
            new_channel->node2_id = node_two;
            generate_random_channel(new_channel,new_network,channel_capacity_random_value[channel_cap_rand_value_ind]);
            channel_cap_rand_value_ind=(channel_cap_rand_value_ind+1)%(channel_capacity_random_value.size());
            channel_id_counter++;
            edge_id_counter += 2;     
        }
    }
    return new_network;
}

// It connects the vector of vector of nodes of connected components such that there are no multiple islands.
void connect_given_nodes(vector<vector<int>>& all_nodes,network *new_network, const network_params& net_param){
    cout<<"Number of connected components: "<<all_nodes.size()<<"\n";
    int channel_id_counter=new_network->channels.size();
    int edge_id_counter = new_network->edges.size();
    // for(int i=0;i<all_nodes.size();i++){
    //     cout<<"Number of nodes: "<<all_nodes[i].size()<<"\n";
    // }
    for(int i=1;i<all_nodes.size();i++){
        for(int j=0;j<min(int(all_nodes[i-1].size()), int(all_nodes[i].size()));j++){
            channel* new_channel = new channel();
            new_channel->id = channel_id_counter;
            new_channel->edge1_id = edge_id_counter;
            new_channel->edge2_id = edge_id_counter+1;
            new_channel->node1_id = all_nodes[i-1][j];
            new_channel->node2_id = all_nodes[i][j];
            generate_random_channel(new_channel,new_network,net_param.capacity.average);
            channel_id_counter++;
            edge_id_counter += 2; 
        }
    }
}

network* initialize_network(network_params net_param){
    network* new_network;

    new_network = generate_random_network(net_param);
    vector<vector<int>> nodes_to_connect = get_isolated_components(new_network);
    connect_given_nodes(nodes_to_connect, new_network, net_param);

    return new_network;

}



void dfs_col(int node_id,vector<vector<int>>& adj, vector<int>& vis,int col,vector<pair<int,int>>& all_nodes,network* new_network){
    vis[node_id]=col;
    auto& node_id_map = new_network->nodes;
    auto itr = node_id_map.find(node_id);
    if(itr!=node_id_map.end()){
        int sz = int((node_id_map[node_id])->open_edges.size());
        all_nodes.push_back({node_id, sz}); // node id and the number of edges this node has.
    }
    
    for(auto x:adj[node_id]){
        if(!vis[x])dfs_col(x,adj,vis,col,all_nodes, new_network);
    }
}

bool comp(const pair<int,int>& a, const pair<int,int>& b){
    if(a.second> b.second)return true;
    return false;
}
vector<vector<int>> get_isolated_components(network *new_network){
    // Insert the nodes and edges in adjacency matrix.
    vector<vector<int>> adj((new_network->nodes).size()+1);
    for(auto itr: new_network->edges){
        // itr is an iterator in map ( edge id mapping to pointer to the edge)
        adj[(itr.second)->from_node_id].push_back((itr.second)->to_node_id);
    }

    // Prints the adjacency matrix.
    for(int i=1;i<=new_network->nodes.size();i++){
        for(int j=0;j<adj[i].size();j++){
            cout<<adj[i][j]<<",";
        }
        cout<<"\n";
    }

    // The adjacency list is formed. Now find the connected components.
    vector<int> vis(new_network->nodes.size()+1,0);
    int col=1;
    int distinct_connected_components = 0;
    vector<vector<int>> nodes_to_connect; // It's a vector of vector. 
    // Each nodes_to_connect[i] represents a vector of nodes(id) from one particular connected 
    // component which needs to be connected with some nodes of other connected component.
    for(int i=1;i<=(new_network->nodes).size();i++){
        if(!vis[i]){
            vector<pair<int,int>> all_nodes; // Stores the node id and the number of edges this node has.
            dfs_col(i,adj,vis,col,all_nodes, new_network);
            sort(all_nodes.begin(),all_nodes.end(),comp);
            col++;
            distinct_connected_components++;
            vector<int> temp;
            for(int j=0;j<3 && j<all_nodes.size();j++){
                temp.push_back(all_nodes[j].first);
            }
            nodes_to_connect.push_back(temp);
        }
    }
    return nodes_to_connect;

}