#include "read_write.h"

// read points from the input file
point_set_t* read_points(char* input)
{
	FILE* c_fp;

	char filename[MAX_FILENAME_LENG];
	sprintf(filename, "../input/%s", input);
	printf("%s\n", filename);
	if ((c_fp = fopen(filename, "r")) == NULL)
	{
		fprintf(stderr, "Cannot open the data file %s.\n", filename);
		exit(0);
	}

	int number_of_points, dim;
	fscanf(c_fp, "%i%i", &number_of_points, &dim);

	point_set_t* point_set = alloc_point_set(number_of_points);

	// read points line by line
	for (int i = 0; i < number_of_points; i++)
	{
		point_t* p = alloc_point(dim, i);
		for (int j = 0; j < dim; j++)
		{
			fscanf(c_fp, "%lf", &p->coord[j]);
		}
		point_set->points[i] = p;
	}

	fclose(c_fp);
	return point_set;
}

// check domination
int dominates(point_t* p1, point_t* p2)
{
	for (int i = 0; i < p1->dim; i++)
        if (p1->coord[i] < p2->coord[i])
            return 0;

	return 1;
}

int dominates_same(point_t* p1, point_t* p2)
{
    int d_num = 0;
    for (int i = 0; i < p1->dim; i++)
    {
        if (p1->coord[i] < p2->coord[i])
        {
            return 0;
        }
        if (p1->coord[i] == p2->coord[i])
        {
            d_num++;
        }
    }
    if(d_num == p1->dim)
    {
        return 0;
    }
    return 1;
}

// compute the skyline set
point_set_t* skyline_point(point_set_t *p)
{
	int i, j, dominated, index = 0, m;
	point_t* pt;

	int* sl = new int[p->numberOfPoints];

	for (i = 0; i < p->numberOfPoints; ++i)
	{
		dominated = 0;
		pt = p->points[i];

		// check if pt is dominated by the skyline so far   
		for (j = 0; j < index && !dominated; ++j)
			if (dominates(p->points[sl[j]], pt))
				dominated = 1;

		if (!dominated)
		{
			// eliminate any points in current skyline that it dominates
			m = index;
			index = 0;
			for (j = 0; j < m; ++j)
				if (!dominates(pt, p->points[sl[j]]))
					sl[index++] = sl[j];

			// add this point as well
			sl[index++] = i;
		}
	}

	point_set_t* skyline = alloc_point_set(index);
	for (int i = 0; i < index; i++)
		skyline->points[i] = p->points[sl[i]];

	delete[] sl;
	return skyline;
}

// prepare the file for computing the convex hull (the candidate utility range R) via half space interaction
void write_hyperplanes(vector<hyperplane_t*> utility_hyperplane, point_t* feasible_pt, char* filename)
{
	//char filename[MAX_FILENAME_LENG];
	int dim = feasible_pt->dim;

	FILE *wPtr = NULL;
	//sprintf(filename, "output/hyperplane_data");

	//while(wPtr == NULL)
	//	wPtr = (FILE *)fopen(filename, "w");
	wPtr = (FILE *)fopen(filename, "w");

	// write the feasible point
	fprintf(wPtr, "%i\n1\n", dim);
	for(int i = 0; i < dim; i++)
		fprintf(wPtr, "%lf ", feasible_pt->coord[i]);
	fprintf(wPtr, "\n");

	// write the halfspaces
	fprintf(wPtr, "%i\n%lu\n", dim+1, utility_hyperplane.size());
	for (int i = 0; i < utility_hyperplane.size(); i++)
	{
		for (int j = 0; j < dim; j++)
		{
			fprintf(wPtr, "%lf ",utility_hyperplane[i]->normal->coord[j]);
		}
		fprintf(wPtr, "%lf ",utility_hyperplane[i]->offset);
		fprintf(wPtr, "\n");
	}

	fclose(wPtr);

}
