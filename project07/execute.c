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

#include <string.h>

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
  char* input_function = "input";
  char* float_function = "float";
  char* int_function = "int";

  int print_comparison = strcmp(print_function,function_name);
  int input_comparison = strcmp(input_function,function_name);
  int float_comparison = strcmp(float_function,function_name);
  int int_comparison = strcmp(int_function,function_name);

  int parameter_type = stmt->types.function_call->parameter->element_type;
  char* parameter_value = stmt->types.function_call->parameter->element_value; 

  if (print_comparison == 0) {
    if (parameter_value == NULL) { // If a print function and no parameter, then print end line
      printf("\n"); 
      return true;
    }
    else if (stmt->types.function_call->parameter->element_type == ELEMENT_STR_LITERAL) { // if a print function and string literal, then print it with an end line
      printf("%s\n", parameter_value);
      return true;
    }
    else if (stmt->types.function_call->parameter->element_type == ELEMENT_INT_LITERAL) { // if a print function and string integer, then print it with an end line
      int number = atoi(parameter_value);
      printf("%d\n", number);
      return true;
    }
    else if (stmt->types.function_call->parameter->element_type == ELEMENT_REAL_LITERAL) { // if a print function and string real, then print it with an end line
      double floating = atof(parameter_value);
      printf("%lf\n", floating);
      return true;
    }
    else if (stmt->types.function_call->parameter->element_type == ELEMENT_TRUE) { // if a print function and bool True, then print True with an end line
      printf("True\n");
      return true;
    }
    else if (stmt->types.function_call->parameter->element_type == ELEMENT_FALSE) { // if a print function and bool False, then print False with an end line
      printf("False\n");
      return true;
    }  
    else {
      // check if it is a variable
      struct RAM_VALUE* VALUE = ram_read_cell_by_name(memory,parameter_value);
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
        printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", parameter_value, stmt->line);
        ram_free_value(VALUE); 
        return false;
      }
    }
  }
  return false;
} 

