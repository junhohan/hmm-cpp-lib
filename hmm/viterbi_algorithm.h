//
//  ViterbiAlgorithm.hpp
//  duo
//
//  Created by Junho Han on 2017. 7. 5..
//  Copyright © 2017년 Duo. All rights reserved.
//

#ifndef VITERBI_ALGORITHM_H_
#define VITERBI_ALGORITHM_H_

#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>
#include "sequence_state.h"
#include "transition.h"
#include "utils.h"

/**
 * Implementation of the Viterbi algorithm for time-inhomogeneous Markov
 * processes, meaning that the set of states and state transition probabilities
 * are not necessarily fixed for all time steps. The plain Viterbi algorithm for
 * stationary Markov processes is described e.g. in Rabiner, Juang, An
 * introduction to Hidden Markov Models, IEEE ASSP Mag., pp 4-16, June 1986.
 *
 * <p>Generally expects logarithmic probabilities as input to prevent arithmetic
 * underflows for small probability values.
 *
 * <p>This algorithm supports storing transition objects in
 * {@link #nextStep(Object, std::vector, Map, Map, Map)}. For instance if a HMM
 * is used for map matching, this could be routes between road position
 * candidates. The transition descriptors of the most likely sequence can be
 * retrieved later in
 * {@link SequenceState#transitionDescriptor} and hence do not need to be stored
 * by the caller. Since the caller does not know in advance which transitions
 * will occur in the most likely sequence, this reduces the number of
 * transitions that need to be kept in memory from t*n² to t*n since only one
 * transition descriptor is stored per back pointer, where t is the number of
 * time steps and n the number of candidates per time step.
 *
 * <p>For long observation sequences, back pointers usually converge to a single
 * path after a certain number of time steps. For instance, when matching GPS
 * coordinates to roads, the last GPS positions in the trace usually do not
 * affect the first road matches anymore. This implementation exploits this fact
 * by letting the Java garbage collector take care of unreachable back pointers.
 * If back pointers converge to a single path after a constant number of time
 * steps, only O(t) back pointers and transition descriptors need to be stored
 * in memory.
 *
 * @param <S> the state type
 * @param <O> the observation type
 * @param <D> the transition descriptor type. Pass {@link Object} if transition
 * descriptors are not needed.
 */

namespace hmm {
/**
 * Stores addition information for each candidate.
 */
template <typename S, typename O, typename D>
class ExtendedState;
template <typename S, typename O, typename D>
bool operator==(const ExtendedState<S, O, D> &lhs,
                const ExtendedState<S, O, D> &rhs);

template <typename S, typename O, typename D>
class ExtendedState {
 public:
  friend bool operator==
      <>(const ExtendedState<S, O, D> &lhs, const ExtendedState<S, O, D> &rhs);
  S state;
  // Back pointer to previous state candidate in the most likely sequence.
  // Back pointers are chained using plain Java references.
  // This allows garbage std::vector of unreachable back pointers.
  ExtendedState<S, O, D> *backPointer = nullptr;
  O observation;
  D transitionDescriptor;

  ExtendedState(S state, ExtendedState<S, O, D> *backPointer, O observation,
                D transitionDescriptor)
      : state(state),
        backPointer(backPointer),
        observation(observation),
        transitionDescriptor(transitionDescriptor) {
    //    printf("backPointer=%p\n", backPointer);
  }
  ~ExtendedState() {
    if (backPointer != nullptr) {
      delete backPointer;
    }
  }
};

template <typename S, typename O, typename D>
bool operator==(const ExtendedState<S, O, D> &lhs,
                const ExtendedState<S, O, D> &rhs) {
  return (lhs.mValue == rhs.mValue);
}

template <typename S, typename O, typename D>
class ForwardStepResult {
 public:
  std::map<S, double> newMessage;
  // Includes back pointers to previous state candidates for retrieving the most
  // likely sequence after the forward pass.
  std::map<S, ExtendedState<S, O, D> *> newExtendedStates;
  ForwardStepResult(int numberStates) {
    //    printf("??????\n");
  }
};

template <typename S, typename O, typename D>
class ViterbiAlgorithm {
 private:
  // Allows to retrieve the most likely sequence using back pointers.
  std::map<S, ExtendedState<S, O, D> *> lastExtendedStates;
  std::vector<S> prevCandidates;

