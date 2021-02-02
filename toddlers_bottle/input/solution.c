#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// 0x2020 = 8224
#define PORT_X "8224"

void stage1(char* argv[101]);
void stage2(void);
void stage3(void);
void stage4(void);
void stage5(void);

int main(void) {

	char * my_argv[101] = { 0 };
	extern char** environ;

	stage1(my_argv);
	stage2();
	stage3();
	stage4();

	// Execute the process.
	execve("/home/input2/input", my_argv, environ);

	return 0;
}

void stage1(char* argv[101]) {

	// Fill the custom argv with junk strings.
	for (unsigned int i=0; i < 101; i++) {
		argv[i] = "A";
	}

	// Construct a custom argv to meet the requirements.
	argv[65] = "\x00";
	argv[66] = "\x20\x0a\x0d";
	argv[67] = PORT_X;
	argv[100] = NULL;
}

void stage2(void) {

	pid_t child;
	int stdin_fds[2];
	int stderr_fds[2];

	// Create the pipes.
	if (pipe(stdin_fds) + pipe(stderr_fds)) {
		printf("pipe failed\n");
		exit(1);
	}

	// Fork the current process.
	child = fork();
	if(child < 0) {
		printf("fork failed\n");
		exit(1);
	}

	// Child process
	if (!child) {
		// Close reading side of pipes.
		close(stdin_fds[0]);
		close(stderr_fds[0]);

		// Write to stdin.
		write(stdin_fds[1], "\x00\x0a\x00\xff", 4);

		// Write to stderr.
		write(stderr_fds[1], "\x00\x0a\x02\xff", 4);

		// Wait for the server to start and execute stage 5.
		sleep(2);
		stage5();

		// End child process.
		exit(0);
	}
	// Parent process.
	else {
		// Close writing side of pipes.
		close(stdin_fds[1]);
		close(stderr_fds[1]);

		// Connect stdin to the stdin pipe.
		dup2(stdin_fds[0], 0);

		// Connect stderr to the stderr pipe.
		dup2(stderr_fds[0], 2);

		// Close pipes.
		close(stdin_fds[0]);
		close(stderr_fds[0]);
	}
}

void stage3(void) {

	// Set an environment variable.
	setenv("\xde\xad\xbe\xef", "\xca\xfe\xba\xbe", 1);
}

void stage4(void) {

	// Open/create a file.
	FILE* file = fopen("\x0a", "w");
	if (!file) {
		printf("file creation failed\n");
		exit(1);
	}

	// Write to the file and then close it.
	fwrite("\x00\x00\x00\x00", 4, 1, file);
	fclose(file);
}

void stage5(void) {

	// Initialize the socket.
	int sock;
	struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_addr.sin_port = htons(atoi(PORT_X));
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket failed\n");
		exit(1);
	}

	// Connect to Port X.
	if (connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr))) {
		printf("connect failed\n");
		exit(1);
	}

	// Send data through the socket, then close it.
	write(sock, "\xde\xad\xbe\xef", 4);
	close(sock);
}