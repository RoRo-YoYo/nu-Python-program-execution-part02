/*execute.c*/

//
// << Execute a nuPython program given aporgram and a memory. Only basic straight-code (assignment, print() function call, and pass)>>
//
// << Rocelyn Young>>
// << Northwestern University >>
// << CS211 >>
// << Spring 2025 >>
// 
// Starter code: Prof. Joe Hummel
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>  // true, false
#include <string.h>
#include <assert.h>

#include "programgraph.h"
#include "ram.h"
#include "execute.h"
#include "math.h"


//
// HELPER FUNCTION
//

// Execute function call of print(). Allow for empty, literal string, and variable as parameter. Else, it's error, including undefined varaible
//
//

static bool execute_function_call(struct STMT* stmt, struct RAM* memory) {

   // get function name and check it if's the function call print
  char* function_name = stmt->types.function_call->function_name;

  char* print_function = "print";

  int print_comparison = strcmp(print_function,function_name);

    if (print_comparison == 0 && stmt->types.function_call->parameter == NULL) { // If a print function and no parameter, then print end line
      printf("\n"); 
      return true;
    }
    else if (print_comparison == 0 && stmt->types.function_call->parameter->element_type == ELEMENT_STR_LITERAL) { // if a print function and string literal, then print it with an end line
      printf("%s\n", stmt->types.function_call->parameter->element_value);
      return true;
    }
    else if (print_comparison == 0 && stmt->types.function_call->parameter->element_type == ELEMENT_INT_LITERAL) { // if a print function and string integer, then print it with an end line
      int number = atoi(stmt->types.function_call->parameter->element_value);
      printf("%d\n", number);
      return true;
    }
    else if (print_comparison == 0 && stmt->types.function_call->parameter->element_type == ELEMENT_REAL_LITERAL) { // if a print function and string real, then print it with an end line
      double floating = atof(stmt->types.function_call->parameter->element_value);
      printf("%lf\n", floating);
      return true;
    }
    else if (print_comparison == 0 && stmt->types.function_call->parameter->element_type == ELEMENT_TRUE) { // if a print function and bool True, then print True with an end line
      printf("True\n");
      return true;
    }
    else if (print_comparison == 0 && stmt->types.function_call->parameter->element_type == ELEMENT_FALSE) { // if a print function and bool False, then print False with an end line
      printf("False\n");
      return true;
    }  
    else {
      // check if it is a variable
      struct RAM_VALUE* VALUE = ram_read_cell_by_name(memory,stmt->types.function_call->parameter->element_value);
      if (VALUE != NULL) { // If it does exist, access the variable
        if (VALUE->value_type == RAM_TYPE_STR){
          printf("%s\n", VALUE->types.s);
        }
        else if (VALUE->value_type == RAM_TYPE_INT ){
          printf("%d\n", VALUE->types.i);
        }
        else if (VALUE->value_type == RAM_TYPE_REAL){
          printf("%lf\n", VALUE->types.d);
        }
        else if (VALUE->value_type == RAM_TYPE_BOOLEAN) { // if boolean, check if 0 or 1. Print accordingly
          if (VALUE->types.i == 1) {
            printf("True\n");
          }
          else {
            printf("False\n");
          }
        }
        ram_free_value(VALUE); 
        return true;
      }
      else {
        printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", stmt->types.function_call->parameter->element_value, stmt->line);
        ram_free_value(VALUE); 
        return false;
      }
    }
} 

//
// Helper function for pointer. Access the address of the identifier/pointer. If valid, continue with placing the new value into the address. Else, error and print so
//

static bool Pointer_Helper(struct STMT* stmt, struct RAM* memory,char* identifier,struct RAM_VALUE value){
    // access the address assiocted with the identifier (pointer)
      struct RAM_VALUE* Value_Address = ram_read_cell_by_name(memory,identifier);

      if (Value_Address != NULL) {      
        // write into memory address  instead of identifier
        bool write_by_addrress = ram_write_cell_by_addr(memory,value,Value_Address->types.i); // integer since it's a pointer 
        if (write_by_addrress == false) {  // if address don't already exist, then error
          printf("**SEMANTIC ERROR: invalid memory address for assignment (line %d)\n", stmt->line);
          ram_free_value(Value_Address);
          return false;
        }  
      }
      else {
          printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", identifier, stmt->line);
          ram_free_value(Value_Address);
          return false;
        }
      ram_free_value(Value_Address);
      return true;  
}

