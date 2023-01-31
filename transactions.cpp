#include <bits/stdc++.h>
#include <cstdlib>
#include "variables.h"
#include "transactions.h"
#define ll long long int

using namespace std;

void revert_payment(vector<pair<edge*,ll>> &edges_updated){
    cout<<"Txn reverted\n";
    for(auto& [cur_edge, amount]: edges_updated){
        cur_edge->balance+=amount;
    }
}

void update_incoming_path_edges(network* cur_network, vector<pair<edge*,ll>> &edges_updated){
    for(auto& [cur_edge, amount]: edges_updated){
        auto itr = cur_network->edges.find(cur_edge->counter_edge_id);
        if(itr!=cur_network->edges.end()){
            (itr->second)->balance-=amount;
        }
    }
}

void process_payments(network* cur_network, const network_params& net_param){
  vector<transaction> transactions_to_execute = get_random_transactions(net_param);
  vector<pair<edge*, long long int>> rem_update_balances; 
  // Stores the edges whose balances will be updated for the respective edges
  // stores pointer of edge and the amount to be added
  // The balances are reduced from which payment went, needs to be added for the counter edges.
  
  // Stores the path and the outgoing edge pointer for each transaction
  // txn_paths[i] represents the pair of ditance and a  vector pair whose each element is {nodes, outgoing edge pointer}
  vector<pair<vector<ll>, vector<pair<int, edge*>>>> txn_paths(int(transactions_to_execute.size()));

  int max_path_size=0;
  // Payment request
  for(int ind=0;ind<transactions_to_execute.size();ind++){
    auto& cur_txn = transactions_to_execute[ind];
    // cur_txn.sender_id=1;
    // cur_txn.receiver_id=2;
    if(payment_request(cur_txn, cur_network)){
      // This transaction might be completed.
      vector<ll> dis;
      vector<pair<int,edge*>> path;
      auto res = get_payment_path(cur_txn, cur_network, net_param);
      dis=res.first;
      path=res.second;
      // cur_txn.txn_path = path;
      txn_paths[ind] = res;
      cur_txn.path_length = path.size()-1;
      max_path_size=max(max_path_size, int(path.size()));
      cout<<"Sender: "<<cur_txn.sender_id<<" Receiver: "<<
      cur_txn.receiver_id<<" Amt: "<<cur_txn.amount<<"\n";
      cout<<"Fee taken "<<cur_txn.fee<<"\n";

      cout<<"Path taken, len :"<<path.size()<<"\n";
      for(auto x:path){
        cout<<x.first<<" --> ";
      }
      cout<<"\n";
    }else{
      // This transaction can't be executed.
    }
  }

  vector<int> transaction_fee(transactions_to_execute.size(),0);
  
  // Stores the pointer to edges in the path going from source to destination,
  // as the amount has been deducted from them. The second paramter is the amount transaferred through this.
  vector<vector<pair< edge*, ll>>> edges_updated(transactions_to_execute.size());


  // 2 if ongoing
  // 1 if completed
  // 0 if cancelled
  vector<int> transaction_status(transactions_to_execute.size(),2);

  //Now all the transaction steps are stored in a vector
  // We execute them one by one
  for(int step=1;step<max_path_size;step++){
    // txn_paths[step] -> pair{ distance vector, vector pair{path node, outgoing edge*}}
    for(int txn_ind=0;txn_ind< transactions_to_execute.size(); txn_ind++){
      if(transaction_status[txn_ind]!=2 || txn_paths[txn_ind].second.size()<= step)continue;
        // cout<<"Executing txns\n";
      auto itr  = cur_network->nodes.find(txn_paths[txn_ind].second[step].first);
      if(itr == cur_network->nodes.end()){
          cout<<"In `line 80 transactions.cpp`, nodes with id "<<txn_paths[txn_ind].second[step].first
          <<" not found in mapping of network, node id to node pointer\n.";
          transactions_to_execute[txn_ind].status=3;
          break;
      }else{
          edge* incoming_edge = txn_paths[txn_ind].second[step-1].second;
          node* path_node = itr->second;
          if(step != txn_paths[txn_ind].second.size()-1){
            int balance_diff = abs(incoming_edge->balance - cur_network->edges[incoming_edge->counter_edge_id]->balance);
              int cur_fee = calc_fee(path_node,transactions_to_execute[txn_ind].amount, balance_diff);
              path_node->fee_collected += cur_fee;
              transaction_fee[txn_ind]+=cur_fee;
          }
          if(path_node->id >= txn_paths[txn_ind].first.size()){
              cout<<"Node id exceeds distance vector size\n";
              break;
          }
          ll amt_though_this_edge = txn_paths[txn_ind].first[path_node->id];

          if(incoming_edge->balance < amt_though_this_edge){
            // Cancel transaction due to lack of balance
            cout<<"Cancelled due to lack of balance\n";
            transaction_status[txn_ind]=0;
            revert_payment(edges_updated[txn_ind]);
            edges_updated[txn_ind].clear();
            transactions_to_execute[txn_ind].status=3;
            break;
          }
          incoming_edge->balance -= amt_though_this_edge;
          edges_updated[txn_ind].push_back({incoming_edge, amt_though_this_edge});

          if(step == txn_paths[txn_ind].second.size()-1){
            // This is the destination,  transaction is complete.
            transaction_status[txn_ind]=1;
            transactions_to_execute[txn_ind].status=5;
          }
    }
        
    }
  }

  for(auto& updated_paths: edges_updated){
    update_incoming_path_edges(cur_network, updated_paths);
  }

  int txn_completed=0;
  long long int average_path_length=0;
  long long int average_fee=0;
  for(int ind=0;ind<transactions_to_execute.size();ind++){
    if(transaction_status[ind]==1){
        txn_completed++;
        average_path_length+= txn_paths[ind].second.size();
        // cout<<transaction_fee[ind]<<"\n";
        average_fee += transaction_fee[ind];
        transactions_to_execute[ind].fee=transaction_fee[ind];
    }
  }

  // Print transaction statistics
  if(txn_completed>0){
    average_path_length/= (txn_completed);
    average_fee/= (txn_completed);
    cout<<"Number of transactions completed: "<<txn_completed<<" out of "<<transactions_to_execute.size()<<"\n";
    cout<<"Average path length: "<<average_path_length<<"\n";
    cout<<"Average fee : "<<average_fee<<"\n";
  }else{
    cout<<"No transaction completed.\n";
  }

}



