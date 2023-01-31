#include <bits/stdc++.h>
#include "variables.h"
#define ll long long int
using namespace std;

// Processes payments
void process_payments(network* cur_network, const network_params& net_param);

// Returns a set of transactions
vector<transaction> get_random_transactions(const network_params& net_param);

// Return false if transaction is not viable, either due to invalid send/receiver id or 
// amount to send exceeds all the edge balances of sender.
bool payment_request(transaction& req, network* cur_network);

// Calculates fee taken by a node
double calc_fee(node* cur_node, long long int amount, int balance_diff);

// Returns the payment path for a given request.
// Returns empty vector if doesn't exist.
pair<vector<ll>,vector<pair<int,edge*>>> get_payment_path(transaction& req, network* cur_network,const network_params& net_param);

// Execute a transaction.
vector<pair<edge*, long long int>> execute_transaction(transaction& req, const vector<int>& dis, const vector<pair<int,edge*>>& path, network* cur_network);