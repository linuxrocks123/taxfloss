#ifndef RELOPS_DISABLE_RELOPS

inline bool operator>(const type& left, const type& right)
{
     return right < left;
}

inline bool operator<=(const type& left, const type& right)
{
     return !(right < left);
}

inline bool operator>=(const type& left, const type& right)
{
     return !(left < right);
}

inline bool operator!=(const type& left, const type& right)
{
     return !(left == right);
}

#endif

#undef RELOPS_DISABLE_RELOPS

#ifndef RELOPS_DISABLE_ASSIGNPLUS

inline type operator+=(type& left, const type& right)
{
     left = left + right;
     return left;
}

#endif

#undef RELOPS_DISABLE_ASSIGNPLUS

#ifndef RELOPS_DISABLE_ASSIGNMINUS

inline type operator-=(type& left, const type& right)
{
     left = left - right;
     return left;
}

#endif

#undef RELOPS_DISABLE_ASSIGNMINUS

#ifndef RELOPS_DISABLE_ASSIGNMULT

inline type operator*=(type& left, const type& right)
{
     left = left * right;
     return left;
}

#endif

#undef RELOPS_DISABLE_ASSIGNMULT

#ifndef RELOPS_DISABLE_ASSIGNDIV

inline type operator/=(type& left, const type& right)
{
     left = left / right;
     return left;
}

#endif

#undef RELOPS_DISABLE_ASSIGNDIV

#ifndef RELOPS_DISABLE_ASSIGNMODULO

inline type operator%=(type& left, const type& right)
{
     left = left % right;
     return left;
}

#endif

#undef RELOPS_DISABLE_ASSIGNMODULO

#ifndef RELOPS_DISABLE_TOSTRING

#include <sstream>
#include <string>

inline std::string to_string(const type& value)
{
     std::ostringstream out;
     out << value;
     return out.str();
}

#endif

#undef RELOPS_DISABLE_TOSTRING

#undef type
