/*
 * Copyright (c) The Shogun Machine Learning Toolbox
 * Written (W) 2012 - 2013 Heiko Strathmann
 * Written (w) 2014 - 2017 Soumyajit De
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the Shogun Development Team.
 */

#include <algorithm>
#include <shogun/distance/CustomDistance.h>
#include <shogun/distance/EuclideanDistance.h>
#include <shogun/io/SGIO.h>
#include <shogun/kernel/GaussianKernel.h>
#include <shogun/kernel/Kernel.h>
#include <shogun/statistical_testing/MMD.h>
#include <shogun/statistical_testing/internals/KernelManager.h>
#include <shogun/statistical_testing/kernelselection/internals/MedianHeuristic.h>
#include <utility>
#include <vector>

using namespace shogun;
using namespace internal;

MedianHeuristic::MedianHeuristic(KernelManager& km, std::shared_ptr<MMD> est) : KernelSelection(km, std::move(est)), distance(nullptr)
{
	for (auto i=0; i<kernel_mgr.num_kernels(); ++i)
	{
		require(kernel_mgr.kernel_at(i)->get_kernel_type()==K_GAUSSIAN,
			"The underlying kernel has to be a GaussianKernel (was {})!",
			kernel_mgr.kernel_at(i)->get_name());
	}
}

MedianHeuristic::~MedianHeuristic()
{
}

void MedianHeuristic::init_measures()
{
	not_implemented(SOURCE_LOCATION);;
}

void MedianHeuristic::compute_measures()
{
	auto tmp=std::make_shared<EuclideanDistance>();
	tmp->set_disable_sqrt(false);

	distance=std::shared_ptr<CustomDistance>(estimator->compute_joint_distance(tmp));


	n=distance->get_num_vec_lhs();
	require(distance->get_num_vec_lhs()==distance->get_num_vec_rhs(),
		"Distance matrix is supposed to be a square matrix (was of dimension {}X{})!",
		distance->get_num_vec_lhs(), distance->get_num_vec_rhs());
	measures=SGVector<float64_t>((n*(n-1))/2);
	index_t write_idx=0;
	for (auto j=0; j<n; ++j)
	{
		for (auto i=j+1; i<n; ++i)
			measures[write_idx++]=distance->distance(i, j);
	}
	std::sort(measures.data(), measures.data()+measures.size());
}

SGVector<float64_t> MedianHeuristic::get_measure_vector()
{
	return measures;
}

SGMatrix<float64_t> MedianHeuristic::get_measure_matrix()
{
	require(distance!=nullptr, "Distance is not initialized!");
	return distance->get_distance_matrix();
}

std::shared_ptr<shogun::Kernel> MedianHeuristic::select_kernel()
{
	compute_measures();
	auto median_distance=measures[measures.size()/2];
	SG_DEBUG("kernel width (shogun): {}", median_distance);

	const auto num_kernels=kernel_mgr.num_kernels();
	measures=SGVector<float64_t>(num_kernels);
	for (auto i=0; i<num_kernels; ++i)
	{
		auto kernel=kernel_mgr.kernel_at(i)->as<GaussianKernel>();
		measures[i]=Math::abs(kernel->get_width()-median_distance);
	}

	auto kernel_idx=(int64_t)std::distance(measures.data(), std::min_element(measures.data(), measures.data()+measures.size()));
	SG_DEBUG("Selected kernel at {} position!", kernel_idx);
	return kernel_mgr.kernel_at(kernel_idx);
}
