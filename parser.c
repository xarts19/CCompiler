#include "parser.h"

extern char current_file[MAX_FILE_NAME_LENGTH];
extern int current_line;

static void tree_print_work(expr* tree, int depth);
static expr* error(const char *message, const char *param);

expr* new_value_expr(token* value) {
    expr* e = (expr*)safe_malloc( sizeof(expr) );
    e->tag = value_exp;
    e->content.value = value;
    return e;
}

expr* new_binary_op_expr(token* operator) {
    expr* e = (expr*)safe_malloc( sizeof(expr) );
    e->tag = binary_op;
    e->content.binary.oper = operator;
    e->content.binary.left = NULL;
    e->content.binary.right = NULL;
    return e;
}

expr* parse(vector* tokens) {
    int index = 0;
    expr* tree = E(tokens, &index);
    return tree;
    /*
    expr* tree = new_binary_op_expr(token_new(operator, "op+"));
    tree->content.binary.left = new_value_expr(token_new(int_num, "5"));
    expr* t = new_binary_op_expr(token_new(operator, "op*"));
    tree->content.binary.right = t;
    t->content.binary.left = new_value_expr(token_new(int_num, "3"));
    t->content.binary.right = new_value_expr(token_new(int_num, "8"));
    return tree;
    */
}

expr* E(vector* tokens, int* index) {
    expr *tree1, *tree2;
    tree1 = B(tokens, index);
    tree2 = E_(tokens, index);
    if (tree2 != NULL) {
        tree2->content.binary.left = tree1;
        return tree2;
    } else
        return tree1;
}

expr* E_(vector* tokens, int* index) {
    token* t = (token*)tokens->elements[*index];
    if (*index >= tokens->size) return NULL;
    if (t->id == cmp_operator) {
        *index += 1;
        expr* tree = new_binary_op_expr(t);
        tree->content.binary.right = B(tokens, index);
        return tree;
    } else
        return NULL;
}

expr* B(vector* tokens, int* index) {
    expr *tree1, *tree2;
    tree1 = C(tokens, index);
    tree2 = B_(tokens, index);
    if (tree2 != NULL) {
        tree2->content.binary.left = tree1;
        return tree2;
    } else
        return tree1;
}

expr* B_(vector* tokens, int* index) {
    token* t = (token*)tokens->elements[*index];
    if (*index >= tokens->size) return NULL;
    if ( strcmp(t->data, "op+") == 0 || strcmp(t->data, "op-") == 0 ) {
        *index += 1;
        expr* tree = new_binary_op_expr(t);
        expr* middle = C(tokens, index);
        expr* tail = B_(tokens, index);
        if (tail != NULL) {
            tail->content.binary.left = middle;
            tree->content.binary.right = tail;
            return tree;
        } else {
            tree->content.binary.right = middle;
            return tree;
        }
    } else
        return NULL;
}

expr* C(vector* tokens, int* index) {
    expr *tree1, *tree2;
    tree1 = D(tokens, index);
    tree2 = C_(tokens, index);
    if (tree2 != NULL) {
        tree2->content.binary.left = tree1;
        return tree2;
    } else
        return tree1;
}

expr* C_(vector* tokens, int* index) {
    token* t = (token*)tokens->elements[*index];
    if (*index >= tokens->size) return NULL;
    if ( strcmp(t->data, "op*") == 0 || strcmp(t->data, "op/") == 0 || strcmp(t->data, "op%") == 0 ) {
        *index += 1;
        expr* tree = new_binary_op_expr(t);
        expr* middle = D(tokens, index);
        expr* tail = C_(tokens, index);
        if (tail != NULL) {
            tail->content.binary.left = middle;
            tree->content.binary.right = tail;
            return tree;
        } else {
            tree->content.binary.right = middle;
            return tree;
        }
    } else
        return NULL;
}

expr* D(vector* tokens, int* index) {
    expr* tree;
    if (*index >= tokens->size) return NULL;
    token* t = (token*)tokens->elements[*index];
    if (t->id == int_num || t->id == float_num || t->id == exp_num || t->id == identifier) {
        *index += 1;
        tree = new_value_expr(t);
        return tree;
    } else if ( strcmp(t->data, "op(") == 0 ) {
        *index +=1;
        tree = E(tokens, index);
        t = (token*)tokens->elements[*index];
        if (strcmp(t->data, "op)") != 0)
            return error("No closing parentethis", "");
        *index += 1;
        return tree;
    } else
        return NULL;
}

void tree_delete(expr* tree) {
    if (tree == NULL) return;
    switch (tree->tag) {
        case value_exp:
            return;
        case binary_op:
            tree_delete(tree->content.binary.left);
            tree_delete(tree->content.binary.right);
            free(tree);
            break;
        case unary_op:
            tree_delete(tree->content.unary.operand);
            free(tree);
            break;
    }
}

void tree_print(expr* tree) {
    int depth = 0;
    tree_print_work(tree, depth);
}

void tree_print_work(expr* tree, int depth) {
    if (tree == NULL) return;
    for (int i=0; i<depth; i++) printf("---");
    switch (tree->tag) {
        case value_exp:
            token_print(tree->content.value);
            printf("\n");
            break;
        case binary_op:
            depth++;
            token_print(tree->content.value);
            printf("\n");
            tree_print_work(tree->content.binary.left, depth);
            tree_print_work(tree->content.binary.right, depth);
            break;
        case unary_op:
            depth++;
            token_print(tree->content.value);
            printf("\n");
            tree_print_work(tree->content.unary.operand, depth);
            break;
    }
}

expr* error(const char *message, const char *param) {
    printf("In file \"%s\": line %d\n", current_file, current_line);
    printf("Parser error: %s%s\n", message, param);
    exit(EXIT_FAILURE);
}
