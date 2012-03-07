#include "preprocessor.h"

#include "token.h"

/* helper function for deleting tokens inside vector and map */
static void hlp_delete_token(void *t) {
    token_delete( (token*) t );
}

void preprocess(vector *tokens) {
    for (int i=0; i < tokens->size; i++) {
        if (((token*)tokens->elements[i])->id == e_comment) {
            vector_erase(tokens, i, hlp_delete_token);
        }
    }
}
