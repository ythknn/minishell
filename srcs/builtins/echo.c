#include "../includes/minishell.h"

static int is_n_flag(char *arg)
{
	int i;

	if (!arg || arg[0] != '-')
		return (0);
	i = 1;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (i > 1); // en az bir 'n' karakteri içermeli
}

int ft_echo(char **args)
{
	int i;
	int n_flag;

	n_flag = 0;
	i = 1;
	// Tüm -n flaglerini kontrol et
	while (args[i] && is_n_flag(args[i]))
	{
		n_flag = 1;
		i++;
	}

	// Argümanları yazdır
	while (args[i])
	{
		write(STDOUT_FILENO, args[i], strlen(args[i]));
		if (args[i + 1])
			write(STDOUT_FILENO, " ", 1);
		i++;
	}

	if (!n_flag)
		write(STDOUT_FILENO, "\n", 1);

	return (0);
}
