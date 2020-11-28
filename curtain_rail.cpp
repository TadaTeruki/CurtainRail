#include <map>
#include <unordered_map>
#include <functional>
#include <initializer_list>

#include <iostream>

namespace teruki_lib{

template <class INDEX, class VALUE, int DIM = 2>
class curtain_rail{

    using cr_iterator =
        typename std::multimap<VALUE, INDEX>::iterator;

    template <class CLASS>
    using cr_list =
        std::initializer_list<CLASS>;

    struct cr_base_value{ VALUE value[4]; };
    
    std::unordered_map<INDEX, cr_base_value> base;
    std::multimap<VALUE, INDEX> data[DIM];

    cr_iterator start_itr[DIM], end_itr[DIM];

    bool cleared = true;

    void pointer_reset(int _dim){
        start_itr[_dim] = data[_dim].begin();
        end_itr[_dim]   = data[_dim].begin();
    }

    void pointer_reset_all(){
        for(int i = 0; i < DIM; i++)
            pointer_reset(i);
    }

    void step_for_insertion(INDEX _index, int _dim){
        if(_dim == DIM-1) return;

        if( start_itr[_dim] != data[_dim].end()
            and start_itr[_dim]->first <= base[_index].value[_dim]
            and(end_itr[_dim] == data[_dim].end()
                or end_itr[_dim]->first >= base[_index].value[_dim] )
            ){
            insert_unit(_index, _dim+1, false);
        }
        else{
            for(int i=_dim+1; i<DIM; i++){
                if( erase_unit(_index, i) == false) break;
            }
        }
    }

    void step(INDEX _index, int _dim, VALUE _val_start, VALUE _val_end){
        
        if(_dim == DIM-1) return;

        if( _val_start <= base[_index].value[_dim]
            and _val_end >= base[_index].value[_dim]
            ){
            insert_unit(_index, _dim+1, true);
        }
        else{
            
            for(int i=_dim+1; i<DIM; i++){
                if( erase_unit(_index, i) == false) break;
            }
            
        }
    }


    void slide_pointer(cr_iterator & _itr, VALUE _val_start, VALUE _val_end, bool _is_start, int _dim){

        const VALUE _value = _is_start ? _val_start:_val_end;

        const std::function<bool(void)>
            itr_is_backward = [&]()
                {return _is_start ? (_itr->first < _value):(_itr->first <= _value); };
        
        bool to_forward = false;

        while(itr_is_backward() and _itr != data[_dim].end()){
            step(_itr->second, _dim, _val_start, _val_end);
            _itr++;
            to_forward = true;
        }

        if(to_forward) return;
        
        while(_itr != data[_dim].begin()){
            _itr--;
            step(_itr->second, _dim, _val_start, _val_end);

            if(itr_is_backward()){
                _itr++;
                return;
            }
            
        }

        return;
    }

    void insert_unit(INDEX _index, int _dim, bool for_search){

        if(find_unit(_index, _dim) != data[_dim].end()) return;

        data[_dim].emplace(base[_index].value[_dim], _index);
        
        if(for_search == true) return;

        if(cleared == true)
            pointer_reset(_dim);
        else{
            step_for_insertion(_index, _dim);
        }
    }

    void search_unit(VALUE _val_start, VALUE _val_end, int _dim){

        cleared = false;

        slide_pointer(start_itr[_dim], _val_start, _val_end, true , _dim);
        slide_pointer(end_itr[_dim]  , _val_start, _val_end, false, _dim);

    }

    bool erase_unit(INDEX _index, int _dim){
        cr_iterator itr = find_unit(_index, _dim);
        if(itr == data[_dim].end()) return false;

        if(itr == start_itr[_dim]) start_itr[_dim]++;
        if(itr == end_itr[_dim]) end_itr[_dim]++;
        
        data[_dim].erase(itr);
        return true;
    }

    cr_iterator find_unit(INDEX _index, int _dim){
        VALUE _value = base[_index].value[_dim];

        cr_iterator itr = data[_dim].find(_value);

        while(1){

            if(itr == data[_dim].end())
                break;

            if(itr->first  != _value)
                break;

            if(itr->second == _index)
                return itr;

            itr++;
        }

        return data[_dim].end();
    }

    void debug_unit(int _dim){
        for(cr_iterator itr = data[_dim].begin(); itr != data[_dim].end(); itr++){
            if(itr == start_itr[_dim]) std::cout<<"vvv"<<std::endl;
            if(itr == end_itr[_dim]) std::cout<<"^^^"<<std::endl;
            std::cout<<itr->second<<":"<<itr->first<<std::endl;
        }

        if(start_itr[_dim] == data[_dim].end()) std::cout<<"vvv"<<std::endl;
        if(end_itr[_dim] == data[_dim].end()) std::cout<<"^^^"<<std::endl;
    }

public:

    curtain_rail(){
        pointer_reset_all();
    }

    bool insert(INDEX _index, cr_list<VALUE> _vals){

        if(_vals.size() != DIM) return false;

        int i = 0;
        cr_base_value cbv;
        for(auto it:_vals){
            cbv.value[i] = it;
            i++;
        }

        base.emplace(_index, cbv);

        insert_unit(_index, 0, false);

        return true;
    }

    bool search(cr_list<cr_list<VALUE>> _l_vals){

        if(_l_vals.size() != DIM) return false;

        for(auto _vals:_l_vals){
            if(_vals.size() != 2) return false; 
        }

        int i = 0;

        for(auto _vals:_l_vals){
            auto _v_itr = _vals.begin();
            VALUE start = *_v_itr;
            _v_itr++; 
            VALUE end   = *_v_itr;

            search_unit(start, end, i);
            i++;
        }

        return true;
    }

    void erase(INDEX _index){
        for(int i=0; i<DIM; i++){
            erase_unit(_index, i);
        }
        base.erase(_index);
    }

    void debug(){
        for(int i=0; i<DIM; i++){
            debug_unit(i);
            std::cout<<"=========="<<std::endl;
        }
    }

};

} // teruki_lib