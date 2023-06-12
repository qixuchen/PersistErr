//#include "stdAfx.h"
#include "lp.h"

#include <vector>

//#define DEBUG_LP

// Takes an array of points s (of size N) and  a point pt and returns
// the direction in which pt has the worst regret ratio (in array v)
// as well as this regret ratio itself. 

/* We solve the following LP with col variables v[0], v[1], ..., v[D], x 
   and row variables q_1 ... q_{K-1}, r1, r_2
   
   Max x
   s.t. - (pt.a[0]-s[0].a[0])v[0] - (pt.a[1]-s[0].a[1])v[1] - ... - (pt.a[D-1]-s[0].a[D-1])v[D-1] + x = q_1
        - (pt.a[0]-s[1].a[0])v[0] - (pt.a[1]-s[1].a[1])v[1] - ... - (pt.a[D-1]-s[1].a[D-1])v[D-1] + x = q_2
        ...
        - (pt.a[0]-s[K-1].a[0])v[0] - (pt.a[1]-s[K-1].a[1])v[1] - ... - (pt.a[D-1]-s[K-1].a[D]-1)v[D-1] + x= q_K
           pt.a[0]v[0] + pt.a[1]v[1]  ....  + pt.a[D-1]v[D-1] = r1
          -pt.a[0]v[0] - pt.a[1]v[1]  ....  - pt.a[D-1]v[D-1] = r2
   variables have the following bounds
       0 <= v[j] < infty
       0 <= x < infty
       -infty < q_i  <=0
       -infty < r1 <= 1
       -infty < r2 <= -1
*/


// Use LP to check whether a point pt is a conical combination of the vectors in ExRays
bool insideCone(std::vector<point_t*> ExRays, point_t* pt)
{
	int M = ExRays.size();
	int D = pt->dim;

	int* ia = new int[1 + D * M];  //TODO: delete
	int* ja = new int[1 + D * M];  //TODO: delete
	double* ar = new double[1 + D * M];   //TODO: delete
	int i, j;
	double epsilon = 0.0000000000001;


	glp_prob *lp;
	lp = glp_create_prob();
	glp_set_prob_name(lp, "inside_cone");
	glp_set_obj_dir(lp, GLP_MAX);


	glp_add_rows(lp, D);  // add D rows: q_1...q_D
							  // Add rows q_1 ... q_D
	for (i = 1; i <= D; i++) {
		char buf[10];
		sprintf(buf, "q%d", i);
		glp_set_row_name(lp, i, buf);
		glp_set_row_bnds(lp, i, GLP_FX, pt->coord[i - 1], pt->coord[i-1]); // qi = pt->coord[i-1]
	}
	

	glp_add_cols(lp, M);    // add D columns: v[1] ... v[D]
								// Add col v[1] ... v[D]
	for (i = 1; i <= M; i++) {
		char buf[10];
		sprintf(buf, "v%d", i);

		glp_set_col_name(lp, i, buf);
		glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // 0 <= v[i] < infty
		glp_set_obj_coef(lp, i, 0.0);  // objective: 0
	}

	int counter = 1;
	// set value on row q1 ... qD
	for (i = 1; i <= D; i++) {
		for (j = 1; j <= M; j++) {

			ia[counter] = i; ja[counter] = j;
			ar[counter++] = ExRays[j-1]->coord[i-1]; 
		}
	}



	// loading data  
	glp_load_matrix(lp, counter - 1, ia, ja, ar);



										  // running simplex
	glp_smcp parm;
	glp_init_smcp(&parm);
	parm.msg_lev = GLP_MSG_OFF; // turn off all message by glp_simplex 

	glp_simplex(lp, &parm);

	bool isOutside = glp_get_status(lp) == GLP_NOFEAS;

	//printf("solution is optimal: %d\n", GLP_OPT);
	//printf("no feasible solution: %d\n", GLP_NOFEAS);


	//printf("return: %d\n", result);
	//for (i = 0; i < M; i++)
	//{
	//	double v = glp_get_col_prim(lp, i + 1);
	//	printf("w%d = %lf\n", i + 1, v);
	//}

	glp_delete_prob(lp); // clean up
	delete[]ia;
	delete[]ja;
	delete[]ar;

	return !isOutside;
}


