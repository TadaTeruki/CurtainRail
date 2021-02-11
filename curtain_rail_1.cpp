#include <map>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <initializer_list>

namespace teruki_lib{

// curtain_rail構造 - 実装方針[1]

// 鍵クラス、値クラス、空間の次元
template <class INDEX, class VALUE, int DIM = 2>
class curtain_rail_1{

    // 先頭/末尾ポインタを表現する型。
    using data_iterator =
        typename std::multimap<VALUE, INDEX>::iterator;

    // 引数用の格納クラス。
    template <class CLASS>
    using cr_list =
        std::initializer_list<CLASS>;
    // 
    struct cr_base_value{ VALUE value[DIM*2]; bool extracted[DIM]; };

    // 任意の鍵データに対応する値を検索するデータ構造。
    std::unordered_map<INDEX, cr_base_value> base;
    // 任意の値に対応する鍵データを検索するデータ構造。
    std::multimap<VALUE, INDEX> data[DIM];

    std::unordered_set<INDEX> extraction;

    // 各次元における索引範囲の先頭/末尾を表すポインタ
    data_iterator start_itr[DIM], end_itr[DIM];

    bool cleared = true;

    // ポインタをリセットする。
    void pointer_reset(int _dim){
        start_itr[_dim] = data[_dim].begin();
        end_itr[_dim]   = data[_dim].begin();
    }

    // 全ポインタをリセットする。
    void pointer_reset_all(){
        for(int i = 0; i < DIM; i++)
            pointer_reset(i);
    }

    bool check_extraction_of_unit(INDEX _index){

        cr_base_value* target = &base[_index];
        bool res = true;
        for(int d=0;d<DIM;d++){
            if(target->extracted[d] == false){
                res = false;
            }
        }
        return res;
    }

    void extract_unit(INDEX _index, int _dim){
        cr_base_value* target = &base[_index];
        target->extracted[_dim] = true;
        if(check_extraction_of_unit(_index) == true){
            extraction.insert(_index);
        }

    }

    void pullout_unit(INDEX _index, int _dim){
        if(check_extraction_of_unit(_index) == true){
            extraction.erase(_index);
        }
        base[_index].extracted[_dim] = false;
    }

    // 要素代入時の頭/末尾ポインタの遷移処理に伴うデータ構造の更新。
    void step_for_insertion(INDEX _index, int _dim){

        if( start_itr[_dim] != data[_dim].end()
            and start_itr[_dim]->first <= base[_index].value[_dim]
            and(end_itr[_dim] == data[_dim].end()
                or end_itr[_dim]->first >= base[_index].value[_dim] )
            ){
            extract_unit(_index, _dim);
        }
        else{
            pullout_unit(_index, _dim);
        }

    }

    // 先頭/末尾ポインタの遷移処理時に伴うデータ構造の更新。
    void step(INDEX _index, int _dim, VALUE _val_start, VALUE _val_end){


        if( _val_start <= base[_index].value[_dim]
            and _val_end >= base[_index].value[_dim]
            ){
            extract_unit(_index, _dim);
        }
        else{
            pullout_unit(_index, _dim);
        }

    }


    // 先頭/末尾ポインタを移動させる。
    void slide_pointer(data_iterator & _itr, VALUE _val_start, VALUE _val_end, bool _is_start, int _dim){

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

    // 一つの次元に対する代入処理。
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

    // 一つの次元に対する索引処理。
    void search_unit(VALUE _val_start, VALUE _val_end, int _dim){

        cleared = false;

        slide_pointer(start_itr[_dim], _val_start, _val_end, true , _dim);
        slide_pointer(end_itr[_dim]  , _val_start, _val_end, false, _dim);

    }

    // 一つの次元に対する削除処理。
    bool erase_unit(INDEX _index, int _dim){
        data_iterator itr = find_unit(_index, _dim);
        if(itr == data[_dim].end()) return false;

        if(itr == start_itr[_dim]) start_itr[_dim]++;
        if(itr == end_itr[_dim]) end_itr[_dim]++;
        
        data[_dim].erase(itr);
        return true;
    }

    // 一つの次元に対する要素検索処理。
    data_iterator find_unit(INDEX _index, int _dim){
        VALUE _value = base[_index].value[_dim];

        data_iterator itr = data[_dim].find(_value);

        while(true){

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

    // 索引結果を返すためのイテレータ

    class iterator {

    public:

        typename std::unordered_set<INDEX>::iterator cont_iter;

        INDEX operator*() const noexcept{
            return *cont_iter;
        }

        friend bool operator==(const iterator& a, const iterator& b){
            return a.cont_iter == b.cont_iter;
        }

        friend bool operator!=(const iterator& a, const iterator& b){
            return a.cont_iter != b.cont_iter;
        }

        iterator operator++(int){
            cont_iter++;
            return *this;
        }

        iterator operator--(int){
            cont_iter--;
            return *this;
        }
    };

public:

    curtain_rail_1(){
        pointer_reset_all();
    }

    template<class CONT>
    bool insert_process(INDEX _index, CONT _vals){

        if(_vals.size() != DIM) return false;

        int i = 0;
        cr_base_value cbv;
        for(auto it:_vals){
            cbv.value[i] = it;
            i++;
        }
        for(int d=0;d<DIM;d++)cbv.extracted[d] = false;

        base.emplace(_index, cbv);

        for(int d=0;d<DIM;d++)insert_unit(_index, d, false);

        return true;
    }

    bool insert(INDEX _index, cr_list<VALUE> _vals){
        return insert_process(_index, _vals);
    }

    template<class CONT>
    bool insert(INDEX _index, CONT _vals){
        return insert_process(_index, _vals);
    }

    template<class CONT>
    bool search_process(CONT _l_vals){

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

    bool search(cr_list<cr_list<VALUE>> _l_vals){
        return search_process(_l_vals);
    }

    template<class CONT>
    bool search(CONT _l_vals){
        return search_process(_l_vals);
    }

    void erase(INDEX _index){
        for(int i=0; i<DIM; i++){
            erase_unit(_index, i);
        }
        base.erase(_index);
    }

    iterator begin(){
        iterator res;
        res.cont_iter = extraction.begin();
        return res;
    }

    iterator end(){
        iterator res;
        res.cont_iter = extraction.end();
        return res;
    }

};

} // teruki_lib