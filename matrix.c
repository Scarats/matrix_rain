#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

# define GREEN "\x1B[32m"  // Green
# define RESET "\x1B[0m"
# define MATRIX 100

volatile sig_atomic_t stop_flag;

// Read from random, else generate an int from a memory address.
int gen_random(int size)
{
	int fd;
	int seed;
	char *addr;

	if (size == 0)
		size = MATRIX;
	addr = NULL;
	seed = 0;
	fd = open("/dev/urandom", O_RDONLY);
	if (fd == -1)
		fd = open("/dev/random", O_RDONLY);
	if (fd == -1)
	{
		addr = malloc(1);
		seed = (int)(uintptr_t)addr;
		free(addr);
	}
	else
	{
		read(fd, &seed, sizeof(seed));
		close(fd);
	}
	return ((seed % size + 1) - size);
}

// Apply random numbers within height and -height to an array of int.
void gen_matrix(int *array, int width, int height)
{
	int i;
	int prev;
	int range;

	i = 1;
	array[0] = gen_random(width);
	prev = array[0];
	while (i < width)
	{
		if ((height - prev) <= 0)
			range = prev - height;
		else
			range = height - prev;
		array[i] = gen_random(range);
		prev = array[i];
		i++;
	}
}

// Add random char if array[i] is positive, else add space.
void fill_buff(int *array, char *buff, int width, int height)
{
	int i;
	int r;
	int ch;
	const int printable_count = 126 - 33 + 1;

	i = 0;
	while (i < width)
	{
		if (array[i] > 0)
		{
			r = gen_random(height);
			if (r < 0)
				r = -r;
			if (r == 0)
				r = 1;
			ch = 33 + (r % printable_count);
			buff[i] = (char)ch;
		}
		else
			buff[i] = ' ';
		if (array[i] >= height)
			array[i] = -height;
		else
			array[i] += 1;
		i++;
	}
	buff[width] = '\0';
}

void print_matrix(int *array, char *buff, int width, int height)
{
	int i;

	i = 0;
	while (!stop_flag)
	{
		fill_buff(array, buff, width, height);
		printf(GREEN "%s\n" RESET, buff);
		if (i == 1000)
		{
			gen_matrix(array, width, height);
			i = 0;
		}
		else
			usleep(20000);
		i++;
	}
}

void handler(int sig)
{
	(void)sig;
    stop_flag = 1;
}

// Usage:
// matrix width height
int main(int argc, char **argv)
{
    struct sigaction sa;
	int width;
	int height;
	int *array;
	char *buff;

	(void)argc;

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);

	buff = NULL;
	if (argv[1])
		width = atoi(argv[1]);
	else
	{
		buff = malloc(1);
		width = (int)(uintptr_t)buff;
		width %= MATRIX;
		free(buff);
	}
	if (width < 1)
		width = MATRIX;

	array = malloc(width * sizeof(*array));
	buff = malloc(width + 1);

	if (argv[2])
		height = atoi(argv[2]);
	else
		height = MATRIX;
	if (height < 1)
		height = MATRIX;

	printf(GREEN "\n\nFollow the white rabbit.\n\n" RESET);
	printf(GREEN "\n\nKnock, knock, Neo.\n\n" RESET);
	gen_matrix(array, width, height);
	print_matrix(array, buff, width, height);

	printf(GREEN "\n\nWake Up, Neo...\n\n" RESET);

	free(array);
	free(buff);
	return (0);
}