// Use LP to find a feasible point of the half space intersection (used later in Qhull for half space intersection)
point_t* find_feasible(std::vector<hyperplane_t*> hyperplane)
{
	int M = hyperplane.size();
	int D = hyperplane[0]->normal->dim;

	// D + 2variables: D for dim, 2 for additional var for feasible
	int* ia = new int[1 + (D + 2) * M];  //TODO: delete
	int* ja = new int[1 + (D + 2) * M];  //TODO: delete
	double* ar = new double[1 + (D + 2) * M];   //TODO: delete
	int i, j;
	double epsilon = 0.0000000000001;


	glp_prob *lp;
	lp = glp_create_prob();
	glp_set_prob_name(lp, "find_feasible");
	glp_set_obj_dir(lp, GLP_MAX);
	glp_add_rows(lp, M);  // add D rows: q_1...q_D
	for (i = 1; i <= M; i++) {
		char buf[10];
		sprintf(buf, "q%d", i);
		glp_set_row_name(lp, i, buf);
		glp_set_row_bnds(lp, i, GLP_UP, 0, 0); // qi = 0
	}

	glp_add_cols(lp, D + 2);    // add D columns: v[1] ... v[D]
	for (i = 1; i <= D + 2; i++) {
		char buf[10];
		sprintf(buf, "v%d", i);
		glp_set_col_name(lp, i, buf);
		if(i <= D)
			glp_set_col_bnds(lp, i, GLP_FR, 0.0, 0.0); // -infty <= v[i] < infty
		else if (i == D + 1)
			glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // 0 <= v[i] < infty
		else
			glp_set_col_bnds(lp, i, GLP_UP, 0.0, D+1);
		if(i == D + 2)
			glp_set_obj_coef(lp, i, 1);  // objective: 0
		else
			glp_set_obj_coef(lp, i, 0.0);  // objective: 0
	}


	int counter = 1;
	// set value on row q1 ... qD
	for (i = 1; i <= M; i++) {
		for (j = 1; j <= D + 2; j++)
		{
			ia[counter] = i; ja[counter] = j;
			if(j <= D)
			{
				ar[counter++] = hyperplane[i-1]->normal->coord[j-1];
				//printf("%lf ", hyperplane[i-1]->normal->coord[j-1]);
			}
			else if (j == D+1)
			{
				ar[counter++] = hyperplane[i-1]->offset;
				//printf("%lf ", hyperplane[i-1]->offset);
			}
			else if (j == D+2)
			{
				ar[counter++] = 1;
				//printf("1.00000\n");
			}
		}
	}

	// loading data  
	glp_load_matrix(lp, counter - 1, ia, ja, ar);

	// running simplex
	glp_smcp parm;
	glp_init_smcp(&parm);
	parm.msg_lev = GLP_MSG_OFF; // turn off all message by glp_simplex
	glp_simplex(lp, &parm);



	double w1 = glp_get_col_prim(lp, D+1);

	if(w1 < 0 || isZero(w1))
	{
        glp_delete_prob(lp); // clean up
        delete[]ia;
        delete[]ja;
        delete[]ar;
		//printf("LP feasible error.\n");
		return NULL;
	}
    else
    {
        point_t* feasible_pt = alloc_point(D);
        for (i = 0; i < D; i++)
        {
            double v = glp_get_col_prim(lp, i + 1);
            feasible_pt->coord[i] = v / w1;
        }

        glp_delete_prob(lp); // clean up
        delete[]ia;
        delete[]ja;
        delete[]ar;
        return feasible_pt;
    }
}

