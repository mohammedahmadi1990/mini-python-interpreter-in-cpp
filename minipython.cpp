#include <iostream>
#include <fstream>
#include <regex>

inline std::string trim(std::string str);
bool is_number(const std::string& s);
std::vector<std::string> split (std::string s, std::string delimiter);
void handle_statement(const auto &line,
                      std::vector<std::string>& variables,
                      std::vector<std::string>& functions,
                      std::vector<std::string>& types,
                      std::vector<std::string>& values,
                      std::vector< std::vector<std::string>>& arrays,
                      int& line_no
);
std::string handle_function(std::string function, std::string param);


/********************************************************************
 MINI PYTHON INTERPRETER USING C++
 Developed By: Mohammed Ahmadi
 Course Name:
 Project:

 Requirements to Run:
   GNU C++ (g++)
 Command to Run the Code
   g++ minipython.cpp -o ˜/bin/mini_python
 Description:

*********************************************************************/

/*___ CONST ERROR MESSAGES ___*/
const std::string INPUT_ARG_ERROR = "Error! INPUT argument is not based on template string as below:\n-o test-cases/tc01";
const std::string INPUT_FILE_ERROR = "Error! Could not open the file - Line: ";
const std::string NO_SUCH_VARIABLE = "Error! No Such Variable - Line: ";
const std::string SYNTAX_PRINT = "Error! Syntax dismissed for print function - Line: ";
const std::string SYNTAX_SIGN = "Error! Syntax dismissed for comparison sign! - Line: ";
const std::string VARIABLE_NAME = "Error! Syntax dismissed for variable name! - Line: ";
const std::string SYNTAX_FUNCTION = "Error! Syntax dismissed for function! - Line: ";

/*___ CONST VALUES ___*/
const std::string SIGNS[] = {">=","<=", ">", "<", "!=", "=="};


