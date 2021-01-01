/******************************************************************* 
 * 
 *    @Time           @Author       @Description
 * ------------      ---------      -----------
 * 2020-12-24 22:03   Lin Sinan         None
 *
 * @File    :   Value.py    
 * @Contact :   mynameisxiaou@gmail.com
 * @github  :   https://github.com/linsinan1995
 * @usage   :   
 * 
 ******************************************************************/

//
// Created by Lin Sinan on 2020-12-24.
//

#include "Interpreter/Runtime.h"
#include "Interpreter/Value.h"

using namespace runtime_ns;

// ctor
RT_Value::RT_Value(float val) : type(FP), data(val) {};
RT_Value::RT_Value(bool val) : type(BOOL), data(val) {};
RT_Value::RT_Value(int val) : type(INT), data(val) {};
// placement new & tagged union for storing mat in union
RT_Value::RT_Value(Mat val) : type(MATRIX) { new (&data.matrix) Mat(std::move(val)); };
RT_Value::RT_Value(std::string val) : type(STRING), data(std::move(val)) {};
RT_Value::RT_Value(Object val) : type(OBJECT) { new (&data.obj) Object(std::move(val)); }

RT_Value RT_Value::operator+(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp + rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int + rhs.data._int);
    }

    if (is_type<MATRIX>() && rhs.is_type<FP>()) {
        Mat mat;
        mat.data = data.matrix.data;
        mat.dim = data.matrix.dim;
        for (float & i : mat.data) i += rhs.data.fp;
        return RT_Value(std::move(mat));
    }

    if (is_type<FP>() && rhs.is_type<MATRIX>()) {
        Mat mat;
        mat.data = rhs.data.matrix.data;
        mat.dim = rhs.data.matrix.dim;
        for (float & i : mat.data) i += data.fp;
        return RT_Value(std::move(mat));
    }

    if (is_type<MATRIX>() && rhs.is_type<MATRIX>()) {
        if (data.matrix.dim.size() != rhs.data.matrix.dim.size())
            return panic_type<RT_Value>("Runtime Error: wrong shape of matrix!\n");

        for (int i=0; i < data.matrix.dim.size(); i++) {
            if (data.matrix.dim[i] != rhs.data.matrix.dim[i])
                return panic_type<RT_Value>("Runtime Error: wrong shape of matrix!\n");
        }

        Mat mat;
        mat.data = data.matrix.data;
        mat.dim = data.matrix.dim;
        for (int i = 0; i < data.matrix.data.size(); i++) {
            mat.data[i] += rhs.data.matrix.data[i];
        }
        return RT_Value(std::move(mat));
    }

    return RT_Value();
}

RT_Value RT_Value::operator-(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp - rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int - rhs.data._int);
    }

    if (is_type<MATRIX>() && rhs.is_type<FP>()) {
        Mat mat;
        mat.data = data.matrix.data;
        mat.dim = data.matrix.dim;
        for (float & i : mat.data) i -= rhs.data.fp;
        return RT_Value(std::move(mat));
    }

    if (is_type<MATRIX>() && rhs.is_type<MATRIX>()) {
        if (data.matrix.dim.size() != rhs.data.matrix.dim.size())
            return panic_type<RT_Value>("Runtime Error: wrong shape of matrix!\n");

        for (int i=0; i < data.matrix.dim.size(); i++) {
            if (data.matrix.dim[i] != rhs.data.matrix.dim[i])
                return panic_type<RT_Value>("Runtime Error: wrong shape of matrix!\n");
        }

        Mat mat;
        mat.data = data.matrix.data;
        mat.dim = data.matrix.dim;
        for (int i = 0; i < data.matrix.data.size(); i++) {
            mat.data[i] -= rhs.data.matrix.data[i];
        }
        return RT_Value(std::move(mat));
    }
    return RT_Value();
}


