#ifndef PARSER_H
#define PARSER_H

enum token_type_of
{
    /* test */
    TOKEN_NAME,
    TOKEN_TYPE,
    TOKEN_NEWER,
    TOKEN_PERM,
    TOKEN_USER,
    TOKEN_GROUP,
    /* operator */
    TOKEN_OR,
    TOKEN_AND,
    TOKEN_NOT,
    TOKEN_LPAR,
    TOKEN_RPAR,
    /* actions */
    TOKEN_PRINT,
    TOKEN_EXEC,
    TOKEN_EXECDIR,
    TOKEN_DELETE,
    /* arguments */
    TOKEN_ARG,
};

enum token_type
{
    TEST,
    OPERATOR,
    ACTION,
    ARGUMENT,
};

struct my_token
{
    enum token_type_of type_of;
    enum token_type type;
    char *args;
};

struct token_model
{
    char *str;
    enum token_type_of type_of;
    enum token_type type;
};

struct stack
{
    struct my_token tokens;
    struct stack *next;
};

struct stack *stack_push(struct stack *s, struct my_token token);
struct stack *stack_pop(struct stack *s);
struct my_token stack_peek(struct stack *s);
int stack_size(struct stack *stack);

struct my_token *parse_expr(char **strs, unsigned length);
struct my_token *create_token(char **strs, unsigned *length,
        struct my_token *tokens);
struct my_token parse_token(char *str);

#endif /* ! PARSER_H */
