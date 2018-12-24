#ifndef ICMD_H
#define ICMD_H


/* API */
int find_icmd(char *cmd);
int call_icmd(int idx, int argc, char **argv);


/* internal commands */
int icmd_help(int argc, char **argv);
int icmd_echo(int argc, char **argv);
int icmd_exit(int argc, char **argv);
int icmd_cls(int argc, char **argv);
int icmd_cd(int argc, char **argv);
int icmd_ls(int argc, char **argv);


#endif /* ICMD_H */