int main(int argc, char *argv[]) {

    /*___ FIELDS ___*/
    std::string filename;
    std::vector<std::string> lines;
    std::vector<std::string> function;
    std::vector<std::string> operation;
    std::vector<std::string> types;
    std::vector<std::string> variables;
    std::vector<std::string> functions;
    std::vector<std::string> values;
    std::vector< std::vector<std::string>> arrays;

    /*___ PROCESS ARGUMENT ___*/
    if (argc == 3) {
        filename = argv[2];
        if(filename.find(".py")==-1){
            filename = filename + ".py";
        }
    } else {
        std::cout << INPUT_ARG_ERROR << std::endl;
        return EXIT_FAILURE;
    }

    /*___ READ FILE ___*/
    std::ifstream input_file(filename);
    if (!input_file.is_open()) {
        std::cerr << INPUT_FILE_ERROR
             << filename << "'" << std::endl;
        return EXIT_FAILURE;
    }
    std::string line;
    while (getline(input_file, line)){
        lines.push_back(line);
    }
    input_file.close();

    bool def_flag = false;
    bool if_flag = false;
    bool result = false;
    bool elseRun = false;
    std::string temp_func_var = "";
    std::string temp_func_desc = "";
    int line_no = 1;

    for (const auto &line : lines) {
        if(!def_flag && temp_func_desc != ""){
            types.push_back("function");
            values.push_back( std::to_string(functions.size()));
            variables.push_back(temp_func_var);
            functions.push_back(temp_func_desc);
            temp_func_desc = "";
        }
        if (line.find("def") != -1 || def_flag) {
            def_flag = true;
            std::string trimmed_line = trim(line);
            if(trimmed_line.find("(") != -1 && trimmed_line.find(")") != -1 && trimmed_line.ends_with(":") ){
                temp_func_var = trimmed_line.substr(4,trimmed_line.find("(")-4);
            }
            if(line.find("  ") != -1) {
                def_flag = false;
            }
            temp_func_desc = temp_func_desc +"\n" + line;
        } else if (line.find("print") != -1) {
            std::string trimmed_line = trim(line);
            if(trimmed_line.ends_with(';'))
                trimmed_line = trimmed_line.substr(0,-2);
            if(trimmed_line.substr(0,5)=="print") {
                std::string sub = trimmed_line.substr(5);
                if(sub.starts_with('(') && sub.ends_with(')')) {
                    sub = sub.substr(1,sub.length()-2);
                    if((sub.starts_with('"') && sub.ends_with('"')) ||(sub.starts_with('\'') && sub.ends_with('\'') )) {
                        sub = sub.substr(1,sub.length()-2);
                        std::cout << sub ;
                    }else{
                        bool flag;
                        for (int i = 0; i < variables.size() ; ++i) {
                            if (variables[i] == sub) {
                                if(types[i]=="int")
                                    std::cout << values[i];
                                else{
                                    std::cout << "[";
                                    for (int j = 0; j < arrays[std::stoi(values[i])].size() ; ++j) {
                                        std::cout << arrays[std::stoi(values[i])][j] ;
                                        if(j != arrays[std::stoi(values[i])].size()-1)
                                            std::cout << ", ";
                                    }
                                    std::cout << "]";
                                }
                                flag = true;
                            }
                        }
                        if(!flag)
                            std::cout<< NO_SUCH_VARIABLE << line_no << std::endl;
                    }
                } else{
                    std::cout<< SYNTAX_PRINT << line_no << std::endl;
                }
            }else{
                std::cout<<SYNTAX_PRINT << line_no << std::endl;
            }

        } else if (line.find("if") != -1 || if_flag){
            if_flag = true;
            if(line.find("if") != -1){
                std::string trimmed_line = trim(line);
                if(trimmed_line.starts_with("if") && trimmed_line.ends_with(":")){
                    trimmed_line = trimmed_line.substr(3);
                    trimmed_line = trimmed_line.substr(0,trimmed_line.find(":"));
                    trimmed_line = trim(trimmed_line);
                    std::string first_var;
                    int first_val = 0;
                    std::string second_var;
                    int second_val = 0;

                        for (int i = 0; i < sizeof(SIGNS); ++i) {
                            if(trimmed_line.find(SIGNS[i]) != -1) {
                                first_var = trimmed_line.substr(0, trimmed_line.find(SIGNS[i]));
                                if (is_number(first_var))
                                    first_val = std::stoi(first_var);
                                else {
                                    for (int i = 0; i < variables.size(); ++i) {
                                        if (first_var == variables[i])
                                            first_val = std::stoi(values[i]);
                                    }
                                }

                                second_var = trimmed_line.substr(trimmed_line.find(SIGNS[i]) + 1,
                                                                 trimmed_line.length());
                                if (is_number(second_var))
                                    second_val = std::stoi(second_var);
                                else {
                                    for (int i = 0; i < variables.size(); ++i) {
                                        if (second_var == variables[i])
                                            second_val = std::stoi(values[i]);
                                    }
                                }
                                if(SIGNS[i] == ">="){
                                    if(first_val >= second_val)
                                        result = true;
                                }else if(SIGNS[i] == "<="){
                                    if(first_val <= second_val)
                                        result = true;
                                }else if(SIGNS[i] == ">"){
                                    if(first_val > second_val)
                                        result = true;
                                }else if(SIGNS[i] == "<"){
                                    if(first_val < second_val)
                                        result = true;
                                }else if(SIGNS[i] == "!="){
                                    if(first_val != second_val)
                                        result = true;
                                }else if(SIGNS[i] == "=="){
                                    if(first_val == second_val)
                                        result = true;
                                }else{
                                    std::cout << SYNTAX_SIGN << line_no << std::endl;
                                }
                                break;
                            }

                        }
                }
            }
            else if(line.find("else") != -1){
                if(!result)
                    elseRun = true;
                result = false;
            }else{
                if(result){
                    handle_statement(line, variables, functions, types, values, arrays, line_no);
                }else{
                    if(elseRun)
                        handle_statement(line, variables, functions, types, values, arrays, line_no);
                }
            }
        }else if (line.find("=") != -1) {
            handle_statement(line, variables, functions, types, values, arrays, line_no);
        }else{
            // ignore
        }
        line_no++;
    }
    return EXIT_SUCCESS;
}

// function for trimming white spaces
inline std::string trim(std::string str)
{
    str.erase(str.find_last_not_of(' ')+1);             //suffixing spaces
    str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
    return str;
}