// solve the LP in frame computation
void solveLP(std::vector<point_t*> B, point_t* b, double& theta, point_t* & pi)
{
	int M = B.size()+1;
	int D = b->dim;

	point_t* mean = alloc_point(D);
	for(int i = 0; i < D; i++)
		mean->coord[i] = 0;
	for(int i = 0; i < B.size(); i++)
	{
		for(int j = 0; j < D; j++)
			mean->coord[j] += B[i]->coord[j];
	}
	for(int i = 0; i < D; i++)
		mean->coord[i] /= B.size();

	int* ia = new int[1 + D * M];  //TODO: delete
	int* ja = new int[1 + D * M];  //TODO: delete
	double* ar = new double[1 + D * M];   //TODO: delete
	int i, j;
	double epsilon = 0.0000000000001;


	glp_prob *lp;
	lp = glp_create_prob();
	glp_set_prob_name(lp, "sloveLP");
	glp_set_obj_dir(lp, GLP_MIN);


	glp_add_rows(lp, D);  // add D rows: q_1...q_D
							  // Add rows q_1 ... q_D
	for (i = 1; i <= D; i++) {
		char buf[10];
		sprintf(buf, "q%d", i);
		glp_set_row_name(lp, i, buf);
		glp_set_row_bnds(lp, i, GLP_FX, b->coord[i - 1], b->coord[i-1]); // qi = pt->coord[i-1]
	}
	

	glp_add_cols(lp, M);    // add D columns: v[1] ... v[D]
								// Add col v[1] ... v[D]
	for (i = 1; i <= M; i++) {
		char buf[10];
		sprintf(buf, "v%d", i);

		glp_set_col_name(lp, i, buf);
		glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // 0 <= v[i] < infty

		if(i == 1)
			glp_set_obj_coef(lp, i, 1); 
		else
			glp_set_obj_coef(lp, i, 0.0); 
	}

	int counter = 1;
	// set value on row q1 ... qD
	for (i = 1; i <= D; i++) {
		for (j = 1; j <= M; j++) {

			if(j == 1)
			{
				ia[counter] = i; ja[counter] = j;
				ar[counter++] = -mean->coord[i-1]; 
			}
			else
			{
				ia[counter] = i; ja[counter] = j;
				ar[counter++] = B[j-2]->coord[i-1]; 
			}
		}
	}



	// loading data  
	glp_load_matrix(lp, counter - 1, ia, ja, ar);

										  // running simplex
	glp_smcp parm;
	glp_init_smcp(&parm);
	parm.msg_lev = GLP_MSG_OFF; // turn off all message by glp_simplex 
	
	glp_simplex(lp, &parm);


	//printf("solution is optimal: %d\n", GLP_OPT);
	//printf("no feasible solution: %d\n", GLP_NOFEAS);
	bool feasible = glp_get_status(lp) == GLP_NOFEAS;

	theta = glp_get_obj_val(lp);
	for(int i = 0; i < D; i++)
	{
		pi->coord[i] = glp_get_row_dual(lp, i + 1);
	}
	//printf("return primal: %lf\n", glp_get_obj_val(lp));
	//for (i = 0; i < M; i++)
	//{
	//	double v = glp_get_col_prim(lp, i + 1);
	//	printf("p%d = %lf\n", i + 1, v);
	//}

	//printf("return dual: \n");
	//double dot_v = 0;
	//for (i = 0; i < D; i++)
	//{
	//	double v = glp_get_row_dual(lp, i + 1);
	//	dot_v += v * b->coord[i];
	//	printf("d%d = %lf\n", i + 1, v);
	//}
	//printf("dual objective: %lf\n", dot_v);

	glp_delete_prob(lp); // clean up
	delete[]ia;
	delete[]ja;
	delete[]ar;

	//printf("LP-verify:\n");
	//if(theta > 0 && insideCone(B, b) || isZero(theta) && !insideCone(B, b))
	//	printf("In-out error.\n");
	//if(!isZero(dot_prod(pi, b) - theta))
	//	printf("Objective value error.\n");
	//for(int i = 0; i < B.size(); i++)
	//{
	//	double v = dot_prod(B[i], pi);
	//	if(v >0 && !isZero(v))
	//		printf("Hyperplane error.\n");
	//}
	//double v = dot_prod(mean,pi);
	//if(v > -1 && !isZero(v+1))
	//	printf("Mean vector error.\n");

	release_point(mean);
}


