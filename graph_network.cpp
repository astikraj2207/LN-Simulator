#include <bits/stdc++.h>
using namespace std;


void dfs_col(int node_id,vector<vector<int>>& adj, vector<int>& vis,int col,vector<pair<int,int>> all_nodes,network* new_network){
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
        adj[itr->from_node_id].push_back(itr->to_node_id);
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
