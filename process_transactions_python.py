import random
import time
import heapq

def process_payments(cur_network, net_param,transactions_to_execute):

    # Code for finding if the node has become inactive randomly with given probability.
    faulty_prob = net_param.faulty_node_probability
    num = 1
    while faulty_prob < 1.0 and net_param.faulty_node_probability > 0.0:
        faulty_prob *= 10
        num *= 10
    distribution = [i for i in range(1, num + 1)]

    random.seed(time.time())
    # Stores the events to be processed i.e. edges to be processed.
    # The top element in the queue is the one with the least time(ready time).
    # {event type, event}
    process_events = []

    # Payment request
    for cur_txn in transactions_to_execute:
        if payment_request(cur_txn, cur_network):
            # This transaction might be completed.
            path = get_payment_path_using_bellman_ford(cur_txn, cur_network, net_param)

            cur_txn.path_length = len(path) - 1
            if not path:
                print("Not possible to reach the destination due to lack of balances.\n\n\n")
                continue
            print(f"Sender: {cur_txn.sender_id} Receiver: {cur_txn.receiver_id} Amt: {cur_txn.amount}\nFee taken {cur_txn.fee}\n")
            print(f"Path taken, len: {len(path)}\n")
            for path_var in path:
                path_var.txn_id = transactions_to_execute.index(cur_txn)
                process_events.append((1, path_var))
                # Event type is 1, i.e. it represents normal hop, not reverted one
                print(f"{path_var.node_id} --> ", end="")
            print("\n")
        else:
            # This transaction can't be executed.
            pass

    hops_processed = [[] for _ in range(len(transactions_to_execute))]
    # Now we have the events to be processed and the time when they are ready to be processed.

    while process_events:
        cur_event = heapq.heappop(process_events)
        cur_txn_id = cur_event[1].txn_id
        if transactions_to_execute[cur_txn_id].status == 1:
            # This transaction is cancelled. Do not process any further steps.
            continue
        cur_path_var = cur_event[1]
        cur_node = cur_network.nodes[cur_path_var.node_id]
        outgoing_edge = cur_path_var.outgoing_edge
        if cur_event[0] == 2:
            # Reverted txn
            outgoing_edge.balance += cur_path_var.amount_passed
        else:
            if cur_path_var.node_id not in cur_network.nodes:
                print("Node not found\n")
                return
            # If this is the destination, update the balances and give the fees to intermediate nodes.
            if not outgoing_edge:
                # This transaction is complete.
                for temp_int_node in hops_processed[cur_txn_id]:
                    if temp_int_node.outgoing_edge.counter_edge_id not in cur_network.edges:
                        print("Edge mapping not found, line 104. \n")
                        return
                    incoming_edge = cur_network.edges[temp_int_node.outgoing_edge.counter_edge_id]
                    incoming_edge.balance += temp_int_node.amount_passed
                    if temp_int_node.node_id not in cur_network.nodes:
                        print("Node mapping not found, line 110 \n")
                        return
                    temp_node = cur_network.nodes[temp_int_node.node_id]
                    temp_node.fee_collected += temp_int_node.fee_taken
               
