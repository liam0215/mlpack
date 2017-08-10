/**
 * @file cv_function_impl.hpp
 * @author Kirill Mishchenko
 *
 * The implementation of the class CVFunction.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_CORE_HPT_CV_FUNCTION_IMPL_HPP
#define MLPACK_CORE_HPT_CV_FUNCTION_IMPL_HPP

namespace mlpack {
namespace hpt {

template<typename CVType,
         typename MLAlgorithm,
         size_t TotalArgs,
         typename... BoundArgs>
template<size_t BoundArgIndex, size_t ParamIndex>
struct CVFunction<CVType, MLAlgorithm, TotalArgs, BoundArgs...>::UseBoundArg<
    BoundArgIndex, ParamIndex, true>
{
  using BoundArgType =
      typename std::tuple_element<BoundArgIndex, BoundArgsTupleType>::type;

  static const bool value = BoundArgType::index == BoundArgIndex + ParamIndex;
};

template<typename CVType,
         typename MLAlgorithm,
         size_t TotalArgs,
         typename... BoundArgs>
template<size_t BoundArgIndex, size_t ParamIndex>
struct CVFunction<CVType, MLAlgorithm, TotalArgs, BoundArgs...>::UseBoundArg<
    BoundArgIndex, ParamIndex, false>
{
  static const bool value = false;
};

template<typename CVType,
         typename MLAlgorithm,
         size_t TotalArgs,
         typename... BoundArgs>
CVFunction<CVType, MLAlgorithm, TotalArgs, BoundArgs...>::CVFunction(
    CVType& cv, const BoundArgs&... args) :
    cv(cv),
    boundArgs(args...),
    bestObjective(std::numeric_limits<double>::max())
{ /* Nothing left to do. */ }

template<typename CVType,
         typename MLAlgorithm,
         size_t TotalArgs,
         typename... BoundArgs>
double CVFunction<CVType, MLAlgorithm, TotalArgs, BoundArgs...>::Evaluate(
    const arma::mat& parameters)
{
  return Evaluate<0, 0>(parameters);
}

template<typename CVType,
         typename MLAlgorithm,
         size_t TotalArgs,
         typename... BoundArgs>
template<size_t BoundArgIndex,
         size_t ParamIndex,
         typename... Args,
         typename>
double CVFunction<CVType, MLAlgorithm, TotalArgs, BoundArgs...>::Evaluate(
    const arma::mat& parameters,
    const Args&... args)
{
  return PutNextArg<BoundArgIndex, ParamIndex>(parameters, args...);
}

template<typename CVType,
         typename MLAlgorithm,
         size_t TotalArgs,
         typename... BoundArgs>
template<size_t BoundArgIndex,
         size_t ParamIndex,
         typename... Args,
         typename,
         typename>
double CVFunction<CVType, MLAlgorithm, TotalArgs, BoundArgs...>::Evaluate(
    const arma::mat& /* parameters */,
    const Args&... args)
{
  double objective = cv.Evaluate(args...);

  // Change the best model if we have got a better score, or if we probably
  // have not assigned any valid (trained) model yet.
  if (bestObjective > objective ||
      bestObjective == std::numeric_limits<double>::max())
  {
    bestObjective = objective;
    bestModel = std::move(cv.Model());
  }

  return objective;
}

template<typename CVType,
         typename MLAlgorithm,
         size_t TotalArgs,
         typename... BoundArgs>
template<size_t BoundArgIndex,
         size_t ParamIndex,
         typename... Args,
         typename>
double CVFunction<CVType, MLAlgorithm, TotalArgs, BoundArgs...>::PutNextArg(
    const arma::mat& parameters,
    const Args&... args)
{
  return Evaluate<BoundArgIndex + 1, ParamIndex>(
      parameters, args..., std::get<BoundArgIndex>(boundArgs).value);
}

template<typename CVType,
         typename MLAlgorithm,
         size_t TotalArgs,
         typename... BoundArgs>
template<size_t BoundArgIndex,
         size_t ParamIndex,
         typename... Args,
         typename,
         typename>
double CVFunction<CVType, MLAlgorithm, TotalArgs, BoundArgs...>::PutNextArg(
    const arma::mat& parameters,
    const Args&... args)
{
  return Evaluate<BoundArgIndex, ParamIndex + 1>(
      parameters, args..., parameters(ParamIndex, 0));
}

} // namespace hpt
} // namespace mlpack

#endif
