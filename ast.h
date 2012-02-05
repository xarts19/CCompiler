#ifndef ATS_H_
#define AST_H_

typedef struct exp_tag {
    enum { int_exp, floating_exp, variable_exp,
           binary_op, unary_op, function_call}       tag;
    union { int                                      integer;
            float                                    floating;
            string                                   variable;
            struct { string           oper;
                     struct exp_tag*  left;
                     struct exp_tag*  right; }       binary;
            struct { string           oper;
                     struct exp_tag*  uexp; }        unary;
            struct { string           name;
                     struct exp_list* arguments; }   call;
        } op;
} exp;

typedef struct exp_list {
  exp*             elem;
  struct exp_list* next;
} exp_list;

exp* new_int_exp(int value) {
    exp *i = (exp*)malloc(sizeof(exp));
    i->tag = int_exp;
    i->op->integer = value;
}

#endif