//
// Put the assignment value into the memory. Helper function for execute_assignment
//
static bool Putting_Value(struct STMT* stmt, struct RAM* memory, char* identifier, void* result, int RAM_TYPE) {
      // Create value 
        struct RAM_VALUE value;
      if  (RAM_TYPE == RAM_TYPE_INT) { // if int, cast into int
        value.value_type = RAM_TYPE;
        int int_result = *((int*) result ); 
        value.types.i = int_result;  }

      else if  (RAM_TYPE == RAM_TYPE_REAL) { // if real, cat into real
        value.value_type = RAM_TYPE;
        double real_result = *((double*) result ); 
        value.types.d = real_result;}

      else if  (RAM_TYPE == RAM_TYPE_STR) { // If string, cast into string
        value.value_type = RAM_TYPE;
        char* string_result = *((char**) result ); 
        value.types.s = string_result;}

      else if  (RAM_TYPE == RAM_TYPE_BOOLEAN) { // If bool, cast into int
        value.value_type = RAM_TYPE;
        int bool_result = *((int*) result );
        value.types.i = bool_result;}

      // if a pointer, write into address of the given pointer 
      if (stmt->types.assignment->isPtrDeref == true) {
        return Pointer_Helper(stmt, memory,identifier,value);
      }
      else {
        // Else, write into memory given the identifier
        ram_write_cell_by_name(memory,value,identifier);}
        return true;
}

static bool assignment_N_function_call(struct STMT* stmt, struct FUNCTION_CALL* function_call, struct RAM* memory, char* identifier) {
  // get function name and check it if's the function call print
  char* function_name = function_call->function_name;
  char* input_function = "input";
  char* float_function = "float";
  char* int_function = "int";
  // Comparing function name
  int input_comparison = strcmp(input_function,function_name);
  int float_comparison = strcmp(float_function,function_name);
  int int_comparison = strcmp(int_function,function_name);

  int parameter_type = function_call->parameter->element_type; // access type
  char* parameter_value = function_call->parameter->element_value; // access value

  bool is_all_zero = false; // determine wether string is all zero
  int char_length = strlen(parameter_value);

  if (input_comparison == 0 ){
    char line[256];
    printf("%s\n",parameter_value); // print the input string
    fgets(line, sizeof(line), stdin); 

    // delete EOL chars from input:
    line[strcspn(line, "\r\n")] = '\0';

    // Allocating memory for the string literal 
    int string_length = strlen(line) + 1; // extra one for null terminator

    char* copy_line = (char*) malloc(string_length * sizeof(char));
    strcpy(copy_line, line); // copy 

    // if (parameter_type == ELEMENT_INT_LITERAL) {
    //   int int_number = atoi(copy_line); // Access integer string and turn it into integer
    //   Putting_Value(stmt,memory,identifier,&int_number,RAM_TYPE_INT);
    // }
    // else if (parameter_type == ELEMENT_REAL_LITERAL) {
    //   double real_number = atof(copy_line); // Access real string and turn it into real
    //   Putting_Value(stmt,memory,identifier,&real_number,RAM_TYPE_REAL);
    // }
    // else if (parameter_type == ELEMENT_STR_LITERAL) {
    bool not_used = Putting_Value(stmt,memory,identifier,&copy_line,RAM_TYPE_STR);
    // }
    // else if (parameter_type == ELEMENT_BOOL_)
    // struct RAM_VALUE NEW_VALUE; // store value
    // NEW_VALUE.value_type = RAM_TYPE_STR;
    // NEW_VALUE.types.s = copy_line;
    // ram_write_cell_by_name(memory,NEW_VALUE,identifier);
    return true;
  
  }
  else { // it's either float or int
    struct RAM_VALUE* VALUE = ram_read_cell_by_name(memory,parameter_value); // access the value (ASSUMED TO BE ALWAYS A VARIABLE)
    if (VALUE != NULL) { // If variable does exist, access the variable (ASSUMED TO BE ALWAYS RAM_TYPE_STR)
        char* variable_value = VALUE->types.s;

        if (variable_value[0] == '0') {
          is_all_zero = true;
        }
        // for (int i = 0; i < char_length; i++) {
        //   if (variable_value[i] == '\0') { // if the end of char*, then just break
        //     break;
        //   }
        //   if (variable_value[i] != '0' || variable_value[i] != '.') {
        //     is_all_zero = false;
        //     // printf("%i\n", i);
        //     // printf("%c\n", variable_value[i]);
        //     // printf("Is not all zero\n");
        //     break;
        //   }
        //   is_all_zero = true;
        //   // printf("Is all zero\n");
        // }

        if (int_comparison == 0) {
          int integer = atoi(VALUE->types.s);
          ram_free_value(VALUE); 
          if (integer == 0 && is_all_zero == false) { // if fail
            printf("**SEMANTIC ERROR: invalid string for int() (line %d)\n", stmt->line);
            return false;
          }
          else {
            bool not_used = Putting_Value(stmt,memory,identifier,&integer,RAM_TYPE_INT);

            // struct RAM_VALUE NEW_VALUE; // store value
            // NEW_VALUE.value_type = RAM_TYPE_INT;
            // NEW_VALUE.types.i = integer;
            // ram_write_cell_by_name(memory,NEW_VALUE,identifier);
            // printf("Wrote into memory\n");
            return true;}}

        else if (float_comparison == 0) {
            double floating = atof(VALUE->types.s);
            ram_free_value(VALUE); 
            if (floating == 0 && is_all_zero == false) { // if fail
              printf("**SEMANTIC ERROR: invalid string for float() (line %d)\n", stmt->line);
              return false;}
            else {
            bool not_used = Putting_Value(stmt,memory,identifier,&floating,RAM_TYPE_REAL);

              // struct RAM_VALUE NEW_VALUE; // store value
              // NEW_VALUE.value_type = RAM_TYPE_REAL;
              // NEW_VALUE.types.d = floating;
              // ram_write_cell_by_name(memory,NEW_VALUE,identifier);
            return true;
            }
            }
    else {
      printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", parameter_value, stmt->line);
      return false;}
    } 
  }
  return false;
}