vector<transaction> get_random_transactions(const network_params& net_param){
    vector<transaction> res;
    
    srand(time(NULL));

    for(int i=0;i<net_param.num_of_txn;i++){
        transaction new_txn;
        new_txn.sender_id = rand()%(net_param.n_nodes) + 1;
        new_txn.receiver_id = rand()%(net_param.n_nodes) + 1;
        new_txn.amount = rand()%(net_param.txn_fee_upper_limit)+1;
        new_txn.fee=0;
        new_txn.status=1;

        // Corner case if sender id and receiver id are same.
        // Increase the sender id if so unless it is the maximum id limit.
        if(new_txn.sender_id == new_txn.receiver_id){
            if(new_txn.sender_id == net_param.n_nodes)new_txn.sender_id=1;
            else new_txn.sender_id+=1;
        }

        res.push_back(new_txn);
    }
    return res;
}

// Status of Txn:
    // 1 if not complete
    // 2 if req is invalid as receiver doesn't exist
    // 3 if req cannot be full-filled due to channel capacity
    // 4 if ongoing
    // 5 if complete
bool payment_request(transaction& req, network* cur_network){
    // Check if the receiver exists in the network.
    if(cur_network->nodes.find(req.receiver_id) == cur_network->nodes.end()){
        req.status = 2;
        return false;
    }
    // Check if the sender exists in the network.
    if(cur_network->nodes.find(req.sender_id) == cur_network->nodes.end()){
        req.status = 2;
        return false;
    }

    // Check if the capacities of the sender is greater than the amount.
    const auto& sender_node = cur_network->nodes[req.sender_id];
    const auto& sender_edges = sender_node->open_edges;
    int edge_exists = 0;
    for(const auto& edge: sender_edges){
        if(edge->balance >= req.amount){
            edge_exists=1;break;
        }
    }
    if(edge_exists)return true;
    else{
        req.status=3;
        return false;
    }

    return true;
}

