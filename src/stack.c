#include <err.h>
#include <stdlib.h>

#include "myfind.h"

struct stack *stack_push(struct stack *s, struct my_token token)
{
    struct stack *push = malloc(sizeof(struct stack));
    if (!push)
        errx(1, "Can't malloc a new token in the stack\n");
    push->tokens = token;
    push->next = s;
    return push;
}

struct stack *stack_pop(struct stack *s)
{
    if (!s)
        return NULL;
    struct stack *top = s;
    s = s->next;
    top->next = NULL;
    free(top);
    return s;
}

struct my_token stack_peek(struct stack *s)
{
    return s->tokens;
}

int stack_size(struct stack *stack)
{
    int len = 0;
    while (stack != NULL)
    {
        len++;
        stack = stack->next;
    }
    return len;
}

void stack_destroy(struct stack *s)
{
    struct stack *tmp = s;
    while (tmp != NULL)
    {
        free(s);
        s = tmp;
        tmp = tmp->next;
    }
    free(s);
}

/* second type of stack with ast tree */

struct stack_tree *stack_push_t(struct stack_tree *s, struct my_ast *ast)
{
    struct stack_tree *push = malloc(sizeof(struct stack_tree));
    if (!push)
        errx(1, "Can't malloc a new token in the stack_tree\n");
    push->ast = ast;
    push->next = s;
    return push;
}

struct stack_tree *stack_pop_t(struct stack_tree *s)
{
    if (!s)
        return NULL;
    struct stack_tree *top = s;
    s = s->next;
    top->next = NULL;
    free(top);
    return s;
}

struct my_ast *stack_peek_t(struct stack_tree *s)
{
    return s->ast;
}

int stack_size_t(struct stack_tree *stack)
{
    int len = 0;
    while (stack != NULL)
    {
        len++;
        stack = stack->next;
    }
    return len;
}

void stack_destroy_t(struct stack_tree *s)
{
    struct stack_tree *tmp = s;
    while (tmp != NULL)
    {
        free(s);
        s = tmp;
        tmp = tmp->next;
    }
    free(s);
}
