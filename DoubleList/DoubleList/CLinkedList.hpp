#pragma once

#include <functional>
#include <utility>
#include <type_traits>
#include <queue>
#include <limits>
#include <numeric>
#include <cmath>
#include <memory>
#include <stdexcept>

// Эта сука ебаная точно работает сейчас

template<typename ValueType>
class ListIterator;

template<typename ValueType>
class CLinkedList;
    
template<typename ValueType>
class Node
{
public:
    template<typename> friend class CLinkedList;
    template<typename> friend class ListIterator;

    using value_type = ValueType;
    using iterator = ListIterator<value_type>;

    Node() = default;
    ~Node() = default;
    Node(ValueType value, int ref_count)
        : val(std::move(value)), prev(this), next(this), deleted(false), ref_count(ref_count) {}
    Node(ValueType value, CLinkedList<value_type>* list) : Node(value, 2) {}
    Node(const Node<ValueType>&) = delete;

    void operator=(const Node<ValueType>&) = delete;
private:
    value_type val;
    Node<value_type>* prev;
    Node<value_type>* next;
    bool deleted;
    int ref_count;
};


template<typename ValueType>
class CLinkedList
{
public:
    using size_type = std::size_t;
    using value_type = ValueType;
    using iterator = ListIterator<value_type>;

    template<typename> friend class ListIterator;

    CLinkedList() : head(nullptr), tail(nullptr), m_size(0) {
        tail = new Node<value_type>();
        head = new Node<value_type>();
        tail->prev = head;
        head->next = tail;

        inc_ref_count(tail);
        inc_ref_count(head);
        inc_ref_count(tail);
        inc_ref_count(head);
    }

    CLinkedList(const CLinkedList& other) = delete;
    CLinkedList(CLinkedList&& x) = delete;
    CLinkedList(std::initializer_list<value_type> l):CLinkedList() {
        for (auto i = l.begin(); i < l.end(); i++)
            push_back(*i);

    }

    ~CLinkedList() {
        Node<value_type>* current = head;
        while (current != nullptr)
        {
            Node<value_type>* next = current->next;
            delete current;
            current = next;
        }
    }

    static void dec_ref_count(Node<value_type>* ptr) {
        if (!ptr) return;

        ptr->ref_count--;

        if (ptr->ref_count != 0) {
            return;
        }

        std::queue<Node<value_type>*> nodesToDelete;
        nodesToDelete.push(ptr);

        while (!nodesToDelete.empty()) {
            auto current = nodesToDelete.front();

            // Check if next node needs to be deleted
            auto nextNode = current->next;
            nextNode->ref_count--;
            if (nextNode->ref_count == 0) nodesToDelete.push(nextNode);

            // Check if prev node needs to be deleted
            auto prevNode = current->next;
            prevNode->ref_count--;
            if (prevNode->ref_count == 0) nodesToDelete.push(prevNode);

            // Delete current node
            auto toTheGraveyard = nodesToDelete.front();
            nodesToDelete.pop();
            delete (toTheGraveyard);
        }

    }

    static void inc_ref_count(Node<value_type>* ptr) {
        if (!ptr) return;
        ptr->ref_count++;
    }

    CLinkedList& operator=(const CLinkedList& other) = delete;
    CLinkedList& operator=(CLinkedList&& x) = delete;

    void push_back(const value_type& value) {
        push_back(value_type(value));
    }
     
    void push_back(value_type&& value) {
        iterator it(tail, this);
        inserts(it,value);
    }

    void push_front(const value_type& value) {
        push_front(value_type(value));
    }

    void push_front(value_type&& value) {
        iterator it(head->next, this);
        inserts(it, value);
    }

    iterator erase(iterator position) {
        auto output = iterator(position.ptr->next, this);

        inc_ref_count(position.ptr->next);
        inc_ref_count(position.ptr->prev);

        if (position.ptr == head->next) {
            head->next = position.ptr->next;
        }
        else {
            position.ptr->prev->next = position.ptr->next;
        }

        if (position.ptr == tail->prev) {
            tail->prev = position.ptr->prev;
        }
        else {
            position.ptr->next->prev = position.ptr->prev;
        }

        m_size--;

        position.ptr->deleted = true;
        dec_ref_count(position.ptr);
        dec_ref_count(position.ptr);

        return output;
    }

    iterator inserts(iterator ptr, value_type value) {
        if (!ptr) return ptr;
        Node<ValueType>* node = new Node<value_type>{ std::move(value), 2 };
            
        node->prev = ptr.ptr->prev;
        node->next = ptr.ptr;
        ptr.ptr->prev->next = node;
        ptr.ptr->prev = node;
            
        iterator it(node, this);
        m_size++;
        return it;
    }

    iterator begin() noexcept {
        iterator ptr(head->next, this);
        return ptr;
    }
    iterator end() noexcept { 
        iterator ptr(tail, this);
        return ptr;

    }

    bool empty() noexcept {
        return begin() == end();
    }

    void clear() noexcept {
        iterator current(head->next, this);
        while (current != iterator(tail, this))
        {
            current = erase(current);
        }
    }

    size_type size() const noexcept {
        return m_size;
    }

private:
    Node<ValueType>* head; 
    Node<ValueType>* tail;
    std::queue<Node<ValueType>*> deleted_nodes;
    size_type m_size;
};

