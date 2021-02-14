#include "Number.hpp"
#include "StringFunctions.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <vector>

using std::abs;
using std::istream;
using std::ostream;
using std::setfill;
using std::setw;
using std::string;
using std::vector;

static int64_t pow10(uint8_t power)
{
     int64_t to_return = 1;
     for(uint8_t i=0; i<power; i++)
          to_return*=10;
     return to_return;
}

Number::Number(std::string str_val)
{
     vector<string> split_result;
     StringFunctions::tokenize(split_result,str_val,".");
     if(split_result.size() > 2 || split_result.size() == 0)
          throw std::invalid_argument("Number constructor: invalid string given");
     if(split_result.size()==1)
     {
          value = stol(split_result[0]);
          if(str_val[0]=='.')
               right = split_result[0].size();
          else
               right = 0;
     }
     if(split_result.size()==2)
     {
          value = stol(split_result[0]);
          right = split_result[1].size();
          value*=pow10(right);
          if(value >= 0)
               value+=stol(split_result[1]);
          else
               value-=stol(split_result[1]);
     }
}

Number::operator int64_t()
{
     int64_t exponent = pow10(abs(right));
     int64_t to_return = value;
     if(right > 0)
          to_return/=exponent;
     else
          to_return*=exponent;
     return to_return;
}

uint8_t Number::get_total_columns() const
{
     int64_t temp_value = value;
     uint8_t to_return = 0;
     while(temp_value)
     {
          temp_value/=10;
          to_return++;
     }

     return to_return;
}

void Number::round_or_truncate(Number amount, int bias)
{
     if(amount.value <= 0)
          throw std::invalid_argument("Right argument to rounding and truncation arguments must be positive.");

     bool i_was_negative = value < 0;
     value = abs(value);
          
     int8_t max_right = std::max(right,amount.right);
     int8_t min_right = std::min(right,amount.right);
     for(Number* num : {this,&amount})
     {
          int8_t shift_needed = max_right - num->right;
          if(num->get_total_columns()+shift_needed > Number::MAX_COLUMNS)
               throw std::range_error("Number rounding error: Column overflow.");
          if(shift_needed)
          {
               num->value*=pow10(shift_needed);
               num->right+=shift_needed;
          }
     }

     //Perform rounding or truncation
     switch(bias)
     {
     case -1: *this = Number{value-value%amount.value,max_right};
          break;

     case 0:
          if(value%amount.value >= amount.value/2 + amount.value%2)
               *this = Number{value+amount.value-value%amount.value,max_right};
          else
               *this = Number{value-value%amount.value,max_right};
          break;

     case 1:
          if(value%amount.value)
               *this = Number{value+amount.value-value%amount.value,max_right};
          break;
     }

     //Fix up significant figures to reflect rounding
     if(!value)
          right = 0;
     while(value && !(value%10))
     {
          value/=10;
          right--;
     }

     //Handle negativity
     if(i_was_negative)
          value=-value;
}

Number operator-(const Number& left)
{
     Number to_return = left;
     to_return.value=-to_return.value;
     return to_return;
}

Number operator+(const Number& left, const Number& right)
{
     int8_t max_right = std::max(left.right,right.right);
     Number modified_left = left;
     Number modified_right = right;
     for(Number* num : {&modified_left,&modified_right})
     {
          int8_t shift_needed = max_right - num->right;
          if(num->get_total_columns()+shift_needed > Number::MAX_COLUMNS)
               throw std::range_error("Number operator+: Try rounding more.");
          if(shift_needed)
          {
               num->value*=pow10(shift_needed);
               num->right+=shift_needed;
          }
     }

     return Number{modified_left.value+modified_right.value,max_right};
}

Number operator-(const Number& left, const Number& right)
{
     return left + -right;
}

Number operator*(const Number& left, const Number& right)
{
     if(left.get_total_columns()+right.get_total_columns() > Number::MAX_COLUMNS)
          throw std::range_error("Number operator*: Try rounding more.");

     return Number{left.value*right.value,left.right+right.right};
}

Number operator/(const Number& left, const Number& right)
{
     Number to_return{left.value/right.value,left.right-right.right};
     uint8_t max_precision = Number::MAX_COLUMNS - std::max(left.get_total_columns(),right.get_total_columns());
     uint64_t divisor = abs(right.value);
     uint64_t remainder = abs(left.value)%divisor;
     remainder*=pow10(max_precision);
     remainder/=divisor;
     to_return.value*=pow10(max_precision);
     if(left.value > 0 && right.value > 0 || left.value < 0 && right.value < 0)
          to_return.value+=remainder;
     else
          to_return.value-=remainder;
     to_return.right+=max_precision;
     return to_return;
}

Number abs(Number n)
{
     if(n.value < 0)
     {
          n.value = -n.value;
          return n;
     }
     else
          return n;
}

bool operator<(const Number& left, const Number& right)
{
     int8_t left_left = left.get_total_columns()-left.right;
     int8_t right_left = right.get_total_columns()-right.right;

     if(left_left == right_left)
     {
          if(left.right==right.right)
               return left.value < right.value;

          int8_t left_shift_amount;
          int8_t right_shift_amount;
          if(left.right > right.right)
          {
               left_shift_amount = left.right - right.right;
               right_shift_amount = 0;
          }
          else
          {
               left_shift_amount = 0;
               right_shift_amount = right.right - left.right;
          }
          
          int64_t left_truncated_value = left.value/pow10(left_shift_amount);
          int64_t right_truncated_value = right.value/pow10(right_shift_amount);
          if(left_truncated_value!=right_truncated_value)
               return left_truncated_value < right_truncated_value;

          //One of these is guaranteed to be zero
          int64_t left_rump = left.value-left_truncated_value*pow10(left_shift_amount);
          int64_t right_rump = right.value-right_truncated_value*pow10(right_shift_amount);
          return left_rump < right_rump;
     }

     bool to_return = left_left < right_left;
     if(left.value < 0 && right.value < 0)
          return !to_return;
     else if(left.value < 0 && right.value > 0)
          return true;
     else if(left.value > 0 && right.value < 0)
          return false;
     else if(left.value==0)
          return right.value > 0;
     else if(right.value==0)
          return left.value < 0;
     else
          return to_return;
}

bool operator==(const Number& left, const Number& right)
{
     int8_t min_right = std::min(left.right,right.right);
     Number modified_left = left;
     Number modified_right = right;
     for(Number* num : {&modified_left,&modified_right})
     {
          int8_t shift_needed = num->right - min_right;
          if(shift_needed)
          {
               if(num->value/pow10(shift_needed)*pow10(shift_needed)!=num->value)
                    return false;
               num->value/=pow10(shift_needed);
               num->right-=shift_needed;
          }
     }

     return modified_left.value==modified_right.value;
}

istream& operator>>(istream& in, Number& right)
{
     string x;
     in >> x;
     right = Number{x};
     return in;
}

ostream& operator<<(ostream& out, const Number& right)
{
     if(right.right <= 0)
     {
          out << right.value;
          for(int i=0; i>right.right; i--)
               out << '0';
     }
     else
          out << (right.value < 0 ? "-" : "") << abs(right.value)/pow10(right.right) << '.' << setfill('0') << setw(right.right) << abs(right.value)%pow10(right.right);
     return out;
}