enum OPERAND_VALUE_TYPES
{
  Operand_Types_INT = 0,
  Operand_Types_REAL,
  Operand_Types_STR,
  Operand_Types_BOOLEAN,
  Operand_Types_INVALID,
};

struct Operand
{
  int operand_Types; 

  union
  {
    int    i; // INT, Not Exist -1
    double d; // REAL
    char*  s; // STR 
  } operand_value;

  bool exist;

  bool identifier;
};

//
// Retrieve the appropriate value given LHS or RHS. Account for if the given variable don't exist
//
static struct Operand retrived_value(struct UNARY_EXPR* unary_expr, struct RAM* memory) {
  // printf("In retrived_value");
  struct Operand Operand;
  Operand.identifier = false;
  if (unary_expr->element->element_type == ELEMENT_INT_LITERAL) {
    int number = atoi(unary_expr->element->element_value); // Access integer string and turn it into integer
    Operand.exist = true;
    Operand.operand_Types = Operand_Types_INT;
    Operand.operand_value.i = number;
    return Operand;
  }
  else if (unary_expr->element->element_type == ELEMENT_REAL_LITERAL) {
    double number = atof(unary_expr->element->element_value); // Access real string and turn it into real
    Operand.exist = true;
    Operand.operand_Types = Operand_Types_REAL;
    Operand.operand_value.d = number;
    return Operand;
  }
  else if (unary_expr->element->element_type == ELEMENT_STR_LITERAL) {
    char* string_value = unary_expr->element->element_value; // Access string
    Operand.exist = true;
    Operand.operand_Types = Operand_Types_STR;
    Operand.operand_value.s = string_value;
    return Operand;
  }
  else if (unary_expr->element->element_type == ELEMENT_IDENTIFIER) { // else, it's variable instead
      char* identifier = unary_expr->element->element_value; // access literal string identifier
      Operand.identifier = true;
      // check if it exist, 
      int exist = ram_get_addr(memory,identifier);

      if (exist != -1) { // it exist
        Operand.exist = true;
        struct RAM_VALUE* COPY_VALUE = ram_read_cell_by_name(memory,identifier);
        
        if (COPY_VALUE->value_type == RAM_TYPE_INT) {
          Operand.operand_Types = Operand_Types_INT;
          Operand.operand_value.i = COPY_VALUE->types.i;
          ram_free_value(COPY_VALUE);
          return Operand;
        }
        else if (COPY_VALUE->value_type == RAM_TYPE_REAL) {
          Operand.operand_Types = Operand_Types_REAL;
          Operand.operand_value.d = COPY_VALUE->types.d;
          ram_free_value(COPY_VALUE);
          return Operand;
        }
        else if (COPY_VALUE->value_type == RAM_TYPE_STR) {
          Operand.operand_Types = Operand_Types_STR;
          int string_lengths = strlen(COPY_VALUE->types.s) + 1; // extra one for null zero
          char* copy_word = (char*) malloc(string_lengths * sizeof(char));
          strcpy(copy_word, COPY_VALUE->types.s); // copy 

          Operand.operand_value.s = copy_word;

          ram_free_value(COPY_VALUE);
          return Operand;
        }
        else if (COPY_VALUE->value_type == RAM_TYPE_BOOLEAN) {
          Operand.operand_Types = Operand_Types_BOOLEAN;
          Operand.operand_value.i = COPY_VALUE->types.i;
          ram_free_value(COPY_VALUE);
          return Operand;        
        }
    
      }
      else { // Else, none of the above
          Operand.exist = false;
          Operand.operand_Types = Operand_Types_INVALID;
          Operand.operand_value.i = -1;
          return Operand; 
      }
  }
  // else, it don't exist
  Operand.exist = false;
  Operand.operand_Types = Operand_Types_INVALID;
  return Operand; 
}

