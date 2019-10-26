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


#ifndef viterbi_algorithm_def_hpp
#define viterbi_algorithm_def_hpp

#include "viterbi_algorithm.h"

#include <algorithm>
#include <utility>

namespace hmm {

template <typename S, typename O, typename D>
void ViterbiAlgorithm<S, O, D>::SetKeepMessageHistory(bool keepMessageHistory) {
  if (keepMessageHistory) {
    message_history = std::vector<std::map<S, double>>();
  } else {
    message_history.clear();
  }
}
template <typename S, typename O, typename D>
bool ViterbiAlgorithm<S, O, D>::processingStarted() {
  return message.size() > 0;
}
template <typename S, typename O, typename D>
void ViterbiAlgorithm<S, O, D>::StartWithInitialStateProbabilities(
    std::vector<S>& initialStates,
    std::map<S, double>& initialLogProbabilities) {
  initializeStateProbabilities(nullptr, initialStates, initialLogProbabilities);
}
template <typename S, typename O, typename D>
void ViterbiAlgorithm<S, O, D>::StartWithInitialObservation(
    O observation, std::vector<S>& candidates,
    std::map<S, double>& emissionLogProbabilities) {
  InitializeStateProbabilities(observation, candidates,
                               emissionLogProbabilities);
}
template <typename S, typename O, typename D>
void ViterbiAlgorithm<S, O, D>::NextStep(
    O observation, std::vector<S>& candidates,
    std::map<S, double>& emissionLogProbabilities,
    std::map<Transition<S>, double>& transitionLogProbabilities,
    std::map<Transition<S>, D>& transitionDescriptors) {
  if (is_broken) {
    return;
  }
  // Forward step
  ForwardStepResult<S, O, D> forwardStepResult =
      ForwardStep(observation, prevCandidates, candidates, message,
                  emissionLogProbabilities, transitionLogProbabilities,
                  transitionDescriptors);
  is_broken = HMMBreak(forwardStepResult.newMessage);
  if (is_broken) {
    return;
  }
  message_history.push_back(forwardStepResult.newMessage);
  message = forwardStepResult.newMessage;
  lastExtendedStates = forwardStepResult.newExtendedStates;
  prevCandidates = std::vector<S>(candidates);  // Defensive copy.
}
template <typename S, typename O, typename D>
void ViterbiAlgorithm<S, O, D>::NextStep(
    O observation, std::vector<S>& candidates,
    std::map<S, double>& emissionLogProbabilities,
    std::map<Transition<S>, double>& transitionLogProbabilities) {
  std::map<Transition<S>, D> tempVar;
  NextStep(observation, candidates, emissionLogProbabilities,
           transitionLogProbabilities, tempVar);
}
template <typename S, typename O, typename D>
std::vector<SequenceState<S, O, D>>
ViterbiAlgorithm<S, O, D>::ComputeMostLikelySequence() {
  if (message.empty()) {
    // Return empty most likely sequence if there are no time steps or if
    // initial observations caused an HMM break.
    return std::vector<SequenceState<S, O, D>>();
  } else {
    return RetrieveMostLikelySequence();
  }
}
template <typename S, typename O, typename D>
bool ViterbiAlgorithm<S, O, D>::IsBroken() {
  return is_broken;
}
template <typename S, typename O, typename D>
std::vector<std::map<S, double>> ViterbiAlgorithm<S, O, D>::MessageHistory() {
  return message_history;
}
template <typename S, typename O, typename D>
std::string ViterbiAlgorithm<S, O, D>::MessageHistoryString() {
  if (message_history.empty()) {
    return "";
  }
  std::string sb;
  sb.append("Message history with log probabilies\n\n");
  int i = 0;
  for (auto message : message_history) {
    sb.append("Time step " + std::to_string(i) + "\n");
    i++;
    for (auto state : message) {
      sb.append(state.first + L": " + message.get(state.first) + L"\n");
    }
    sb.append("\n");
  }
  return sb;
}
template <typename S, typename O, typename D>
bool ViterbiAlgorithm<S, O, D>::HMMBreak(const std::map<S, double>& message) {
  for (auto logProbability : message) {
    if (logProbability.second != -std::numeric_limits<double>::infinity()) {
      return false;
    }
  }
  return true;
}
template <typename S, typename O, typename D>
void ViterbiAlgorithm<S, O, D>::InitializeStateProbabilities(
    O observation, std::vector<S>& candidates,
    std::map<S, double>& initialLogProbabilities) {
  if (!message.empty()) {
    return;
  }

  // Set initial log probability for each start state candidate based on first
  // observation. Do not assign initialLogProbabilities directly to message to
  // not rely on its iteration order.
  std::map<S, double> initialMessage;
  for (auto candidate : candidates) {
    auto search = initialLogProbabilities.find(candidate);
    if (search == initialLogProbabilities.end()) {
      printf("ERR: No initial probability for a candidate\n");
      return;
    }
    const double logProbability = initialLogProbabilities[candidate];
    auto rst = initialMessage.emplace(candidate, logProbability);
    if (!rst.second) {
      printf("ERR: initialMessage emplace failed, key is already exists.\n");
    }
  }
  is_broken = HMMBreak(initialMessage);
  if (is_broken) {
    printf("ERR: HMM Break\n");
    return;
  }
  message = initialMessage;
  message_history.push_back(message);
  // lastExtendedStates = new std::map<S, ExtendedState<S, O, D>*>();
  for (auto candidate : candidates) {
    auto tempVar =
        new ExtendedState<S, O, D>(candidate, nullptr, observation, D());
    auto rst = lastExtendedStates.emplace(candidate, tempVar);
    if (!rst.second) {
      printf("ERR: lastExtendedStates emplace failed, key is already exists.\n");
    }
  }
  prevCandidates = std::vector<S>(candidates);  // Defensive copy.
}
template <typename S, typename O, typename D>
ForwardStepResult<S, O, D> ViterbiAlgorithm<S, O, D>::ForwardStep(
    O observation, std::vector<S>& prevCandidates,
    std::vector<S>& curCandidates, std::map<S, double>& message,
    std::map<S, double>& emissionLogProbabilities,
    std::map<Transition<S>, double>& transitionLogProbabilities,
    std::map<Transition<S>, D>& transitionDescriptors) {
  ForwardStepResult<S, O, D>* result =
      new ForwardStepResult<S, O, D>((int)curCandidates.size());

  for (auto curState : curCandidates) {
    //double maxLogProbability = -1000;//-std::numeric_limits<double>::infinity();
    double maxLogProbability = -std::numeric_limits<double>::infinity();
    S* maxPrevState = nullptr;
    for (auto& prevState : prevCandidates) {
      double logProbability =
          message[prevState] +
          TransitionLogProbability(prevState, curState,
                                   transitionLogProbabilities);
      if (logProbability > maxLogProbability) {
        maxLogProbability = logProbability;
        maxPrevState = &prevState;
      }
    }
    // Throws NullPointerException if curState is not stored in the map.
    //printf("gpsmsmt maxLogProbability: %f\n", maxLogProbability);
    auto rst = result->newMessage.emplace(curState,
        maxLogProbability + emissionLogProbabilities[curState]);
    if (!rst.second) {
      printf("ERR: ForwardStep newMessage emplace failed, key is already exists.\n");
    }
    // Note that maxPrevState == null if there is no transition with non-zero
    // probability. In this case curState has zero probability and will not be
    // part of the most likely sequence, so we don't need an ExtendedState.
    if (maxPrevState != nullptr) {
      Transition<S>* const transition =
          new Transition<S>(*maxPrevState, curState);
      ExtendedState<S, O, D>* const extendedState = new ExtendedState<S, O, D>(
          curState, lastExtendedStates[*maxPrevState], observation,
          transitionDescriptors[*transition]);
      auto rst = result->newExtendedStates.emplace(curState, extendedState);
      if (!rst.second) {
        printf("ERR: ForwardStep newExtendedStates emplace failed, key is already exists.\n");
      }
    }
  }
  return *result;
}
template <typename S, typename O, typename D>
double ViterbiAlgorithm<S, O, D>::TransitionLogProbability(
    S prevState, S curState,
    std::map<Transition<S>, double>& transitionLogProbabilities) {
  auto key = Transition<S>(prevState, curState);
  auto found = transitionLogProbabilities.find(key);
  if (found == transitionLogProbabilities.end()) {
    // Transition has zero probability.
    return -std::numeric_limits<double>::infinity();
  }
  return transitionLogProbabilities[key];
}
template <typename S, typename O, typename D>
S ViterbiAlgorithm<S, O, D>::MostLikelyState() {
  // Otherwise an HMM break would have occurred and message would be null.
  if (message.empty()) {
    printf("ERR: message is empty. MostLikelyState()\n");
    return S();
  }

  S result;
  const double kErrorDouble = -std::numeric_limits<double>::infinity();
  double maxLogProbability = kErrorDouble;
  for (auto entry : message) {
    if (entry.second > maxLogProbability) {
      result = entry.first;
      maxLogProbability = entry.second;
    }
  }
  // Otherwise an HMM break would have occurred.
  if (maxLogProbability == kErrorDouble) {
    printf("ERR: result is empty. MostLikelyState()\n");
    return S();
  }
  return result;
}
template <typename S, typename O, typename D>
std::vector<SequenceState<S, O, D>>
ViterbiAlgorithm<S, O, D>::RetrieveMostLikelySequence() {
  // Otherwise an HMM break would have occurred and message would be null.
  if (message.empty()) {
    printf("ERR: message is empty. RetrieveMostLikelySequence()\n");
    return std::vector<SequenceState<S, O, D>>();
  }
  S lastState = MostLikelyState();
  // Retrieve most likely state sequence in reverse order
  std::vector<SequenceState<S, O, D>> result;
  ExtendedState<S, O, D>* es = lastExtendedStates[lastState];
  while (es != nullptr) {
    SequenceState<S, O, D> ss(es->state, es->observation,
                              es->transitionDescriptor);
    result.push_back(ss);
    if (es->backPointer == nullptr) {
      es = nullptr;
    } else {
      es = es->backPointer;
    }
  }
  // std::reverse(result.begin(), result.end());
  std::vector<SequenceState<S, O, D>> real_result;
  for (auto i = result.rbegin(); i < result.rend(); ++i) {
    real_result.push_back(*i);
  }

  return real_result;
}

}  // namespace hmm

#endif /* viterbi_algorithm_def_hpp */
