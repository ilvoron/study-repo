// For a singly linked list, write a function to insert an element
// `g++ -std=c++20 insert_node_list.cpp`

#include <iostream>

namespace junior {

template <typename T>
struct Node {
    T value;
    Node* next = nullptr;
};

// According to the contract, `new_node` does not belong to any other list
template <typename T>
void insert_after(Node<T>* bind_node, Node<T>* new_node) {
    if (!bind_node || !new_node) { return; }
    if (bind_node == new_node) { return; }
    new_node->next = bind_node->next;
    bind_node->next = new_node;
}

// According to the contract, `new_node` does not belong to any other list
template <typename T>
Node<T>* push_front(Node<T>* head, Node<T>* new_node) {
    if (!new_node) { return head; }
    if (head == new_node) { return head; }
    new_node->next = head;
    return new_node;
}

// According to the contract, `new_node` does not belong to any other list
template <typename T>
Node<T>* push_back(Node<T>* head, Node<T>* new_node) {
    if (!head) { return new_node; }
    if (!new_node) { return head; }
    auto* last_node = head;
    while (last_node->next) { last_node = last_node->next; }
    last_node->next = new_node;
    return head;
}

template <typename T>
requires (requires(T t) { std::cout << t; })
void print_list(const Node<T>* head) {
    while (head) {
        std::cout << head->value << " ";
        head = head->next;
    }
    std::cout << std::endl;
}

}

int main() {
    junior::Node node5 { 5 };
    junior::Node node4 { 4, &node5 };
    junior::Node node2 { 2, &node4 };
    junior::print_list(&node2);
    
    junior::Node node3 { 3 };
    junior::insert_after(&node2, &node3);
    junior::print_list(&node2);
    
    junior::Node node1 { 1 };
    auto* new_head = junior::push_front(&node2, &node1);
    junior::print_list(new_head);
    
    junior::Node node6 { 6 };
    new_head = junior::push_back(new_head, &node6);
    junior::print_list(new_head);
}