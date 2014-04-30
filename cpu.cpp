#include <iostream>
#include <stdlib.h>
#include <vector>
#include <glm/glm.hpp>
using namespace std;


typedef struct node_t
{
    float x;
    float y;
    node_t():x(0.0f),y(0.0f) {};
    node_t(const float& x1, const float& y1): x(x1),y(y1) {};
    inline bool operator >(const node_t& t) const
    { if(x>=t.x && y>=t.y) return true; else return false;}
    inline bool operator <(const node_t& t) const
    { if(x<t.x && y<t.y) return true; else return false;}
} vec2;


typedef struct quad_rect_t
{
    vec2 top_right;
    vec2 bottom_left; // vertices
    unsigned int num_child; // child information
    unsigned int indx_body; // pointer to body
    unsigned int quad_child[4]; // pointer to child
    quad_rect_t(const vec2& bl, const vec2& tr): top_right(tr), bottom_left(bl) {for(int i=0; i<4; i++) quad_child[i]=-1; num_child = 0; indx_body = -1;}

    int quad_partition(const vec2& body)
    {
            
        float mid_xx = (top_right.x + bottom_left.x)/2.0f;
        float mid_yy = (top_right.y + bottom_left.y)/2.0f;
        vector<vec2> temp;
        temp.push_back(vec2(bottom_left.x,mid_yy));
        temp.push_back(vec2(mid_xx,top_right.y));
        temp.push_back(vec2(mid_xx,mid_yy));
        temp.push_back(vec2(top_right));
        temp.push_back(vec2(bottom_left));
        temp.push_back(vec2(mid_xx,mid_yy));
        temp.push_back(vec2(mid_xx,bottom_left.y));
        temp.push_back(vec2(top_right.x,mid_yy));
        for(int i=0; i< temp.size(); i+=2)
        {
            if(body > temp[i] && body < temp[i+1])
                return i/2;
        }
    }

} quad_rect;


struct quad_tree
{    
    typedef enum  
    {  
        TL = 0,  
        TR = 1,  
        BL = 2,  
        BR = 3  
    }QuadrantEnum;


    vector<vec2> _nodes;
    vector<quad_rect> _cell;
    quad_tree(){};
    //build tree
    void quad_tree_build(const quad_rect& root)
    {
        _cell.push_back(root);
    }


    void quad_tree_insert(const vec2& body)
    {
        _nodes.push_back(body);
        unsigned int n=0;
        while(_cell[n].num_child!=0)
        {
            if(_cell[n].quad_child[0]==-1)
            {
                tree_create_branches(n);
                //update current body information
                unsigned int this_nodes = _cell[n].indx_body;
                int this_index_child_1 = _cell[n].quad_partition(_nodes[this_nodes]);
                 
                unsigned int this_branch = _cell[n].quad_child[this_index_child_1];
                //update the body 
                _cell[this_branch].indx_body = _cell[n].indx_body;
                _cell[this_branch].num_child = 1;
                _cell[n].indx_body = -1;
                //update the next body's pointer
                _cell[n].num_child++;

                int next_index = _cell[n].quad_partition(body);
                n = _cell[n].quad_child[next_index];
                
            }
            else
            {
                _cell[n].num_child++;
                int next_index = _cell[n].quad_partition(body);                
                n = _cell[n].quad_child[next_index];
            }
        }
        _cell[n].num_child++;
        _cell[n].indx_body = _nodes.size()-1;
    }
    
    void print()
    {
        cout<< "Child:"<< _cell.back().top_right.x <<"  "<<_cell.back().top_right.y<<endl;
        cout<< "Child:"<< _cell.back().bottom_left.x <<"  "<<_cell.back().bottom_left.y<<endl;
    }


    void tree_create_branches(const int& n)
    {
        quad_rect node(_cell[n]);
        
        float mid_xx = (node.top_right.x + node.bottom_left.x)/2.0f;
        float mid_yy = (node.top_right.y + node.bottom_left.y)/2.0f;
        
        _cell.push_back(    quad_rect(vec2(node.bottom_left.x,mid_yy),vec2(mid_xx,node.top_right.y))   );
        _cell[n].quad_child[TL] = _cell.size()-1;
        _cell.push_back(    quad_rect(vec2(mid_xx,mid_yy), node.top_right)    );
        _cell[n].quad_child[TR] = _cell.size()-1;
        _cell.push_back(    quad_rect(node.bottom_left,vec2(mid_xx,mid_yy))    );
        _cell[n].quad_child[BL] = _cell.size()-1;
        _cell.push_back(    quad_rect(vec2(mid_xx,node.bottom_left.y),vec2(node.top_right.x,mid_yy))   );
        _cell[n].quad_child[BR] = _cell.size()-1;
    }

};


int main(int argc, char** argv)
{
    // compute bounding;
    vector<vec2> nodes;
    vector<quad_rect> cell;
    srand((unsigned)time(NULL));  


    quad_rect root(vec2(0,0),vec2(500,500));

    quad_tree qtree;
    qtree.quad_tree_build(root);

    for(int i=0; i<5000; i++)
    {
        float x =500.0f * rand() / float(RAND_MAX);
        float y =500.0f * rand() / float(RAND_MAX);
        qtree.quad_tree_insert(vec2(x,y));
    }
    for(int i=0; i<1 ; i++)
        cout << qtree._nodes[i].x << " "<< qtree._nodes[i].y<<endl;
    for(int i=0; i<10 ; i++)
        cout << qtree._cell[i].num_child << " "<< qtree._cell[i].indx_body << " " << qtree._cell[i].quad_child[0] <<endl;
    cout<<endl;    

    // build tree


    return 0;
}
