#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define BUFSIZE 1
#define CMDMAXSIZE 256
#define STACKMAXSIZE 256
#define VARNAMESIZE 12
#define REGNAMESIZE 2
#define ENDOFCOMMAND 0x1E

#define CREATEIFNOTEXISTS 1
#define ERRORIFNOTEXISTS 0

enum commands {PUSH = 0x01, POP=0x02, MOV=0x03, DRAW=0x04, BYTE=0x05, VARDMP=0x06};

struct reg {
	char name[REGNAMESIZE];
	char value;
};

struct registers {
	struct reg bs;
	struct reg sp;
	struct reg ga;
	struct reg gb;
	struct reg gc;
	struct reg gd;
	struct reg ge;
	struct reg gf;
	struct reg gg;
	struct reg gh;
	struct reg gi;
	struct reg ss;
};

struct registers Registers;

void init_registers() {
	strcpy(Registers.bs.name, "bs");
	Registers.bs.value = 0x0;
	strcpy(Registers.sp.name, "sp");
        Registers.sp.value = 0x0;
	strcpy(Registers.ga.name, "ga");
        Registers.ga.value = 0x0;
	strcpy(Registers.gb.name, "gb");
        Registers.gb.value = 0x0;
	strcpy(Registers.gc.name, "gc");
        Registers.gc.value = 0x0;
	strcpy(Registers.gd.name, "gd");
        Registers.gd.value = 0x0;
	strcpy(Registers.ge.name, "ge");
        Registers.ge.value = 0x0;
	strcpy(Registers.gf.name, "gf");
        Registers.gf.value = 0x0;
	strcpy(Registers.gg.name, "gg");
        Registers.gg.value = 0x0;
	strcpy(Registers.gh.name, "gh");
        Registers.gh.value = 0x0;
	strcpy(Registers.gi.name, "gi");
        Registers.gi.value = 0x0;
	strcpy(Registers.ss.name, "ss");
        Registers.ss.value = 0x0;
}

struct reg *get_register(char name[2]) {
	struct reg *current = &Registers;
 	for (; current->name[0] != name[0] || current->name[1]; current++) {}
	return current;
}

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
	strcpy(Variables->var.name, "__VERSION");
	Variables->var.value = '1';
}

void write_var(char const *name, char value, unsigned int flg) {
	//puts("write_var function called\n");
	struct varlst *current;
	bool exists = false;
	for (current = Variables; current->next != NULL; current = current->next) {
		//printf("%s - %s\n", current->var.name, name);
		if (strcmp(current->var.name, name) == 0) {
			exists = true;
			break;
		} 
	}
	//for last element
	if (strcmp(current->var.name, name) == 0) {
                exists = true;
        }
	//printf("%p", current);
	struct varlst *vars;
	if (!exists) {
		if (flg == CREATEIFNOTEXISTS) {
			vars = (struct varlst*)malloc(sizeof(vars));
			//memset(&vars->var.name, 0, VARNAMESIZE);
			//for (size_t i = 0; name[i] != '\0' && i < VARNAMESIZE; ++i) {
			//	vars->var.name[i] = name[i];
			//} // DANGEROUS
			//puts("CRIFNEX");
			strcpy(vars->var.name, name);
		} else {
			perror("Variable not exists!\n");
		}
	} else {
		vars = current;
	}
	vars->var.value = value;
	if (!exists) {
		current->next = vars;
	}
}

char read_var(char const *name) {
	if (name[0] == '$') {
		return *(name + 1);
	}
	struct varlst *current;
	bool flag = false;
        for (current = Variables; current->next != NULL; current = current->next) {
                if (strcmp(current->var.name, name) == 0) {
			flag = true;
			break;
                }
        }
	// for last element
	if (strcmp(current->var.name, name) == 0) {
        	flag = true;
        }
	if (!flag) {
		perror("Variable not exsits!\n");
		abort();
	}
	return current->var.value;
}

void vardump() {
	struct varlst *current;
        for (current = Variables; current->next != NULL; current = current->next) {
                printf("%s) %c\n", current->var.name, current->var.value);
        }
	// for last element
        printf("%s) %c\n", current->var.name, current->var.value);

}

struct stack {
	size_t stacksize;
	char stack[STACKMAXSIZE];
};

struct stack Stack;

void init_stack() {
	Stack.stacksize = 0;
	memset(Stack.stack, 0x0, STACKMAXSIZE);
}

void push(char arg) {
	Stack.stack[Stack.stacksize++] = arg;
	//printf("%c\n", arg);
}

char pop() {
	if (Stack.stacksize <= 0 ) {
		perror("Attempt for pop from empty stack!\n");
	}
	char arg = Stack.stack[--Stack.stacksize];
	Stack.stack[Stack.stacksize + 1] = 0x0;
	//printf("%c\n", arg);
	return arg;
}

void execute(char const *text, size_t size) {
	char command = text[0];
	char returns = 0x0;
	//puts("execute function called\n");
	switch (command) {
		case PUSH:
			printf("push %s\n", text + 1);
			//printf("%d\n", size);
			//if (size != 2) {
			//	perror("Invalid file");
			//	abort();
			//}
			push(read_var(text + 1));
			break;
		case POP:
			printf("pop %s\n", text + 1);
			write_var(text + 1, pop(), ERRORIFNOTEXISTS);
			break;
		case BYTE:
			printf("byte %s\n", text + 1);
			write_var(text + 1, 0, CREATEIFNOTEXISTS);
			break;
		case VARDMP:
			printf("vardmp\n");
			vardump();
			break;
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
			if (buf[i] == ENDOFCOMMAND) {
                                execute(text, filepos-1); 
                                memset(&text, 0, CMDMAXSIZE); 
                                memset(&buf, 0, BUFSIZE); 
                                filepos = 0; 
                                continue; 
                        } 

		}
	} while (size > 0);
}

int main(int argc, char const **argv) {
	if (argc == 2) {
		init_stack();
		init_registers();
		init_variables();
		read_bin(argv[1]);
	} else {
		perror("Invalid number of arguments!");
		abort();
	}
	return 0;
}
