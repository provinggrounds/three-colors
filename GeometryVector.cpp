/*
#include "GeometryVector.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>


double get_det(gsl_matrix * mat_ptr) 
{
	int sign=0; 
	double det=0.0; 
	int row_sq = mat_ptr->size1;
	gsl_permutation * p = gsl_permutation_calloc(row_sq);
	gsl_matrix * tmp_ptr = gsl_matrix_calloc(row_sq, row_sq);
	int * signum = &sign;
	gsl_matrix_memcpy(tmp_ptr, mat_ptr);
	gsl_linalg_LU_decomp(tmp_ptr, p, signum);
	det = gsl_linalg_LU_det(tmp_ptr, *signum);
	gsl_permutation_free(p);
	gsl_matrix_free(tmp_ptr);
	return det;
}

double Volume (const GeometryVector * vecs, DimensionType dimension)
{
	gsl_matrix * mat=gsl_matrix_alloc(dimension, dimension);
	for( ::DimensionType i=0; i<dimension; i++)
		for( ::DimensionType j=0; j<dimension; j++)
			gsl_matrix_set(mat, i, j, vecs[i].x[j]);

	double result=std::abs(get_det(mat));

	gsl_matrix_free(mat);

	return result;
}
*/