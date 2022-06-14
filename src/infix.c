#include "evalexpr.h"
#include <stdlib.h>

/* add numbers to the output */
void number(char **s, char **output)
{
    while (**s >= '0' && **s <= '9')
    {
        **output = **s;
        (*s)++;
        (*output)++;
    }
    (*s)--;
    **output = ' ';
    (*output)++;
}

/* add an operande to the ope stack */
struct stack *ope_stack(struct stack *stack, char c)
{
    union int_or_char n;
    n.char_t = c;
    return stack_push(stack, OPERANDE, n);
}

/*compute the length of the original char * */
int length(char *s)
{
    int len = 0;
    while (s[len] != '\0')
        len++;
    return len;
}

/* case of a right parenthesis */
struct stack *right_par(struct stack *ope, char **output, int *error)
{
    while (stack_peek(ope).char_t != '(' && ope)
    {
        **output = stack_peek(ope).char_t;
        (*output)++;
        **output = ' ';
        (*output)++;

        ope = stack_pop(ope);
    }
    if (!ope)
        *error = 2;

    if (ope)
        ope = stack_pop(ope);
    return ope;
}

/* check priority */
int nb_prio(char c)
{
    if (c == '(')
        return 4;
    if (c == '^')
        return 3;
    if (c == '*' || c == '/' || c == '%')
        return 2;
    return 1;
}

/* Left assos = 1 and rigth assos = 0 */
struct stack *priority(struct stack *ope, char c, int assos, char **output)
{
    if (ope)
    {
        int prT = nb_prio(stack_peek(ope).char_t);
        int prS = nb_prio(c);
        if (prT > prS && prT != 0)
            while (ope && stack_peek(ope).char_t != '('
                    && (prT > prS || (prT == prS && assos)))
            {
                **output = stack_peek(ope).char_t;
                (*output)++;
                **output = ' ';
                (*output)++;

                ope = stack_pop(ope);
            }
    }
    ope = ope_stack(ope, c);
    return ope;
}

/* empty the ope stack at the end */
void empty_ope(struct stack *ope, char **output)
{
    while (ope)
    {
        **output = stack_peek(ope).char_t;
        (*output)++;
        **output = ' ';
        (*output)++;
        ope = stack_pop(ope);
    }
}

/* main function */
char *infix(char *s, int *error)
{
    struct stack *ope = NULL;
    char *output = malloc(sizeof(char) * length(s) * 2);
    char *keep_adress = output;
    while (*s != '\0')
    {
        if (*s >= '0' && *s <= '9')
            number(&s, &output);
        else if (*s == '(')
            ope = ope_stack(ope, '(');
        else if (*s == ')')
            ope = right_par(ope, &output, error);
        else if (*s == '^')
            ope = priority(ope, *s, 0, &output);
        else if (*s == '/' || *s == '*' || *s == '%' || *s == '+' || *s == '-')
            ope = priority(ope, *s, 1, &output);
        else if (*s != ' ')
        {
            *error = 1;
            return output;
        }
        s++;
    }
    empty_ope(ope, &output);
    return keep_adress;
}
