/**
* Copyright (C) 2016, BMW AG
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

#ifndef UTILS_H_
#define UTILS_H_

#include <cmath>
#include <unordered_map>

class Utils {
 public:
  static int initialHashMapCapacity(int maxElements);

  template <typename S>
  static std::unordered_map<S, double> logToNonLogProbabilities(
      std::unordered_map<S, double> &logProbabilities);

  /// <summary>
  /// Note that this check must not be used for probability densities.
  /// </summary>
  static bool probabilityInRange(double probability, double delta);
};

#endif  // UTILS_H_
