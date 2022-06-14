#define _DEFAULT_SOURCE
#include <dirent.h>
#include <err.h>
#include <fnmatch.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "memory.h"
#include "myfind.h"

int type_test(char *path, char *arg)
{
    char c = arg[0];
    struct stat stats;
    if (stat(path, &stats) == -1)
        err(1, "%s:", path);
    mode_t m = stats.st_mode;
    switch (c)
    {
    case 'd':
        return (S_ISDIR(m));
    case 'f':
        return (S_ISREG(m));
    case 'l':
        return (S_ISLNK(m));
    case 'p':
        return (S_ISFIFO(m));
    case 's':
        return (S_ISSOCK(m));
    case 'b':
        return (S_ISBLK(m));
    case 'c':
        return (S_ISCHR(m));
    default:
        errx(1, "Wrong argument after -type");
    }
}

int newer_test(char *path, char *arg)
{
    struct stat stat_other;
    if (stat(arg, &stat_other) == -1)
        err(1, "%s:", path);
    struct stat stat_cur;
    if (stat(path, &stat_cur) == -1)
        err(1, "%s:", path);
    return (stat_other.st_mtim.tv_nsec < stat_cur.st_mtim.tv_sec);
}

char *correct_path(char *path, char *arg)
{
    int len = 0;
    while (arg[len] != '\0')
        len++;
    int len_p = 0;
    while (path[len_p] != '\0')
        len_p++;
    len--;
    if (arg[len] != ';')
        errx(1, "A ';' is needed at the end of the exec");
    char *com = my_calloc(len, sizeof(char));
    int j = 0;
    for (int i = 0; i < len; i++)
    {
        if (arg[i] != '{')
            com[j++] = arg[i];
        else
        {
            com = my_reallocarray(com, len + len_p, sizeof(char));
            strcat(com, path);
            i++;
            j += len_p;
        }
    }
    return com;
}

int exec_test(char *path, char *arg, int **print)
{
    int pid = fork();
    if (pid == -1)
        errx(1, "Fork failed");
    char *com = correct_path(path, arg);
    if (pid == 0)
        execlp("/bin/sh", "myfind", "-c", com, NULL);
    else
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 1)
            return 0;
    }
    free(com);
    **print = 1;
    return 1;
}

int execdir_test(char *name, char *arg, int **print)
{
    int pid = fork();
    if (pid == -1)
        errx(1, "Fork failed");
    int len = 0;
    while (name[len] != '\0')
        len++;
    char *new_name = my_malloc(len + 3);
    strcpy(new_name, "./");
    strcat(new_name, name);
    char *com = correct_path(new_name, arg);
    free(new_name);

    if (pid == 0)
        execlp("/bin/sh", "myfind", "-c", com, NULL);
    else
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 1)
            return 0;
    }

    free(com);
    **print = 1;
    return 1;
}

int delete (char *path)
{
    if (remove(path) == 0)
        return 1;
    errx(1, "cannot delete ‘%s’: No such file or directory", path);
}

int user(char *path, char *arg)
{
    struct stat sb;
    stat(path, &sb);

    struct passwd *pw = getpwuid(sb.st_uid);
    return (strcmp(arg, pw->pw_name) == 0);
}

int group(char *path, char *arg)
{
    struct stat sb;
    stat(path, &sb);

    struct group *gr = getgrgid(sb.st_gid);

    return (strcmp(arg, gr->gr_name) == 0);
}

int perm(char *path, char *arg)
{
    struct stat buf;
    stat(path, &buf);
    int statchmod = buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);

    int md = 0;
    int i = 1;

    while (statchmod != 0)
    {
        md += (statchmod % 8) * i;
        statchmod /= 8;
        i *= 10;
    }

    return (md == atoi(arg));
}

/* evaluate the expression */
int eval_expr(struct my_ast *ast, char *path, char *name, int *print)
{
    if (ast->token.type_of == TOKEN_PRINT)
    {
        printf("%s\n", path);
        *print = 1;
        return 1;
    }
    else if (ast->token.type_of == TOKEN_NAME)
    {
        if (ast->token.args == NULL)
            errx(1, "An argument is needed after -name");
        return (!fnmatch(ast->token.args, name, 0));
    }
    else if (ast->token.type_of == TOKEN_TYPE)
    {
        if (ast->token.args == NULL)
            errx(1, "An argument is needed after -type");
        return type_test(path, ast->token.args);
    }
    else if (ast->token.type_of == TOKEN_NEWER)
    {
        if (ast->token.args == NULL)
            errx(1, "An argument is needed after -newer");
        return newer_test(path, ast->token.args);
    }
    else if (ast->token.type_of == TOKEN_EXEC)
    {
        if (ast->token.args == NULL)
            errx(1, "Arguments are needed after -exec");
        return exec_test(path, ast->token.args, &print);
    }
    else if (ast->token.type_of == TOKEN_EXECDIR)
    {
        if (ast->token.args == NULL)
            errx(1, "Argument are needed after -execdir");
        return execdir_test(name, ast->token.args, &print);
    }
    else if (ast->token.type_of == TOKEN_DELETE)
        return delete (path);
    else if (ast->token.type_of == TOKEN_USER)
    {
        if (ast->token.args == NULL)
            errx(1, "Argument are needed after -user");
        return user(path, ast->token.args);
    }
    else if (ast->token.type_of == TOKEN_GROUP)
    {
        if (ast->token.args == NULL)
            errx(1, "Argument are needed after -group");
        return group(path, ast->token.args);
    }
    else if (ast->token.type_of == TOKEN_PERM)
    {
        if (ast->token.args == NULL)
            errx(1, "Argument are needed after -perm");
        return perm(path, ast->token.args);
    }
    else
        errx(1, "This expression is not implemented\n");
    return 0;
}

/* evalue the ast */
int eval_ast(struct my_ast *ast, char *path, char *name, int *print)
{
    if (ast == NULL)
        errx(1, "Wait child, but none");
    if (ast->token.type_of == TOKEN_OR)
    {
        int left = eval_ast(ast->children.left, path, name, print);
        if (left)
            return 1;
        int right = eval_ast(ast->children.right, path, name, print);
        if (right)
            return 1;
        return 0;
    }
    if (ast->token.type_of == TOKEN_AND)
    {
        int left = eval_ast(ast->children.left, path, name, print);
        if (!left)
            return 0;
        int right = eval_ast(ast->children.right, path, name, print);
        if (right)
            return 1;
        return 0;
    }
    return eval_expr(ast, path, name, print);
}
