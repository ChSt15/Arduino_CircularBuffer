#ifndef BUFFER_HELPER_H
#define BUFFER_HELPER_H


/**
 * This class implements a queue and stack type buffer.
 * This can also sort stored values and be used for processing measurements
*/



#include "stdint.h"
#include "math.h"


/**
 * Buffer class that can be used as queue or stack.
 * Can also be used to sort values and calculate median, average, deviation.
 */
template<typename T, uint32_t size_> 
class Buffer {
public:

    /**
     * Constructor for Buffer class.
     * <type, size> 
     * "Type" is the data type (uint32_t, int float etc.)
     * "size" is the maximum number of samples that will be stored.
     */
    Buffer(){}

    ~Buffer(){} //Remember to delete data on destruction

    /**
     * @returns number of elements in buffer
     */
    uint32_t available() const;

    /**
     * @returns number of empty spaces that can be filled.
     */
    uint32_t availableSpace() const;

    /**
     * Places a new element to the front of the buffer.
     *
     * @param element element to be placed into buffer.
     * @param overwrite Overwrites elements at back if true. Default false.
     * @return true if placed into buffer.
     */
    bool placeFront(const T &element, const bool overwrite = false);

    /**
     * Places a new element to the back of the buffer.
     *
     * @param element element to be placed into buffer.
     * @param overwrite Overwrites elements at front if true. Default false.
     * @return true if placed into buffer.
     */
    bool placeBack(const T &element, const bool overwrite = false);

    /**
     * Takes a element from the front of the buffer and places it into element.
     * peekFront() wont remove element.
     * Calling this on an empty buffer will simply return false.
     *
     * @param element Variable whos data will be overwritten.
     * @return true if removed from buffer.
     */
    bool takeFront(T* element);

    /**
     * Takes a element from the back of the buffer and places it into element.
     * peekBack() wont remove element.
     * Calling this on an empty buffer will simply return false.
     *
     * @param element Variable whos data will be overwritten.
     * @return true if removed from buffer.
     */
    bool takeBack(T* element);

    /**
     * Copies a element from the front of the buffer and places it into element.
     * takeFront() will remove element.
     * Calling this on an empty buffer will simply return false.
     *
     * @param element Variable whos data will be overwritten.
     * @return true if removed from buffer.
     */
    bool peekFront(T* element);

    /**
     * Copies a element from the back of the buffer and places it into element.
     * takeBack() will remove element.
     * Calling this on an empty buffer will simply return false.
     *
     * @param element Variable whos data will be overwritten.
     * @return true if removed from buffer.
     */
    bool peekBack(T* element);

    /**
     * Removes the element at the front.
     * Calling this on an empty buffer will do nothing.
     */
    void removeFront();

    /**
     * Removes the element at the back.
     * Calling this on an empty buffer will do nothing.
     */
    void removeBack();

    /**
     * Removes element that the given pointer points to.
     * @param pointerToElement Pointer to element to remove.
     * @returns true if element was found and removed.
     */
    bool removeElement(T* pointerToElement);

    /**
     * Removes element at given index.
     * @param index Index of element to be removed.
     * @returns true if element was found and removed.
     */
    bool removeElementIndex(uint32_t index);

    /**
     * Places element into given index
     * @param index Index of position to place element
     * @returns true if element was placed in buffer.
     */
    //bool insertElementIndex(const T &element, uint32_t index);

    /**
     * @returns the sum of all elements.
     */
    T getSum() const;

    /**
     * @returns the average of all elements.
     */
    T getAverage() const;

    /**
     * Sorts the elements. This will change the array!
     */
    void sortElements();

    /**
     * Sorts elements and returns median.
     * @return median.
     */
    T getMedian() const;

    /**
     * @returns the standard deviation.
     */
    T getStandardDeviation() const;

    /**
     * @returns the standard error.
     */
    T getStandardError() const;

    /**
     * Used to access buffer like an array.
     * Indexes outside of buffer will wrap around
     * buffer.
     * Starts from back of buffer("oldest" element or first that was placed inside).
     * @returns copy of element from index.
     */
    T& operator[] (const uint32_t &index);

