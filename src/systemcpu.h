#ifndef SYSTEMCPU_H
#define SYSTEMCPU_H
#include <iostream>
#include <vector_types.h>

using namespace std;
typedef struct _point2
{
    float x;
    float y;
    _point2(){x=0;y=0;}
    _point2(const float& _x, const float& _y): x(_x),y(_y){};
} point2;

typedef struct _cell
{
    unsigned int _num;
    float _mass;
    int _level;
    float _r;
    point2 _middle;
    float3 _center;
    unsigned int _child[4];
    _cell(){_center.x =0; _center.y=0; _num=0; _level=0;}
    void middle(const point2& tr,const point2& bl)
    {
        _middle.x = (tr.x + bl.x)/2;
        _middle.y = (tr.y + bl.y)/2;
        _r = tr.x - bl.x;
    }
} cell;


class systemcpu
{

    public:
    unsigned int top;
    cell *_cells;
    unsigned int *_sorted;
    unsigned int *_lsorted;
    float3 *_bodies;
    point2 *_v;
    point2 *_a;
    unsigned int _noden;
    float _time;
    float _tstep;
    float _rho;
    float _alpha;
    float _mass;
    float _as;
    float _ad;
    int _MS;
    int _NP;
    int _NH;
    int _n;
    int _maxlevel;
    bool _lock;

    systemcpu(){};

    void initialize(const int&); // init of bodies;
    void treebuilding(); // build the tree;
    void insert(); //insert the number;
    void computeCenter();
    void sortBodies();
    void updateBodies();
    void bounding(); //bounding the space
    void computeForce();
    void traverseTree();
    ~systemcpu();
};

inline point2 maxfloat3(const float3& f1, const point2& f2)
{
    return point2( (f1.x>f2.x)?f1.x:f2.x,(f1.y>f2.y)?f1.y:f2.y);
}

inline point2 minfloat3(const float3& f1, const point2& f2)
{
    return point2((f1.x<f2.x)?f1.x:f2.x,(f1.y<f2.y)?f1.y:f2.y);
}

#include "systemcpu_imp.h"

#endif
