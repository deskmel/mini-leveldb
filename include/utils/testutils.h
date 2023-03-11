#ifndef _TESTUTILS_H_
#define _TESTUTILS_H_

#include <string>
#include "gtest/gtest.h"

// inline int RandomSeed(){
//   return testing::UnitTest::GetInstance()->random_seed();
// }

std::string RandomKey(int len);

std::string RandomString(int len,std::string* dst);

#endif