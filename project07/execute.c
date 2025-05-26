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
  int comparison = strcmp(print_function,function_name);
  if (comparison == 0 && stmt->types.function_call->parameter == NULL) { // If a print function and no parameter, then print end line
    printf("\n"); 
    return true;
  }
  else if (comparison == 0 && stmt->types.function_call->parameter->element_type == ELEMENT_STR_LITERAL) { // if a print function and string literal, then print it with an end line
    printf("%s\n", stmt->types.function_call->parameter->element_value);
    return true;
  }
  else if (comparison == 0 && stmt->types.function_call->parameter->element_type == ELEMENT_INT_LITERAL) { // if a print function and string integer, then print it with an end line
    int number = atoi(stmt->types.function_call->parameter->element_value);
    printf("%d\n", number);
    return true;
  }
  else if (comparison == 0 && stmt->types.function_call->parameter->element_type == ELEMENT_REAL_LITERAL) { // if a print function and string real, then print it with an end line
    double floating = atof(stmt->types.function_call->parameter->element_value);
    printf("%lf\n", floating);
    return true;
  }
  else if (comparison == 0 && stmt->types.function_call->parameter->element_type == ELEMENT_TRUE) { // if a print function and bool True, then print True with an end line
    printf("True\n");
    return true;
  }
  else if (comparison == 0 && stmt->types.function_call->parameter->element_type == ELEMENT_FALSE) { // if a print function and bool False, then print False with an end line
    printf("False\n");
    return true;
  }  
  else {
    // check if it is a variable
    struct RAM_VALUE* VALUE = ram_read_cell_by_name(memory,stmt->types.function_call->parameter->element_value);
    char* name = stmt->types.function_call->parameter->element_value;
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
      printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", name, stmt->line);
      ram_free_value(VALUE); 
      return false;
    }
  }
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
struct Operand retrived_value(struct UNARY_EXPR* unary_expr, struct RAM* memory) {
  struct Operand Operand;
  if (unary_expr->element->element_type == ELEMENT_INT_LITERAL) {
    int number = atoi(unary_expr->element->element_value); // Access integer string and turn it into integer
    Operand.exist = true;
    Operand.operand_Types = Operand_Types_INT;
    Operand.operand_value.i = number;
    return Operand;
  }
  else if (unary_expr->element->element_type == ELEMENT_REAL_LITERAL) {
    int number = atof(unary_expr->element->element_value); // Access real string and turn it into real
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
  else {
    // else, it's variable instead
      char* name = unary_expr->element->element_value;
      // check if is exist, 
      int exist = ram_get_addr(memory,name);

      if (exist != -1) {
        struct RAM_VALUE* COPY_VALUE = ram_read_cell_by_name(memory,name);

        if (COPY_VALUE->value_type == RAM_TYPE_INT) {
          Operand.operand_Types = Operand_Types_INT;
          Operand.operand_value.i = COPY_VALUE->types.i;
          return Operand;
        }
        else if (COPY_VALUE->value_type == RAM_TYPE_REAL) {
          Operand.operand_Types = Operand_Types_REAL;
          Operand.operand_value.d = COPY_VALUE->types.d;
          return Operand;
        }
        else if (COPY_VALUE->value_type == RAM_TYPE_STR) {
          Operand.operand_Types = Operand_Types_STR;
          Operand.operand_value.s = COPY_VALUE->types.s;
          return Operand;
        }
      }
    // Then, it don't exist
    Operand.exist = false;
    Operand.operand_Types = Operand_Types_INVALID;
    Operand.operand_value.i = -1;
    return Operand; 
    }
  }

//
// Struct to helper with returning both the result of an operation and whether it is a success (true/false)
//

struct Results {
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
static struct Results return_results(struct STMT* stmt,struct EXPR* expr, int operation_results, bool element_exist_left,bool element_exist_right) {
  struct Results results;

  if (element_exist_left == true && element_exist_right == true) {
    results.operation_result = operation_results;
    results.success = true;
    return results;
  }
  else {
    results.operation_result = operation_results;
    results.success = false;
    char* left_name  = expr->lhs->element->element_value;
    char* right_name  = expr->rhs->element->element_value;
    if (element_exist_left == false) {
      printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", left_name, stmt->line);
    }
    else {
      printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", right_name, stmt->line);
    }
      return results;
    }  
}
//
// Execute binary expression for integer. Used as a helper function for execute_binary_expression.
//
struct Results integer_binary_expression(struct STMT* stmt,struct EXPR* expr, int left_value, int right_value, bool element_exist_left,bool element_exist_right) {
    struct Results results;
    if (expr->operator_type == OPERATOR_PLUS) {
      int sum = left_value + right_value;
      return return_results(stmt, expr, sum, element_exist_left, element_exist_right);
    }

