#include <algorithm>
#include <utility>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>

using namespace std;
string st[26];int in[256];
string program[100000][5];
string prsize,prname,start,exeaddress;
void m()
{
    in['0']=0; in['1']=1;
    in['2']=2; in['3']=3;
    in['4']=4; in['5']=5;
    in['6']=6; in['7']=7;
    in['8']=8; in['9']=9;
    in['A']=10; in['B']=11;
    in['C']=12; in['D']=13;
    in['E']=14; in['F']=15;

    st[0]="0"; st[1]="1";
    st[2]="2"; st[3]="3";
    st[4]="4"; st[5]="5";
    st[6]="6"; st[7]="7";
    st[8]="8"; st[9]="9";
    st[10]="A"; st[11]="B";
    st[12]="C"; st[13]="D";
    st[14]="E"; st[15]="F";
}
string conv(int i)
{
    string s="";
    while(i!=0)
    {
        s=st[i%16]+s;
        i/=16;
    }
    return s;
}
int conv(string s)
{
    int n=0,l=s.size()-1,p=1;
    for(;l>=0;l--){n+=in[s[l]]*p;p*=16;}
    return n;
}
string intstr(int n)
{
    string s="";
    while(n!=0)
    {
        s=st[n%10]+s;
        n/=10;
    }
    return (s=="")?"0":s;
}
void printprogram()
{
    cout<<"\t"+prname<<"\tSTART\t"<<intstr(conv(start))<<endl;
    for(int i=0;i<100000;i++)
    {
        if(program[i][0]!=""&&i>=conv(start))
        {
            if(program[i][2][0]=='R'&&program[i][2][1]=='E')
            {
                int w=i+1;
                int q;
                if(program[i][2][3]=='W') q=3;
                else q=1;
                while(program[w][0]==""&&(w-i)<=4096*q) w++;
                program[i][3]=intstr((w-i)/q);
            }
            cout<<program[i][4]+"\t"+conv(i)+"\t"+program[i][1]+"\t";
            cout<<program[i][2]+"\t"+program[i][3]<<endl;
        }
    }
    cout<<"\t\tEND\t"<<intstr(conv(exeaddress))<<endl;
}
int main()
{
    m();
    map<string,string> d;
    ifstream finop  ("opcode.txt");
    string opcode,nemonic,tem;
    while(!finop.eof())
    {
        finop>>nemonic>>tem>>opcode;
        d[opcode]=nemonic;
    }
    d["BYTE"]="BYTE";
    d["WORD"]="WORD";
    d["RESB"]="RESB";

    ifstream fin ("easy.txt");
    string line;
    int x,y;
    string lbl="LBL0";
    map<string,string> label;
    string presentaddress;

    string startaddress,codesize,instruction,objcode,mneunomic,operand;
    int recordsize,address;

    while(getline(fin,line))
    {
        if(line[0]=='H')
        {
            prname=line.substr(1,6);
            start=line.substr(7,6);
            prsize=line.substr(13,6);
        }
        else if(line[0]=='E')
        {
            exeaddress=line.substr(1,6);
        }
        else if(line[0]=='T')
        {
            startaddress=line.substr(1,6);
            codesize=line.substr(7,2);
            instruction=line.substr(9,line.size()-8);
            int recordsize=strtoul(codesize.c_str(), NULL, 16);
            address=strtoul(startaddress.c_str(), NULL, 16);
            recordsize*=2;
            int n=0,i;
            while(n<recordsize)
            {
                int m=instruction.size();
                objcode=instruction.substr(n,min(6,m-n));
                mneunomic=objcode.substr(0,2);
                if(objcode.size()==2) operand="";
                else if(objcode.size()==4) operand=objcode.substr(2,2);
                else operand=objcode.substr(2,4);
                presentaddress=conv(address);
       //         cout<<objcode+"\t"+presentaddress<<endl;
                x=conv(presentaddress);
                y=conv(operand);
                i=6;
                if(program[x][0]=="RESW")
                {
                    if(objcode[0]=='0'&&objcode[1]=='0')
                    {
                        program[x][0]="WORD";
                        program[x][1]="    ";
                        program[x][2]="WORD";
                        program[x][3]=intstr(conv(objcode));
                        program[x][4]=objcode;
                        mneunomic="WORD";
                    }
                    else
                    {
                        program[x][0]="BYTE";
                        program[x][1]="    ";
                        program[x][2]="BYTE";
                        operand="";
                        int k=1;
                        while(k<=3&&d[mneunomic]==""&&k*2<=objcode.size())
                        {
                            operand+=mneunomic;
                            mneunomic=objcode.substr(k*2,2);
                            k++;
                        }
                        k--;
                     //   cout<<k<<endl;
                        program[x][3]="X`"+operand+"`";
                        program[x][4]=operand;
                        mneunomic="BYTE";
                        i=k*2;
                    }
                }
                else
                {
                    if(program[x][0]!="LABL") program[x][1]="    ";
                    program[x][0]="INST";
                    program[x][2]=d[mneunomic];
                    program[x][3]=operand;
                    program[x][4]=objcode;
                    if((mneunomic=="3C"||mneunomic=="30"||mneunomic=="34"||mneunomic=="38"||mneunomic=="48"))
                    {
                        program[x][3]=lbl;
                        program[y][0]="LABL";
                        program[y][1]=lbl;
                        lbl[3]=(char)(lbl[3]+1);
                //        cout<<conv(x)+"\t\t"+conv(y)<<endl;
                    }
                    else if(mneunomic=="4C")
                    {
                        program[x][3]="";
                        program[y][0]="";
                    }
                    else if(program[y][0]!="WORD"&&program[y][0]!="INST")
                    {
                        program[y][0]=program[y][2]="RESW";
                    }
                    else
                    {
                        if(program[y][0]=="INST")
                        {
                            program[y][0]="WORD";
                            program[y][2]="WORD";
                            if(program[conv(program[y][3])][0]=="RESW")
                            {
                                program[conv(program[y][3])][0]="    ";
                            }
                            program[y][3]=intstr(conv(program[y][3]));
                        }
                    }
                    if((in[operand[0]]>=8)&&(in[operand[0]]<16))
                    {
                        program[y][0]="";
                        operand[0]=st[in[operand[0]-8]][0];
                        y=conv(operand);
                        program[y][0]=program[y][2]="RESB";
                        operand+=",X";
                        program[x][3]=operand;
                    }
                }
                //if(program[conv("1027")][1]!="") cout<<program[conv("1027")][3]+"|"+"\t"+conv(address)<<endl;
                address+=(i/2);
                n+=i;
         //       cout<<program[x][0]<<endl<<endl;
            }
        }
    }
    printprogram();
}
