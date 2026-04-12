// There is a structure of the “singly linked list” type. Write a function that reverses the list. In other words, the first element becomes the last, and the last becomes the first
// `g++ -std=c++20 reverse-list.cpp`

#include <iostream>

namespace junior {

template <typename T>
struct Node {
    T value;
    Node<T>* next = nullptr;
};

template <typename T>
Node<T>* reverse_list(Node<T>* head) {
    Node<T>* prev = nullptr;
    Node<T>* cur = head;
    Node<T>* next = nullptr;
    
    while (cur) {
        next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }

    return prev;
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
    junior::Node node3 { 3, &node4 };
    junior::Node node2 { 2, &node3 };
    junior::Node node1 { 1, &node2 };
    junior::print_list(&node1);
    
    auto* reversed = junior::reverse_list(&node1);
    junior::print_list(reversed);
}