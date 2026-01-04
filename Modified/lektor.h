// original https://github.com/KenshiReclaimer/KenshiLib
// Modified by Lucius on 2026-01-01:
// changed lektor()
// Added ~lektor(), void push_back(T), void insert(T, uint32_t), void remove(T), void push_back_unique(T)

#pragma once

#include <assert.h>
#include <ogre/OgreMemorySTLAllocator.h>
#include <stdint.h>

template <typename T>
class lektor : Ogre::STLAllocator<T, Ogre::CategorisedAllocPolicy<Ogre::MEMCATEGORY_GENERAL> >
{
    // Ogre::STLAllocator<int,Ogre::CategorisedAllocPolicy<0> > offset = 0x0, length = 0x8
public:
    lektor()
        : count(0), maxSize(10), stuff(nullptr)
    {
        stuff = new T[maxSize];
    }

    typedef       T* iterator;
    typedef const T* const_iterator;

    iterator       begin() { return stuff; }
    const_iterator begin() const { return stuff; }
    iterator       end() { return stuff + count; }
    const_iterator end()   const { return stuff + count; }

    T& at(uint32_t index) {
        assert(stuff && index < count);
        return stuff[index];
    }

    T& operator[](uint32_t index) {
        return at(index);
    }

    const T& operator[](uint32_t index) const {
        assert(stuff && index < count);
        return stuff[index];
    }

    bool valid() const { return stuff != nullptr; }
    void clear() { count = 0; }

    uint32_t size()     const { return count; }
    uint32_t capacity() const { return maxSize; }

    T& find_first(bool* found, bool(*cmpfn)(T&))
    {
        for (uint32_t i = 0; i < maxSize; ++i) {
            if (cmpfn(stuff[i])) {
                return stuff[i];
            }
        }
    }

    uint32_t count;
    uint32_t maxSize;
    T* stuff;

    virtual ~lektor() { if (stuff != nullptr) delete[] stuff; }

    void reserve(uint32_t newSize)
    {
        if (newSize == 0)
            newSize = 10;

        if (maxSize < newSize)
        {
            auto newStuff = new T[newSize];
            for (uint32_t i = 0; i < count; ++i)
            {
                newStuff[i] = stuff[i];
            }

            delete[] stuff;
            stuff = newStuff;
            maxSize = newSize;
        }
    }

    void push_back(T x)
    {
        if (maxSize <= count)
            reserve(maxSize * 2);

        stuff[count] = x;
        ++count;
    }

    void insert(T x, uint32_t index)
    {
        if (maxSize <= count)
            reserve(maxSize * 2);

        ++count;

        for (uint32_t i = count - 1; i > index; --i)
        {
            stuff[i] = stuff[i - 1];
        }

        stuff[index] = x;
    }

    void remove(T x)
    {
        for (uint32_t i = 0; i < count; ++i)
        {
            if (stuff[i] == x)
            {
                if (i + 1 < count)
                {
                    for (uint32_t j = i + 1; j < count; ++j)
                    {
                        stuff[j - 1] = stuff[j];
                    }
                }
                --count;
                break;
            }
        }
    }

    void push_back_unique(T x)
    {
        if (count != 0)
        {
            for (uint32_t i = 0; i < count; ++i)
            {
                if (stuff[i] == x)
                    return;
            }
        }

        if (maxSize <= count)
            reserve(maxSize * 2);

        stuff[count] = x;
        ++count;
    }
};
