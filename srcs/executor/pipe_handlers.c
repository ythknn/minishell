/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_handlers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 00:00:00 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/12 20:17:00 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	execute_child_process(t_command *current,
		t_shell *shell, char *path)
{
	int	status;

	if (setup_redirections(current->redirections) != 0)
	{
		free_shell(shell);
		exit(1);
	}
	if (is_builtin(current->args[0]))
	{
		status = execute_builtin(current->args, shell);
		free_shell(shell);
		exit(status);
	}
	execve(path, current->args, shell->env_array);
	print_error(current->args[0], NULL, strerror(errno));
	free_shell(shell);
	exit(1);
}

void	execute_child_process_with_heredoc(t_command *current,
		t_shell *shell, char *path)
{
	int	status;

	if (is_builtin(current->args[0]))
	{
		status = execute_builtin(current->args, shell);
		free_shell(shell);
		exit(status);
	}
	execve(path, current->args, shell->env_array);
	print_error(current->args[0], NULL, strerror(errno));
	free_shell(shell);
	exit(1);
}

int	handle_empty_command(t_command *current,
		int *pipe_fd, int *prev_pipe_read)
{
	if (current->next)
	{
		close(pipe_fd[1]);
		*prev_pipe_read = pipe_fd[0];
	}
	return (0);
}

int	handle_builtin_single_command(t_command *current,
		t_shell *shell, char *path)
{
	int	last_status;

	if (path)
		free(path);
	last_status = execute_builtin(current->args, shell);
	shell->exit_status = last_status;
	return (last_status);
}

int	handle_command_not_found(t_command *current,
		t_shell *shell, int *pipe_fd, int prev_pipe_read)
{
	int	last_status;

	print_command_not_found(current->args[0]);
	last_status = 127;
	shell->exit_status = last_status;
	if (current->next)
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
	}
	if (prev_pipe_read != -1)
		close(prev_pipe_read);
	return (last_status);
}
