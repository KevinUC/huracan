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

Errors can occur during parsing and executing phase. We added all possible
error types to an enum. We also implemented a _printErrorMessage_ function
that takes in an error type and print out the corresponding error message by
using switch statements. The _printErrorMessage_ might only be invoked during
the parsing and executing phase.

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

As soon as a parsing error is found, printErrorMessage will be invoked and the shell will jump back to read the next input from user.

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
then the corresponding *Node*will be removed from the _List_.

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
task
in the command, then we need to set stdin to the read end of the previous pipe
so that the current task can read the output from the previous task as its
input. If the current task is not the last task, then we need to set up a new
pipe and point stdout to the its write end so that the output of current task
will be seen by the next task as its input. Finally, if the current task is
the last one, then we must point stdout to its original value. We were able
to do this by using _dup_ to make a copy of _stdout_ before the loop starts.

We then fork a new process and pass task arguments to _execvp_. In case of
built-in commands like _pwd_, _cd_ etc, no child process will be created and the shell will handle it by invoking self-defined functions such as _myPwd_.
We then use _dup2_ to recover stdin from its copy after all tasks have been
invoked.

If the current command should run in background (_bg_ is true), then the
executing function will build up a new Node struct that contains all relevant
information and call _addNode_ to add to the background job list. Otherwise,
it will call _waitpid_ to wait for all active tasks to finish and then print
out the exit status message.

# Testing:

We tested our executable with the given testing script. We then created some
complicated pipe tests to see if out program handles it properly.We also ran
all the commands in the assignment prompt manually. All outputs matched
expected values.

# Sources that help us to complete the assignment:

_memset()_: https://stackoverflow.com/questions/3389464/
initializing-a-structure-array-using-memset

_open()_: http://codewiki.wikidot.com/c:system-calls:open

_strcat()_: https://stackoverflow.com/questions/308695/
how-do-i-concatenate-const-literal-strings-in-c

_execvp()_: https://stackoverflow.com/questions/20063803/
handling-errors-from-execvp

_waitpid()_: https://linux.die.net/man/2/waitpid
  
_waitpid()_:https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.1.0/
com.ibm.zos.v2r1.bpxbd00/rtwaip.htm

Other than that, we also looked at many piazza posts as well as the links
provided by the assignment prompt for guidance.
