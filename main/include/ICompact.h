#ifndef ICOMPACT_H
#define ICOMPACT_H

class IVector;
#include "ILogger.h"
#include<stddef.h>
class ICompact
{
public:
    class iterator;

    /*factories*/

    static ICompact* createCompact(IVector const* const begin, IVector const* const end, ILogger*logger);

    /*static operations*/
    static ICompact* intersection(ICompact const* const left, ICompact const* const right, ILogger*logger);
    
    //union
    static ICompact* add(ICompact const* const left, ICompact const* const right, ILogger*logger);

    //methods kill closed!!!
    //static ICompact* Difference(ICompact const* const left, ICompact const* const right, ILogger*logger);
    //static ICompact* SymDifference(ICompact const* const left, ICompact const* const right, ILogger*logger);

    static ICompact* makeConvex(ICompact const* const left, ICompact const* const right, ILogger*logger);

    /* returns a step, end, begin with which you can iterate over all domains of compact*/
    virtual IVector* getBegin() const = 0;
    virtual IVector* getEnd() const = 0;

    virtual iterator* end(IVector const* const step = 0) = 0;
    virtual iterator* begin(IVector const* const step = 0) = 0;

    virtual RESULT_CODE isContains(IVector const* const vec, bool& result) const = 0;
    virtual RESULT_CODE isSubSet(ICompact const* const other,bool& result) const = 0;
    virtual RESULT_CODE isIntersects(ICompact const* const other, bool& result) const = 0;

    virtual size_t getDim() const = 0;
    virtual ICompact* clone() const = 0;

    /*dtor*/
    virtual ~ICompact() = 0;

    class iterator
    {
    public:
        //adds step to current value in iterator
        //+step
        virtual RESULT_CODE doStep() = 0;

        virtual IVector* getPoint() const = 0;

        //change order of step
        virtual RESULT_CODE setDirection(IVector const* const dir) = 0;

        /*dtor*/
        virtual ~iterator() = default;
    protected:
        iterator() = default;

       
    private:
        /*non default copyable*/
        iterator(const iterator& other) = delete;
        void operator=( const iterator& other) = delete;
    };
protected:
    ICompact() = default;

private:
    /*non default copyable*/
    ICompact(const ICompact& other) = delete;
    void operator=( const ICompact& other) = delete;
};

#endif // ICOMPACT_H
