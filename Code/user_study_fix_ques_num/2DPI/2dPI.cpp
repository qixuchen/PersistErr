#include "2dPI.h"
#include <sys/time.h>
#define NN 1

struct MinHeapCmp
{
    inline bool operator()(const utility_t* y, const utility_t* z)const
    {
        double v1=y->x;
        double v2=z->x;
        //printf("v1%lf v2%lf\n", v1, v2);
        if(v1 - v2<0.000000001&&v1 - v2>-0.000000001)
        { 
            if(y->place == z->place)
            {
                return y->time < z->time;
            }
            else
            {
                return y->place > z->place;
            }
            
        }
        else if(v1 > v2)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

/*
 * @brief Used for 2D dimensional datasets.
 * Start from x-axis to y-axis, record the range of utility and the corresponding top-k record.
 * Then find one of the top-k point by asking questions
 * @param original_set       The original dataset
 * @param u                  User's utility function
 * @param k                  The threshold
 */
int twoDPI(std::vector<point_t*> &top_set, point_t* u, int k)
{
    std::vector<point_t*> point_set; //the sorted version of onion_set
    point_t* u0 = alloc_point(2);
    u0->coord[0] = 0;
    u0->coord[1] = 1;
    sort_point(top_set, point_set, u0);
    //use array to store whether the order of each pair of points changes
    int M = point_set.size();
    bool** is_insert = new bool* [M];
    for(int i=0; i<M;i++)
        is_insert[i] = new bool[M];
    for(int i=0; i < M; i++)
    {
        for(int j=0; j < M; j++)
        {
            is_insert[point_set[i]->pid][point_set[j]->pid]=false;
        }
    }

    //obtain the utility vectors for points order change
    std::vector<utility_t*> min_h;
    double time = 0; //a count to discriminate when the item inserted into min_h
    utility_t* ut;
    for(int i=0; i < M-1; i++)
    {
        double x0;
        //note that the point might be overlap
        if(point_set[i]->coord[1]==point_set[i+1]->coord[1])
        {
            x0=2;
        }
        else
        {
            x0=(point_set[i]->coord[1]-point_set[i+1]->coord[1])/
               (point_set[i+1]->coord[0] - point_set[i+1]->coord[1] - point_set[i]->coord[0] + point_set[i]->coord[1]);
        }
        //note that x0 in [0,1]
        if(x0 >= 0 && x0 <= 1)
        {
            ut=alloc_utility();
            ut->x=x0;
            is_insert[point_set[i]->pid][point_set[i+1]->pid]=true;
            is_insert[point_set[i+1]->pid][point_set[i]->pid]=true;
            ut->place=i;
            ut->time=time;
            time=time+0.01;
            point_set[i]->dim=i;
            ut->point_up=point_set[i];
            point_set[i+1]->dim=i+1;
            ut->point_down=point_set[i+1];
            min_h.push_back(ut);
        }
    }
    //build heap
    std::make_heap(min_h.begin(), min_h.end(), MinHeapCmp());
    /*
    printf("utility\n");
    for(int i=0; i<min_h.size(); i++)
    {
        printf("u %lf place %d time %lf point %d point %d\n", min_h[i]->x, min_h[i]->place, min_h[i]->time, min_h[i]->point_up->id, min_h[i]->point_down->id);
    }
    */
    std::vector<point_t*> represent_point;
    for(int i=0; i<k; i++)
        point_set[i]->value = 1;
    int index = 1, count =0;
    std::vector<utility_t*> border;
    while(min_h.size()>0||min_h[0]->x>1)
    {
        utility_t* current_u;
        std::pop_heap(min_h.begin(), min_h.end(), MinHeapCmp());
        current_u=min_h[min_h.size()-1];
        min_h.pop_back();
        //change the location of point_up, point_down
        point_t* p0=current_u->point_down;
        int place_down=current_u->point_down->dim;
        point_set.erase(point_set.begin()+place_down);
        point_set.insert(point_set.begin()+place_down-1, p0);
        current_u->point_up->dim++;
        current_u->point_down->dim--;

        if(place_down==k)
        {
            //if the point is not top-k any more, delete it
            if(current_u->point_up->value ==index)
            {
                count++;
            }
            current_u->point_up->value = -1;
            current_u->point_down->value = index + 1;
            //if there is no more point in the current_top_k
            if(count >= k)
            {
                double bx;
                if(border.size()>0)
                    bx=border[border.size()-1]->x;
                else
                    bx=0;
                if(current_u->x-bx>0)
                {
                    //see as two different border
                    represent_point.push_back(current_u->point_up);
                    border.push_back(current_u);
                }
                count=0; index++;
            }
        }
        //insert new utility record place_down place_down+1
        if(place_down + 1 < point_set.size() && is_insert[point_set[place_down]->pid][point_set[place_down+1]->pid]==false)
        {
            double x0;
            //note that the point might be overlap
            if(point_set[place_down]->coord[1]==point_set[place_down+1]->coord[1])
            {
                x0=2;
            }
            else
            {
                x0=(point_set[place_down]->coord[1]-point_set[place_down+1]->coord[1])/
                   (point_set[place_down+1]->coord[0] - point_set[place_down+1]->coord[1] - point_set[place_down]->coord[0] + point_set[place_down]->coord[1]);
            }
            if(x0>=0&&x0<=1)
            {
                ut=alloc_utility();
                ut->x=x0;
                is_insert[point_set[place_down]->pid][point_set[place_down+1]->pid]=true;
                is_insert[point_set[place_down+1]->pid][point_set[place_down]->pid]=true;
                ut->place=place_down;
                ut->time=time;
                time=time+0.01;
                point_set[place_down]->dim=place_down;
                ut->point_up=point_set[place_down];
                point_set[place_down+1]->dim=place_down+1;
                ut->point_down=point_set[place_down+1];
                min_h.push_back(ut);
                std::push_heap(min_h.begin(), min_h.end(), MinHeapCmp());
            }
        }
        //insert new utility record place_down-2 place_down-1
        if(place_down > 1 && is_insert[point_set[place_down-2]->pid][point_set[place_down-1]->pid]==false)
        {
            double x0;
            //note that the point might be overlap
            if(point_set[place_down-2]->coord[1]==point_set[place_down-1]->coord[1])
            {
                x0=2;
            }
            else
            {
                x0=(point_set[place_down-2]->coord[1]-point_set[place_down-1]->coord[1])/
                   (point_set[place_down-1]->coord[0] - point_set[place_down-1]->coord[1] - point_set[place_down-2]->coord[0] + point_set[place_down-2]->coord[1]);
            }
            if(x0>=0&&x0<=1)
            {
                ut=alloc_utility();
                ut->x=x0;
                is_insert[point_set[place_down-2]->pid][point_set[place_down-1]->pid]=true;
                is_insert[point_set[place_down-1]->pid][point_set[place_down-2]->pid]=true;
                ut->place=place_down-2;
                ut->time=time;
                time=time+0.01;
                point_set[place_down-2]->dim=place_down-2;
                ut->point_up=point_set[place_down-2];
                point_set[place_down-1]->dim=place_down-1;
                ut->point_down=point_set[place_down-1];
                min_h.push_back(ut);
                std::push_heap(min_h.begin(), min_h.end(), MinHeapCmp());
            }
        }
    }
    for(int i=0; i < point_set.size(); i++)
    {
        if(point_set[i]->value ==index)
        {
            represent_point.push_back(point_set[i]);
        }
    }
    for(int i=0; i<M;i++)
        delete []is_insert[i];
    delete []is_insert;
    //now we have represent_point, border. Ask questions
    /*
    for(int i=0; i<border.size();i++)
    {
        printf("border %d u %lf\n", i, border[i]->x);
    }
    for(int i=0; i<represent_point.size();i++)
    {
        printf("point %d %lf %lf\n", represent_point[i]->id, represent_point[i]->coord[0], represent_point[i]->coord[1]);
    }
    */
    int numOfQuestion=0;
    int b_left=0, b_right=border.size()-1;
    while(b_left <= b_right)
    {
        numOfQuestion++;
        int index = (b_left+b_right)/2;
        double v1 = dot_prod(u, border[index]->point_up);
        double v2 = dot_prod(u, border[index]->point_down);
        if(v1>v2)
        {
            b_right=index-1;
        }
        else
        {
            b_left=index+1;
        }
    }
    printf("|%30s |%10d |%10s |\n", "2D-PI", numOfQuestion, "--");
    printf("|%30s |%10s |%10d |\n", "Point", "--", represent_point[b_left]->id);
    printf("---------------------------------------------------------\n");
    return numOfQuestion;
}
