#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define P_END 0
#define P_BEG 1

typedef struct Command { //single command e.g: {{"cut", "-b", "1-20"}, 3}
    char **expression;
    int no_elements;
} Command;

typedef struct Component { //whole component e.g: grep "^s" | cut -b "1-20", and no_commands = 2, name = "skladnik1"
    char *name;
    Command *commands;
    int no_commands;
} Component;


void parse_expression(char *expr, Command *com) {
    int idx = -1;
    char *tok = strtok(expr, " ");
    while(tok != NULL) {
        idx++;
        com->expression[idx] = malloc(sizeof (tok));
        strcpy(com->expression[idx], tok);
        tok = strtok(NULL, " ");
        //printf("TOK %s\n", tok);
    }
}

Command split_to_command(char *tmp) {
    int cnt = 1;
    for(int i = 0; i < strlen(tmp); i++) {
        if(tmp[i] == ' ') ++cnt;
    }

    Command res;// = malloc(sizeof (Command));
    res.no_elements = 0;
    res.expression = malloc(sizeof (char *));


    int idx = -1;
    char *tok = strtok(tmp, " \t\n");
    while(tok != NULL) {
        idx++;
        res.no_elements++;
        res.expression = realloc(res.expression, res.no_elements * sizeof (char *));
        res.expression[idx] = malloc(sizeof (tok));
        sprintf(res.expression[idx], "%.*s", (size_t)(strlen(tok)), tok);
        //printf("%s\n", tok);
        tok = strtok(NULL, " \t\n");
    }
    res.expression = realloc(res.expression, (res.no_elements + 1) * sizeof (char *));
    res.expression[idx + 1] = NULL;
    return res;
}


void parse_line(char *line, Component *comp) {
    comp->no_commands = 1;
    for(int i = 0; i < strlen(line); i++) {
        if(line[i] == '|')  {
            (comp->no_commands)++;
        }
    }

    char *name = strtok(line, "=");
    comp->name = malloc(sizeof name);
    sprintf(comp->name, "%.*s", (int)(strlen(name) - 1), name);

    comp->commands = malloc(sizeof (Command) * comp->no_commands);

    int id = -1;

    char **lines = malloc(sizeof (char*) * comp->no_commands);


    char *exp = strtok(NULL, "|");
    //printf("%s\n", exp);
    while(exp != NULL) {
        id++;
        lines[id]= malloc(sizeof (exp));
        sprintf(lines[id], "%s", exp + 1);
        exp = strtok(NULL, "|");
    }
    for(int i = 0; i < comp->no_commands; i++) {
        //printf("%s\n", lines[i]);
        comp->commands[i] = split_to_command(lines[i]);
    }

}

void parse_component(char *line, Component *components, int no_components) {

    Command **to_run = malloc(sizeof (Command *));

    int id = -1;
    char *exp = strtok(line, "| \n\t");
    while(exp != NULL) {
        printf("EXECUTE [%s]\n", exp);

        for(int i = 0; i < no_components; i++) {
            //printf("## %d\n", i);
            //printf("%s %s\n", exp, components[i].name);
            if(strcmp(exp, components[i].name) == 0) {
                for(int j = 0; j < components[i].no_commands; j++) {

                    for(int k = 0; k <  components[i].commands[j].no_elements; k++) {
                        printf("%s\n", components[i].commands[j].expression[k]);
                        id++;
                        to_run = realloc(to_run, (id + 1) * sizeof (Command *));
                        to_run[id] = &components[i].commands[j];
                    }
                    //puts("");

                }
                //puts("");
            }
        }
        exp = strtok(NULL, "| \n\t");
    }

    int fds[100][2];


    for(int i = 0; i <= id; i++) {
        if (pipe(fds[i]) < 0) {
            puts("ERROR can't make a pipe");
            exit(1);
        }
    }

    for(int i = 0; i <= id; i++) {
        if (fork() == 0) {
            //printf("%d\n",i);
            if( i > 0) { //pierwszy nie potrzebuje in
                dup2(fds[i - 1][P_END], STDIN_FILENO);
            }
            if(i < id - 1) { //ostatni nie potrzebuje out
                dup2(fds[i][P_BEG], STDOUT_FILENO);
            }
            for(int j = 0; j < id; j++) { //dup2 duplikuje, czyli zamykam zeby nie czekac w programie na EOF
                close(fds[j][P_BEG]);
                close(fds[j][P_END]);
            }
            execvp(to_run[i]->expression[0], to_run[i]->expression);
        }
        //printf("%d\n",i);
    }
    //zamykam w rodzicu
    for(int j = 0; j < 3; j++) {
        close(fds[j][P_BEG]);
        close(fds[j][P_END]);
    }
    for (int i = 0; i < 3; i++)
        wait(NULL);


}


void parse_file(char *file_name) {
    Component *components = NULL;
    int no_components = 0;

    //printf("File: %s \n", file_name);
    FILE *f = fopen(file_name, "r");
    if(f == NULL) {
        printf("Error while reading: %s\n", strerror(errno));
        exit(1);
    }

    char *line;
    size_t len = 0, read;
    while((read = getline(&line, &len, f)) != -1) {
        if(read == 0) continue;
        if(line[0] == '#') {
            printf("Comment: %s", line);
            continue;
        }


        char *eq = strchr(line, '=');
        //printf( " %s\n", eq );
        if(eq != NULL) {
            puts("Adding command");
            no_components++;
            components = realloc(components, no_components * sizeof (Component));
            parse_line(line, &components[no_components - 1]);
        } else {
            puts("Result of command");
            parse_component(line, components, no_components);
        }


        //puts("");

    }
    free(line);

    return;
}


int main(int argc, char **argv) {

    if(argc != 2) {
        puts("ERROR wrong angurments");
    }

    parse_file(argv[1]);

    return 0;


    int status;
    int i;


    char *args[3][4] = {
            {"cat", "/etc/passwd", NULL},
            {"grep", "^s", NULL},
            {"cut", "-b", "1-20", NULL}
    };

    int fds[4][2];


    for(int i = 0; i < 3; i++) {
        if (pipe(fds[i]) < 0) {
            puts("ERROR can't make a pipe");
            exit(1);
        }
    }

    for(int i = 0; i < 3; i++) {
        if (fork() == 0) {
            //printf("%d\n",i);
            if( i > 0) { //pierwszy nie potrzebuje in
                dup2(fds[i - 1][P_END], STDIN_FILENO);
            }
            if(i < 3 - 1) { //ostatni nie potrzebuje out
                dup2(fds[i][P_BEG], STDOUT_FILENO);
            }
            for(int j = 0; j < 2; j++) { //dup2 duplikuje, czyli zamykam zeby nie czekac w programie na EOF
                close(fds[j][P_BEG]);
                close(fds[j][P_END]);
            }
            execvp(args[i][0], args[i]);
        }
        //printf("%d\n",i);
    }
    //zamykam w rodzicu
    for(int j = 0; j < 3; j++) {
        close(fds[j][P_BEG]);
        close(fds[j][P_END]);
    }
    for (i = 0; i < 3; i++)
        wait(&status);


    return 0;
}