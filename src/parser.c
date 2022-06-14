#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "myfind.h"

/* empty the ope stack at the end */
struct stack *empty_ope(struct stack *ope, struct stack *output)
{
    while (ope)
    {
        output = stack_push(output, stack_peek(ope));
        ope = stack_pop(ope);
    }
    return output;
}

/* check priority */
int priority(struct my_token token)
{
    if (token.type_of == TOKEN_AND)
        return 2;
    if (token.type_of == TOKEN_OR)
        return 1;
    return 0;
}

/* create token add for implicit add */
struct my_token and_implicit()
{
    struct my_token token;
    token.type = OPERATOR;
    token.type_of = TOKEN_AND;
    token.args = NULL;
    return token;
}

/* build the stack which will be used to build the ast */
struct stack *build_stack(struct my_token *tokens, int length)
{
    struct stack *ope = NULL;
    struct stack *output = NULL;
    int ope_before = 1;
    for (int i = 0; i < length; i++)
    {
        if (tokens[i].type != OPERATOR)
        {
            if (!ope_before)
                ope = stack_push(ope, and_implicit());
            else
                ope_before = 0;
            output = stack_push(output, tokens[i]);
        }
        else if (tokens[i].type_of == TOKEN_LPAR)
            ope = stack_push(ope, tokens[i]);
        else if (tokens[i].type_of == TOKEN_RPAR)
        {
            while (ope && stack_peek(ope).type_of != TOKEN_LPAR)
            {
                output = stack_push(output, stack_peek(ope));
                ope = stack_pop(ope);
            }
            if (!ope)
                errx(1, "Mismatch parenthesis");
            ope = stack_pop(ope);
        }
        else if (tokens[i].type_of == TOKEN_NOT)
            continue;
        else
        {
            while (ope && stack_peek(ope).type_of != TOKEN_LPAR
                   && priority(stack_peek(ope)) >= priority(tokens[i]))
            {
                output = stack_push(output, stack_peek(ope));
                ope = stack_pop(ope);
            }
            ope = stack_push(ope, tokens[i]);
            ope_before = 1;
        }
    }
    output = empty_ope(ope, output);
    return output;
}

/* associate the token to a str */
struct my_token parse_token(char *str)
{
    static struct token_model exprs[] = { { "-a", TOKEN_AND, OPERATOR },
                                          { "-o", TOKEN_OR, OPERATOR },
                                          { "!", TOKEN_NOT, OPERATOR },
                                          { "(", TOKEN_LPAR, OPERATOR },
                                          { ")", TOKEN_RPAR, OPERATOR },
                                          { "-print", TOKEN_PRINT, ACTION },
                                          { "-exec", TOKEN_EXEC, ACTION },
                                          { "-execdir", TOKEN_EXECDIR, ACTION },
                                          { "-delete", TOKEN_DELETE, ACTION },
                                          { "-name", TOKEN_NAME, TEST },
                                          { "-type", TOKEN_TYPE, TEST },
                                          { "-newer", TOKEN_NEWER, TEST },
                                          { "-perm", TOKEN_PERM, TEST },
                                          { "-user", TOKEN_USER, TEST },
                                          { "-group", TOKEN_GROUP, TEST }

    };

    struct my_token token;
    for (unsigned i = 0; i < sizeof(exprs) / sizeof(*exprs); ++i)
    {
        if (strcmp(str, exprs[i].str) == 0)
        {
            token.type_of = exprs[i].type_of;
            token.type = exprs[i].type;
            token.args = NULL;
            return token;
        }
    }

    token.type = ARGUMENT;
    token.type_of = TOKEN_ARG;
    token.args = NULL;
    return token;
}

/* create the tokens table */
struct my_token *create_token(char **strs, unsigned *length,
                              struct my_token *tokens)
{
    unsigned final_length = *length;
    int j = 0;
    for (unsigned i = 0; i < *length; ++i)
    {
        tokens[j] = parse_token(strs[i]);
        if (tokens[j].type == ARGUMENT)
        {
            int has_arg = 0;
            int len = 0;
            if (j > 0 && tokens[j - 1].args != NULL)
            {
                char *first_args = tokens[j - 1].args;
                while (first_args[len] != '\0')
                    len++;
                has_arg = 1;
                len++;
            }
            char *name = strs[i];
            int n = 0;
            while (name[n] != '\0')
                n++;
            len += n + 1;
            if (has_arg)
            {
                len++;
                char *args = my_calloc(len, sizeof(char));
                strcat(args, tokens[j - 1].args);
                strcat(args, " ");
                strcat(args, name);
                tokens[j - 1].args = args;
            }
            else
            {
                char *args = my_calloc(len, sizeof(char));
                strcat(args, name);
                tokens[j - 1].args = args;
            }
            final_length--;
        }
        else
            j++;
    }
    if (final_length != *length)
    {
        *length = final_length;
        tokens = my_reallocarray(tokens, *length, sizeof(struct my_token));
    }

    return tokens;
}

/* main function */
struct stack *parse_expr(char **strs, unsigned length)
{
    struct my_token *tokens = my_malloc(length * sizeof(struct my_token));
    tokens = create_token(strs, &length, tokens);

    struct stack *stack = build_stack(tokens, length);
    free(tokens);
    return stack;
}
