/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_io.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 19:26:36 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/08 20:34:12 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	handle_input_redirection(t_redir *current)
{
	int	fd;

	fd = open(current->file, O_RDONLY);
	if (fd == -1)
	{
		print_error("minishell", current->file, strerror(errno));
		return (1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}

static int	handle_output_redirection(t_redir *current)
{
	int	fd;

	fd = open(current->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		print_error("minishell", current->file, strerror(errno));
		return (1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

static int	handle_append_redirection(t_redir *current)
{
	int	fd;

	fd = open(current->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		print_error("minishell", current->file, strerror(errno));
		return (1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

int	apply_file_redirections(t_redir *redirs)
{
	t_redir	*current;

	current = redirs;
	while (current)
	{
		if (current->type == T_REDIR_IN)
		{
			if (handle_input_redirection(current))
				return (1);
		}
		else if (current->type == T_REDIR_OUT)
		{
			if (handle_output_redirection(current))
				return (1);
		}
		else if (current->type == T_REDIR_APPEND)
		{
			if (handle_append_redirection(current))
				return (1);
		}
		current = current->next;
	}
	return (0);
}
