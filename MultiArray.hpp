#include <cstdarg>

using namespace std;

//This class implements an efficient multi-dimensional array for C++.

/*This code is licensed Apachev2.  Contact me if you want to use
  it in an incompatibly licensed FLOSS project:
  I may relicense it for you depending on the situation.*/

//See test_multi_array.cpp for usage example.

template<class T, int d>
class MultiArray
{
     friend class MultiArray<T,d+1>;
     
     MultiArray<T,d-1> nextD;
     T* begin_ptr;
     int multiplier;
     
public:
     void initialize(T* begin_ptr_,va_list args)
          {
               begin_ptr = begin_ptr_;
               multiplier = 1;
               va_arg(args,int);
               va_list next_va;
               va_copy(next_va,args);
               for(int i=1; i<d; i++)
                    multiplier*=va_arg(args,int);
               va_end(args);
               nextD.initialize(begin_ptr,next_va);
          }

     MultiArray() { }
     
     MultiArray(T* begin_ptr_, ...) : begin_ptr(begin_ptr_)
          {
               va_list args;
               va_start(args,begin_ptr_);
               initialize(begin_ptr,args);
          }

     MultiArray<T,d-1> operator[](int x)
          {
               nextD.begin_ptr = begin_ptr+x*multiplier;
               return nextD;
          }
};

template<class T>
class MultiArray<T,1>
{
     friend class MultiArray<T,2>;
     
     T* begin_ptr;

public:
     T& operator[](int x)
          {
               return *(begin_ptr+x);
          }

     void initialize(T* begin_ptr_,va_list args) { }
};
