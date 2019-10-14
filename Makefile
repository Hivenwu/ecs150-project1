sshell: sshell.c modsys.c background.c
	gcc -Wall -Werror -o sshell sshell.c modsys.c background.c -I.

external_syscall: external_syscall.c
	gcc -Wall -Werror -o external_syscall external_syscall.c -I.
