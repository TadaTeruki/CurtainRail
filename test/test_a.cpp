
#include "../curtain_rail.cpp"

#include <iostream>
#include <random>
#include <vector>

template<class KEY, class VALUE>
void test(unsigned int repeat = 0, unsigned int search = 0){

    const int dim = 2;
    const int n = 100000;
    const int maxval = 10000;
    const int maxnoise = 100;

    std::random_device rd;
    std::vector<std::vector<VALUE>> data = std::vector<std::vector<VALUE>>(n,std::vector<VALUE>(dim, 0));
    std::vector<std::vector<VALUE>> range = std::vector<std::vector<VALUE>>(dim,std::vector<VALUE>(2, 0));
    std::vector<KEY> res;

    teruki_lib::curtain_rail<KEY, VALUE, dim> cont;
    int ad=0;
    for(auto& it:data){
        for(auto& jt:it){
            jt = rd()%maxval;
            //std::cout<<jt<<std::endl;
        }
        cont.insert(ad, it);
        ad++;
    }

    for(auto& it:range){
        it[0] = maxval/4;
        it[1] = maxval/4*3;
    }

    for(int r=0;r<search;r++){

        cont.search(range);

        int cont_size = 0;
        for(auto it=cont.begin(); it!=cont.end(); it++){
            cont_size++;
        }

        int actual_size = 0;
        for(int i=0;i<data.size();i++){
            bool res = true;
            for(int d=0;d<dim;d++){
                if(data[i][d] < range[d][0] || data[i][d] > range[d][1]){
                    res = false;
                }
            }
            actual_size += res;
        }

        if(cont_size != actual_size){
            std::cout<<"extracted data size is invalid"<<std::endl;
        }

        for(auto& it:range){
            int noise = rd()%(maxnoise*2)-maxnoise;
            it[0] += noise;
            it[1] += noise;
        }
            
    }

    
    if(repeat != 0) test<KEY, VALUE>(repeat-1, search);
}

int main(){
    int A = 50;
    int B = 30;
    // 1回の索引とB回の再索引 をA回繰り返すテスト
    test<int, int>(A,B);
}