    /**
     * Used to access buffer like an array.
     * Indexes outside of buffer will wrap around
     * buffer.
     * Starts from back of buffer("oldest" element or first that was placed inside).
     * @returns copy of element from index.
     */
    T operator[] (const uint32_t &index) const;

    /**
     * Needs to be overloaded to also copy the data to instance.
     * Not doing this will cause 2 buffers to share the exact same elements.
     */
    Buffer operator = (const Buffer &toBeCopied);

    /**
     * Removes all items from buffer. Not computationaly intensive.
     */
    void clear();


private:

    void quickSort(const uint32_t &left, const uint32_t &right);

    uint32_t quickSortPartition(const uint32_t &left, const uint32_t &right);

    //Array for element storage
    T bufferArray_[size_];

    //Should always have number of elements.
    uint32_t numElements_ = 0;

    //Points to index ahead of first element
    uint32_t front_ = 0;
    //Points to index of last element
    uint32_t back_ = 0;


};



template<typename T, uint32_t size_> 
T Buffer<T, size_>::getStandardError() const {
    if (numElements_ < 2) return T();
    return getStandardDeviation()/sqrt(numElements_);
}


template<typename T, uint32_t size_> 
T Buffer<T, size_>::getStandardDeviation() const {

    if (numElements_ < 2) return T();

    T standardDev = 0;

    T avg = getAverage();

    for (uint32_t i = 0; i < numElements_; i++) {

        T diff = (*this)[i] - avg;

        standardDev = standardDev + diff*diff;

    }

    standardDev = sqrt(standardDev/(numElements_-1));

    return standardDev;
    
}


template<typename T, uint32_t size_> 
T Buffer<T, size_>::getMedian() const {

    if (numElements_ == 0) return T();

    T median;

    Buffer<T, size_> bufferSorted;

    bufferSorted = *this;

    bufferSorted.sortElements();

    if (numElements_%2 == 0) { //Is even?

        median = ((*this)[numElements_/2] + (*this)[numElements_/2+1])/2;

    } else { //Odd
    
        median = (*this)[numElements_/2];

    }

    return median;
    
}


template<typename T, uint32_t size_> 
T Buffer<T, size_>::getAverage() const {
    if (numElements_ == 0) return T();
    return getSum()/numElements_;
}


template<typename T, uint32_t size_> 
T Buffer<T, size_>::getSum() const {

    T sum_ = 0;

    for (uint32_t i = 0; i < numElements_; i++) {
        sum_ = sum_ + (*this)[i];
    }

    return sum_;

}


template<typename T, uint32_t size_> 
uint32_t Buffer<T, size_>::quickSortPartition(const uint32_t &left, const uint32_t &right) {

    T pivot = (*this)[right];

    uint32_t i = left;

    for (uint32_t j = left; j < right; j++) {

        if ((*this)[j] < pivot) {

            //swap
            T temp = (*this)[i];
            (*this)[i] = (*this)[j];
            (*this)[j] = temp;

            i++;

        }

    }

    //swap
    T temp = (*this)[i];
    (*this)[i] = (*this)[right];
    (*this)[right] = temp;

    return i;

}


template<typename T, uint32_t size_> 
void Buffer<T, size_>::quickSort(const uint32_t &left, const uint32_t &right) {
    
    if (left < right) {

        uint32_t p = quickSortPartition(left, right);
        if (p != left) quickSort(left, p-1);
        quickSort(p+1, right);

    }

}



template<typename T, uint32_t size_> 
void Buffer<T, size_>::sortElements() {

    //Check if nothing to sort
    if (numElements_ < 2) return;

    quickSort(0, numElements_-1);

}



template<typename T, uint32_t size_> 
void Buffer<T, size_>::clear() {
    front_ = back_ = numElements_ = 0;
}


