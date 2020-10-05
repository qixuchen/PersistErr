//#include "stdafx.h"

#include "operation.h"

// is x equal to 0 ?
int isZero(double x)
{
	return x > -EQN_EPS && x < EQN_EPS;
}

// compute L1 distance
DIST_TYPE calc_l1_dist( point_t* point_v1, point_t* point_v2)
{
	int i, dim;
	DIST_TYPE diff;

	diff = 0;
	
	dim = point_v1->dim;
	for( i=0; i<dim; i++)
	{
		diff += ( DIST_TYPE) abs( point_v1->coord[ i] - point_v2->coord[ i]);
	}

	return diff;
}

/*
 *	Calculate the norm of a given vector
 */
DIST_TYPE calc_len(point_t* point_v)
{
	int i, dim;
	DIST_TYPE diff;

	diff = 0;
	
	dim = point_v->dim;
	for( i=0; i<dim; i++)
	{
		diff += point_v->coord[i] * point_v->coord[i];
	}

	return ( DIST_TYPE)sqrt( diff);

}


/*
 *  Create a new point give the target point
 *  (useful when some point need to be freed)
 */
point_t* copy(point_t * point_v2)
{
	if(point_v2 == NULL)
		return NULL;

	point_t* point_v1 = alloc_point(point_v2->dim);

	for(int i = 0; i< point_v2->dim; i++)
		point_v1->coord[i] = point_v2->coord[i];

	return point_v1;
}

/*
 *	Calculate the dot product between two points
 */
double dot_prod(point_t* point_v1, point_t* point_v2)
{
	int dim = point_v1->dim;
	double result = 0;

	int i;
	for(i = 0; i < dim; i++)
	{
		result += point_v1->coord[i]*point_v2->coord[i];
	}
	return result;
}

/*
 *	Calculate the dot product between two points
 */
double dot_prod(point_t* point_v1, double* v)
{
	int dim = point_v1->dim;
	double result = 0;

	int i;
	for(i = 0; i < dim; i++)
	{
		result += point_v1->coord[i]*v[i];
	}
	return result;
}

/*
 *	Calculate the dot product between two points
 */
double dot_prod(double* v1, double* v2, int dim)
{
	double result = 0;
	int i;
	for (i = 0; i < dim; i++)
	{
		result += v1[i] * v2[i];
	}
	return result;
}

/*
 *	Calculate the subtraction between two points.
 *  Remember to release the returned point to save memory.
 */
point_t* sub(point_t* point_v1, point_t* point_v2)
{
	point_t* result = alloc_point(point_v1->dim);

	int i;
	for(i = 0; i < point_v1->dim; i++)
	{
		result->coord[i] = point_v1->coord[i] - point_v2->coord[i];
	}
	return result;
}

/*
 *	Calculate the addition between two points.
 *  Remember to release the returned point to save memory.
 */
point_t* add(point_t* point_v1, point_t* point_v2)
{
	point_t* result = alloc_point(point_v1->dim);

	int i;
	for(i = 0; i < point_v1->dim; i++)
	{
		result->coord[i] = point_v1->coord[i] + point_v2->coord[i];
	}
	return result;
}

/*
 *	Scale the given point
 *  Remember to release the returned point to save memory.
 */
point_t* scale(double c, point_t* point_v)
{
	point_t* result = alloc_point(point_v->dim);

	int i;
	for(i = 0; i < point_v->dim; i++)
	{
		result->coord[i] = point_v->coord[i] * c;
	}

	return result;
}

// compute the rank of the guass matrix (used for computing frames)
int guassRank(vector< point_t* > P)
{
	int dim = P[0]->dim;

	std::vector<vector<double> > A;
	for(int i = 0; i < P.size(); i++)
	{
		vector<double> v;
		for(int j = 0; j < dim; j++)
		{
			v.push_back(P[i]->coord[j]);
		}
		A.push_back(v);
	}
	while(A.size() < dim)
	{
		vector<double> v;
		for(int j = 0; j < dim; j++)
		{
			v.push_back(0);
		}
		A.push_back(v);

	}
	
	int n = A.size();
	int d = A[0].size();

	//for (int i = 0; i < n; i++)
	//{
	//	for (int j = 0; j < d; j++)
	//		printf("%lf ", A[i][j]);
	//	printf("\n");
	//}
	//printf("\n");

	for (int i = 0; i< d; i++) {
		// Search for maximum in this column
		double maxEl = abs(A[i][i]);
		int maxRow = i;
		for (int k = i + 1; k<n; k++) {
			if (abs(A[k][i]) > maxEl) {
				maxEl = abs(A[k][i]);
				maxRow = k;
			}
		}

		// Swap maximum row with current row (column by column)
		for (int k = i; k<d; k++) {
			double tmp = A[maxRow][k];
			A[maxRow][k] = A[i][k];
			A[i][k] = tmp;
		}

		// Make all rows below this one 0 in current column
		for (int k = i + 1; k<n; k++) {
			double c = -A[k][i] / A[i][i];
			for (int j = i; j<d; j++) {
				if (i == j) {
					A[k][j] = 0;
				}
				else {
					A[k][j] += c * A[i][j];
				}
			}
		}
	}

	int count = 0;
	for (int i = 0; i < n; i++)
	{
		bool allZero = true;
		for (int j = 0; j < d; j++)
		{
			if (!isZero(A[i][j]))
			{
				allZero = false;
				break;
			}
		}

		if (!allZero)
			count++;
	}
	return count;
}

// check if two vectors are linearly independent (used for computing frames)
bool linearInd(point_t* p, point_t* q)
{
	int dim = p->dim;

	bool pZero = true, qZero = true;
	double ratio;
	for(int i = 0; i < dim; i++)
	{
		if(!isZero(q->coord[i]))
		{
			qZero = false;
			ratio = p->coord[i] / q->coord[i];
		}
		if(!isZero(p->coord[i]))
			pZero = false;
	}

	if(pZero || qZero)
		return true;

	for(int i = 0; i < dim; i++)
	{
		if(!isZero(ratio - p->coord[i] / q->coord[i]))
			return true;
	}

	return false;
}

// compute the intersection betwee a hyperplane and a ray shooting in s
double compute_intersection_len(hyperplane_t *hp, point_t* s)
{
	return calc_len(s) * (hp->offset / (dot_prod(hp->normal, s)));
}

// get the pointer of point in P
point_t* getPoint(point_set_t* p, double* point)
{
	int dim = p->points[0]->dim;

	for(int i = 0; i < p->numberOfPoints; i++)
	{
		bool match = true;
		for(int j = 0; j < dim; j++)
		{
			if(!isZero(p->points[i]->coord[j] - point[j]))
			{
				match = false;
				break;
			
			}
		}
		if(match)
			return p->points[i];
	}
	return NULL;
}
