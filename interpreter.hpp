#pragma once

#include "Declaration.hpp"

#include <string>
#include <map>

extern std::map<std::string,std::list<Declaration>*> ast_roots;

//Return values are error messages
std::string parse_and_check(char** filenames);
std::string execute();