template<typename T, uint32_t size_> 
bool Buffer<T, size_>::removeElementIndex(uint32_t index) {

    //Make sure buffer isnt empty
    if (numElements_ == 0) return false;
    
    //Special case if numberElements if 1. Algorithm below cant handle this case.
    if (numElements_ == 1) {
        clear();
        return true;
    }

    //We have to move all elements ahead the one to be removed, one place down.
    for (uint32_t j = index; j < numElements_-1; j++) {

        ((*this)[j]) = ((*this)[j+1]);

    }

    numElements_--;

    //If we exited the loop then item was not found.
    return true;

}


template<typename T, uint32_t size_> 
bool Buffer<T, size_>::removeElement(T* pointerToElement) {

    //Make sure buffer isnt empty
    if (numElements_ == 0) return false;
    
    //Find element it points to
    for (uint32_t i = 0; i < numElements_; i++) {

        //Check if this is the item
        if (&((*this)[i]) == pointerToElement) {

            return removeElementIndex(i);

        }

    }

    //If we exited the loop then item was not found.
    return false;

}


template<typename T, uint32_t size_> 
T& Buffer<T, size_>::operator[] (const uint32_t &index) {
    return bufferArray_[(back_ + index)%numElements_];
}


template<typename T, uint32_t size_> 
T Buffer<T, size_>::operator[] (const uint32_t &index) const {
    return bufferArray_[(back_ + index)%numElements_];
}


template<typename T, uint32_t size_> 
Buffer<T, size_> Buffer<T, size_>::operator = (const Buffer &toBeCopied) {

    uint32_t sizeToBeCopied = toBeCopied.available();

    for (uint32_t i = 0; i < sizeToBeCopied; i++) this->placeFront(toBeCopied[i]);

    return *this;

}


template<typename T, uint32_t size_> 
bool Buffer<T, size_>::peekFront(T* element) {

    if (numElements_ == 0) return false;

    *element = bufferArray_[(front_+1)%size_];

    return true;

}


template<typename T, uint32_t size_> 
bool Buffer<T, size_>::peekBack(T* element) {

    if (numElements_ == 0) return false;

    *element = bufferArray_[back_];

    return true;

}


template<typename T, uint32_t size_> 
bool Buffer<T, size_>::takeFront(T* element) {

    if (numElements_ == 0) return false;

    if (front_ == 0) front_ = size_-1;
    else front_--;

    *element = bufferArray_[front_];

    numElements_--;

    return true;

}


template<typename T, uint32_t size_> 
bool Buffer<T, size_>::takeBack(T* element) {

    if (numElements_ == 0) return false;

    *element = bufferArray_[back_];

    back_ = (back_+1)%size_;

    numElements_--;

    return true;

}


template<typename T, uint32_t size_>
uint32_t Buffer<T, size_>::available() const {
    return numElements_;
}


template<typename T, uint32_t size_> 
uint32_t Buffer<T, size_>::availableSpace() const {
    return size_ - numElements_;
}


template<typename T, uint32_t size_> 
void Buffer<T, size_>::removeFront() {

    if (numElements_ == 0) return;

    if (front_ == 0) front_ = size_-1;
    else front_--;
    numElements_--;

}


template<typename T, uint32_t size_> 
void Buffer<T, size_>::removeBack() {

    if (numElements_ == 0) return;

    back_ = (back_+1)%size_;
    numElements_--;

}


template<typename T, uint32_t size_> 
bool Buffer<T, size_>::placeFront(const T &element, const bool overwrite) {

    if (numElements_ == size_) {

        if (overwrite) removeBack();
        else return false;

    }

    bufferArray_[front_] = element;

    front_ = (front_+1)%size_;
    numElements_++;

    return true;

}


template<typename T, uint32_t size_> 
bool Buffer<T, size_>::placeBack(const T &element, const bool overwrite) {

    if (numElements_ == size_) {

        if (overwrite) removeFront();
        else return false;

    }

    if (back_ == 0) back_ = size_-1;
    else back_--;

    bufferArray_[back_] = element;

    numElements_++;

    return true;

}







#endif