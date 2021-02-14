#include <iostream>
#include <string>

#include "StringFunctions.h"

using namespace std;

int get_num()
{
     string x;
     int to_return;
     getline(cin,x);
     x = StringFunctions::replace(x,",","");
     to_return = stoi(x);
     return to_return;
}

int main()
{
     while(cin)
     {
          vector<vector<int>> lines_vec{5};
          for(int i=0; i<6; i++)
          {
               try
               {
                    lines_vec[0].push_back(get_num());
                    lines_vec[1].push_back(get_num());
                    lines_vec[2].push_back(get_num());
                    lines_vec[3].push_back(get_num());
                    lines_vec[4].push_back(get_num());
               }
               catch(std::invalid_argument e)
               {
                    goto skip;
               }
               string x;
               getline(cin,x);
               getline(cin,x);
               if(x!="")
                    cout << "ERROR: " << x << endl;
          }

          for(int i=0; i<5; i++)
               cout << " | " << lines_vec[i][0] << " <= x < " << lines_vec[i][1] << " | " << lines_vec[i][2] << " | " << lines_vec[i][3] << " | " << lines_vec[i][4] << " | " << lines_vec[i][5] << " | \n";
     skip: ;
     }
     return 0;
}