void multiply_mat_mat(const std::vector<float> &mat_a, const std::vector<float> &mat_b,
                      std::vector<float> &mat_c, int I, int J, int K) {
    for (int i = 0; i < I; i++) {
        for (int j = 0; j < J; j++) {
            for (int k = 0; k < K; k++) {
                int mat_a_idx = i * K + k;
                int mat_b_idx = k * J + j;
                int mat_c_idx = i * J + j;
                mat_c[mat_c_idx] += mat_b[mat_b_idx] *
                                    mat_a[mat_a_idx];
            }
        }
    }

}

void multiply_vec_mat(const std::vector<float> &vec_a, const std::vector<float> &mat_b,
                      std::vector<float> &vec_c, int I, int K) {
    for (int i = 0; i < I; i++) {
        for (int k = 0; k < K; k++) {
            int vec_a_idx = k;
            int mat_b_idx = k * I + i;
            int vec_c_idx = i;
            vec_c[vec_c_idx] += vec_a[vec_a_idx] *
                                mat_b[mat_b_idx];
        }
    }
}

void multiply_mat_vec(const std::vector<float> &mat_a, const std::vector<float> &vec_b,
                      std::vector<float> &vec_c, int I, int K) {
    for (int i = 0; i < I; i++) {
        for (int k = 0; k < K; k++) {
            int mat_a_idx = i * K + k;
            int vec_b_idx = k;
            int vec_c_idx = i;
            vec_c[vec_c_idx] += mat_a[mat_a_idx] *
                                vec_b[vec_b_idx];
        }
    }
}

RT_Value RT_Value::operator*(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp * rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int * rhs.data._int);
    }


    if ((is_type<FP>() && rhs.is_type<MATRIX>()) ||
        (is_type<MATRIX>() && rhs.is_type<FP>())) {
        Mat mat;
        mat.data = data.matrix.data;
        mat.dim = data.matrix.dim;
        for (float & i : mat.data) i *= rhs.data.fp;
        return RT_Value(std::move(mat));
    }

    if (is_type<MATRIX>() && rhs.is_type<MATRIX>()) {
        if (data.matrix.dim.size() > 2 || rhs.data.matrix.dim.size() > 2)
            return panic_type<RT_Value>("Runtime Error: Doesn't support matrix with more than 2 dimensions!\n");


        // one dimension
        if (data.matrix.dim.size() == 1 && rhs.data.matrix.dim.size() == 1) {
            if (data.matrix.dim[0] != rhs.data.matrix.dim[0])
                return panic_type<RT_Value>("Runtime Error: Wrong shape of matrix! "
                                            "The shape of matrix_1 is %d, but the "
                                            "shape of matrix_2 is %d\n",
                                            data.matrix.dim[0], rhs.data.matrix.dim[0]);
            float res = 0;
            for (int i = 0; i < data.matrix.data.size(); i++) {
                res += data.matrix.data[i] * rhs.data.matrix.data[i];
            }
            return RT_Value(res);
        }

        Mat mat;

        int size_of_mat;
        if ((data.matrix.dim.size() == 1) && rhs.data.matrix.dim[0] == data.matrix.dim[0]) {
            size_of_mat = rhs.data.matrix.dim[1];
            mat.dim = std::vector<int> { size_of_mat };
            mat.data = std::vector<float>(size_of_mat);
            int I = rhs.data.matrix.dim[1], K = rhs.data.matrix.dim[0];
            multiply_vec_mat(data.matrix.data, rhs.data.matrix.data,
                             mat.data, I, K);
        } else if ((rhs.data.matrix.dim.size() == 1) && data.matrix.dim[1] == rhs.data.matrix.dim[0]) {
            size_of_mat = data.matrix.dim[0];
            mat.dim = std::vector<int> { size_of_mat };
            mat.data = std::vector<float>(size_of_mat);
            int I = data.matrix.dim[0], K = data.matrix.dim[1];
            // one dimension
            multiply_mat_vec(data.matrix.data, rhs.data.matrix.data,
                             mat.data, I, K);

        } else if (rhs.data.matrix.dim.size() == 1 || data.matrix.dim.size() == 1)  {
            return panic_type<RT_Value>("Runtime Error: Wrong shape of matrix!\n");
        } else if (data.matrix.dim[1] == rhs.data.matrix.dim[0]) {
            size_of_mat = rhs.data.matrix.dim[1]*data.matrix.dim[0];
            mat.dim = std::vector<int> {data.matrix.dim[0], rhs.data.matrix.dim[1]};
            mat.data = std::vector<float>(size_of_mat);
            int I = data.matrix.dim[0], J = rhs.data.matrix.dim[1], K = data.matrix.dim[1];
            // two dimension
            multiply_mat_mat(data.matrix.data, rhs.data.matrix.data, mat.data,
                             I, J, K);
        } else
            return panic_type<RT_Value>("Runtime Error: wrong shape of matrix!\n");


        return RT_Value(std::move(mat));
    }
    return RT_Value();
}