//
// Struct to helper with returning both the result of an operation and whether it is a success (true/false)
//
enum RESULT_VALUE_TYPES
{
  Result_Types_INT = 0,
  Result_Types_REAL,
  Result_Types_STR,
  Result_Types_BOOL,
  Result_Types_INVALID,
};

struct Results {
  int result_types;

  union
  {
    int    i; // INT, Not Exist -1
    double d; // REAL
    char*  s; // STR 
  } operation_result;


  bool success;

};

//
// Check the validity of the rhs or/and lhs (If it's a variable, does it exist). And return the struct Result accordingly. Print so if it is undefined
//
static struct Results return_results(struct STMT* stmt,struct EXPR* expr, struct Results* results, bool element_exist_left,bool element_exist_right) {
  if (element_exist_left == true && element_exist_right == true) {
    results->success = true;
    return *results;
  }
  else {
    results->success = false;
    char* left_name  = expr->lhs->element->element_value;
    char* right_name  = expr->rhs->element->element_value;
    if (element_exist_left == false) {
      printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", left_name, stmt->line);
    }
    else {
      printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", right_name, stmt->line);
    }
      return *results;
    }  
}
//
// Execute relational binary expression for real or int. Use as helper function integer_binary_expression and real_binary_expression
//
static struct Results num_Relational_Operation(struct EXPR* expr, double real_left_value, double real_right_value, bool is_int) {
  struct Results results;

  double left_value = real_left_value; // default is a float
  double right_value = real_right_value;

  if (is_int == true) { // if it is an integer operation, then convert to int
    left_value = (int)real_left_value; // Cast to int
    right_value = (int)real_right_value; // Cast to int
  }

  if (expr->operator_type ==  OPERATOR_EQUAL) {
      if (left_value == right_value) {
          results.operation_result.i = 1;
          results.success = true;
          results.result_types = Result_Types_BOOL;
          return results;
      }
      else {
          results.operation_result.i = 0;
          results.success = true;
          results.result_types = Result_Types_BOOL;
          return results;
      }
    }
    else if (expr->operator_type ==  OPERATOR_NOT_EQUAL) {
      if (left_value != right_value) {
          results.operation_result.i = 1;
          results.success = true;
          results.result_types = Result_Types_BOOL;
          return results;
      }
      else {
          results.operation_result.i = 0;
          results.success = true;
          results.result_types = Result_Types_BOOL;
          return results;
      }
    }    
    else if (expr->operator_type ==  OPERATOR_LT) {
      if (left_value < right_value) {
          results.operation_result.i = 1;
          results.success = true;
          results.result_types = Result_Types_BOOL;
          return results;
      }
      else {
          results.operation_result.i = 0;
          results.success = true;
          results.result_types = Result_Types_BOOL;
          return results;
      }
    }      
    else if (expr->operator_type ==  OPERATOR_GT) {
      if (left_value > right_value) {
          results.operation_result.i = 1;
          results.success = true;
          results.result_types = Result_Types_BOOL;
          return results;
      }
      else {
          results.operation_result.i = 0;
          results.success = true;
          results.result_types = Result_Types_BOOL;
          return results;
      }
    }     
    else if (expr->operator_type == OPERATOR_GTE) {
      if (left_value >= right_value) {
          results.operation_result.i = 1;
          results.success = true;
          results.result_types = Result_Types_BOOL;
          return results;
      }
      else {
          results.operation_result.i = 0;
          results.success = true;
          results.result_types = Result_Types_BOOL;
          return results;
      }
    }    
    else if (expr->operator_type == OPERATOR_LTE) {
      if (left_value <= right_value) {
          results.operation_result.i = 1;
          results.success = true;
          results.result_types = Result_Types_BOOL;
          return results;
      }
      else {
          results.operation_result.i = 0;
          results.success = true;
          results.result_types = Result_Types_BOOL;
          return results;
      }
    }                  
  results.operation_result.i = 0;
  results.success = false;
  results.result_types = Result_Types_INVALID;
  return results;
}


