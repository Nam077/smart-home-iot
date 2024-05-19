#ifndef VECTOR_H
#define VECTOR_H

#include <stdexcept>

/**
 * @brief A template class that mimics the behavior of a dynamic array with the ability to resize itself automatically.
 * 
 * @tparam T The data type of the elements stored in the vector.
 */
template<typename T>
class Vector {
private:
    T* _elements; ///< Pointer to the array of elements
    int _capacity; ///< Maximum number of elements that can be stored
    int _size; ///< Current number of elements stored

public:
    /**
     * @brief Constructs the vector with a specified initial capacity (default 10).
     * 
     * @param initialCapacity Initial capacity of the vector.
     */
    Vector(int initialCapacity = 10) {
        _capacity = initialCapacity;
        _elements = new T[_capacity];
        _size = 0;
    }

    /**
     * @brief Destroys the vector and frees the allocated memory.
     */
    ~Vector() {
        delete[] _elements;
    }

    /**
     * @brief Adds an element to the end of the vector, resizing if necessary.
     * 
     * @param element The element to add.
     */
    void push_back(const T& element) {
        if (_size >= _capacity) {
            _capacity *= 2;
            T* newElements = new T[_capacity];
            for (int i = 0; i < _size; ++i) {
                newElements[i] = _elements[i];
            }
            delete[] _elements;
            _elements = newElements;
        }
        _elements[_size++] = element;
    }

    /**
     * @brief Removes the last element from the vector, if the vector is not empty.
     */
    void pop_back() {
        if (_size > 0) {
            --_size;
        }
    }

    /**
     * @brief Resizes the vector to contain newSize elements.
     * 
     * @param newSize The new size of the vector.
     */
    void resize(int newSize) {
        if (newSize < _size) {
            _size = newSize;
        } else {
            reserve(newSize);
            for (int i = _size; i < newSize; ++i) {
                _elements[i] = T();
            }
            _size = newSize;
        }
    }

    /**
     * @brief Ensures that the vector can hold at least newCapacity elements.
     * 
     * @param newCapacity The new capacity of the vector.
     */
    void reserve(int newCapacity) {
        if (newCapacity > _capacity) {
            T* newElements = new T[newCapacity];
            for (int i = 0; i < _size; ++i) {
                newElements[i] = _elements[i];
            }
            delete[] _elements;
            _elements = newElements;
            _capacity = newCapacity;
        }
    }

    /**
     * @brief Reduces memory usage by freeing unused memory.
     */
    void shrink_to_fit() {
        if (_size < _capacity) {
            reserve(_size);
        }
    }

    /**
     * @brief Assigns new contents to the vector, replacing its current contents, and modifying its size accordingly.
     * 
     * @param first Input iterator to the initial position in a sequence.
     * @param last Input iterator to the final position in a sequence.
     */
    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last) {
        clear();
        while (first != last) {
            push_back(*first);
            ++first;
        }
    }

    /**
     * @brief Inserts an element at the specified index.
     * 
     * @param index Position where the element will be inserted.
     * @param element The element to insert.
     */
    void insert(int index, const T& element) {
        if (index < 0 || index > _size) {
            throw std::out_of_range("Index out of range");
        }
        if (_size >= _capacity) {
            _capacity *= 2;
            T* newElements = new T[_capacity];
            for (int i = 0; i < index; ++i) {
                newElements[i] = _elements[i];
            }
            newElements[index] = element;
            for (int i = index; i < _size; ++i) {
                newElements[i + 1] = _elements[i];
            }
            delete[] _elements;
            _elements = newElements;
        } else {
            for (int i = _size; i > index; --i) {
                _elements[i] = _elements[i - 1];
            }
            _elements[index] = element;
        }
        ++_size;
    }

    /**
     * @brief Removes the element at the specified index.
     * 
     * @param index Position of the element to remove.
     */
    void erase(int index) {
        if (index < 0 || index >= _size) {
            throw std::out_of_range("Index out of range");
        }
        for (int i = index; i < _size - 1; ++i) {
            _elements[i] = _elements[i + 1];
        }
        --_size;
    }

    /**
     * @brief Exchanges the contents of the vector with those of another vector of the same type.
     * 
     * @param other Another vector to exchange the contents with.
     */
    void swap(Vector<T>& other) {
        std::swap(_elements, other._elements);
        std::swap(_capacity, other._capacity);
        std::swap(_size, other._size);
    }

    /**
     * @brief Returns a reference to the first element in the vector.
     * 
     * @return Reference to the first element.
     * @throw std::out_of_range If the vector is empty.
     */
    T& front() {
        if (_size > 0) {
            return _elements[0];
        } else {
            throw std::out_of_range("Vector is empty");
        }
    }

    /**
     * @brief Returns a reference to the last element in the vector.
     * 
     * @return Reference to the last element.
     * @throw std::out_of_range If the vector is empty.
     */
    T& back() {
        if (_size > 0) {
            return _elements[_size - 1];
        } else {
            throw std::out_of_range("Vector is empty");
        }
    }

    /**
     * @brief Returns a pointer to the first element.
     * 
     * @return Pointer to the first element.
     */
    T* begin() {
        return _elements;
    }

    /**
     * @brief Returns a pointer to the element following the last element.
     * 
     * @return Pointer to the element following the last element.
     */
    T* end() {
        return _elements + _size;
    }

    /**
     * @brief Returns a pointer to the last element.
     * 
     * @return Pointer to the last element.
     */
    T* rbegin() {
        return _elements + _size - 1;
    }

    /**
     * @brief Returns a pointer to the element before the first element.
     * 
     * @return Pointer to the element before the first element.
     */
    T* rend() {
        return _elements - 1;
    }

    /**
     * @brief Clears all elements from the vector.
     */
    void clear() {
        _size = 0;
    }

    /**
     * @brief Checks if the vector is empty.
     * 
     * @return True if the vector is empty, false otherwise.
     */
    bool empty() const {
        return _size == 0;
    }

    /**
     * @brief Returns the number of elements in the vector.
     * 
     * @return Number of elements.
     */
    int size() const {
        return _size;
    }

    /**
     * @brief Returns the maximum number of elements that the vector can hold.
     * 
     * @return Capacity of the vector.
     */
    int capacity() const {
        return _capacity;
    }

    /**
     * @brief Accesses the specified element with bounds checking.
     * 
     * @param index The index of the element to access.
     * @return Reference to the requested element.
     * @throw std::out_of_range If index is out of range.
     */
    T& at(int index) {
        if (index < 0 || index >= _size) {
            throw std::out_of_range("Index out of range");
        }
        return _elements[index];
    }

    /**
     * @brief Accesses the specified element without bounds checking.
     * 
     * @param index The index of the element to access.
     * @return Reference to the requested element.
     */
    T& operator[](int index) {
        return _elements[index];
    }
};

#endif