RT_Value RT_Value::operator/(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp / rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int / rhs.data._int);
    }

    if (is_type<MATRIX>() && rhs.is_type<FP>()) {
        Mat mat;
        mat.data = data.matrix.data;
        mat.dim = data.matrix.dim;
        for (float & i : mat.data) i /= rhs.data.fp;
        return RT_Value(std::move(mat));
    }

    return RT_Value();
}

RT_Value RT_Value::operator%(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int / rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator>(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp > rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int > rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator<(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp < rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int < rhs.data._int);
    }

    return RT_Value();
}
RT_Value RT_Value::operator==(RT_Value rhs) {

    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp == rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int == rhs.data._int);
    }

    if (is_type<MATRIX>() && rhs.is_type<MATRIX>()) {
        if (data.matrix.dim.size() != rhs.data.matrix.dim.size())
            return panic_type<RT_Value>("Runtime Error: wrong shape of matrix!\n");

        for (int i=0; i < data.matrix.dim.size(); i++) {
            if (data.matrix.dim[i] != rhs.data.matrix.dim[i])
                return panic_type<RT_Value>("Runtime Error: wrong shape of matrix!\n");
        }

        Mat mat;
        mat.data = data.matrix.data;
        mat.dim = data.matrix.dim;
        for (int i = 0; i < data.matrix.data.size(); i++) {
            if (mat.data[i] != rhs.data.matrix.data[i]) return RT_Value(false);
        }

        return RT_Value(true);
    }
    return RT_Value();
}

