# High level workflow:

**Step 1:** Check to see if any previous background commands have been completed,
and if so, print their return status and remove them from the corresponding
data structure.

**Step 2:** Print the shell prompt.

**Step 3:** Read user inputs and parse them into different tasks, which are stored
in a data structure, if parse error occurs, print out the error and skip to
**Step 1.**

**Step 4:** execute all the tasks parsed in step 3. If the command runs in
background, store their information into the corresponding data structure and
skip. Otherwise, wait for all active tasks to finish before jumping back to
**Step 1.**

# Error handling (high level):

Errors can occur during parsing and executing phases. We added all possible
error types to an enum. We also implemented a _printErrorMessage_ function
that takes in an enum error type and print out the corresponding error message
by using switch statements. The _printErrorMessage_ function might only be
invoked during the parsing and executing phase.

# Command parsing:

Each command is broken down into one or more tasks. A task is a command that
doesn't contain any pipe. We apply the OOD design principle by creating a Task
struct to store all useful information about a task such as its program/
executable name, argument lists, argument count, and finally input/output
redirect filenames.

To parse the entire command, we start by using _fgets_ to obtain user input.
Then our parser function will parse the user input into an array of task
struct. The parser reads one char at a time. Chars are categorized into two
sets: normal chars and special chars. Special chars include newline, input/
output redirect sign, pipe sign, background job sign and finally the space.
Whenever a special char is encountered, the parser will jump into different
branches based on what the special char is. For example, when a pipe sign is
seen, the parser will know that the parsing of the current task is done and a
new task will come afterwards. The parser will then dynamically allocate some
memory, copy the current task into the task struct array and reset relevant
counter variables to get ready for parsing the next task. Similarly, when an
input/output redirect sign is reached, the parser will move ahead to read the
entire filename and open the file before copying the file descriptor to the
current task struct. Moreover, the “_bg_” flag will be set to true whenever an
ampersand is detected.

As soon as a parsing error is found, _printErrorMessage_ will be invoked and
the shell will jump back to read the next input from user. For example, when
_<_ is encountered, the parser will invoke _getFileName_ to retrieve the file
name. If the file name has length zero, then the parser will immediately
return with a parsing error status. When the _main_ function receives
this status, it will invoke _printErrorMessage_ and then jump to the next
iteration.

# Background commands (high level):

Each background command is represented by a Node struct, which contains number
of tasks within the command, command name, the mapping between one task to its
pid number etc. We use a linked list structure _List_ to store all the running
background commands. We call the _addNode_ function to add a new _Node_ to the
_List_. _processList_ will look over all _Nodes_ in the _List_ and remove
_Nodes_ that have been completed before printing the exit status message. To
accomplish that,the _processList_ function calls _waitpid_ with the _WNOHANG_
flag and the pid number of all tasks within a _Node_. If the return value is
positive, it then knows that specific task has completed. In this way,
_processList_ will see if all the tasks within a _Node_ have completed. If so,
then the corresponding _Node_ will be removed from the _List_.

_addNode_ will only be evoked during the execution phase. _processList_ will
be called in two places:1. before a new prompt is printed 2. before the exit
status of completed foreground tasks are printed

# Command executing:

This phase will happen after the parsing is all completed. The executing
function will now have access to the task list, command name, number of tasks,
background job flag etc. It will then iterate through the task list and do the
execution by handling input/output redirect, setting up pipes, forking child
processes, invoking _execvp_ and wait for active tasks to finish before
printing out their return status.

Before forking a child process to execute the task, we first set up pipes and
modify _stdout_, _stdin_ accordingly. If the current task is not the first
task in the command, then we need to set stdin to the read end of the previous
pipe so that the current task can read the output from the previous task as its
input. If the current task is not the last task, then we need to set up a new
pipe and point stdout to the its write end so that the output of current task
will be seen by the next task as its input. Finally, if the current task is
the last one, then we must point stdout to its original value. We were able
to do this by using _dup_ to make a copy of _stdout_ before the loop starts.

In case of built-in commands like _pwd_, _cd_ etc, no child process will be
created and the shell will handle it by invoking self-defined functions
such as _myPwd_.

If the command needs to be handled by an external program, We need to fork a
new process and use _dup2_ to make _stdout_ and _stdin_ point to the
corresponding input/output redirect files if they exists. Finally, we pass the
command arguments and other parameters to _execvp_ and let it execute. If
_execvp_ fails and _errono_ is set to _EONENT_, we know that the external
program does not exist, and thus we will print out the error message and exit
the child process with status 1. If _execvp_ fails for other reasons, then the
exit status of the child process will be set to _errno_ so that we can trace
back the error.

After all tasks have been invoked, we need to use _dup2_ to recover _stdin_
from its copy. Failing to do so will cause _stdin_ point to arbitary values,
which then will cause the shell to malfunction in following iterations.

If the current command should run in background (_bg_ is true), then the
executing function will build up a new _Node_ struct that contains all relevant
information and call _addNode_ to add to the background job list. Otherwise,
it will call _waitpid_ to wait for all active tasks to finish and then print
out the exit status message. The exit status of any child process can be
extracted by applying the _WEXITSTATUS_ macro to the status.

To keep track of the exit status of each individual task, we used two arrays
to maintain the mapping. We have a _taskToPid_ array that maps task index
within a command to the pid number of the forked child process that runs the
task. This array will be filled out by the parent process after _fork_ is
invoked. We also have a _taskToExitStatus_ array that stores the exit
status of the child process with respective to the individual task. We
implemented a function called _findTaskNum_, which returns the task index of
the given pid number by iterating through the _taskToPid_ array until the pid
number matches. With the help of _findTaskNum_, we can easily fill out the
_taskToExitStatus_ array after the completion of the child processes.

# Testing:

We tested our executable with the given testing script. We then created some
complicated pipe tests to see if out program handles it properly.We also ran
all the commands in the assignment prompt manually. All outputs matched
expected values.

# Sources that help us to complete the assignment:

_memset_:
https://stackoverflow.com/questions/3389464/
initializing-a-structure-array-using-memset

_open_:
http://codewiki.wikidot.com/c:system-calls:open

_strcat_:
https://stackoverflow.com/questions/308695/
how-do-i-concatenate-const-literal-strings-in-c

_execvp_:
https://stackoverflow.com/questions/20063803/handling-errors-from-execvp

_waitpid_:
https://linux.die.net/man/2/waitpid

_malloc_:
https://www.tutorialspoint.com/c_standard_library/c_function_malloc.htm

_dup2_:
https://stackoverflow.com/questions/11042218/c-restore-stdout-to-terminal

_chdir_ :
http://man7.org/linux/man-pages/man2/chdir.2.html

_getcwd_:
https://pubs.opengroup.org/onlinepubs/9699919799/functions/getcwd.html

_UNIX redirect_:
https://www.guru99.com/linux-redirection.html

Other than that, we also looked at many piazza posts as well as the links
provided by the assignment prompt for guidance.
