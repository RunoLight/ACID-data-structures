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
    ListIterator(const  ListIterator& other) : ptr(other.ptr), list(other.list)
    {
        CLinkedList<ValueType>::inc_ref_count(ptr);
    }
    ListIterator(Node<value_type>* _new_ptr, CLinkedList<value_type>* _list) : ptr(_new_ptr), list(_list)
    {
        CLinkedList<ValueType>::inc_ref_count(ptr);
    }

    ~ListIterator() {
        if (!ptr) return;

        CLinkedList<ValueType>::dec_ref_count(ptr);
        if (ptr->ref_count == 0) {
            ptr = nullptr;
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

        *this = ListIterator(this->ptr->next, list);

        if (ptr) {
            while (this->ptr->deleted && ptr->next) {
                *this = ListIterator(this->ptr->next, list);
            }
        }

        return *this;

    }

    // postfix ++
    ListIterator operator++(int) {
        if (!ptr->next) throw (std::out_of_range("Invalid index"));

        ListIterator new_ptr(*this);

        if (new_ptr.ptr->next) {

            *this = ListIterator(new_ptr.ptr->next, list);

            while (ptr->deleted && ptr->next) {
                *this = ListIterator(this->ptr->next, list);
            }
        }

        //return new_ptr.ptr;
        return ListIterator(new_ptr.ptr, list);
    }

    // prefix --
    ListIterator& operator--() {
        if (!ptr->prev->prev) throw std::out_of_range("Invalid index");

        *this = ListIterator(this->ptr->prev, list);

        if (ptr) {
            while (this->ptr->deleted && ptr->prev) {
                *this = ListIterator(this->ptr->prev, list);
            }
        }

        return *this;
    }

    // postfix --
    ListIterator operator--(int) {
        if (!ptr->prev->prev) throw std::out_of_range("Invalid index");

        ListIterator new_ptr(*this);

        if (new_ptr.ptr->prev) {
            *this = ListIterator(new_ptr.ptr->prev, list);
            while (ptr->deleted && ptr->prev)
            {
                *this = ListIterator(this->ptr->prev, list);
            }
        }

        return ListIterator(new_ptr.ptr, list);
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

    int getRefCount()
    {
        return ptr->ref_count;
    }

private:
    Node<value_type>* ptr;
    CLinkedList<value_type>* list;
};