void handle_statement(const auto &line,
                      std::vector<std::string>& variables,
                      std::vector<std::string>& functions,
                      std::vector<std::string>& types,
                      std::vector<std::string>& values,
                      std::vector< std::vector<std::string>>& arrays,
                      int& line_no
              ){
    std::string trimmed_line = trim(line);
    if(trimmed_line.ends_with(';'))
        trimmed_line = trimmed_line.substr(0,-2);
    std::string name  = trimmed_line.substr(0,trimmed_line.find("="));
    std::string value  = trimmed_line.substr(trimmed_line.find("=")+1,trimmed_line.length());
    if(name.find("\\") != -1 || name.find(":") != -1 || name.find("\"") != -1 || name.find("'") != -1 || name.find(";") != -1 || name.find("/") != -1)
        std::cout << VARIABLE_NAME << line_no << std::endl;
    else{
        name = trim(name);
        variables.push_back(name);
    }
    if(value.find("+") != -1){
        // save the added value
        std::string sub1 = value.substr(0,value.find("+"));
        std::string sub2 = value.substr(value.find("+")+1,-2);
        int sub11 = 0;
        int sub22 = 0;
        int sum = 0;
        bool array1_flag = false;
        bool array2_flag = false;
        std::vector<std::string> arr1;
        std::vector<std::string> arr2;
        std::vector<std::string> result;
        if(is_number(sub1))
            sub11 = std::stoi( sub1 );
        else
            for (int i = 0; i < variables.size() ; ++i)
                if(trim(variables[i])==trim(sub1)) {
                    sub11 = std::stoi(values[i]);
                    if(types[i]=="array") {
                        array1_flag = true;
                        arr1 = arrays[sub11];
                    }
                }

        if(is_number(sub2))
            sub22 = std::stoi( sub2 );
        else
            for (int i = 0; i < variables.size() ; ++i)
                if(trim(variables[i])==trim(sub2)) {
                    sub22 = std::stoi(values[i]);
                    if(types[i]=="array") {
                        array2_flag = true;
                        arr2 = arrays[sub22];
                    }
                }

        if(array1_flag && array2_flag){
            for (int i = 0; i < arr1.size(); ++i) {
                result.push_back(trim(arr1[i]));
            }
            for (int i = 0; i < arr2.size(); ++i) {
                result.push_back(trim(arr2[i]));
            }

            types.push_back("array");
            values.push_back(std::to_string(arrays.size()));
            arrays.push_back(result);
        }else {
            sum = sub11 + sub22;
            types.push_back("int");
            values.push_back(std::to_string(sum));
        }

    }else{
        // no number
        if(value.find("[") != -1 && value.find("]") != -1) {
            value = trim(value);
            if(value.find(",") != -1){
                value = value.substr(1, value.length() - 2);
                std::vector<std::string> vect = split(value, ",");
                types.push_back("array");
                values.push_back(std::to_string(arrays.size()));
                arrays.push_back(vect);
            }else{
                std::string sub_var = value.substr(0, value.find("["));
                sub_var = trim(sub_var);
                value = value.substr( value.find("[")+1);
                int sub_val = std::stoi(value.substr(0,value.length()-1));
                int sub_res = 0;
                for (int i = 0; i < variables.size(); ++i) {
                    if(sub_var==variables[i] && types[i]=="array")
                        sub_res = std::stoi(arrays[std::stoi(values[i])][sub_val]);
                }
                types.push_back("int");
                values.push_back(std::to_string(sub_res));
            }

        }else{
            if(value.find("(") != -1 && value.find(")") != -1){
                std::string func_name = value.substr(0,value.find("("));
                func_name = trim(func_name);
                std::string func_param = value.substr(value.find("(")+1);
                func_param = func_param.substr(0, func_param.length()-1);
                func_name = trim(func_name);
                func_param = trim(func_param);

                for (int i = 0; i < variables.size(); ++i) {
                    if(func_name==variables[i] && types[i]=="function"){
                        std::string result = handle_function(functions[std::stoi(values[i])],func_param);
                        if(result != "") {
                            types.push_back("int");
                            values.push_back(result);
                        }else
                        {
                            std::cout<< SYNTAX_FUNCTION << line_no << std::endl;
                        }
                    }
                }
            }else {
                types.push_back("int");
                values.push_back(value);
            }
        }

    }
}

std::string handle_function(std::string function, std::string param)
{
    std::string arg = "";
    std::string output = "";
    arg = function.substr(0,function.find(":"));
    arg = trim(arg);
    arg = arg.substr(arg.find("(")+1,arg.length()-arg.find(")"));
    arg = trim(arg);
    if(function.find("return") != -1) {
        output = function.substr(function.find("return")+7);
    }
    if(arg == trim(output))
        return param;
    return "";
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

std::vector<std::string> split (std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

