#include "myfind.h"

#include <dirent.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "memory.h"

void free_ast(struct my_ast *ast)
{
    if (ast == NULL)
        return;
    struct my_token token = ast->token;
    if (token.args != NULL)
        free(token.args);
    struct my_ast *ast_left = ast->children.left;
    struct my_ast *ast_right = ast->children.right;
    free(ast);
    free_ast(ast_left);
    free_ast(ast_right);
}

void basic_find(char *path, size_t len)
{
    DIR *dir = opendir(path);
    if (!dir)
        return;

    struct dirent *read;

    while ((read = readdir(dir)) != NULL)
    {
        char *name = read->d_name;
        if (name[0] != '.')
        {
            size_t n = 0;
            while (name[n] != '\0')
                n++;
            n += len + 2;
            char *dest = malloc(n);
            dest[0] = '\0';
            strcat(dest, path);
            strcat(dest, name);
            printf("%s\n", dest);
            strcat(dest, "/");
            basic_find(dest, n);
            free(dest);
        }
    }
    closedir(dir);
}

void rec_file(char *path, struct stack *rpn, struct my_ast *ast, size_t len)
{
    DIR *dir = opendir(path);
    if (!dir)
        return;

    struct dirent *read;

    int add = 0;

    if (len == 2)
        add = 1;

    while ((read = readdir(dir)) != NULL)
    {
        char *name = read->d_name;
        if (name[0] != '.')
        {
            size_t n = 0;
            while (name[n] != '\0')
                n++;
            n += len + 2;
            char *dest = malloc(n);
            dest[0] = '\0';
            strcat(dest, path);
            strcat(dest, name);

            int print = 0;
            int res = eval_ast(ast, dest, name, &print);
            if (print == 0 && res)
            {
                if (add)
                {
                    printf(".\n");
                    add = 0;
                }
                printf("%s\n", dest);
            }
            strcat(dest, "/");
            rec_file(dest, rpn, ast, n);
            free(dest);
        }
    }

    closedir(dir);
}

void myfind(int argc, char *argv[], char *path, int k)
{
    int len = 0;
    while (path[len] != '\0')
        len++;

    int n = argc - k;

    struct stack *rpn = parse_expr(argv + k, n);
    struct my_ast *ast = build_ast(rpn);

    int print = 0;
    int res = eval_ast(ast, path, path, &print);
    if (print == 0 && res)
        printf("%s\n", path);
    if (path[0] != '.')
        strcat(path, "/");
    else
        path = "./";
    len++;

    rec_file(path, rpn, ast, len);

    if (ast)
        free_ast(ast);
}

int main(int argc, char *argv[])
{
    if (argc > 1 && argv[1][0] != '-' && argv[1][0] != '!')
    {
        size_t len = strlen(argv[1]) + 2;
        char *path = my_malloc(len);
        strcpy(path, argv[1]);
        if (argc == 2)
        {
            printf("%s\n", path);
            strcat(path, "/");
            basic_find(path, len);
            free(path);
        }
        else
        {
            myfind(argc, argv, path, 2);
            free(path);
        }
    }
    else
    {
        if (argc == 1)
        {
            char *path = "./";
            printf(".\n");
            basic_find(path, 2);
        }
        else
        {
            char *path = my_malloc(3);
            path = ".";
            myfind(argc, argv, path, 1);
        }
    }
    return 0;
}