static bool assignment_N_function_call(struct STMT* stmt, struct FUNCTION_CALL* function_call, struct RAM* memory, char* identifier) {
  printf("assignment_N_function_call\n");
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
  printf("%s\n", parameter_value);

  bool is_all_zero = false; // determine wether string is all zero
  int char_length = strlen(parameter_value);

  if (input_comparison == 0 ){
    char line[256];

    fgets(line, sizeof(line), stdin); 

    // delete EOL chars from input:
    line[strcspn(line, "\r\n")] = '\0';

    int string_length = strlen(line) + 1; // extra one for null terminator

    char* copy_line = (char*) malloc(string_length * sizeof(char));
    strcpy(copy_line, line); // copy 

    struct RAM_VALUE NEW_VALUE; // store value
    NEW_VALUE.value_type = RAM_TYPE_STR;
    NEW_VALUE.types.s = copy_line;
    ram_write_cell_by_name(memory,NEW_VALUE,identifier);

    return true;
    
  }
  else { // it's either float or int
    struct RAM_VALUE* VALUE = ram_read_cell_by_name(memory,parameter_value); // access the value (ASSUMED TO BE ALWAYS A VARIABLE)
    if (VALUE != NULL) { // If variable does exist, access the variable (ASSUMED TO BE ALWAYS RAM_TYPE_STR)
        char* variable_value = VALUE->types.s;
        printf("The value is %s\n", variable_value);


        for (int i = 0; i < char_length; i++) {
          if (variable_value[i] == '\0') { // if the end of char*, then just break
            printf("NULL terminator\n");
            break;
          }
          if (variable_value[i] != '0') {
            is_all_zero = false;
            printf("%i\n", i);
            printf("%c\n", variable_value[i]);

            printf("Is not all zero\n");
            break;
          }
          is_all_zero = true;
          printf("Is all zero\n");
        }

        if (int_comparison == 0) {
          printf("is int()\n");
          int integer = atoi(VALUE->types.s);
          ram_free_value(VALUE); 
          if (integer == 0 && is_all_zero == false) { // if fail
            printf("**SEMANTIC ERROR: invalid string for int() (line %d)\n", stmt->line);
            return false;
          }
          else {
            struct RAM_VALUE NEW_VALUE; // store value
            NEW_VALUE.value_type = RAM_TYPE_INT;
            NEW_VALUE.types.i = integer;
            ram_write_cell_by_name(memory,NEW_VALUE,identifier);
            printf("Wrote into memory\n");
            return true;}}

        else if (float_comparison == 0) {
            double floating = atof(VALUE->types.s);
            ram_free_value(VALUE); 
            if (floating == 0 && is_all_zero == false) { // if fail
              printf("**SEMANTIC ERROR: invalid string for float() (line %d)\n", stmt->line);
              return false;}
            else {
              struct RAM_VALUE NEW_VALUE; // store value
              NEW_VALUE.value_type = RAM_TYPE_REAL;
              NEW_VALUE.types.d = floating;
              ram_write_cell_by_name(memory,NEW_VALUE,identifier);
              return true;}
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
};

//
// Retrieve the appropriate value given LHS or RHS. Account for if the given variable don't exist
//
static struct Operand retrived_value(struct UNARY_EXPR* unary_expr, struct RAM* memory) {
  struct Operand Operand;
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
    // printf("ACCESSING STRING\n");
    Operand.exist = true;
    Operand.operand_Types = Operand_Types_STR;
    Operand.operand_value.s = string_value;
    return Operand;
  }
  else if (unary_expr->element->element_type == ELEMENT_IDENTIFIER) { // else, it's variable instead
      char* name = unary_expr->element->element_value; // access literal string identifier

      // check if it exist, 
      int exist = ram_get_addr(memory,name);

      if (exist != -1) { // it exist
        Operand.exist = true;
        struct RAM_VALUE* COPY_VALUE = ram_read_cell_by_name(memory,name);

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
          // Operand.operand_Types = Operand_Types_STR;
          int string_lengths = strlen(COPY_VALUE->types.s) + 1; // extra one for null zero
          char* copy_word = (char*) malloc(string_lengths * sizeof(char));
          strcpy(copy_word, COPY_VALUE->types.s); // copy 

          Operand.operand_Types = Operand_Types_STR;
          Operand.operand_value.s = copy_word;

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
  // else { // Else, it don't exist
  Operand.exist = false;
  Operand.operand_Types = Operand_Types_INVALID;
  return Operand; 
  // }
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
static struct Results num_Relational_Operation(struct EXPR* expr, double real_left_value, int real_right_value, bool is_int) {
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
      // printf("REAL ADDING OPERATION\n");
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
        int quotient = left_value /right_value ;
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
    if (expr->operator_type == OPERATOR_PLUS) {
      // printf("STRING ADDING OPERATION\n");
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
      if (string_comparison <+ 0) {
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
    printf("INT OPERATION\n");
    return integer_binary_expression(stmt, expr, left.operand_value.i, right.operand_value.i,element_exist_left,element_exist_right); // if both are integer, perform integer operation
  }

  else if (left.operand_Types == Operand_Types_REAL || right.operand_Types == Operand_Types_REAL) { // if either is a real number, then perform real operation
    printf("REAL OPERATION\n");
    if (left.operand_Types == Operand_Types_INT) { // if left is integer, access it and convert to real
      double left_double = left.operand_value.i;
      return real_binary_expression(stmt, expr, left_double, right.operand_value.d,element_exist_left,element_exist_right);
    }
    if (right.operand_Types == Operand_Types_INT) { // if right is integer, access it and convert to real
      double right_double = right.operand_value.i;
      return real_binary_expression(stmt, expr, left.operand_value.d, right_double,element_exist_left, element_exist_right);
    }
    else { // if both is real, then simply plug in
    printf("BOTH REAL\n");
    return real_binary_expression(stmt, expr, left.operand_value.d, right.operand_value.d,element_exist_left,element_exist_right);
  }
  }
  else if (left.operand_Types == Operand_Types_STR && right.operand_Types == Operand_Types_STR) { // if both are string, perform string operation
    printf("%s\n", left.operand_value.s);
    printf("%s\n", right.operand_value.s);

    return string_binary_expression(stmt, expr, left.operand_value.s, right.operand_value.s,element_exist_left,element_exist_right);
  }
  printf("SEMANTIC ERROR: invalid operand types (line %d)",stmt->line);
  results.operation_result.i = 0;
  results.success = false;
  return results;
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
// Execute assignment for string, real, and string. Also allow for variable assigment. An assignment with no expression result in error
//
static bool execute_assignment(struct STMT* stmt, struct RAM* memory) {
  printf("execute_assignment");
  char* identifier = stmt->types.assignment->var_name;   // Get var name

  if (stmt->types.assignment->rhs == NULL){ // if rhs don't exist, error
    printf("**SEMANTIC ERROR: unknown function (line %d)\n", stmt->line);
    return false;
}
  // Check if expression 
  if (stmt->types.assignment->rhs->value_type == VALUE_EXPR) { // If it is an expression
      printf("IS EXPRESSION");

    if (stmt->types.assignment->rhs->types.expr->lhs->element->element_type == ELEMENT_INT_LITERAL && stmt->types.assignment->rhs->types.expr->isBinaryExpr == false) { // check if it is uniteral integer expression

      int number = atoi(stmt->types.assignment->rhs->types.expr->lhs->element->element_value); // Access integer string and turn it into integer
      
      // Create value 
      struct RAM_VALUE value;
      value.value_type = RAM_TYPE_INT;
      value.types.i = number;  

    // if a pointer, write into address of the given pointer 
    if (stmt->types.assignment->isPtrDeref == true) {
      return Pointer_Helper(stmt, memory,identifier,value);
    }
    else {
      // Write into memory
      ram_write_cell_by_name(memory,value,identifier);}
      return true;
    }

    else if (stmt->types.assignment->rhs->types.expr->lhs->element->element_type == ELEMENT_REAL_LITERAL && stmt->types.assignment->rhs->types.expr->isBinaryExpr == false) { // check if it is uniteral real expression
      double floating = atof(stmt->types.assignment->rhs->types.expr->lhs->element->element_value); // Access REAL string and turn it into float
      
      // Create value 
      struct RAM_VALUE value;
      value.value_type = RAM_TYPE_REAL;
      value.types.d = floating;  

      // if a pointer, write into address of the given pointer 
      if (stmt->types.assignment->isPtrDeref == true) {
        return Pointer_Helper(stmt, memory,identifier,value);
      }  
      else {// Write into memory
        ram_write_cell_by_name(memory,value,identifier);
      return true;}
    }

    else if (stmt->types.assignment->rhs->types.expr->lhs->element->element_type == ELEMENT_STR_LITERAL && stmt->types.assignment->rhs->types.expr->isBinaryExpr == false) { // check if it is uniteral string expression
      char* strings_literal = stmt->types.assignment->rhs->types.expr->lhs->element->element_value; // Access string 
        // Create value 
        struct RAM_VALUE value;
        value.value_type = RAM_TYPE_STR;
        value.types.s = strings_literal;  

      // if a pointer, write into address of the given pointer 
      if (stmt->types.assignment->isPtrDeref == true) {
        return Pointer_Helper(stmt, memory,identifier,value);
      }  
      else {      
        // Write into memory
        ram_write_cell_by_name(memory,value,identifier);
      return true;}
    }
    else if (stmt->types.assignment->rhs->types.expr->lhs->element->element_type == ELEMENT_TRUE && stmt->types.assignment->rhs->types.expr->isBinaryExpr == false) { // check if it is uniteral string expression
      // Create value 
        struct RAM_VALUE value;
        value.value_type = RAM_TYPE_BOOLEAN;
        value.types.i = 1;

      // if a pointer, write into address of the given pointer  (MUST UPDATE POint_Helper for Boolean)
      if (stmt->types.assignment->isPtrDeref == true) {
        return Pointer_Helper(stmt, memory,identifier,value);
      }  
      else {      
        // Write into memory
        ram_write_cell_by_name(memory,value,identifier);
      return true;}
    }
    else if ( stmt->types.assignment->rhs->types.expr->lhs->element->element_type == ELEMENT_FALSE && stmt->types.assignment->rhs->types.expr->isBinaryExpr == false) { // check if it is uniteral string expression
      // Create value 
      struct RAM_VALUE value;
      value.value_type = RAM_TYPE_BOOLEAN;
      value.types.i = 0; 

      // if a pointer, write into address of the given pointer  (MUST UPDATE POint_Helper for Boolean)
      if (stmt->types.assignment->isPtrDeref == true) {
        return Pointer_Helper(stmt, memory,identifier,value);
      }  
      else {      
        // Write into memory
        ram_write_cell_by_name(memory,value,identifier);
      return true;}
    }    
    else if (stmt->types.assignment->rhs->types.expr->lhs->element->element_type == ELEMENT_IDENTIFIER && stmt->types.assignment->rhs->types.expr->isBinaryExpr == false) { // check if it is uniteral identifier expression
      char* name = stmt->types.assignment->rhs->types.expr->lhs->element->element_value;
      struct RAM_VALUE* COPY_VALUE = ram_read_cell_by_name(memory,name);

      if (COPY_VALUE != NULL) { // If it does exist, access the value
        if (COPY_VALUE->value_type == RAM_TYPE_STR) { // check if string
          // Pass value to RAM_VALUE
          struct RAM_VALUE value;
          value.value_type = COPY_VALUE->value_type;
          value.types.s = COPY_VALUE->types.s; 

          // if a pointer, 
          if (stmt->types.assignment->isPtrDeref == true) {
            return Pointer_Helper(stmt, memory,identifier,value);
          }  
          else {
            ram_write_cell_by_name(memory,value,identifier);
            ram_free_value(COPY_VALUE); 
            return true; 
          } // Update value
        }

        else if (COPY_VALUE->value_type == RAM_TYPE_INT ){ // check if integer
          // Pass value to RAM_VALUE
          struct RAM_VALUE value;
          value.value_type = COPY_VALUE->value_type;
          value.types.i = COPY_VALUE->types.i;

          // if a pointer, write into address of the given pointer 
          if (stmt->types.assignment->isPtrDeref == true) {
            return Pointer_Helper(stmt, memory,identifier,value);
          }  
          else {
            ram_write_cell_by_name(memory,value,identifier);} // Update value
            ram_free_value(COPY_VALUE); 
            return true; 
        }    

        else if (COPY_VALUE->value_type == RAM_TYPE_REAL) { // check if REAL
          // Pass value to RAM_VALUE
          struct RAM_VALUE value;
          value.value_type = COPY_VALUE->value_type;
          value.types.d = COPY_VALUE->types.d; 

          if (stmt->types.assignment->isPtrDeref == true) {
            return Pointer_Helper(stmt, memory,identifier,value);
          }  
          else {
          ram_write_cell_by_name(memory,value,identifier);}// Update value 
          ram_free_value(COPY_VALUE); 
          return true;    
        }
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
          value.value_type = RAM_TYPE_INT;  // Create value
          value.types.i = results.operation_result.i;}

          else if (results.result_types == Result_Types_REAL) {
          value.value_type = RAM_TYPE_REAL;  // Create value
          value.types.d = results.operation_result.d;}

          else if (results.result_types == Result_Types_STR) {
          value.value_type = RAM_TYPE_STR;  // Create value
          value.types.s = results.operation_result.s;
        }
        else if (results.result_types == Result_Types_BOOL) {
          value.value_type = RAM_TYPE_BOOLEAN;  // Create value
          value.types.i = results.operation_result.i;
        }
          if (stmt->types.assignment->isPtrDeref == true) {
            return Pointer_Helper(stmt, memory,identifier,value);
          }
          else {
            // Write into memory
            ram_write_cell_by_name(memory,value,identifier);
          return true;}
      }
      else { // Error
        return false;
      }
    }
  } 
  return false;
}


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
      printf("is an assignment\n");
      bool result;
      if (stmt->types.assignment->rhs->value_type == VALUE_FUNCTION_CALL){ // if it is a function call
        printf("is an assignment & function call\n");
        char* identifier = stmt->types.assignment->var_name;   // Get var name
        result = assignment_N_function_call(stmt, stmt->types.assignment->rhs->types.function_call,memory,identifier);}
      else {
        result = execute_assignment(stmt,memory);}
      if (result == false) {
        printf("IF\n");
        break;
      }
      printf("AFTER\n");
      stmt = stmt->types.assignment->next_stmt; // advance
    } // if

    else if (stmt->stmt_type == STMT_FUNCTION_CALL) {
      bool result = execute_function_call(stmt, memory);
      if (result == false) {
        break;
      }
      stmt = stmt->types.function_call->next_stmt; // advance
  
    } // else if

    else {
      assert(stmt->stmt_type == STMT_PASS); 
      stmt = stmt->types.pass->next_stmt; // advance

    } // else

  } // while
}
