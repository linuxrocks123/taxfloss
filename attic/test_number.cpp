#include "Number.hpp"
#include <iostream>

using namespace std;

int main()
{
    Number x;
    cin >> x;
    Number y;
    cin >> y;
    cout << x+y << endl;
    cout << x-y << endl;
    cout << x*y << endl;
    Number z = x/y;
    z.round(2);
    cout << z << endl;
    return 0;
}
