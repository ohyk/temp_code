#ifndef ABS_LINEAR_H
#define ABS_LINEAR_H

template <class T>
class AbsLinear
{
public:
    using size_type = size_t;
    using pos_type  = int;

public:
    virtual ~AbsLinear() {}

    //================================= ADT ==================================
    //
    //========================================================================
    virtual size_type size() const                          = 0;
    virtual bool      empty() const                         = 0;
    virtual T&        get(pos_type index)                   = 0;
    virtual void      erase(pos_type index)                 = 0;
    virtual pos_type  index_of(T const& elem)               = 0;
    virtual void      insert(pos_type index, T const& elem) = 0;
};

#endif /* end of include guard: ABS_LINEAR_H */
