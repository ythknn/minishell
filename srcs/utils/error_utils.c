/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:35:49 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/21 16:44:58 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
	print_error(path, NULL, "Is a directory");
}
