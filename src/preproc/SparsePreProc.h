//#ifndef _CSPARSEPREPROC__H__
#define _CSPARSEPREPROC__H__

#include "features/SparseFeatures.h"
#include "lib/common.h"
#include "preproc/PreProc.h"

#include <stdio.h>


template <class ST> class CSparsePreProc : public CPreProc
{
public:
	CSparsePreProc(const char *name, const char* id) : CPreProc(name,id)
	{
		CSparseFeatures<ST>::TSparse* a;
	}

	//public typedef typename CSparseFeatures<ST>::TSparse TSparse_t;
	/// apply preproc on feature matrix
	/// result in feature matrix
	/// return pointer to feature_matrix, i.e. f->get_feature_matrix();
	//virtual CSparseFeatures<ST>::TSparse* apply_to_sparse_feature_matrix(CSparseFeatures* f)=0;
	//virtual CSparseFeatures<ST>::TSparse apply_to_sparse_feature_matrix(CSparseFeatures* f)=0;

	/// apply preproc on single feature vector
	/// result in feature matrix

	//virtual CSparseFeatures<ST>::TSparse* apply_to_sparse_feature_vector(CSparseFeatures<ST>::TSparse* f, int &len)=0;
};
#endif
