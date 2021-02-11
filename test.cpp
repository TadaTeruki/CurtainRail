
#include "curtain_rail.cpp"

#include <iostream>
teruki_lib::curtain_rail<int, double, 2> cont;

int main(){
    cont.insert(0, {8.0 , 13.0});
    cont.insert(1, {13.0, 17.5});
    cont.insert(2, {25.0, 2.5 });
    cont.insert(8, {25.0, 2.5 });
    cont.insert(4, {15.0, 8.0 });
    cont.insert(6, {5.0 , 22.2});

    cont.search({{10.0, 20.0}, {6.0, 18.0}});
    cont.search({{5.0, 9.0}, {2.0, 15.0}});

    cont.erase(4);

    cont.search({{14.0, 30.0}, {2.0, 6.0}});


    for(auto it = cont.begin(); it != cont.end(); it++){
        std::cout<<(*it)<<std::endl;
    }
    
}
