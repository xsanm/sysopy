#define  _GNU_SOURCE
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

void execute_commands(Command **to_run, int cnt);
Command split_to_command(char *tmp);
void parse_line(char *line, Component *comp);
void parse_file(char *file_name);
void parse_component(char *line, Component *components, int no_components);



int main(int argc, char **argv) {

    if(argc != 2) {
        puts("ERROR wrong angurments");
    }
    parse_file(argv[1]);

    return 0;

}

Command split_to_command(char *tmp) {
    Command res;
    res.no_elements = 0;
    res.expression = malloc(sizeof (char *));

    char *tok = strtok(tmp, " \t\n");
    while(tok != NULL) {
        res.no_elements++;
        res.expression = realloc(res.expression, res.no_elements * sizeof (char *));
        res.expression[res.no_elements - 1] = malloc(sizeof (tok));
        strcpy(res.expression[res.no_elements - 1], tok);
        tok = strtok(NULL, " \t\n");
    }
    res.expression = realloc(res.expression, (res.no_elements + 1) * sizeof (char *));
    res.expression[res.no_elements] = NULL;
    return res;
}

void parse_line(char *line, Component *comp) {
    comp->no_commands = 1;
    for(int i = 0; i < strlen(line); i++) {
        if(line[i] == '|')  {
            (comp->no_commands)++;
        }
    }

    //extracing name
    char *name = strtok(line, "=");
    comp->name = malloc(sizeof name);
    sprintf(comp->name, "%.*s", (int)(strlen(name) - 1), name);

    //parsing commands
    comp->commands = malloc(sizeof (Command) * comp->no_commands);
    char **lines = malloc(sizeof (char*) * comp->no_commands);
    int id = -1;

    char *exp = strtok(NULL, "|");
    while(exp != NULL) {
        id++;
        lines[id]= malloc(sizeof (exp));
        sprintf(lines[id], "%s", exp + 1);
        exp = strtok(NULL, "|");
    }

    //spliiting each command from char** to Command object
    for(int i = 0; i < comp->no_commands; i++) {
        comp->commands[i] = split_to_command(lines[i]);
    }

    for(int i = 0; i < comp->no_commands; i++) free(lines[i]);
    free(lines);
}

void execute_commands(Command **to_run, int cnt){
    int **fds = malloc(sizeof (int *) * cnt);

    //creating pipes
    for(int i = 0; i < cnt ; i++) {
        fds[i] = malloc(sizeof (int) * 2);
        if (pipe(fds[i]) < 0) {
            puts("ERROR can't make a pipe");
            exit(1);
        }
    }

    for(int i = 0; i < cnt; i++) {
        if (fork() == 0) {
            if( i > 0) { //pierwszy nie potrzebuje in
                dup2(fds[i - 1][P_END], STDIN_FILENO);
            }
            if(i < cnt - 1) { //ostatni nie potrzebuje out
                dup2(fds[i][P_BEG], STDOUT_FILENO);
            }
            for(int j = 0; j < cnt; j++) { //dup2 duplikuje, czyli zamykam zeby nie czekac w programie na EOF
                close(fds[j][P_BEG]);
                close(fds[j][P_END]);
            }
            execvp(to_run[i]->expression[0], to_run[i]->expression);
        }
    }
    //zamykam w rodzicu
    for(int j = 0; j < cnt; j++) {
        close(fds[j][P_BEG]);
        close(fds[j][P_END]);
    }
    for (int i = 0; i < cnt; i++)
        wait(NULL);

    puts("\n");
    for(int i = 0; i < cnt ; i++) {
        free(fds[i]);
    }
    free(fds);
}

void parse_component(char *line, Component *components, int no_components) {
    puts("\n###################");

    Command **to_run = malloc(sizeof (Command *));

    int id = -1;
    char *exp = strtok(line, "| \n\t");
    while(exp != NULL) {
        for(int i = 0; i < no_components; i++) {
            if(strcmp(exp, components[i].name) == 0) {
                for(int j = 0; j < components[i].no_commands; j++) {
                    for(int k = 0; k <  components[i].commands[j].no_elements; k++) {
                        //printf("%s\n", components[i].commands[j].expression[k]);
                        id++;
                        to_run = realloc(to_run, (id + 1) * sizeof (Command *));
                        to_run[id] = &components[i].commands[j];
                    }
                }
            }
        }
        exp = strtok(NULL, "| \n\t");
    }
    execute_commands(to_run, id + 1);


    free(to_run);
}

void parse_file(char *file_name) {
    Component *components = NULL;
    int no_components = 0;

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
        if(eq != NULL) {
            //puts("Adding command");
            no_components++;
            components = realloc(components, no_components * sizeof (Component));
            parse_line(line, &components[no_components - 1]);
        } else {
            //puts("Result of command");
            parse_component(line, components, no_components);
        }
    }
    free(line);
    for(int i = 0; i < no_components; i++) {
        for(int j = 0; j < components[i].no_commands; j++) {
            for(int k = 0; k < components[i].commands[j].no_elements; k++)
                free(components[i].commands[j].expression[k]);
            free(components[i].commands[j].expression);
        }
        free(components[i].commands);
        free(components[i].name);
    }
    free(components);
    fclose(f);
    return;
}