// Check whether the hyperplane is the boundary of the range R(the intersection of the halfspace)
// hyperplane
// halfspace: a vector contains the range R
// If it is on the boundary, return true. Otherwise return false
bool check_hyperplane_on_boundary(hyperplane_t* hyperplane, halfspace_set_t* half_set)
{
	int M = half_set->halfspaces.size();
	int D = half_set->halfspaces[0]->normal->dim;
	
	// M for the M halfspace; 1 for the utility vector
	int* ia = new int[1 + D * (2*M + 2)];  //TODO: delete
	int* ja = new int[1 + D * (2*M + 2)];  //TODO: delete
	double* ar = new double[1 + D * (2*M + 2)];   //TODO: delete
	int i, j;
	double epsilon = 0.0000000000001;

	glp_prob *lp;
	lp = glp_create_prob();
	glp_set_prob_name(lp, "check_hyperplane_on_boundary");
	glp_set_obj_dir(lp, GLP_MAX);
	

	glp_add_rows(lp, 2*M + 2); 
	for (i = 1; i <= 2*M + 2; i++) 
	{
		char buf[10];
		sprintf(buf, "q%d", i);
		glp_set_row_name(lp, i, buf);
		if(i <= M)
		{
			glp_set_row_bnds(lp, i, GLP_LO, 0, 0.0); // qi >=0
		}
		else if(i == 2*M+2)
		{
			glp_set_row_bnds(lp, i, GLP_FX, 1.0, 1.0); // qi ==0
		}
		else{
			glp_set_row_bnds(lp, i, GLP_FX, 0.0, 0.0); // qi ==1
		}

	}
	
	glp_add_cols(lp, D);// add D columns: v_1 ... v_D
	for (i = 1; i <= D; i++) 
	{
		char buf[10];
		sprintf(buf, "v%d", i);
		glp_set_col_name(lp, i, buf);
		//glp_set_col_bnds(lp, i, GLP_FR, 0.0, 0.0); // -infty <= v[i] < infty
		glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // 0 <= v_i < infty

		glp_set_obj_coef(lp, i, 1.0);  // objective v_1+v_2+...+v_D
	}

	int counter = 1;
	//set the matrix
	for (i = 1; i <= 2*M + 2; i++) 
	{
		for (j = 1; j <= D; j++) 
		{
			ia[counter] = i; ja[counter] = j;
			if(i<=M)
			{
				ar[counter++] = half_set->halfspaces[i-1]->normal->coord[j-1];
				//printf("%lf ", half_set->halfspaces[i-1]->normal->coord[j-1]);
			}
			else if(M<i && i<=2*M)
			{
				ar[counter++] = half_set->halfspaces[i-M-1]->normal->coord[j-1];
			}
			else if(i== 2*M + 1)
			{
				ar[counter++] = hyperplane->normal->coord[j-1];
				//printf("%lf ", hyperplane->normal->coord[j-1]);
			}
			else{
				ar[counter++]= 1;
			}
		}
	}

	// loading data  
	glp_load_matrix(lp, counter-1, ia, ja, ar);
	// running simplex
	glp_smcp parm;
	glp_init_smcp(&parm);
	parm.msg_lev = GLP_MSG_OFF; // turn off all message by glp_simplex 
	glp_simplex(lp, &parm);

	double z=glp_get_obj_val(lp);
	// clean up
	glp_delete_prob(lp); 
	delete[]ia;
	delete[]ja;
	delete[]ar;
	if(z!=0)
	{
		//printf("LP feasible.\n");
		return true;
	}
	else
	{
		//printf("LP feasible error.\n");
		//printf("%lf\n", z);
		return false;
	}
	return true;
}

/*int M = half_set->halfspaces.size();
	if(M<1)
	{
		printf("%s\n", "The halfspace_set is empty"); 
		return -1;
	}
	int D = half_set->halfspaces[0]->normal->dim;
	for(int i=0; i<M; i++)
	{
		int count1=0, count2=0;
		for(int j=0; j<D; j++)
		{	
			if(half_set->halfspaces[i]->normal->coord[j] + hyperplane->normal->coord[j]==0)
			{
				count1++;
			}
			else if (half_set->halfspaces[i]->normal->coord[j] == hyperplane->normal->coord[j])
			{
				count2++;
			}
			else
			{
				break;
			}
		}
		if(count1==D||count2==D)
		{
			return i;
		}
	}
	return -1;
	*/

