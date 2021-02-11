
#include "../curtain_rail_1.cpp"
#include "../curtain_rail_2.cpp"

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

    teruki_lib::curtain_rail_1<KEY, VALUE, dim> cont_1;
    teruki_lib::curtain_rail_2<KEY, VALUE, dim> cont_2;

    int ad=0;
    for(auto& it:data){
        for(auto& jt:it){
            jt = rd()%maxval;
            //std::cout<<jt<<std::endl;
        }
        cont_1.insert(ad, it);
        cont_2.insert(ad, it);
        ad++;
    }

    for(auto& it:range){
        it[0] = maxval/4;
        it[1] = maxval/4*3;
    }

    for(int r=0;r<search;r++){

        cont_1.search(range);

        int cont_1_size = 0;
        for(auto it=cont_1.begin(); it!=cont_1.end(); it++){
            cont_1_size++;
        }

        cont_2.search(range);

        int cont_2_size = 0;
        for(auto it=cont_2.begin(); it!=cont_2.end(); it++){
            cont_2_size++;
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

        if(cont_1_size != actual_size){
            std::cout<<"cont_1 : extracted data size is invalid"<<std::endl;
        }

        if(cont_2_size != actual_size){
            std::cout<<"cont_2 : extracted data size is invalid"<<std::endl;
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