//The i-th intermediate node will charge an imbalance fee
// of f im
// i = IP(δi,i+1 + 2α) − IP(δi,i+1), where δi,j + 2α is
// the imbalance degree after payment transfer.
// δi,j = |bi,j −bj,i|
long long int imbalance_penalty(int imbalance_fee_const, long long int balance_diff){
    return imbalance_fee_const*(balance_diff*balance_diff);
}
// Calculates fee taken by a node
double calc_fee(node* cur_node, long long int amount, int balance_diff){
    double fee=0.0;
    fee+= cur_node->node_fee->base_fee;
    fee+= ((cur_node->node_fee->proportional_fee_const)*(amount))/1000;
    // Add imbalance fee.
    fee+= (imbalance_penalty(cur_node->node_fee->imbalance_fee_const, balance_diff + 2*amount) 
    - imbalance_penalty(cur_node->node_fee->imbalance_fee_const, balance_diff))/1000;
    // Add time value fee.
    return fee;
}

// returns the distance of each node from receiver node and the path to be followed from sender to receiver as well as the pointer to incoming edge.
// {`a`, {`b`, `c`}}
// `a`: distance vector from receiver node, dis[i] denotes the distance of node `i` from the receiver node
// 'b': node id in the path followed from sender to receiver
// `c`: pointer to the incoming edge to this node, i.e. from which edge we came to this node
// Returns empty pair of vectors in case no path exist.
pair<vector<ll>,vector<pair<int,edge*>>> get_payment_path(transaction& req, network* cur_network,const network_params& net_param){
    int start_node = req.receiver_id; // Vs
    int dest_node = req.sender_id; // Vr
    int total_nodes = net_param.n_nodes;
    vector<ll> dis(total_nodes+1,1e18);
    vector<int> hops(total_nodes+1,0);
    vector<pair<int,edge*>> prev_node(total_nodes+1,{-1,NULL});
    vector<bool> vis(total_nodes+1,false);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>, greater<pair<ll,int>>> q;
    q.push({req.amount,start_node});
    dis[start_node]=req.amount;
    // `q` stores {distance, node}

    while(!q.empty()){
        auto cur_node = q.top();q.pop();
        if(vis[cur_node.second])continue;
        vis[cur_node.second]=true;
        // printing details
        // cout<<"-------See the cur node "<<cur_node.second<<" with dis "<<cur_node.first<<"\n";
        const auto itr = (cur_network->nodes).find(cur_node.second);
        if(itr == (cur_network->nodes).end()){
            cout<<"In `get_payment_path()` function, mapping of "<<
            cur_node.second<<" node doesn't exist\n";
            break;
        }
        // cout<<"HHHH\n";
        ll amount_to_be_transferred = cur_node.first;
        // We are traversing the graph in reverse order, i.e.
        // we are going to source from destinantion.
        // cout<<"Number of edges "<<((cur_network->nodes[cur_node.second])->open_edges.size())<<"\n";
        for(auto& cur_edge: (cur_network->nodes[cur_node.second])->open_edges){
            // This edge is the counter edge, we need to get the balance of 
            // the counter edge.
            
            if(cur_network->edges.find(cur_edge->counter_edge_id)==cur_network->edges.end()){
                cout<<"Edge id mapping not found in `get_payment_path()` function, edge id: "
                << cur_edge->counter_edge_id<<"\n";
            }
            edge* counter_edge = cur_network->edges[cur_edge->counter_edge_id];
            // cout<<"Counter edge "<<counter_edge->id<<" and the node "<<cur_edge->to_node_id<<"\n";
            // If the balance isn't sufficient, we cannot go through this edge.
            if(counter_edge->balance < amount_to_be_transferred){
                // cout<<"Lack of balance\n";
                continue;
            }

            // cout<<"The node: "<<cur_edge->to_node_id<<" has capacity\n";
            ll balance_diff = abs(cur_edge->balance - counter_edge->balance);
            ll fee_taken = calc_fee(cur_network->nodes[cur_edge->to_node_id],req.amount, balance_diff);
            // cout<<"Fee taken is "<<fee_taken<<"\n";
            if(dis[cur_edge->to_node_id] > dis[cur_node.second] + fee_taken){
                dis[cur_edge->to_node_id] = dis[cur_node.second] + fee_taken;
                prev_node[cur_edge->to_node_id] = {cur_node.second, counter_edge};
                hops[cur_edge->to_node_id]++;
                if(!vis[cur_edge->to_node_id])q.push({dis[cur_edge->to_node_id],cur_edge->to_node_id});
            }
        }
    }

    if(dis[dest_node]>1e17){
        // We cannot reach via any possible path. i.e. no path can fullfil the required capacity.
        req.status = 3; // req can't be completed due to limited channel capacity.
        vector<ll> empty_vec={};
        vector<pair<int,edge*>> empty_path_vec;
        return make_pair(empty_vec,empty_path_vec);
        // Empty vector indicates that no path exists
    }else{
        req.fee = dis[dest_node] -req.amount;
    }
    // Restore path.
    vector<pair<int,edge*>> path;
    for(int v = dest_node; v!=-1 && v!= start_node; v = prev_node[v].first){
        // cout<<"###### v: "<<v<<"\n";
        path.push_back({v,prev_node[v].second});
    }
    path.push_back({start_node,NULL});
    // reverse(path.begin(),path.end());
    return make_pair(dis, path);

}

