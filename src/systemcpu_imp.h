//#include "systemcpu.h"
#include <float.h>
systemcpu::~systemcpu()
{
    delete[] _bodies;
    delete[] _cells;
    delete[] _sorted;
    delete[] _a;
    delete[] _v;
}



void systemcpu::initialize(const int& n)
{
    _maxlevel = 0;
    _n = n;
    _v = new point2[n];
    _a = new point2[n];
    _bodies = new float3[n];
    _cells = new cell[2*n];
    _noden = 2*n;
    _time = 0.0f;
    _tstep = 0.05f;
    _rho = 0.4f;
    _alpha = 3.0f;
    _NP = n*_rho;
    _NH = n*(1-_rho);
    _MS = 50;
    _ad = 25;
    _as = _ad / _alpha;
    _mass = 1;

    float c = 10 * _tstep * _tstep /(4.0f * M_PI); 

    srand((unsigned)time(NULL));
    point2 minp(FLT_MAX,FLT_MAX),maxp(FLT_MIN,FLT_MIN);

    for(int i=0; i< _NH; ++i)
    {
        float r = _as * rand() / float(RAND_MAX);
        float cita = 2 * M_PI * rand() / float(RAND_MAX);
        float ccita = cos(asin(r/_as));

        float dx = r* cos(cita);
        float dy = r* sin(cita);

        _bodies[i].x = dx + _MS/2;
        _bodies[i].y = dy + _MS/2; 
        _bodies[i].z = _mass;

        float ds = r;
        float coscita = cos(asin(r/_as));
        float ha = c * _NH * _mass * (1 - coscita * coscita *coscita ) / ds;
        float hax = -ha*dx/r;
        float hay = -ha*dy/r;
        float vr = sqrtf(ha*r);

        _v[i].x = -vr*dy/r; 
        _v[i].y = vr*dx/r; 
        _a[i].x = hax;
        _a[i].y = hay;

        minp = minfloat3(_bodies[i],minp);
        maxp = maxfloat3(_bodies[i],maxp); 
    }

    for(int i = _NH; i< _n; ++i)
    {
        float r = (_ad - _as) * rand() / float(RAND_MAX) + _as;
        float cita = 2 * M_PI * rand() / float(RAND_MAX);

        float dx = r* cos(cita);
        float dy = r* sin(cita);

        _bodies[i].x = dx + _MS/2;
        _bodies[i].y = dy + _MS/2; 
        _bodies[i].z = _mass;

        float ha = c * _NH / r;
        float hax = -ha*dx/r;
        float hay = -ha*dy/r;
        float vr = sqrtf(ha*r);

        _v[i].x = -vr*dy/r; 
        _v[i].y = vr*dx/r; 

        _a[i].x = hax;
        _a[i].y = hay;


        minp = minfloat3(_bodies[i],minp);
        maxp = maxfloat3(_bodies[i],maxp); 
    }

    // bounding the root node;
    _cells[0].middle(maxp,minp);
    top = 0; 
    //child pointer to null;
    for(int i=0; i<2*n; i++)
    {
        _cells[i]._child[0] = -1;
        _cells[i]._child[1] = -1;
        _cells[i]._child[2] = -1;
        _cells[i]._child[3] = -1;
    }
}