RT_Value RT_Value::operator>=(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp >= rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int >= rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator<=(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (is_type<FP>() && rhs.is_type<FP>()) {
        return RT_Value(data.fp <= rhs.data.fp);
    }

    if (is_type<INT>() && rhs.is_type<INT>()) {
        return RT_Value(data._int <= rhs.data._int);
    }

    return RT_Value();
}

RT_Value RT_Value::operator^(RT_Value rhs) {
    if (is_type<VOID>() || rhs.is_type<VOID>()) {
        panic("At least one variable is NULL\n");
    }

    if (!(rhs.is_type<INT>())) {
        panic("Runtime Error : the second operand of ^ operator should be integer!\n");
    }

    if (!(rhs.is_type<INT>() || rhs.is_type<FP>())) {
        panic("Runtime Error : the first operand of ^ operator should be integer or float!\n");
    }

    for (int i = 0; i < rhs.data._int; i++) {
        rhs = rhs * rhs;
    }

    return rhs;
}

bool RT_Value::to_bool() {
    if (is_type<BOOL>()) return data._bool;
    if (is_type<VOID>()) return false;
    if (is_type<INT>()) return data._int == 0;
    if (is_type<FP>()) return data.fp == 0.;
    if (is_type<MATRIX>()) return data.matrix.data.empty();
    if (is_type<OBJECT>()) return data.obj.member_functions.empty() &&
                                  data.obj.member_vars.empty();
    panic("Runtime Error : Unexpected RT_Value type!\n");
    return false;
}

RT_Value &RT_Value::operator=(RT_Value val) {
    if(&val == this)
        return *this;

    type = val.type;
    switch (type) {
        default:
            break;
        case FP:
            data.fp = val.data.fp;
            break;
        case INT:
            data._int = val.data._int;
            break;
        case BOOL:
            data._bool = val.data._bool;
            break;
        case STRING:
            data._str = val.data._str;
            break;
        case MATRIX:
            new (&data.matrix) Mat(val.data.matrix);
            break;
        case OBJECT:
            new (&data.obj) Object(val.data.obj);
            break;
    }
    return *this;
}

RT_Value::RT_Value(const RT_Value &val) {
    type = val.type;
    switch (val.type) {
        default:
            break;
        case FP:
            data.fp = val.data.fp;
            break;
        case INT:
            data._int = val.data._int;
            break;
        case BOOL:
            data._bool = val.data._bool;
            break;
        case STRING:
            data._str = val.data._str;
            break;
        case MATRIX:
            new (&data.matrix) Mat(val.data.matrix);
            break;
        case OBJECT:
            new (&data.obj) Object(val.data.obj);
            break;
    }
}

RT_Value::RT_Value(RT_Value &&val) noexcept {
    type = val.type;
    switch (val.type) {
        default:
            break;
        case FP:
            data.fp = val.data.fp;
            break;
        case INT:
            data._int = val.data._int;
            break;
        case BOOL:
            data._bool = val.data._bool;
            break;
        case MATRIX:
            new (&data.matrix) Mat(std::move(val.data.matrix));
            break;
        case STRING:
            data._str = std::move(val.data._str);
            break;
        case OBJECT:
            new (&data.obj) Object(std::move(val.data.obj));
            break;
    }
}

void mat_print_helepr(std::ostream &os, const std::vector<float> &data, const std::vector<int> &dim, int idx = 0, int offset = 0) {
    if (idx + 1 == dim.size()) {
        int len = dim[idx];
        for (int i=0; i<len; i++) {
            os << data[offset+i] << " ";
        }
        os << "\n";
        return;
    }
    // prefix array
    for (int i = 0; i < dim[idx]; i++) {
        mat_print_helepr(os, data, dim, idx+1, offset);
        int off = 1;
        if (idx + 1 < dim.size()) {
            for (int j=idx+1; j<dim.size(); j++)
                off *= dim[j];
        }
        offset += off;
    }
    os << "\n";
}

std::ostream &operator<<(std::ostream &os, const RT_Value &val) {
    switch (val.type) {
        default:
            return os;
        case STRING:
            os << val.data._str;
            return os;
        case FP:
            os << std::to_string(val.data.fp);
            return os;
        case INT:
            os << std::to_string(val.data._int);
            return os;
        case BOOL:
            os << std::boolalpha << val.data._bool;
            return os;
        case MATRIX:
            os << "dims: ";
            for (auto i : val.data.matrix.dim) os << i << " ";
            os << "\n";
            mat_print_helepr(os, val.data.matrix.data, val.data.matrix.dim);
            return os;
        case OBJECT:
            os << "Class: " << val.data.obj.type_name << "\n";
            if (val.data.obj.member_vars.empty()) std::cout << "No member variables!\n";
            else std::cout << "[Member variables]:\n";

            for (auto [var, val] : val.data.obj.member_vars) {
                std::cout << var << " = " << val << "\n";
            }

            if (val.data.obj.member_functions.empty()) std::cout << "No member functions!\n";
            else std::cout << "[Member variables]:\n";

            for (auto [var, val] : val.data.obj.member_functions) {
                std::cout << var << "\n";
            }
            return os;
    }
}

RT_Value* Object::get_variable(const std::string &name) {
    if (auto var = member_vars.find(name); var != member_vars.end()) {
        return var->second;
    }

    return nullptr;
}

RT_Function* Object::get_function(const std::string &name) {
    if (auto var = member_functions.find(name); var != member_functions.end()) {
        return var->second;
    }

    return nullptr;
}

void Object::update_variable(const std::string &name, RT_Value *ptr_value) {
    if (auto var = member_vars.find(name); var != member_vars.end()) {
//        std::cout << "assigning value addr: " << ptr_value << std::endl;
//        std::cout << "assigned value addr: " << var->second << std::endl;
        *var->second = *ptr_value;
//        std::cout << "final value addr: " << var->second << std::endl;
        return;
    }

    panic("Runtime notice: Try to update an invalid"
          " member variable %s in class %s.\n",
          name.c_str(), this->type_name.c_str());
}