// Use LP to check whether the hyperplane intersects with the range R(the intersection of the halfspace)
// hyperplane
// halfspace: a vector contains the range R
bool check_hyperplane_intersect(hyperplane_t* hyperplane, halfspace_set_t* half_set, int direction)
{
	int M = half_set->halfspaces.size();
	int D = half_set->halfspaces[0]->normal->dim;

	// M for the M halfspace; 1 for the utility vector
	int* ia = new int[1 + D * (M + 2)];  //TODO: delete
	int* ja = new int[1 + D * (M + 2)];  //TODO: delete
	double* ar = new double[1 + D * (M + 2)];   //TODO: delete
	int i, j;
	double epsilon = 0.0000000000001;

	glp_prob *lp;
	lp = glp_create_prob();
	glp_set_prob_name(lp, "check_hyperplane_intersect");
	glp_set_obj_dir(lp, GLP_MAX);
	

	glp_add_rows(lp, M + 2);  // add M+1 rows: q_1...q_(M+1)
	for (i = 1; i <= M + 2; i++) {
		char buf[10];
		sprintf(buf, "q%d", i);
		glp_set_row_name(lp, i, buf);
		if(i <= M){
			glp_set_row_bnds(lp, i, GLP_LO, 0, 0.0); // qi >=0
		}
		else if(i == M+1){
			glp_set_row_bnds(lp, i, GLP_FX, 0.0, 0.0); // qi ==0
		}
		else{
			glp_set_row_bnds(lp, i, GLP_FX, 1.0, 1.0); // qi ==1
		}

	}
	
	glp_add_cols(lp, D);// add D columns: v_1 ... v_D
	for (i = 1; i <= D; i++) {
		char buf[10];
		sprintf(buf, "v%d", i);
		glp_set_col_name(lp, i, buf);
		//glp_set_col_bnds(lp, i, GLP_FR, 0.0, 0.0); // -infty <= v[i] < infty
		glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // 0 <= v_i < infty

		glp_set_obj_coef(lp, i, 1.0);  // objective v_1+v_2+...+v_D
	}

	int counter = 1;
	//set the matrix
	for (i = 1; i <= M + 2; i++) {
		for (j = 1; j <= D; j++) {
			ia[counter] = i; ja[counter] = j;
			if(i <= M){
				ar[counter++] = half_set->halfspaces[i-1]->normal->coord[j-1];
				//printf("%lf ", half_set->halfspaces[i-1]->normal->coord[j-1]);
			}
			else if(i== M + 1)
			{
			    if(direction == -1)
				    ar[counter++] = hyperplane->normal->coord[j-1];
                else
                    ar[counter++] = -hyperplane->normal->coord[j-1];
				//printf("%lf ", hyperplane->normal->coord[j-1]);
			}
			else{
				ar[counter++]= 1;
			}
		}
	}

	//printf("%d\n", counter);
	// loading data  
	glp_load_matrix(lp, counter-1, ia, ja, ar);
	// running simplex
	glp_smcp parm;
	glp_init_smcp(&parm);
	parm.msg_lev = GLP_MSG_OFF; // turn off all message by glp_simplex 
	glp_simplex(lp, &parm);

	double z=glp_get_obj_val(lp);
	// clean up
	glp_delete_prob(lp); 
	delete[]ia;
	delete[]ja;
	delete[]ar;
	if(z!=0)
	{
		//printf("LP feasible.\n");
		return true;
	}
	else
	{
		//printf("LP feasible error.\n");
		//printf("%lf\n", z);
		return false;
	}
}