  // For each state s_t of the current time step t, message.get(s_t) contains
  // the log probability of the most likely sequence ending in state s_t with
  // given observations o_1, ..., o_t.
  //
  // Formally, this is max log p(s_1, ..., s_t, o_1, ..., o_t) w.r.t. s_1, ...,
  // s_{t-1}. Note that to compute the most likely state sequence, it is
  // sufficient and more efficient to compute in each time step the joint
  // probability of states and observations instead of computing the conditional
  // probability of states given the observations.
  std::map<S, double> message;
  bool is_broken = false;
  // ForwardBackwardAlgorithm<S, O> *forwardBackward;
  std::vector<std::map<S, double>> message_history;  // For debugging only.

  /// Need to construct a new instance for each sequence of observations.
 public:
  ViterbiAlgorithm() {}
  ~ViterbiAlgorithm() {}
  // Whether to store intermediate forward messages
  // (probabilities of intermediate most likely paths) for debugging.
  // Default: false
  // Must be called before processing is started.
  void SetKeepMessageHistory(bool keepMessageHistory);
  bool processingStarted();
  // Lets the HMM computation start with the given initial state probabilities.
  void StartWithInitialStateProbabilities(
      std::vector<S> &initialStates,
      std::map<S, double> &initialLogProbabilities);
  // Lets the HMM computation start at the given first observation and uses the
  // given emission probabilities as the initial state probability for each
  // starting state s.
  void StartWithInitialObservation(
      O observation, std::vector<S> &candidates,
      std::map<S, double> &emissionLogProbabilities);
  // Processes the next time step. Must not be called if the HMM is broken.
  void NextStep(O observation, std::vector<S> &candidates,
                std::map<S, double> &emissionLogProbabilities,
                std::map<Transition<S>, double> &transitionLogProbabilities,
                std::map<Transition<S>, D> &transitionDescriptors);
  // See nextStep(Object, std::vector, Map, Map, Map)
  void NextStep(O observation, std::vector<S> &candidates,
                std::map<S, double> &emissionLogProbabilities,
                std::map<Transition<S>, double> &transitionLogProbabilities);
  // Returns the most likely sequence of states for all time steps. This
  // includes the initial states / initial observation time step. If an HMM
  // break occurred in the last time step t, then the most likely sequence up to
  // t-1 is returned. See also <seealso cref="#isBroken()"/>.
  //
  // <para>Formally, the most likely sequence is argmax p([s_0,] s_1, ..., s_T |
  // o_1, ..., o_T) with respect to s_1, ..., s_T, where s_t is a state
  // candidate at time step t, o_t is the observation at time step t and T is
  // the number of time steps.
  std::vector<SequenceState<S, O, D>> ComputeMostLikelySequence();
  // Returns whether an HMM occurred in the last time step.
  // An HMM break means that the probability of all states equals zero.
  bool IsBroken();
  //  Returns the sequence of intermediate forward messages for each time step.
  //  Returns null if message history is not kept.
  std::vector<std::map<S, double>> MessageHistory();
  std::string MessageHistoryString();
  // Returns whether the specified message is either empty or only contains
  // state candidates with zero probability and thus causes the HMM to break.
 private:
  bool HMMBreak(const std::map<S, double> &message);
  // Use only if HMM only starts with first observation.
  void InitializeStateProbabilities(
      O observation, std::vector<S> &candidates,
      std::map<S, double> &initialLogProbabilities);
  /// Computes the new forward message and the back pointers to the previous
  /// states.
  ForwardStepResult<S, O, D> ForwardStep(
      O observation, std::vector<S> &prevCandidates,
      std::vector<S> &curCandidates, std::map<S, double> &message,
      std::map<S, double> &emissionLogProbabilities,
      std::map<Transition<S>, double> &transitionLogProbabilities,
      std::map<Transition<S>, D> &transitionDescriptors);

  double TransitionLogProbability(
      S prevState, S curState,
      std::map<Transition<S>, double> &transitionLogProbabilities);
  // Retrieves the first state of the current forward message with maximum
  // probability.
  S MostLikelyState();  // Retrieves most likely sequence from the internal back
                        // pointer sequence.
  std::vector<SequenceState<S, O, D>> RetrieveMostLikelySequence();
};

}  // namespace hmm

#include "viterbi_algorithm_def.h"
#endif  // VITERBI_ALGORITHM_H_
