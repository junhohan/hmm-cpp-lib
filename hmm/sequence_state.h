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

/**
 * State of the most likely sequence with additional information.
 *
 * @param <S> the state type
 * @param <O> the observation type
 * @param <D> the transition descriptor type
 */

#ifndef SEQUENCE_STATE_H_
#define SEQUENCE_STATE_H_

#include <iostream>

namespace hmm {

template <typename S, typename O, typename D>
class SequenceState;
template <typename S, typename O, typename D>
bool operator==(const SequenceState<S, O, D>& lhs,
                const SequenceState<S, O, D>& rhs);

template <typename S, typename O, typename D>
class SequenceState {
 public:
  S state;
  /**
   * Null if HMM was started with initial state probabilities and state is the
   * initial state.
   */
  O observation;
  /**
   * Null if transition descriptor was not provided.
   */
  D transitionDescriptor;

 public:
  SequenceState(S state, O observation, D transitionDescriptor);
  friend bool operator==<>(const SequenceState& lhs, const SequenceState& rhs);
  SequenceState<S, O, D>& operator=(SequenceState<S, O, D>& rhs);

  std::string ToString() {
    return "SequenceState [state=" + state + ", observation=" + observation +
           ", transitionDescriptor=" + transitionDescriptor + "]";
  }
};

template <typename S, typename O, typename D>
SequenceState<S, O, D>& SequenceState<S, O, D>::operator=(
    SequenceState<S, O, D>& rhs) {
  state = rhs.state;
  observation = rhs.observation;
  transitionDescriptor = rhs.transitionDescriptor;
  return *this;
}

template <typename S, typename O, typename D>
SequenceState<S, O, D>::SequenceState(S state, O observation,
                                      D transitionDescriptor)
    : state(state),
      observation(observation),
      transitionDescriptor(transitionDescriptor) {}
template <typename S, typename O, typename D>
bool operator==(const SequenceState<S, O, D>& lhs,
                const SequenceState<S, O, D>& rhs) {
  return lhs.state == rhs.state && lhs.observation == rhs.observation &&
         lhs.transitionDescriptor == rhs.transitionDescriptor;
}

}  //  namespace hmm

#endif  // SEQUENCE_STATE_H_
