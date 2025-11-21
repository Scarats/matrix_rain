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

volatile sig_atomic_t g_stop_flag;

// Read from random, else generate an int from a memory address.
int	gen_random(int size)
{
	int		fd;
	int		seed;
	char	*addr;

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
void	gen_matrix(int *array, int width, int height)
{
	int	i;
	int	prev;
	int	range;

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
void	fill_buff(int *array, char *buff, int width, int height)
{
	int			i;
	int			r;
	const int	printable_count = 126 - 33 + 1;

	i = -1;
	while (++i < width)
	{
		if (array[i] > 0)
		{
			r = gen_random(height);
			if (r < 0)
				r = -r;
			if (r == 0)
				r = 1;
			buff[i] = (char)(33 + (r % printable_count));
		}
		else
			buff[i] = ' ';
		if (array[i] >= height)
			array[i] = -height;
		else
			array[i] += 1;
	}
	buff[width] = '\0';
}

void	print_matrix(int *array, char *buff, int width, int height)
{
	int	i;

	i = 0;
	while (!g_stop_flag)
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

void	matrix_rest(int height, int width, int *array, char *buff)
{
	if (height == 0)
		height = MATRIX;
	printf(GREEN "\n\nFollow the white rabbit.\n\n" RESET);
	printf(GREEN "\n\nKnock, knock, Neo.\n\n" RESET);
	gen_matrix(array, width, height);
	print_matrix(array, buff, width, height);
	printf(GREEN "\n\nWake Up, Neo...\n\n" RESET);
	free(array);
	free(buff);
}

// Usage:
// matrix width height
int	main(int ac, char **argv)
{
	int		width;
	int		height;
	int		*array;
	char	*buff;

	(void)ac;
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
	if (width < 1 || width > 10000)
		width = MATRIX;
	array = malloc(width * sizeof(*array));
	buff = malloc(width + 1);
	if (argv[2] && (atoi(argv[2]) > 0 && atoi(argv[2]) < 10000))
		height = atoi(argv[2]);
	else
		height = MATRIX;
	matrix_rest(height, width, array, buff);
	return (0);
}
