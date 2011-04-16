/**
 * \filter first_order_filter.h
 */

#ifndef FIRST_ORDER_FILTER
#define FIRST_ORDER_FILTER

#include <boost/math/constants/constants.hpp>

#include "config.h"

namespace DSP
{

/**
 * A first order allpass filter
 */
template<class Data_Type>
class AllPassFilter
{
public:
  typedef Data_Type DataType;
private:
  DataType buffer_in;

  DataType sampling_frequency;
  DataType cut_frequency;
  DataType c;

  void compute_coeffs()
  {
    c = (std::tan(boost::math::constants::pi<DataType>() * cut_frequency / sampling_frequency) - 1) / (std::tan(boost::math::constants::pi<DataType>() * cut_frequency / sampling_frequency) + 1);
  }

public:
  AllPassFilter()
  :buffer_in(0)
  {
  }

  void set_sampling_frequency(DataType sampling_frequency)
  {
    this->sampling_frequency = sampling_frequency;
    compute_coeffs();
  }

  void set_cut_frequency(DataType cut_frequency)
  {
    this->cut_frequency = cut_frequency;
    compute_coeffs();
  }

  template<class DataTypeIn>
  void process(const DataTypeIn* RESTRICT in, DataType* RESTRICT out, long size)
  {
    for(int i = 0; i < size; ++i)
    {
      DataType xh = in[i] - c * buffer_in;
      out[i] = c * xh + buffer_in;
      buffer_in = xh;
    }
  }
};

/**
 * A first order lowpass filter
 */
template<class Data_Type>
class LowPassFilter
{
public:
  typedef Data_Type DataType;
private:
  AllPassFilter<DataType> all_pass_filter;

public:
  void set_sampling_frequency(DataType sampling_frequency)
  {
    all_pass_filter.set_sampling_frequency(sampling_frequency);
  }

  void set_cut_frequency(DataType cut_frequency)
  {
    all_pass_filter.set_cut_frequency(cut_frequency);
  }

  template<class DataTypeIn>
  void process(const DataTypeIn* RESTRICT in, DataType* RESTRICT out, long size)
  {
    all_pass_filter.process(in, out, size);

    for(int i = 0; i < size; ++i)
    {
      out[i] = (out[i] + in[i]) / 2;
    }
  }
};

template<class Data_Type>
class HighPassFilter
{
public:
  typedef Data_Type DataType;
private:
  AllPassFilter<DataType> all_pass_filter;

public:
  void set_sampling_frequency(DataType sampling_frequency)
  {
    all_pass_filter.set_sampling_frequency(sampling_frequency);
  }

  void set_cut_frequency(DataType cut_frequency)
  {
    all_pass_filter.set_cut_frequency(cut_frequency);
  }

  template<class DataTypeIn>
  void process(const DataTypeIn* RESTRICT in, DataType* RESTRICT out, long size)
  {
    all_pass_filter.process(in, out, size);

    for(int i = 0; i < size; ++i)
    {
      out[i] = (out[i] - in[i]) / 2;
    }
  }
};
}

#endif