// Use LP to check whether the the intersection of the halfspaces is feasible
bool check_halfspace_intersect(halfspace_set_t* half_set)
{
	int M = half_set->halfspaces.size();
	int D = half_set->halfspaces[0]->normal->dim;
	//printf("%d\n", M);
	//printf("%d\n", D);
	// M for the M halfspace; 1 for the utility vector
	int* ia = new int[1 + D * (M + 1)];  //TODO: delete
	int* ja = new int[1 + D * (M + 1)];  //TODO: delete
	double* ar = new double[1 + D * (M + 1)];   //TODO: delete
	int i, j;
	double epsilon = 0.0000000000001;


	glp_prob *lp;
	lp = glp_create_prob();
	glp_set_prob_name(lp, "check_halfspace_intersect");
	glp_set_obj_dir(lp, GLP_MAX);


	glp_add_rows(lp, M + 1);  // add M+1 rows: q_1...q_(M+1)
	for (i = 1; i <= M + 1; i++) {
		char buf[10];
		sprintf(buf, "q%d", i);
		glp_set_row_name(lp, i, buf);
		if(i <= M){
			glp_set_row_bnds(lp, i, GLP_LO, 0.0, 0.0); // qi >=0
		}
		else{
			glp_set_row_bnds(lp, i, GLP_FX, 1.0, 1.0); // qi ==1
		}

	}
	

	glp_add_cols(lp, D);// add D columns: v_1 ... v_D
	for (i = 1; i <= D; i++) {
		char buf[10];
		sprintf(buf, "v%d", i);
		glp_set_col_name(lp, i, buf);
		//glp_set_col_bnds(lp, i, GLP_FR, 0.0, 0.0); // -infty <= v[i] < infty
		glp_set_col_bnds(lp, i, GLP_LO, 0.0, 0.0); // 0 <= v_i < infty

		glp_set_obj_coef(lp, i, 1.0);  // objective v_1+v_2+...+v_D
	}


	int counter = 1;
	//set the matrix
	for (i = 1; i <= M + 1; i++) {
		for (j = 1; j <= D; j++) {

			ia[counter] = i; ja[counter] = j;
			
			if(i<=M){
				ar[counter++] = half_set->halfspaces[i-1]->normal->coord[j-1];
				//printf("%lf ", halfspace[i-1]->normal->coord[j-1]);
			}
			else
				ar[counter++]= 1;
			
		}
	}
	//printf("%d\n", counter);
	// loading data  
	glp_load_matrix(lp, counter-1, ia, ja, ar);
	// running simplex
	glp_smcp parm;
	glp_init_smcp(&parm);
	parm.msg_lev = GLP_MSG_OFF; // turn off all message by glp_simplex 
	glp_simplex(lp, &parm);

	double z=glp_get_obj_val(lp);
	if(z!=0)
	{
		//printf("LP feasible.\n");
		/*for (i = 1; i <= D; i++) {
			double w;
			w = glp_get_col_prim(lp, i);
			half_set->check_point->coord[i-1]=w;
			//printf("%lf ",w);
		}
		*/
		// clean up
		glp_delete_prob(lp); 
		delete[]ia;
		delete[]ja;
		delete[]ar;
		return true;
	}
	else
	{
		//printf("LP feasible error.\n");
		//printf("%lf\n", z);
		glp_delete_prob(lp); 
		delete[]ia;
		delete[]ja;
		delete[]ar;
		return false;
	}
	return true;
}



//Find the least number of halfspaces which construct a polytope
//Prune the halfspaces which are unnecessary
void prune_halfspace(halfspace_set_t* half_set)
{

	int M=half_set->halfspaces.size();
	//check there are halfspaces in the set
	if(M<=0){
		return;
	}
	int D=half_set->halfspaces[0]->normal->dim;
	int num=0;
	for(int i=0; i<M; i++)
	{
		point_t* p = alloc_point(D);
		for(int j=0; j<D; j++){
			p->coord[j] = half_set->halfspaces[num]->normal->coord[j];
			//printf("%lf ", half_set->halfspaces[num]->normal->coord[j]);
		}
		//printf("\n");
		hyperplane_t* hyper;
		hyper = (hyperplane_t*)malloc(sizeof(hyperplane_t));
		memset(hyper, 0, sizeof(hyperplane_t));
		hyper->normal = p;
		hyper->offset =half_set->halfspaces[num]->offset;
		bool check_result=check_hyperplane_intersect(hyper, half_set, true);
		if(check_result==true)
		{
			num++;
		}
		else
		{
			half_set->halfspaces.erase(half_set->halfspaces.begin()+num);
		}
	}

}