// There is a structure of the “singly linked list” type. Write a function that reverses the list. In other words, the first element becomes the last, and the last becomes the first
// `g++ -std=c++20 reverse-list.cpp`

#include <iostream>

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

int main() {
    Node node5 { 5 };
    Node node4 { 4, &node5 };
    Node node3 { 3, &node4 };
    Node node2 { 2, &node3 };
    Node node1 { 1, &node2 };
    print_list(&node1);
    
    auto* reversed = reverse_list(&node1);
    print_list(reversed);
}