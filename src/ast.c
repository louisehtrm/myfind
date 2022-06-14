#include <err.h>
#include <stddef.h>

#include "memory.h"
#include "myfind.h"

/* convert stack with ast and reverse stack */
struct stack_tree *reverse_stack(struct stack *old)
{
    struct stack_tree *new = NULL;
    while (old)
    {
        struct my_ast *ast = my_malloc(sizeof(struct my_ast));
        ast->token = stack_peek(old);
        ast->children.left = NULL;
        ast->children.right = NULL;
        old = stack_pop(old);
        new = stack_push_t(new, ast);
    }
    return new;
}

/* add and operator and add its children */
struct stack_tree *add_and(struct stack_tree *st_ast, struct my_ast *ast)
{
    if (stack_size_t(st_ast) < 2)
        errx(1, "Operator -a need two expression to be used");
    ast->children.right = stack_peek_t(st_ast);
    st_ast = stack_pop_t(st_ast);
    ast->children.left = stack_peek_t(st_ast);
    st_ast = stack_pop_t(st_ast);
    return stack_push_t(st_ast, ast);
}

/* add or operator and add its children */
struct stack_tree *add_or(struct stack_tree *st_ast, struct my_ast *ast)
{
    if (stack_size_t(st_ast) < 2)
        errx(1, "Operator -a need two expression to be used");
    ast->children.right = stack_peek_t(st_ast);
    st_ast = stack_pop_t(st_ast);
    ast->children.left = stack_peek_t(st_ast);
    st_ast = stack_pop_t(st_ast);
    return stack_push_t(st_ast, ast);
}

/* build the ast with rpn */
struct my_ast *build_ast(struct stack *s)
{
    struct stack_tree *stack = reverse_stack(s);
    struct stack_tree *st_ast = NULL;
    while (stack != NULL)
    {
        struct my_ast *ast = stack_peek_t(stack);
        stack = stack_pop_t(stack);
        if (ast->token.type != OPERATOR)
            st_ast = stack_push_t(st_ast, ast);
        else if (ast->token.type_of == TOKEN_AND)
            st_ast = add_and(st_ast, ast);
        else if (ast->token.type_of == TOKEN_OR)
            st_ast = add_or(st_ast, ast);
        else if (ast->token.type_of == TOKEN_NOT)
            continue;
        else
            errx(1, "Token type '%d' not usable here", ast->token.type_of);
    }

    if (st_ast)
    {
        struct my_ast *ast = stack_peek_t(st_ast);
        st_ast = stack_pop_t(st_ast);
        if (st_ast)
            errx(1, "Wrong implementation: the stack is not empty at the end");
        return ast;
    }
    errx(1, "Nothing ast left in the stack at the end");
}
