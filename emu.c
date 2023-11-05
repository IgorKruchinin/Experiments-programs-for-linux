#include <fcntl.h>
#define BUFSIZE 1
#define CMDMAXSIZE 256
#define STACKMAXSIZE 256
#define VARNAMESIZE 8

enum commands {PUSH = 0x01, POP=0x02, DRAW=0x03};

struct variable {
	char name[VARNAMESIZE];
	char value;
};

struct varlst {
	struct variable var;
	struct varlst *next;
};

struct varlst *Variables;

void init_variables() {
	Variables = (struct varlist*)malloc(sizeof(Variables));
	Variables->var.name = "__VERSION";
	Variables->var.value = 0x01;
}

void write_var(char const *name; char value) {
	struct varlst *current;
	bool exists = false;
	for (current = Variables.next; current->next != NULL; current = current->next) {
		bool flag = true;
		for (size_t i = 0; i < VARNAMESIZE; ++i) {
			if (current->var.name[i] != name[i])
				flag = false;
				break;
			}
		}
		if (flag) {
			exists = true;
			break;
		} 
	}
	struct varlst *vars;
	if (!exists) {
		vars = (struct varlst*)malloc(sizeof(vars));
		memset(&vars->var.name, 0, VARNAMESIZE);
		for (size_t i = 0; name[i] != '\0' && i < VARNAMESIZE; ++i) {
			vars->var.name[i] = name[i];
		} // DANGEROUS
	}
	else {
		vars = current;
	}
	vars->var.value = value;
	if (!exists) {
		current->next = vars;
	}
}

char read_var(char const *name) {
	if (name[0] == '$') {
		return name[1];
	}
	
        for (current = Variables.next; current->next != NULL; current = current->next) {
		bool flag = true;
	        for (size_t i = 0; i < VARNAMESIZE; ++i) {
                        if (current->var.name[i] != name[i])
                                flag = false;
                                break;
                        }
                }
                if (flag) {
                        break;
                }
        }
	if (!flag) {
		perror("Variable not exsits!\n");
		abort();
	}
	return current->var.value;
}


struct stack {
	size_t stacksize;
	char stack[STACKMAXSIZE];
}

struct stack Stack;

void init_stack() {
	Stack.stacksize = 0;
	memset(&Stack->stack, 0x0, STACKMAXSIZE);
};

void push(char arg) {
	Stack.stack[stacksize++] = arg;
}

char pop() {
	char arg = Stack.stack[Stack.stacksize--];
	Stack.stack[Stack.Stacksize + 1] = 0x0;
	return arg;
}

void execute(char const *text, size_t size) {
	char command = text[0];
	char returns = 0x0;
	switch (command) {
		case PUSH:
			if (size != 2) {
				perror("Invalid file");
				abort();
			}
			push(read_var(text + 1));
			break;
		case POP:
			write_var(text + 1, pop());
			break;
		}
	}
}

void read_bin(const char *path) {
	int fd = open(path, O_RDONLY);
	char buf[BUFSIZE];
	memset(&buf, 0, BUFSIZE);
	size_t size = -1;
	char text[CMDMAXSIZE];
	size_t filepos = 0;
	do {
		memset(&buf, 0, BUFSIZE);
		size = read(fd, &buf, BUFSIZE);
		for (size_t i = 0; i < BUFSIZE; ++i) {
			text[filepos++] = buf[i];
			if (buf[i] == 0x1E) {
                                execute(text); 
                                memset(&text, 0, CMDMAXSIZE); 
                                memset(&buf, 0, BUFSIZE); 
                                filepos = 0; 
                                continue; 
                        } 

		}
	while (size > 0);
}

int main(int argc, char const **argv) {
	if (argc == 2) {
		read_bin(path);
	} else {
		perror("Invalid number of commands!):
		abort();
	}
	return 0;
]
