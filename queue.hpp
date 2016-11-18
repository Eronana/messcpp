#ifndef __MESSCPP_QUEUE
#define __MESSCPP_QUEUE
#include <vector>
template<typename T>
class Queue
{
private:
    std::vector<T> data;
    int base;
public:
    Queue():base(0){}
    size_t size() const
    {
        return data.size()-base;
    }
    void push(const T &a)
    {
        data.push_back(a);
    }
    void pop(const int n)
    {
        if((base+=n)==data.size())
        {
            data.clear();
            base=0;
        }
    }
    const T &operator[](const int index) const
    {
        return data[index+base];
    }
    const T* begin() const
    {
        return &data[base];
    }
};
#endif
