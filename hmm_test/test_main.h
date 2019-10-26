/**
* Copyright (C) 2015~2016, BMW AG
* Author: Stefan Holder (stefan.holder@bmw.de)
* C++ Conversion: Junho Han (junhohan.kr@gmail.com)
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef TEST_MAIN_H_
#define TEST_MAIN_H_

#include <iostream>
#include <map>
#include <vector>
#include "rain.h"

namespace hmm {

class TestMain {
 public:
  void TestComputeMostLikelySequence();
  void TestDeterministicCandidateOrder();
  void TestEmptySequence();
  void TestBreakAtInitialMessage();
  void TestEmptyInitialMessage();
  void TestBreakAtFirstTransition();
  void TestBreakAtFirstTransitionWithNoCandidates();
  void TestBreakAtSecondTransition();
  void CheckMessageHistory(
      std::vector<std::map<Rain, double>> expectedMessageHistory,
      std::vector<std::map<Rain, double>> actualMessageHistory);
  void CheckMessage(std::map<Rain, double> expectedMessage,
                    std::map<Rain, double> actualMessage);
};

}  // namespace hmm
#endif  // TEST_MAIN_H_
