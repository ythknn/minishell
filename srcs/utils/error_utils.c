/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:35:49 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/15 18:35:50 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	print_error(char *cmd, char *arg, char *msg)
{
	ft_putstr_fd("minishell: ", 2);
	if (cmd)
	{
		ft_putstr_fd(cmd, 2);
		ft_putstr_fd(": ", 2);
	}
	if (arg)
	{
		ft_putstr_fd(arg, 2);
		ft_putstr_fd(": ", 2);
	}
	ft_putstr_fd(msg, 2);
	ft_putstr_fd("\n", 2);
}

void	print_syntax_error(char *token)
{
	print_error("syntax error near unexpected token", token, "");
}

void	print_command_not_found(char *cmd)
{
	print_error(cmd, NULL, "command not found");
}

void	print_permission_denied(char *path)
{
	print_error(path, NULL, "Permission denied");
}

void	print_no_such_file(char *path)
{
	print_error(path, NULL, "No such file or directory");
}

void	print_is_directory(char *path)
{
	print_error(path, NULL, "is a directory");
}
