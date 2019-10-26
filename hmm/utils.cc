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

#include "utils.h"

int Utils::initialHashMapCapacity(int maxElements) {
  // Default load factor of HashMaps is 0.75
  return static_cast<int>(maxElements / 0.75) + 1;
}

template <typename S>
std::unordered_map<S, double> Utils::logToNonLogProbabilities(
    std::unordered_map<S, double> &logProbabilities) {
  const std::unordered_map<S, double> result =
      new std::unordered_map<S, double>();
  for (auto entry : logProbabilities) {
    auto rst = result.emplace(entry.first, std::exp(entry.second));
    if (!rst.second) {
      printf("ERR: logToNonLogProbabilities emplace failed, key is already exists.\n");
    }
  }
  return result;
}
bool Utils::probabilityInRange(double probability, double delta) {
  return probability >= -delta && probability <= 1.0 + delta;
}