//
// Execute binary expression for integer. Used as a helper function for execute_binary_expression.
//
static struct Results integer_binary_expression(struct STMT* stmt,struct EXPR* expr, int left_value, int right_value, bool element_exist_left,bool element_exist_right) {
    struct Results results;
    if (expr->operator_type == OPERATOR_PLUS) {
      int sum = left_value + right_value;
      results.operation_result.i = sum;
      results.result_types = Result_Types_INT;
      return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
    }
    else if (expr->operator_type == OPERATOR_MINUS) {
      int difference = left_value - right_value;
      results.operation_result.i = difference;
      results.result_types = Result_Types_INT;
      return return_results(stmt, expr, &results, element_exist_left,element_exist_right);
    }
    else if (expr->operator_type ==  OPERATOR_ASTERISK) {
      int product = left_value * right_value;
      results.operation_result.i = product;
      results.result_types = Result_Types_INT;
      return return_results(stmt, expr, &results, element_exist_left,element_exist_right);
    }
    else if (expr->operator_type ==  OPERATOR_POWER) {
      int power = pow(left_value, right_value);
      results.operation_result.i = power;
      results.result_types = Result_Types_INT;
       return return_results(stmt, expr, &results, element_exist_left,element_exist_right);
    }
    else if (expr->operator_type ==  OPERATOR_MOD) {
      if (right_value != 0) { //if a valid denominator, continue
        int remainder = left_value % right_value;
        results.operation_result.i = remainder;
        results.result_types = Result_Types_INT;

        return return_results(stmt, expr, &results, element_exist_left,element_exist_right);}

      else {
        printf("**SEMANTIC ERROR: mod by 0 (line %d)\n", stmt->line);
        results.operation_result.i = 0;
        results.success = false;
        results.result_types = Result_Types_INVALID;
        return results;
      }  
    }
    else if (expr->operator_type ==  OPERATOR_DIV) {
      if (right_value  != 0) { //if a valid denominator, continue
        int quotient = left_value / right_value ;
        results.operation_result.i = quotient;
        results.result_types = Result_Types_INT;
       return return_results(stmt, expr, &results, element_exist_left,element_exist_right);}

      else {
        printf("**SEMANTIC ERROR: divide by 0 (line %d)\n", stmt->line);
        results.operation_result.i = 0;
        results.success = false;
        results.result_types = Result_Types_INVALID;
        return results;
      }  
    } 
    else {
       return num_Relational_Operation(expr,left_value,right_value,true);
    }
}  

//
// Execute binary expression for integer. Used as a helper function for execute_binary_expression.
//
static struct Results real_binary_expression(struct STMT* stmt,struct EXPR* expr, double left_value, double right_value, bool element_exist_left,bool element_exist_right) {
    struct Results results;
    if (expr->operator_type == OPERATOR_PLUS) {
      double sum = left_value + right_value;
      results.operation_result.d = sum;
      results.result_types = Result_Types_REAL;
      return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
    }

    else if (expr->operator_type == OPERATOR_MINUS) {
      double difference = left_value - right_value;
      results.operation_result.d = difference;
      results.result_types = Result_Types_REAL;
      return return_results(stmt, expr, &results, element_exist_left,element_exist_right);
    }
    else if (expr->operator_type ==  OPERATOR_ASTERISK) {
      double product = left_value * right_value;
      results.operation_result.d = product;
      results.result_types = Result_Types_REAL;
      return return_results(stmt, expr, &results, element_exist_left,element_exist_right);
    }
    else if (expr->operator_type ==  OPERATOR_POWER) {
      double power = pow(left_value, right_value);
      results.operation_result.d = power;
      results.result_types = Result_Types_REAL;
      return return_results(stmt, expr, &results, element_exist_left,element_exist_right);
    }
    else if (expr->operator_type ==  OPERATOR_MOD) {
      if (right_value != 0) { //if a valid denominator, continue
        double remainder = fmod(left_value, right_value);
        results.operation_result.d = remainder;
        results.result_types = Result_Types_REAL;
        return return_results(stmt, expr, &results, element_exist_left,element_exist_right);}
      else {
        printf("**SEMANTIC ERROR: mod by 0 (line %d)\n", stmt->line);
        results.operation_result.d = 0;
        results.result_types = Result_Types_INVALID;
        results.success = false;
        return results;
      }  
    }
    else if (expr->operator_type ==  OPERATOR_DIV) {
      if (right_value  != 0) { //if a valid denominator, continue
        double quotient = left_value / right_value ;
        results.operation_result.d = quotient;
        results.result_types = Result_Types_REAL;
       return return_results(stmt, expr, &results, element_exist_left,element_exist_right);}

      else {
        printf("**SEMANTIC ERROR: divide by 0 (line %d)\n", stmt->line);
        results.operation_result.d = 0;
        results.result_types = Result_Types_INVALID;
        results.success = false;
        return results;
      }  
    }
    else {
       return num_Relational_Operation(expr,left_value,right_value,false);
    }    
}  

