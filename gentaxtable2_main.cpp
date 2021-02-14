#include <iostream>
#include <string>

#include "StringFunctions.h"

using namespace std;

int get_num()
{
     string x;
     int to_return;
     getline(cin,x);
     to_return = stoi(x);
     return to_return;
}

int main()
{
     while(cin)
     {
          vector<int> lines_vec;
          try
          {
               lines_vec.push_back(get_num());
               lines_vec.push_back(get_num());
               lines_vec.push_back(get_num());
               lines_vec.push_back(get_num());
               lines_vec.push_back(get_num());
               lines_vec.push_back(get_num());
          }
          catch(std::invalid_argument e)
          {
               continue;
          }

          cout << " | " << lines_vec[0] << " <= x < " << lines_vec[1] << " | " << lines_vec[2] << " | " << lines_vec[3] << " | " << lines_vec[4] << " | " << lines_vec[5] << " | \n";

          string temp;
          getline(cin,temp);
          getline(cin,temp);
     }
     return 0;
}
