#pragma once
#include "CLinkedList.hpp"

template<typename ValueType>
class ListIterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = ValueType;
    using reference = ValueType&;
    using pointer = ValueType*;

    template<typename>
    friend class CLinkedList;


    ListIterator()noexcept = default;
    ListIterator(const  ListIterator& other) : ptr(other.ptr)//, list(other.list)
    {
        CLinkedList<ValueType>::inc_ref_count(ptr);
    }
    explicit ListIterator(Node<value_type>* _new_ptr) : ptr(_new_ptr)//, list(_list)
    {
        CLinkedList<ValueType>::inc_ref_count(ptr);
    }

    ~ListIterator() {
        if (!ptr) return;

        if (ptr->ref_count == 1) {
            CLinkedList<ValueType>::dec_ref_count(ptr);
            ptr = nullptr;
        }
        else {
            CLinkedList<ValueType>::dec_ref_count(ptr);
        }
    }

    ListIterator& operator=(const  ListIterator& other) {
        CLinkedList<ValueType>::dec_ref_count(ptr);

        this->ptr = other.ptr;

        CLinkedList<ValueType>::inc_ref_count(ptr);

        return *this;
    }

    reference operator*() const {
        if (ptr->deleted) throw (std::out_of_range("Invalid index"));

        return ptr->val;
    }

    pointer operator->() const {
        if (ptr->deleted) throw (std::out_of_range("Invalid index"));

        return &(ptr->val);
    }

    // prefix ++
    ListIterator& operator++() {
        if (!ptr->next) throw (std::out_of_range("Invalid index"));

        *this = this->ptr->next;

        if (ptr) {
            while (this->ptr->deleted && ptr->next) {
                *this = this->ptr->next;
            }
        }

        return *this;

    }

    // postfix ++
    ListIterator operator++(int) {
        if (!ptr->next) throw (std::out_of_range("Invalid index"));

        ListIterator new_ptr(*this);

        if (new_ptr.ptr->next) {
            *this = new_ptr.ptr->next;
            while (ptr->deleted && ptr->next) {
                *this = this->ptr->next;
            }
        }

        return new_ptr.ptr;
    }

    // prefix --
    ListIterator& operator--() {
        if (!ptr->prev->prev) throw std::out_of_range("Invalid index");

        *this = this->ptr->prev;

        if (ptr) {
            while (this->ptr->deleted && ptr->prev) {
                *this = this->ptr->prev;
            }
        }

        return *this;
    }

    // postfix --
    ListIterator operator--(int) {
        if (!ptr->prev->prev) throw std::out_of_range("Invalid index");

        ListIterator new_ptr(*this);

        if (new_ptr.ptr->prev) {
            *this = new_ptr.ptr->prev;
            while (ptr->deleted && ptr->prev)
                *this = this->ptr->prev;
        }

        return new_ptr.ptr;
    }

    friend bool operator==(const ListIterator<ValueType>& a, const ListIterator<ValueType>& b) {
        return a.ptr == b.ptr;
    }

    friend bool operator!=(const ListIterator<ValueType>& a, const ListIterator<ValueType>& b) {
        return !(a == b);
    }

    operator bool() const {
        return ptr;
    }

private:
    Node<value_type>* ptr;
    CLinkedList<value_type>* list;
};