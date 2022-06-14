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

struct my_ast
{
    struct my_token token;
    struct
    {
        struct my_ast *left;
        struct my_ast *right;
    } children;
};

struct stack
{
    struct my_token tokens;
    struct stack *next;
};

struct stack_tree
{
    struct my_ast *ast;
    struct stack_tree *next;
};

struct stack *stack_push(struct stack *s, struct my_token token);
struct stack *stack_pop(struct stack *s);
struct my_token stack_peek(struct stack *s);
int stack_size(struct stack *stack);

struct stack_tree *stack_push_t(struct stack_tree *s, struct my_ast *ast);
struct stack_tree *stack_pop_t(struct stack_tree *s);
struct my_ast *stack_peek_t(struct stack_tree *s);
int stack_size_t(struct stack_tree *stack);

struct stack *parse_expr(char **strs, unsigned length);
struct my_token *create_token(char **strs, unsigned *length,
                              struct my_token *tokens);
struct my_token parse_token(char *str);

struct my_ast *build_ast(struct stack *s);
struct stack_tree *add_or(struct stack_tree *st_ast, struct my_ast *ast);
struct stack_tree *add_and(struct stack_tree *st_ast, struct my_ast *ast);
struct stack_tree *reverse_stack(struct stack *old);

int eval_ast(struct my_ast *ast, char *path, char *name, int *print);
int eval_expr(struct my_ast *ast, char *path, char *name, int *print);

#endif /* ! PARSER_H */
