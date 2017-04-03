#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

/*Let’s look at a shell from the top down. A shell does three main things in its lifetime.

    Initialize: In this step, a typical shell would read and execute its configuration files. These change aspects of the shell’s behavior.
    Interpret: Next, the shell reads commands from stdin (which could be interactive, or a file) and executes them.
    Terminate: After its commands are executed, the shell executes any shutdown commands, frees up any memory, and terminates.
*/
struct command
{
  const char **argv;
};

/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_ls(char **args);
int lsh_pipe(int n, struct command *cmd);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "ls-custom",
  "pipe"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
  &lsh_ls,
  &lsh_pipe
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

int lsh_help(char **args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int lsh_exit(char **args)
{
  return 0;
}

/*In the code below, we first fetch the current working directory through the environment variable PWD.
Since now we get the working directory, we open it and iterate over its contents by reading the directory.
If the name of the file/folder begins with a '.', then just ignore it as the standard 'ls' also 
does not display the file beginning with '.' if its executed without any flags.
With each file/folder (whose name does not begin with '.') encountered, we display the name of it on stdout.*/

int lsh_ls(char **args)
{
    char *curr_dir = NULL; 
    DIR *dp = NULL; 
    struct dirent *dptr = NULL; 
    unsigned int count = 0; 
  
    curr_dir = getenv("PWD"); 
    if(NULL == curr_dir) 
    { 
        printf("\n ERROR : Could not get the working directory\n"); 
        return -1; 
    } 
  
    dp = opendir((const char*)curr_dir); 
    if(NULL == dp) 
    { 
        printf("\n ERROR : Could not open the working directory\n"); 
        return -1; 
    } 
  
    printf("\n"); 
    for(count = 0; NULL != (dptr = readdir(dp)); count++) 
    { 
        // Check if the name of the file/folder begins with '.' 
        // If yes, then do not display it. 
        if(dptr->d_name[0] != '.') 
            printf("%s  ",dptr->d_name); 
    } 
   printf("\n");   
  return 1;
}

int spawn_proc (int in, int out, struct command *cmd)
{
  pid_t pid;

  if ((pid = fork ()) == 0)
    {
      if (in != 0)
        {
          dup2 (in, 0);
          close (in);
        }

      if (out != 1)
        {
          dup2 (out, 1);
          close (out);
        }

      return execvp (cmd->argv [0], (char * const *)cmd->argv);
    }

  return pid;
}

int lsh_pipe (int n, struct command *cmd)
{
  int i;
  pid_t pid;
  int in, fd [2];

  /* The first process should get its input from the original file descriptor 0.  */
  in = 0;

  /* Note the loop bound, we spawn here all, but the last stage of the pipeline.  */
  for (i = 0; i < n - 1; ++i)
    {
      pipe (fd);

      /* f [1] is the write end of the pipe, we carry `in` from the prev iteration.  */
      spawn_proc (in, fd [1], cmd + i);

      /* No need for the write end of the pipe, the child will write here.  */
      close (fd [1]);

      /* Keep the read end of the pipe, the next child will read from there.  */
      in = fd [0];
    }

  /* Last stage of the pipeline - set stdin be the read end of the previous pipe
     and output to the original file descriptor 1. */  
  if (in != 0)
    dup2 (in, 0);

  /* Execute the last stage with the current process. */
  return execvp (cmd [i].argv [0], (char * const *)cmd [i].argv);
}


/*All this does is check if the command equals each builtin, and if so, run it. 
If it doesn’t match a builtin, it calls lsh_launch() to launch the process. */
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

/* I’m going to make a glaring simplification here, 
 and say that we won’t allow quoting or backslash escaping in our command line arguments. 
 Instead, we will simply use whitespace to separate arguments from each other. 
 So the command echo "this message" would not call echo with a single argument this message, 
 but rather it would call echo with two arguments: "this and message".
At the start of the function, we begin tokenizing by calling strtok. It returns a pointer to the first token. 
What strtok() actually does is return pointers to within the string you give it, and place \0 bytes at the end of each token. 
We store each pointer in an array (buffer) of character pointers.
Finally, we reallocate the array of pointers if necessary. 
The process repeats until no token is returned by strtok, at which point we null-terminate the list of tokens.
So, once all is said and done, we have an array of tokens, ready to execute.
*/

char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}


char *lsh_read_line(void)
{
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  getline(&line, &bufsize, stdin);
  return line;
}

void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}



int main(int argc, char **argv)
{
  // Load config files, if any.
  const char *lscmd[] = { "ls", "-al", NULL};
  const char *wccmd[] = { "wc", NULL};
  const char *awkcmd[] = { "awk", "{print $2}", NULL};
 
  struct command cmd [] = { {lscmd}, {wccmd}, {awkcmd} };
  // Run command loop.

  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