void systemcpu::treebuilding()
{
    bool triger = true;
    int n = 0;
    for(int i=0; i<_n; i++)
    {
        unsigned int ch;
        int depth=0;
        int j = 0;
        if(triger)
        {
            if( _bodies[i].x > _cells[0]._middle.x ) j += 1;
            if( _bodies[i].y > _cells[0]._middle.y ) j += 2;
            ch = _cells[0]._child[j];

            _cells[0]._center.x += _bodies[i].x * _bodies[i].z;
            _cells[0]._center.y += _bodies[i].y * _bodies[i].z;
            _cells[0]._center.z += _bodies[i].z;
            triger=false;
        }

        while(ch < _noden && ch >= 0)
        {
            n = ch; 
            j = 0;
            if( _bodies[i].x > _cells[n]._middle.x ) j += 1;
            if( _bodies[i].y > _cells[n]._middle.y ) j += 2;
            ch = _cells[n]._child[j];
            _cells[n]._center.x += _bodies[i].x * _bodies[i].z;
            _cells[n]._center.y += _bodies[i].y * _bodies[i].z;
            _cells[n]._center.z += _bodies[i].z;
            _cells[n]._num ++;
        }


        if(ch == -1)
        {
            _cells[n]._child[j] = _noden + i;
        }
        else
        {
            while(true)
            {
                int pointer = ++top;
                // bounding the root node;

                float r = 0.5 * _cells[n]._r;
                float x = (j%2)*r;
                float y = (j/2)*r;

                _cells[n]._child[j] = pointer;
                _cells[pointer]._middle.x = _cells[n]._middle.x + x - r/2.0f;
                _cells[pointer]._middle.y = _cells[n]._middle.y + y - r/2.0f;
                _cells[pointer]._r = r;
                _cells[pointer]._level = 1 + _cells[n]._level;
                _maxlevel = max(_maxlevel,_cells[pointer]._level);

                j = 0;
                if( _bodies[i].x > _cells[pointer]._middle.x ) j += 1;
                if( _bodies[i].y > _cells[pointer]._middle.y ) j += 2;
                _cells[pointer]._child[j] = i + _noden;
                _cells[pointer]._center.x += _bodies[i].x * _bodies[i].z;
                _cells[pointer]._center.y += _bodies[i].y * _bodies[i].z;
                _cells[pointer]._center.z += _bodies[i].z;
                _cells[pointer]._num ++;

                j = 0;
                unsigned int body_ch = ch - _noden;
                if( _bodies[body_ch].x > _cells[pointer]._middle.x ) j += 1;
                if( _bodies[body_ch].y > _cells[pointer]._middle.y ) j += 2;
                body_ch = _cells[pointer]._child[j];

                _cells[pointer]._center.x += _bodies[i].x * _bodies[i].z;
                _cells[pointer]._center.y += _bodies[i].y * _bodies[i].z;
                _cells[pointer]._center.z += _bodies[i].z;
                _cells[pointer]._num ++;

                if( body_ch == -1)
                {
                    _cells[pointer]._child[j] = ch;
                    break;
                }
                else
                {
                    if(_bodies[body_ch-_noden].x == _bodies[i].x && _bodies[body_ch - _noden].y == _bodies[i].y)
                    {
                        break;
                    }
                    n = pointer;
                    //ch = body_ch;
                }
            }
        }

        triger = true;
        n = 0;
    }
    computeCenter();
    sortBodies();
}


void systemcpu::computeCenter()
{
    for(int i=0; i<= top; i++)
    {
        _cells[i]._center.x /= _cells[i]._center.z;
        _cells[i]._center.y /= _cells[i]._center.z;
    }
}

void systemcpu::sortBodies()
{
    delete[] _sorted;
    delete[] _lsorted;
    _sorted = new unsigned int[(_maxlevel+1)*_n];
    _lsorted = new unsigned int[(_maxlevel+1)*_n];
    for(int i=0; i<(_maxlevel+1)*_n; i++)
    {
        _sorted[i] = -1;
        _lsorted[i] = -1;
    }

    unsigned int root = 0;
    unsigned int index = 0;
    vector<unsigned int> temp;

    temp.push_back(root);
    while(temp.size()!=0)
    {

        root=temp.back();
        temp.pop_back();
        _sorted[index] = root;
        index++;

        if(root>=_noden)
            continue;
        _lsorted[index-1] = _cells[root]._level;

        if (_cells[root]._child[3]!=-1)
            temp.push_back( _cells[root]._child[3]);
        if (_cells[root]._child[2]!=-1)
            temp.push_back( _cells[root]._child[2]);
        if (_cells[root]._child[1]!=-1)
            temp.push_back( _cells[root]._child[1]);
        if (_cells[root]._child[0]!=-1)
            temp.push_back( _cells[root]._child[0]);
    }

}

void systemcpu::computeForce()
{

    float c = 10 * _tstep * _tstep /(4.0f * M_PI); 
    for(int i=0; i<_n; i++)
    {
        float ax=0, ay=0;

        for(int j=0; j<_n; j++)
        {
            if(i!=j)
            {

                float dx = _bodies[j].x - _bodies[i].x;
                float dy = _bodies[j].y - _bodies[i].y;
                float ds = dx*dx + dy*dy + 0.025;
                float r = sqrtf(ds);
                float a = c  / r;
                ax += a*dx/r;
                ay += a*dy/r;

            }
        }

        float dx = _MS/2 - _bodies[i].x;
        float dy = _MS/2 - _bodies[i].y;
        float ds = dx*dx + dy*dy;
        float r = sqrtf(ds);
        float ha=0,hax=0,hay=0;
        if(r > _as )
        {
            ha = c*_NH  / ds;
        }
        else
        {
            float coscita = cos(asin(r/_as));
            ha = c * _NH * (1 - coscita * coscita *coscita ) / ds;

        }
        hax = ha*dx/r;
        hay = ha*dy/r;

        ax+=hax;
        ay+=hay;
        
        _v[i].x += (ax-_a[i].x) * _tstep * 0.5;
        _v[i].y += (ay-_a[i].x) * _tstep * 0.5;
        _a[i].x = ax;
        _a[i].y = ay;


    }
}

