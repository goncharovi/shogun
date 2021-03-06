/*
 * Copyright (c) The Shogun Machine Learning Toolbox
 * Written (w) 2016 Soumyajit De
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

#include <shogun/base/ShogunEnv.h>
#include <shogun/lib/config.h>
#ifdef HAVE_OPENMP
#include <shogun/base/Parallel.h>
#include <gtest/gtest.h>
#include <omp.h>

using namespace shogun;

TEST(Parallel, openmp_get_num_threads)
{
	int32_t omp_num_threads=omp_get_num_threads();
	ASSERT_EQ(1, omp_num_threads);

	int32_t sg_num_threads = env()->get_num_threads();
#pragma omp parallel
	{
		#pragma omp master
			omp_num_threads=omp_get_num_threads();
	}

	ASSERT_EQ(sg_num_threads, omp_num_threads);
}

TEST(Parallel, openmp_set_num_threads)
{
	int32_t orig_num_threads = env()->get_num_threads();
	int32_t omp_num_threads=omp_get_num_threads();

	int32_t desired_num_threads=10;
	env()->set_num_threads(desired_num_threads);

#pragma omp parallel
	{
		#pragma omp master
			omp_num_threads=omp_get_num_threads();
	}

	ASSERT_EQ(desired_num_threads, omp_num_threads);

	env()->set_num_threads(orig_num_threads);
}
#endif // HAVE_OPENMP
