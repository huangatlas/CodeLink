#include "Block.h"
#include "Desk.h"
#include "Link.h"
#include "CodeEditor.h"
using namespace std;
using namespace nana;
Block::Block(Desk* f,string s,int x,int y,int w,int h)
{
    dsk=f;
    id=dsk->blockset.size();
    dsk->blockset.push_back(this);
    create(*dsk, rectangle(x, y, w, h));
    caption(s);
    bs=UnSelected;
    bgcolor(colors::white);
    dg.trigger(*this);
    dg.target(*this);
    events().dbl_click([this]()
    {
        CodeEditor ce(this);
    });
    events().mouse_down([this](const arg_mouse& e)
    {
        if(e.left_button)
        {
            dsk->curblock=this;
            if(dsk->curlink!=nullptr)
                dsk->curlink->unFocus();
            cout<<caption()<<s(id)<<endl;
        }
    });
    events().mouse_up([this]()
    {
        bs=UnSelected;
        dg.target(*this);
    });
    events().mouse_move([this](const arg_mouse& e)
    {
        int x=e.pos.x;
        int y=e.pos.y;
        int side=checkBorder(x,y);
        cursor(changeCursor(side));
        if((side==LeftBottomCorner||side==LeftTopCorner||side==RightBottomCorner||side==RightTopCorner)&&e.left_button)
        {
            bs=ReSizing;
            sSide=side;
        }
        if(bs==ReSizing)
        {
            dg.remove_target(*this);
            cout<<"romove target"<<endl;
            int w=size().width;
            int h=size().height;
            int dw,dh;
            if(sSide==LeftTopCorner||sSide==LeftBottomCorner)
            {
                dw=2-x;
                w=w+dw;
                move(pos().x-dw,pos().y);
            }
            if(sSide==RightTopCorner||sSide==RightBottomCorner)
            {
                dw=x-w+2;
                w=w+dw;
            }
            if(sSide==LeftTopCorner||sSide==RightTopCorner)
            {
                dh=2-y;
                h=h+dh;
                move(pos().x,pos().y-dh);
            }
            if(sSide ==LeftBottomCorner||sSide==RightBottomCorner)
            {
                dh=y-h+2;
                h=h+dh;
            }
            w=max(12,w);
            h=max(12,h);
            size(rectangle(2,2,w,h));
            setPorts(info.itype.size(),1);
        }
    });
    events().resizing([this]{drawing{*dsk} .update();});
    events().move([this]{drawing{*dsk} .update();});
    events().key_press([this](const arg_keyboard& e)
    {
        cout<<"key_press"<<endl;
        if(e.key==keyboard::os_del)
            dsk->deleteBlock();
    });
}
void Block::setInports(int n)
{
    clearInPorts();
    nana::size siz=size();
    int dy=siz.height/(n+1);
    int y=dy;
    for(int i=0; i<n; i++)
    {
        Port* P=new Port(this,InPort,i,0,y);
        inps.push_back(P);
        y=y+dy;
    }
}
void Block::setOutports(int n)
{
    clearOutPorts();
    nana::size siz=size();
    int dy=siz.height/(n+1);
    int y=dy;
    for(int i=0; i<n; i++)
    {
        Port* P=new Port(this,OutPort,i,siz.width-8,y);
        outps.push_back(P);
        y=y+dy;
    }
}
void Block::setPorts(int ins,int outs)
{
    setInports(ins);
    setOutports(outs);
}
void Block::clearPorts()
{
    clearInPorts();
    clearOutPorts();
}
void Block::clearInPorts()
{
    for(unsigned int i=0; i<inps.size(); i++)
    {
        delete inps[i];
    }
    inps.clear();
}
void Block::clearOutPorts()
{
    for(unsigned int i=0; i<outps.size(); i++)
    {
        delete outps[i];
    }
    outps.clear();
}
point Block::inport(int i)
{
    point p=pos()+inps[i]->cpos();
    return p;
}
point Block::outport(int i)
{
    point p=pos()+outps[i]->cpos();
    return p;
}
int Block::getHeight()
{
    nana::size z=size();
    return z.height;
}
int Block::getWidth()
{
    nana::size z=size();
    return z.width;
}
string Block::getFileName()
{
    return caption()+".cpp";
}
bool Block::isEmpty()
{
    return caption().empty();
}
int Block::checkBorder(int x,int y)
{
    int dz=4;
    int w=size().width;
    int h=size().height;
    if(x<0||x>w||y<0||y>h)
        return OutSide;
    if(x>=w-dz&&y>=h-dz)
        return RightBottomCorner;
    if(x<=dz&&y>=h-dz)
        return LeftBottomCorner;
    if(x<=dz&&y<=dz)
        return LeftTopCorner;
    if(x>=w-dz&&y<=dz)
        return RightTopCorner;
    if(x>=w-dz)
        return RightSide;
    if(x<=dz)
        return LeftSide;
    if(y>=h-dz)
        return BottomSide;
    if(y<=dz)
        return TopSide;
    return InSide;
}
nana::cursor Block::changeCursor(int side)
{
    if(side==LeftTopCorner)
        return nana::cursor::size_top_left;
    if(side==RightTopCorner)
        return nana::cursor::size_top_right;
    if(side==LeftBottomCorner)
        return nana::cursor::size_bottom_left;
    if(side==RightBottomCorner)
        return nana::cursor::size_bottom_right;
    return nana::cursor::arrow;
}
Block::~Block()
{
    clearPorts();
}
