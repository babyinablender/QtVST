/**
 * \file main.cpp
 */

#include <fstream>

#include <boost/math/constants/constants.hpp>

#include "newton_raphson_optimizer.h"
#include "simple_overdrive.h"
#include "oversampling_filter.h"
#include "first_order_filter.h"
#include "second_order_filter.h"
#include "decimation_filter.h"

const unsigned long size = 2000000;
const unsigned int sample_rate = 96000;
const unsigned int max_frequency = 20000;
double in[size];
double out[size];

const int oversampling = 8;

double in_oversampled[oversampling * size];
double out_oversampled[oversampling * size];

int main(int argc, char** argv)
{
  DSP::OversamplingFilter<oversampling, DSP::Oversampling6points5order<double>, double> oversampling_filter;

  DSP::SimpleOverdrive<double> overdrive(1./sample_rate / oversampling, 10000, 22e-9, 1e-12, 26e-3);
  DSP::NewtonRaphsonOptimizer<DSP::SimpleOverdrive<double> > filter(overdrive);

//  DSP::DecimationFilter<DSP::LowPassFilter<double>, double> low_filter;
  DSP::SecondOrderFilter<DSP::LowPassCoefficients<double>, double> low_filter;
  DSP::DecimationFilter<oversampling, DSP::SecondOrderFilter<DSP::LowPassCoefficients<double>, double>, double> decimation_low_filter;

  for(int i = 0; i < size; ++i)
  {
    double j = static_cast<double>(i) / sample_rate;
    in[i] = 20 * std::sin(boost::math::constants::pi<double>() * max_frequency * (1. / 1000 + 999. * i / (size * 1000)) * j);
  }

  low_filter.set_sampling_frequency(sample_rate * oversampling);
  low_filter.set_cut_frequency(max_frequency);
  decimation_low_filter.get_filter().set_sampling_frequency(sample_rate * oversampling);
  decimation_low_filter.get_filter().set_cut_frequency(max_frequency);

  oversampling_filter.process(in, in_oversampled, size);
  filter.process(in_oversampled, out_oversampled, oversampling * size);
  low_filter.process(out_oversampled, in_oversampled, oversampling * size);
  decimation_low_filter.process(in_oversampled, out, oversampling * size);

  std::ofstream infile("in_overdrive.raw", std::ofstream::binary);
  infile.write(reinterpret_cast<const char*>(in), size * sizeof(double));

  std::ofstream outfile("out_overdrive.raw", std::ofstream::binary);
  outfile.write(reinterpret_cast<const char*>(out), size * sizeof(double));
  
  return 0;
}