void systemcpu::traverseTree()
{

    float c =  10 * _tstep * _tstep /(4.0f * M_PI); 
    for(int i=0; i<_n; i++)
    {

        unsigned int root = 0;
        unsigned int index = 0;
        vector<unsigned int> temp;
        float ax=0, ay=0;

        temp.push_back(root);
        while(temp.size()!=0)
        {
            root=temp.back();
            temp.pop_back();
            index++;

            if(root>=_noden && _cells[root]._num!=1)
            {
                unsigned int j = root - _noden; 
                if(i!=j && j < _n)
                {

                    float dx = _bodies[j].x - _bodies[i].x;
                    float dy = _bodies[j].y - _bodies[i].y;
                    float ds = dx*dx + dy*dy + 0.025;
                    float r = sqrtf(ds);
                    float a = c  / r;
                    ax += a*dx/r;
                    ay += a*dy/r;

                }
                continue;
            }
            else
            {
                float dx = _cells[root]._middle.x - _bodies[i].x;
                float dy = _cells[root]._middle.y - _bodies[i].y;
                float ds = dx*dx + dy*dy + 0.025;
                float r = sqrtf(ds);
                //ax += a*dx/r;
                //ay += a*dy/r;
                if( _cells[root]._r/r < 0.5)
                {
                    float a = c*_cells[root]._num  / r;
                    ax += a*dx/r;
                    ay += a*dy/r;
                    //cout << ax << " "<< _cells[root]._num<<endl;
                    continue;
                }
                else
                {
                    
                }
            }

            if (_cells[root]._child[3] < _noden + _n)
                temp.push_back( _cells[root]._child[3]);
            if (_cells[root]._child[2] < _noden + _n)
                temp.push_back( _cells[root]._child[2]);
            if (_cells[root]._child[1] < _noden + _n)
                temp.push_back( _cells[root]._child[1]);
            if (_cells[root]._child[0] < _noden + _n)
                temp.push_back( _cells[root]._child[0]);
        }

        float dx = _MS/2 - _bodies[i].x;
        float dy = _MS/2 - _bodies[i].y;
        float ds = dx*dx + dy*dy;
        float r = sqrtf(ds);
        float ha=0,hax=0,hay=0;
        if(r > _as )
        {
            ha = c  /ds;//_NH  / ds;
        }
        else
        {
            float coscita = cos(asin(r/_as));
            ha = c * _NH * (1 - coscita * coscita *coscita ) / ds;

        }
        hax = ha*dx/r;
        hay = ha*dy/r;

        ax+=hax;
        ay+=hay;
        
        _v[i].x += (ax-_a[i].x) * _tstep * 0.5;
        _v[i].y += (ay-_a[i].x) * _tstep * 0.5;
        _a[i].x = ax;
        _a[i].y = ay;
    }
}

void systemcpu::updateBodies()
{
    for(int i=0; i<_n; i++)
    {
        float dax = _a[i].x * _tstep*0.5;
        float day = _a[i].y * _tstep*0.5;
        float vx = _v[i].x + dax;
        float vy = _v[i].y + day;
        _bodies[i].x += vx*_tstep; 
        _bodies[i].y += vy*_tstep;
        _v[i].x = vx + dax;
        _v[i].y = vy + day;

    }
}

void systemcpu::bounding()
{
    point2 minp(FLT_MAX,FLT_MAX),maxp(FLT_MIN,FLT_MIN);

    for(int i = 0; i<_n; i++)
    {
        minp = minfloat3(_bodies[i],minp);
        maxp = maxfloat3(_bodies[i],maxp); 
    }

    // bounding the root node;

    //child pointer to null;
    for(int i=0; i <= top; i++)
    {
        _cells[i]._center.x = 0;
        _cells[i]._center.y = 0;
        _cells[i]._child[0] = -1;
        _cells[i]._child[1] = -1;
        _cells[i]._child[2] = -1;
        _cells[i]._child[3] = -1;
    }
    _cells[0].middle(maxp,minp);

    top = 0; 

}
