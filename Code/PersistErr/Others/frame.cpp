#include "frame.h"

void findExtremeRay(vector<point_t*> P, vector<int> active,  point_t* pi, point_t* sigma, int& newRay)
{
	double max = -INF;
	vector<int> U;

	for(int i = 0; i < P.size(); i++)
	{
		double v = dot_prod(pi, P[i]);
		if(active[i] && v > 0)
		{
			double ratio = dot_prod(sigma, P[i]) / v;
			if(ratio > max && !isZero(ratio - max))
			{
				U.clear();
				max = ratio;
				U.push_back(i);
			}
			else if (isZero(ratio - max))
			{
				U.push_back(i);
			}
		}
	}

	if(U.size() <= 2)
		newRay = U[0];
	else
	{
		vector<point_t*> tmpP;
		vector<int> tmpB;
		for(int i = 0; i < U.size(); i++)
		{
			tmpP.push_back(P[U[i]]);
		}

		frameConeFastLP(tmpP, tmpB);
		newRay = U[tmpB[0]];

		//printf("Test\n");
		//system("pause");
	}
}

void initial(vector<point_t*> P, vector<int>& active, int& rank, vector<int>& B, point_t*& sigma)
{
	int dim = P[0]->dim;
	B.clear();
	rank = guassRank(P);
	//printf("Init-rank: %d\n", rank);

	vector<int> idxs;
	double theta;
	point_t* minus = alloc_point(dim);
	for(int i = 0; i < dim; i++)
		minus->coord[i] = - P[0]->coord[i];
	point_t* pi = alloc_point(dim);
	solveLP(P, minus, theta, pi);

	//printf("Init-theta: %lf\n", theta);
	//printf("Init-pi:\n");
	//print_point(pi);

	
	for(int i = 0; i < P.size(); i++)
	{
		active.push_back(1);
		//printf("%lf\n", dot_prod(pi, P[i]));
		if(isZero(dot_prod(pi, P[i])))
			idxs.push_back(i);
	}
	//printf("Init-size: %d\n", idxs.size());

	//if(idxs.size() < rank - 1)
	//{
	//	for(int i = 0; i < P.size(); i++)
	//		print_point(P[i]);
	//}

	if(idxs.size() > rank - 1)
	{
		//for(int i = 0; i < idxs.size(); i++)
		//	print_point(P[idxs[i]]);



		vector<point_t*> tmpP;
		vector<int> tmpB;
		for(int i = 0; i < idxs.size(); i++)
			tmpP.push_back(P[idxs[i]]);

		frameConeFastLP(tmpP, tmpB);

		for(int i = 0; i < tmpB.size(); i++)
		{
			B.push_back(idxs[tmpB[i]]);
			active[idxs[tmpB[i]]] = 0;
		}
		//printf("Test\n");
		//system("pause");
	}
	else
	{
		for(int i = 0; i < idxs.size(); i++)
		{
			B.push_back(idxs[i]);
			active[idxs[i]] = 0;
		}

	}

	point_t* pi2 = copy(pi);
	int index = 1;
	while(!linearInd(pi, pi2))
	{
		for(int i = 0; i < dim; i++)
			minus->coord[i] = -P[index]->coord[i];

		solveLP(P, minus, theta, pi2);
		index++;
	}

	//printf("Init-pi2 (index = %i):\n", index);
	//print_point(pi2);
	//print_point(pi);
	//
	//if(linearInd(pi, pi2))
	//	printf("linear independent\n");
	//else
	//	printf("not linear independent\n");

	sigma = alloc_point(dim);
	for(int i = 0; i < dim; i++)
		sigma->coord[i] = 0.5 * (pi->coord[i] + pi2->coord[i]);

	//printf("Init-sigma:\n");
	//print_point(sigma);

	int newRay;
	findExtremeRay(P, active, scale(-1, pi), sigma, newRay);
	B.push_back(newRay);
	active[newRay] = 0;

	//printf("Init-printing B: ");
	//printIndex(B);

	release_point(minus);
	release_point(pi);
	release_point(pi2);
}

// frame compuatation (all at once)
void frameConeFastLP(vector<point_t*> rays, vector<int>& B)
{
	int dim = rays[0]->dim;

	int rank;
	point_t* sigma;
	vector<int> active;
	point_t* pi = alloc_point(dim);

	initial(rays, active, rank, B, sigma);
	
	for(int i = 0; i < rays.size(); )
	{
		if(active[i])
		{
			//printIndex(B);

			point_t* b = rays[i];

			vector<point_t*> frame;
			for(int j = 0; j < B.size(); j++)
				frame.push_back(rays[B[j]]);

			double theta;
			solveLP(frame, b, theta, pi);

			//for(int i = 0; i < frame.size(); i++)
			//	print_point(frame[i]);
			//print_point(b);

			//printf("pi:\n");
			//print_point(pi);
			//printf("b:\n");
			//print_point(b);
			//printf("dot_prod %lf\n", dot_prod(pi, b));
			double v = dot_prod(pi, b);
			if(v > 0 && ! isZero(v))
			{
				int newRay;
				findExtremeRay(rays, active, pi, sigma, newRay);

				B.push_back(newRay);
				active[newRay] = 0;
			}
			else
			{
				active[i] = 0;
				i++;
			}
		}
		else
		{
			i++;
		}
	}

	release_point(pi);
}

// parital frame compuatation (only s)
void partialConeFastLP(vector<point_t*> rays, vector<int>& B, int s)
{
	int dim = rays[0]->dim;

	int rank;
	point_t* sigma;
	vector<int> active;
	point_t* pi = alloc_point(dim);

	initial(rays, active, rank, B, sigma);
	
	for(int i = 0; i < rays.size() && B.size() < s; )
	{
		if(active[i])
		{
			//printIndex(B);

			point_t* b = rays[i];

			vector<point_t*> frame;
			for(int j = 0; j < B.size(); j++)
				frame.push_back(rays[B[j]]);

			double theta;
			solveLP(frame, b, theta, pi);

			//for(int i = 0; i < frame.size(); i++)
			//	print_point(frame[i]);
			//print_point(b);

			//printf("pi:\n");
			//print_point(pi);
			//printf("b:\n");
			//print_point(b);
			//printf("dot_prod %lf\n", dot_prod(pi, b));
			double v = dot_prod(pi, b);
			if(v > 0 && ! isZero(v))
			{
				int newRay;
				findExtremeRay(rays, active, pi, sigma, newRay);

				B.push_back(newRay);
				active[newRay] = 0;
			}
			else
			{
				active[i] = 0;
				i++;
			}
		}
		else
		{
			i++;
		}
	}

	release_point(pi);
	release_point(sigma);
}

// frame compuatation (all at once) - naive implementation
void frameConeLP(std::vector<point_t*> rays, std::vector<int>& idxs)
{
	int dim = rays[0]->dim;
	idxs.clear();

	point_t* zero = alloc_point(dim);
	for (int i = 0; i < dim; i++)
		zero->coord[i] = 0;

	for (int i = 0; i < rays.size(); i++)
	{
		point_t* ray = rays[i];

		rays[i] = zero;

		if (!insideCone(rays, ray))
		{
			idxs.push_back(i);
			rays[i] = ray;
		}
	}

	release_point(zero);
}