/*
vector<pair<edge*, long long int>> execute_transaction(transaction& req, const vector<int>& dis, const vector<pair<int,edge*>>& path, network* cur_network){
    vector<pair<edge*, long long int>> rem_update_balances; 
    if(path.size()<2)return rem_update_balances;
    node* prev_node = cur_network->nodes[path[0].first];
    req.status = 0; // ongoing transaction
    int transaction_fee=0;
    // path[i] = { node id, edge id of outgoing edge}
    // O ----> 
    // cout<<"Executing transactions\n";
    // Calculate fee
    for(int i=1;i<path.size();i++){
        auto itr  = cur_network->nodes.find(path[i].first);
        if(itr == cur_network->nodes.end()){
            cout<<"In `execute_network` function, nodes with id "<<path[i].first
            <<" not found in mapping of network, node id to node pointer\n.";
            req.status=3;
            return rem_update_balances;
        }else{
            node* path_node = itr->second;
            if(i!=path.size()-1){
                int cur_fee = calc_fee(path_node,req.amount);
                path_node->fee_collected += cur_fee;
                transaction_fee+=cur_fee;
            }
            if(path_node->id >= dis.size()){
                cout<<"Node id exceeds distance vector size\n";break;
            }
            ll amt_though_this_edge = dis[path_node->id];
            // for the incoming edge to this node
            edge* incoming_edge = path[i-1].second;
            incoming_edge->balance -= amt_though_this_edge;

            // for the outgoing edge from this node
            auto edge_itr = cur_network->edges.find(path[i-1].second->counter_edge_id);
            if(edge_itr == cur_network->edges.end()){
                cout<<"Edge problem.\n";
                req.status=3;
                return rem_update_balances;
            }else{
                edge* outgoing_edge = edge_itr->second;
                rem_update_balances.push_back({outgoing_edge, amt_though_this_edge});
                //outgoing_edge->balance += amt_though_this_edge;
            }
            
        }
    }
    req.fee = transaction_fee;
    req.status = 5;
    return rem_update_balances;
}
*/