//
// Execute binary expression for string. Involved concatenation and relational operations. Use as helpful function for binary_execute_expression
//
static struct Results string_binary_expression(struct STMT* stmt,struct EXPR* expr, char* left_value, char* right_value, bool element_exist_left,bool element_exist_right) {
    struct Results results;
    // printf("string_binary_expression");
    if (expr->operator_type == OPERATOR_PLUS) {
      int string_lengths = strlen(left_value) +  strlen(right_value) + 1; // extra one for null zero
      char* new_word = (char*) malloc(string_lengths * sizeof(char));

      strcpy(new_word, left_value); // copy the first word into the new string array
      strcat(new_word, right_value); // Combine

      results.operation_result.s = new_word; 
      results.result_types = Result_Types_STR;
      return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
    }   
    else if (expr->operator_type == OPERATOR_EQUAL) {
      int string_comparison = strcmp(left_value,right_value);
      if (string_comparison == 0) {
        results.operation_result.i = 1; 
        results.result_types = Result_Types_BOOL;
        return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
      }
      else {
        results.operation_result.i = 0; 
        results.result_types = Result_Types_BOOL;
        return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
      }
    }       
    else if (expr->operator_type == OPERATOR_NOT_EQUAL) {
      int string_comparison = strcmp(left_value,right_value);
      if (string_comparison != 0) {
        results.operation_result.i = 1; 
        results.result_types = Result_Types_BOOL;
        return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
      }
      else {
        results.operation_result.i = 0; 
        results.result_types = Result_Types_BOOL;
        return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
      }
    }   
    else if (expr->operator_type == OPERATOR_GT) {
      int string_comparison = strcmp(left_value,right_value);
      if (string_comparison > 0) {
        results.operation_result.i = 1; 
        results.result_types = Result_Types_BOOL;
        return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
      }
      else {
        results.operation_result.i = 0; 
        results.result_types = Result_Types_BOOL;
        return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
      }
    }    
    else if (expr->operator_type == OPERATOR_GTE) {
      int string_comparison = strcmp(left_value,right_value);
      if (string_comparison >= 0) {
        results.operation_result.i = 1; 
        results.result_types = Result_Types_BOOL;
        return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
      }
      else {
        results.operation_result.i = 0; 
        results.result_types = Result_Types_BOOL;
        return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
      }
    }     
    else if (expr->operator_type == OPERATOR_LT) {
      int string_comparison = strcmp(left_value,right_value);
      if (string_comparison < 0) {
        results.operation_result.i = 1; 
        results.result_types = Result_Types_BOOL;
        return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
      }
      else {
        results.operation_result.i = 0; 
        results.result_types = Result_Types_BOOL;
        return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
      }
    }  
    else if (expr->operator_type == OPERATOR_LTE) {
      int string_comparison = strcmp(left_value,right_value);
      if (string_comparison <= 0) {
        results.operation_result.i = 1; 
        results.result_types = Result_Types_BOOL;
        return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
      }
      else {
        results.operation_result.i = 0; 
        results.result_types = Result_Types_BOOL;
        return return_results(stmt, expr, &results, element_exist_left, element_exist_right);
      }
    }               
    else { 
      printf("SEMANTIC ERROR: invalid operand types (line %d)\n",stmt->line);
      results.result_types = Result_Types_INVALID;
      results.success = false;
      return results;
    }
}

//
// Execute binary expression, used as a helper function for execute_assignment. Handles undefined variable and invalid operation, such as divide by 0 or mod by 0. Return struct
// 

static struct Results execute_binary_expression(struct EXPR* expr, struct RAM* memory,struct STMT* stmt) {
  struct Results results;

  struct Operand left =  retrived_value(expr->lhs,memory);
  struct Operand right = retrived_value(expr->rhs,memory);
  bool element_exist_left = left.exist;
  bool element_exist_right = right.exist;

  if (left.operand_Types == Operand_Types_INT && right.operand_Types == Operand_Types_INT) {
    return integer_binary_expression(stmt, expr, left.operand_value.i, right.operand_value.i,element_exist_left,element_exist_right); // if both are integer, perform integer operation
  }