    else if (expr->operator_type == OPERATOR_MINUS) {
      int difference = left_value - right_value;
      results.operation_result = difference;
      return return_results(stmt, expr, difference, element_exist_left,element_exist_right);
    }

    else if (expr->operator_type ==  OPERATOR_ASTERISK) {
      int product = left_value * right_value;
      results.operation_result = product;
      return return_results(stmt, expr, product, element_exist_left,element_exist_right);
    }

    else if (expr->operator_type ==  OPERATOR_POWER) {
      int power = pow(left_value, right_value);
      results.operation_result = power;
       return return_results(stmt, expr, power, element_exist_left,element_exist_right);
    }

    else if (expr->operator_type ==  OPERATOR_MOD) {
      if (right_value != 0) { //if a valid denominator, continue
        int remainder = left_value % right_value;
        results.operation_result = remainder;
        return return_results(stmt, expr, remainder, element_exist_left,element_exist_right);}

      else {
        printf("**SEMANTIC ERROR: mod by 0 (line %d)\n", stmt->line);
        results.operation_result = 0;
        results.success = false;
        return results;
      }  
    }

    else if (expr->operator_type ==  OPERATOR_DIV) {
      if (right_value  != 0) { //if a valid denominator, continue
        int quotient = left_value /right_value ;
        results.operation_result = quotient;
       return return_results(stmt, expr, quotient, element_exist_left,element_exist_right);}

      else {
        printf("**SEMANTIC ERROR: divide by 0 (line %d)\n", stmt->line);
        results.operation_result = 0;
        results.success = false;
        return results;
      }  
    }
  results.operation_result = 0;
  results.success = false;
  return results;
}  

//
// Execute binary expression for integer. Used as a helper function for execute_binary_expression.
//
struct Results real_binary_expression(struct STMT* stmt,struct EXPR* expr, double left_value, double right_value, bool element_exist_left,bool element_exist_right) {
    struct Results results;
    if (expr->operator_type == OPERATOR_PLUS) {
      printf("REAL ADDING OPERATION\n");
      double sum = left_value + right_value;
      return return_results(stmt, expr, sum, element_exist_left, element_exist_right);
    }

    else if (expr->operator_type == OPERATOR_MINUS) {
      double difference = left_value - right_value;
      results.operation_result = difference;
      return return_results(stmt, expr, difference, element_exist_left,element_exist_right);
    }

    else if (expr->operator_type ==  OPERATOR_ASTERISK) {
      double product = left_value * right_value;
      results.operation_result = product;
      return return_results(stmt, expr, product, element_exist_left,element_exist_right);
    }

    else if (expr->operator_type ==  OPERATOR_POWER) {
      double power = pow(left_value, right_value);
      results.operation_result = power;
      return return_results(stmt, expr, power, element_exist_left,element_exist_right);
    }

    else if (expr->operator_type ==  OPERATOR_MOD) {
      if (right_value != 0) { //if a valid denominator, continue
        double remainder = fmod(left_value, right_value);
        results.operation_result = remainder;
        return return_results(stmt, expr, remainder, element_exist_left,element_exist_right);}

      else {
        printf("**SEMANTIC ERROR: mod by 0 (line %d)\n", stmt->line);
        results.operation_result = 0;
        results.success = false;
        return results;
      }  
    }

    else if (expr->operator_type ==  OPERATOR_DIV) {
      if (right_value  != 0) { //if a valid denominator, continue
        int quotient = left_value /right_value ;
        results.operation_result = quotient;
       return return_results(stmt, expr, quotient, element_exist_left,element_exist_right);}

      else {
        printf("**SEMANTIC ERROR: divide by 0 (line %d)\n", stmt->line);
        results.operation_result = 0;
        results.success = false;
        return results;
      }  
    }
  results.operation_result = 0;
  results.success = false;
  return results;
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

  results.operation_result = 0;
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
  // Get var name
  char* identifier = stmt->types.assignment->var_name;
  if (stmt->types.assignment->rhs == NULL){ // if rhs don't exist, error
    printf("**SEMANTIC ERROR: unknown function (line %d)\n", stmt->line);
    return false;
}
  // Check if expression 
  if (stmt->types.assignment->rhs->value_type == VALUE_EXPR) { // If it is an expression

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
          // Create value 
          struct RAM_VALUE value;
          value.value_type = RAM_TYPE_INT;
          value.types.i = results.operation_result;  

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
      bool result = execute_assignment(stmt,memory);
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

    else {
      assert(stmt->stmt_type == STMT_PASS); 
      stmt = stmt->types.pass->next_stmt; // advance

    } // else

  } // while
}
