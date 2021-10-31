#pragma once

#include <cstdlib>

template <typename T>
class SimpleVector {
public:
    SimpleVector();
    explicit SimpleVector(size_t size);
    ~SimpleVector();

    T& operator[](size_t index);

    T* begin();
    T* end();

    size_t Size() const;
    size_t Capacity() const;
    void PushBack(const T& value);

private:
    T* data_;
    size_t size_;
    size_t capacity_;
};

template <typename T>
SimpleVector<T>::SimpleVector()
    : data_(nullptr)
    , size_(0)
    , capacity_(0) {}

template <typename T>
SimpleVector<T>::SimpleVector(size_t size) {
    data_ = new T[size];
    size_ = size;
    capacity_ = size;
}

template <typename T>
SimpleVector<T>::~SimpleVector() {
    delete[] data_;
}

template <typename T>
T& SimpleVector<T>::operator[](size_t index) {
    return data_[index];
}

template<typename T>
T* SimpleVector<T>::begin() {
    return data_;
}

template<typename T>
T* SimpleVector<T>::end() {
    return data_ + size_;
}

template<typename T>
size_t SimpleVector<T>::Size() const {
    return size_;
}

template<typename T>
size_t SimpleVector<T>::Capacity() const {
    return capacity_;
}

template<typename T>
void SimpleVector<T>::PushBack(const T& value) {
    if (size_ == capacity_) {
        size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
        T* new_data_ = new T[new_capacity];

        for (int i = 0; i < capacity_; ++i)
            new_data_[i] = data_[i];

        delete[] data_;
        data_ = new_data_;
        capacity_ = new_capacity;
    }

    data_[size_++] = value;
}
