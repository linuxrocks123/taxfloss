#pragma once

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

class Number
{
     friend Number abs(Number n);
     friend Number operator-(const Number& left);
     friend Number operator+(const Number& left, const Number& right);
     friend Number operator-(const Number& left, const Number& right);
     friend Number operator*(const Number& left, const Number& right);
     friend Number operator/(const Number& left, const Number& right);

     friend bool operator<(const Number& left, const Number& right);
     friend bool operator==(const Number& left, const Number& right);

     friend std::istream& operator>>(std::istream& left, Number& right);
     friend std::ostream& operator<<(std::ostream& left, const Number& right);
public:
     Number(int64_t value_, int8_t right_ = 0) : value(value_), right(right_)
          {
               uint8_t total_columns = get_total_columns();
               if(get_total_columns() > MAX_COLUMNS)
                    throw std::range_error("Number constructor: Try rounding more.");
          }

     Number(std::string str_val);

     Number() = default;

     explicit operator int64_t();

     void round(const Number& amount) { round_or_truncate(amount,0); }
     void roundup(const Number& amount) { round_or_truncate(amount,1); }
     void truncate(const Number& amount) { round_or_truncate(amount,-1); }
     
private:
     const static uint8_t MAX_COLUMNS = 18;

     uint8_t get_total_columns() const;
     void round_or_truncate(Number amount, int bias);
     
     int64_t value;
     int8_t right;
};

Number abs(Number n);

Number operator-(const Number& left);
Number operator+(const Number& left, const Number& right);
Number operator-(const Number& left, const Number& right);
Number operator*(const Number& left, const Number& right);
Number operator/(const Number& left, const Number& right);

bool operator<(const Number& left, const Number& right);
bool operator==(const Number& left, const Number& right);

std::istream& operator>>(std::istream& left, Number& right);
std::ostream& operator<<(std::ostream& left, const Number& right);

#define type Number
#define RELOPS_DISABLE_ASSIGNMODULO
#include "relops.cxx"
