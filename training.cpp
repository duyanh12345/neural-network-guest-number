#include <bits/stdc++.h>
using namespace std;

//*********************** DEFINE CLASS ********************

class neron;

class layer{
    public :
        layer(int num_neron,int n_layer);
        vector < neron > m_layer;
    private:
};

//*********************** CONECTION ***********************

struct conection{
    double weight=1.0f*rand()/RAND_MAX;
    double deltaweight=0;
};

//************************ NERON ***************************

class neron{
    public :
        neron(int n_layer);
        vector < conection > m_neron;
        double output;
        void calOutput(layer &prelayer,int n_index);
        double getOutput();
        double gradient;
    private :
};

neron::neron(int n_layer){

    for(int i=0;i<n_layer;++i)
    {
        m_neron.push_back(conection());
    }
}

void neron::calOutput(layer &prelayer,int n_index)
{   
    output=0;

    for(int i=0;i<prelayer.m_layer.size();++i)
    {   
        output+=prelayer.m_layer[i].getOutput()*prelayer.m_layer[i].m_neron[n_index].weight;
    }
}

double neron::getOutput()
{
    return max(output,0.0)/10;
}

//************************ LAYER **************************

layer::layer(int num_neron,int n_layer){

    for(int i=0;i<=num_neron;++i)
    {
        m_layer.push_back(neron(n_layer));
    }

    m_layer.back().output=1;
}

//*************************** NET ******************************

class net{
    public :
        net(vector < int > &topo);
        vector < layer > m_net;
        void input(vector <double > &inputval);
        void result(vector < double > &resultval);
        void backpro(vector < double > &targetval);
        double alpha=0.15;
        double beta=0.5;
        double eta=1;
    private :
};

net::net(vector < int > &topo){

    for(int cnt=0;cnt<topo.size();++cnt)
    {   
        int n_layer=cnt==topo.size()-1 ? 0 : topo[cnt+1];

        m_net.push_back(layer(topo[cnt],n_layer));
    }
}

void net::input(vector < double > &inputval)
{
    for(int i=0;i<inputval.size();++i)
    {
        m_net[0].m_layer[i].output=inputval[i];
    }
}

void net::result(vector < double > &resultval)
{   
    for(int i=1;i<m_net.size();++i)
    {
        for(int j=0;j<m_net[i].m_layer.size()-1;++j)
        m_net[i].m_layer[j].calOutput(m_net[i-1],j);
    }

    for(int cnt=0;cnt<m_net.back().m_layer.size();++cnt)
    {   
        resultval.push_back(m_net.back().m_layer[cnt].getOutput());
    }
}

void net::backpro(vector < double > &targetval)
{   
    for(int i=0;i<m_net.back().m_layer.size();++i)
    {
        m_net.back().m_layer[i].gradient=2*(m_net.back().m_layer[i].getOutput()-targetval[i])/m_net.back().m_layer.size();
    }

    for(int i=m_net.size()-2;i>=0;--i)
    { 
        for(int j=0;j<m_net[i].m_layer.size();++j)
        {   
            m_net[i].m_layer[j].gradient=0;

            for(int z=0;z<m_net[i].m_layer[j].m_neron.size();++z)
            {   
                conection &tmp=m_net[i].m_layer[j].m_neron[z];
                //dF/dweight
                double val=m_net[i+1].m_layer[z].gradient;
                val*=(m_net[i+1].m_layer[z].getOutput()>0) ? 0.1 : 0;
                val*=m_net[i].m_layer[j].getOutput();
                val=val*alpha+tmp.deltaweight*beta;
                tmp.deltaweight=val;
                tmp.weight-=tmp.deltaweight*eta;
                
                //dF/dgetoutput
                val=m_net[i+1].m_layer[z].gradient;
                val*=(m_net[i+1].m_layer[z].getOutput()>0) ? 0.1 : 0;
                val*=tmp.weight;

                m_net[i].m_layer[j].gradient+=val;
            }
        }
    }
}

//*************************** MAIN *****************************

int numbers_test;
int label[60015];
int image[60015][30][30];
int image1[30][30];

int main (){   

    srand(time(NULL));
    vector < int > topo;
    topo.push_back(784);
    topo.push_back(15);
    topo.push_back(12);
    topo.push_back(10);
    net mynet(topo);
    
    ifstream in_weight;
    in_weight.open("trainingweight.dat");
    
    for(int i=0;i<topo.size();++i)
    for(int j=0;j<mynet.m_net[i].m_layer.size();++j)
    for(int z=0;z<mynet.m_net[i].m_layer[j].m_neron.size();++z)
    in_weight >> mynet.m_net[i].m_layer[j].m_neron[z].weight;

    in_weight.close();

    vector < double > inputval;
    vector <double> resultval;
    vector <double> targetval;

    ios_base :: sync_with_stdio(false);
    
    ifstream input;
    input.open("training_input.dat");

    input >> numbers_test;

    for(int i=0;i<numbers_test;++i)
    {   
        input >> label[i];
        
        int x=28;
        int y=28;

        for(int j1=0;j1<28;++j1)
        for(int j2=0;j2<28;++j2)
        {
            input >> image1[j1][j2];

            if(image1[j1][j2])
            x=min(x,j1),y=min(y,j2);
        }

        for(int j1=0;j1<28;++j1)
        for(int j2=0;j2<28;++j2)
        if(j1+x<28&&j2+y<28)
        image[i][j1][j2]=image1[j1+x][j2+y];
        else
        image[i][j1][j2]=0;

        if(i%1000==999)
        cerr << i/1000<<" ";
    }

    input.close();
    
    cerr << "start\n";

    int rate=0;

    for(int j=0;j<600000;++j)
    {   
        int i=rand()%numbers_test;

        inputval.clear();
        resultval.clear();
        targetval.clear();

        for(int j=0;j<=9;++j)
        if(j==label[i])
        targetval.push_back(1);
        else
        targetval.push_back(0);
        
        for(int j1=0;j1<28;++j1)
        for(int j2=0;j2<28;++j2)
            inputval.push_back(10.0*image[i][j1][j2]/255);

        mynet.input(inputval);
        mynet.result(resultval);
        mynet.backpro(targetval);

        int ans=0;

        for(int j=1;j<10;++j)
        if(resultval[j]>resultval[ans])
        ans=j;

        if(ans==label[i])
        rate++;

        if(j%1000==999)
        cerr <<j<<":"<<100.0f*rate/(j+1)<<"%\n",mynet.eta-=1.0/600;
    }
    
    ofstream out_weight;
    out_weight.open("trainingweight.dat");
    
    for(int i=0;i<topo.size();++i)
    for(int j=0;j<mynet.m_net[i].m_layer.size();++j)
    for(int z=0;z<mynet.m_net[i].m_layer[j].m_neron.size();++z)
    out_weight << mynet.m_net[i].m_layer[j].m_neron[z].weight<<" ";

    out_weight.close();
}