  else if ((left.operand_Types == Operand_Types_REAL || right.operand_Types == Operand_Types_REAL) && ( element_exist_left == true && element_exist_right == true) && (left.operand_Types != Operand_Types_STR && right.operand_Types != Operand_Types_STR)) { // if either is a real number, but neither is string, and both exist, then perform real operation 
    if (left.operand_Types == Operand_Types_INT) { // if left is integer, access it and convert to real
      double left_double = left.operand_value.i;
      return real_binary_expression(stmt, expr, left_double, right.operand_value.d,element_exist_left,element_exist_right);
    }
    if (right.operand_Types == Operand_Types_INT) { // if right is integer, access it and convert to real
      double right_double = right.operand_value.i;
      return real_binary_expression(stmt, expr, left.operand_value.d, right_double,element_exist_left, element_exist_right);
    }
    else { // if both is real, then simply plug in
    return real_binary_expression(stmt, expr, left.operand_value.d, right.operand_value.d,element_exist_left,element_exist_right);
  }
  }
  else if (left.operand_Types == Operand_Types_STR && right.operand_Types == Operand_Types_STR) { // if both are string, perform string operation

      return string_binary_expression(stmt, expr, left.operand_value.s, right.operand_value.s,element_exist_left,element_exist_right);
  }
  else if ((right.identifier == true && element_exist_right == false) || (left.identifier == true && element_exist_left == false)) { // acoutn for non-existant string variable
    if (left.identifier == true) {
      printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", expr->lhs->element->element_value, stmt->line);}
    else if (right.identifier == true) {
      printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", expr->rhs->element->element_value, stmt->line);}
    results.operation_result.i = 0;
    results.success = false;
    return results;
  }
  else {
    printf("**SEMANTIC ERROR: invalid operand types (line %d)\n",stmt->line);
    results.operation_result.i = 0;
    results.success = false;
    return results;
}
}

//
// Execute assignment for string, real, and string. Also allow for variable assigment. An assignment with no expression result in error
//
static bool execute_assignment(struct STMT* stmt, struct RAM* memory) {
  char* identifier = stmt->types.assignment->var_name;   // Get var name

  if (stmt->types.assignment->rhs == NULL){ // if rhs don't exist, error
    printf("**SEMANTIC ERROR: unknown function (line %d)\n", stmt->line);
    return false;
}
  // Check if expression 
  if (stmt->types.assignment->rhs->value_type == VALUE_EXPR) { // If it is an expression

    if (stmt->types.assignment->rhs->types.expr->lhs->element->element_type == ELEMENT_INT_LITERAL && stmt->types.assignment->rhs->types.expr->isBinaryExpr == false) { // check if it is uniteral integer expression
      int number = atoi(stmt->types.assignment->rhs->types.expr->lhs->element->element_value); // Access integer string and turn it into integer      
      return Putting_Value(stmt,memory,identifier,&number,RAM_TYPE_INT);}

    else if (stmt->types.assignment->rhs->types.expr->lhs->element->element_type == ELEMENT_REAL_LITERAL && stmt->types.assignment->rhs->types.expr->isBinaryExpr == false) { // check if it is uniteral real expression
      double floating = atof(stmt->types.assignment->rhs->types.expr->lhs->element->element_value); // Access REAL string and turn it into float
      return Putting_Value(stmt,memory,identifier,&floating,RAM_TYPE_REAL);}

    else if (stmt->types.assignment->rhs->types.expr->lhs->element->element_type == ELEMENT_STR_LITERAL && stmt->types.assignment->rhs->types.expr->isBinaryExpr == false) { // check if it is uniteral string expression
      char* strings_literal = stmt->types.assignment->rhs->types.expr->lhs->element->element_value; // Access string 
      return Putting_Value(stmt,memory,identifier,&strings_literal,RAM_TYPE_STR);}

    else if (stmt->types.assignment->rhs->types.expr->lhs->element->element_type == ELEMENT_TRUE && stmt->types.assignment->rhs->types.expr->isBinaryExpr == false) { // check if it is uniteral string expression and True boolean
      int true_value = 1;  
      return Putting_Value(stmt,memory,identifier,&true_value,RAM_TYPE_BOOLEAN);}

    else if ( stmt->types.assignment->rhs->types.expr->lhs->element->element_type == ELEMENT_FALSE && stmt->types.assignment->rhs->types.expr->isBinaryExpr == false) { // check if it is uniteral string expression and false boolean
      int false_value = 0;  
      return Putting_Value(stmt,memory,identifier,&false_value,RAM_TYPE_BOOLEAN);}

    else if (stmt->types.assignment->rhs->types.expr->lhs->element->element_type == ELEMENT_IDENTIFIER && stmt->types.assignment->rhs->types.expr->isBinaryExpr == false) { // check if it is uniteral identifier expression
      char* name = stmt->types.assignment->rhs->types.expr->lhs->element->element_value;
      struct RAM_VALUE* COPY_VALUE = ram_read_cell_by_name(memory,name);

      if (COPY_VALUE != NULL) { // If it does exist, access the value

        if (COPY_VALUE->value_type == RAM_TYPE_STR) { // check if string
          char* string_value  = COPY_VALUE->types.s; 
          return Putting_Value(stmt,memory,identifier,&string_value,RAM_TYPE_STR);}

        else if (COPY_VALUE->value_type == RAM_TYPE_INT ){ // check if INT
          int int_value  = COPY_VALUE->types.i; 
          return Putting_Value(stmt,memory,identifier,&int_value,RAM_TYPE_INT);}

        else if (COPY_VALUE->value_type == RAM_TYPE_REAL) { // check if REAL
          double real_value  = COPY_VALUE->types.d; 
          return Putting_Value(stmt,memory,identifier,&real_value,RAM_TYPE_REAL);}

        else if (COPY_VALUE->value_type == RAM_TYPE_BOOLEAN) { // check if BOOL
          int bool_value  = COPY_VALUE->types.i; 
          return Putting_Value(stmt,memory,identifier,&bool_value,RAM_TYPE_BOOLEAN);}        
      }
      else {
      printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", name, stmt->line);
      ram_free_value(COPY_VALUE); 
      return false;   
      }
    }
    else if (stmt->types.assignment->rhs->types.expr->isBinaryExpr == true) {
    struct Results results = execute_binary_expression(stmt->types.assignment->rhs->types.expr, memory, stmt);
            
      if (results.success == true) { // if operation was success, put value in
          struct RAM_VALUE value;
          // Check what type of value it is and update accordingly
          if (results.result_types == Result_Types_INT) { 
            int int_value  = results.operation_result.i; 
            return Putting_Value(stmt,memory,identifier,&int_value,RAM_TYPE_INT);     }

          else if (results.result_types == Result_Types_REAL) {
            double real_value  = results.operation_result.d; 
            return Putting_Value(stmt,memory,identifier,&real_value,RAM_TYPE_REAL); }

          else if (results.result_types == Result_Types_STR) {
            char* string_value  = results.operation_result.s; 
            return Putting_Value(stmt,memory,identifier,&string_value,RAM_TYPE_STR);  }

          else if (results.result_types == Result_Types_BOOL) {
            int bool_value  = results.operation_result.i; 
            return Putting_Value(stmt,memory,identifier,&bool_value, RAM_TYPE_BOOLEAN); }
      }
      else { // Error
        return false;}
    }
  } 
  return false;
}

// execute_expr
//
// Given an expression such as x * y or a > b, executes
// the expression and returns the result as a RAM_VALUE.
// If the execution fails with a semantic error, an
// error message is output and NULL is returned.
//
// NOTE: this function allocates memory for the value that
// is returned. This implies if the return value != NULL, 
// the caller takes ownership of the copy and must
// eventually free this memory via ram_free_value().
//
// static struct RAM_VALUE* execute_expr(struct STMT* stmt, struct RAM* memory, struct EXPR* expr){

// }


//
// Public functions:
//

//
// execute
//
// Given a nuPython program graph and a memory, 
// executes the statements in the program graph.
// If a semantic error occurs (e.g. type error),
// and error message is output, execution stops,
// and the function returns.
//
void execute(struct STMT* program, struct RAM* memory)
{
  struct STMT* stmt = program;

  while (stmt != NULL ) { // traverse through the program statements
    if (stmt->stmt_type == STMT_ASSIGNMENT) {
      bool result;
      if (stmt->types.assignment->rhs->value_type == VALUE_FUNCTION_CALL){ // if it is a function call
        char* identifier = stmt->types.assignment->var_name;   // Get var name
        result = assignment_N_function_call(stmt, stmt->types.assignment->rhs->types.function_call,memory,identifier);}
      else {
        result = execute_assignment(stmt,memory);}
      if (result == false) {
        break;
      }
      stmt = stmt->types.assignment->next_stmt; // advance
    } // if

    else if (stmt->stmt_type == STMT_FUNCTION_CALL) {
      bool result = execute_function_call(stmt, memory);
      if (result == false) {

        break;
      }

      stmt = stmt->types.function_call->next_stmt; // advance
  
    } // else if

    else if ((stmt->stmt_type == STMT_PASS))  {
      // assert(stmt->stmt_type == STMT_PASS); 
      stmt = stmt->types.pass->next_stmt; // advance

    } // else

    else if (stmt->stmt_type == STMT_IF_THEN_ELSE) {
      // if result is true, go to true math

      // else, go to false path

    } // else

    else { // while statement
    // if result is true, go back to the beginning og while

    // else, go to the next stament
    }
  } // while
}
