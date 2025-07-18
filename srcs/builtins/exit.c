/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:14:19 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/18 18:50:52 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	is_valid_number(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	handle_exit_error(char **args, t_shell *shell, int error_type)
{
	if (error_type == 1)
	{
		print_error("exit", args[1], "numeric argument required");
		shell->exit_status = 255;
		if (!shell->interactive)
		{
			free_shell(shell);
			exit(255);
		}
		return (2);
	}
	print_error("exit", NULL, "too many arguments");
	shell->exit_status = ERROR;
	if (!shell->interactive)
	{
		free_shell(shell);
		exit(ERROR);
	}
	return (ERROR);
}

int	ft_exit(char **args, t_shell *shell)
{
	int	status;

	printf("exit\n");
	if (!args[1])
	{
		free_shell(shell);
		exit(shell->exit_status);
	}
	if (!is_valid_number(args[1]))
		return (handle_exit_error(args, shell, 1));
	status = ft_atoi(args[1]);
	if (args[2])
		return (handle_exit_error(args, shell, 2));
	free_shell(shell);
	exit(status);
}
