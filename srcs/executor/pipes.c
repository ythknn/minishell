/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:23:59 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/04 17:36:23 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void setup_child_pipes(int prev_pipe_read, int *pipe_fd, t_command *current)
{
	if (prev_pipe_read != -1)
	{
		dup2(prev_pipe_read, STDIN_FILENO);
		close(prev_pipe_read);
	}
	if (current->next)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
	}
}

static void execute_child_process(t_command *current, t_shell *shell, char *path)
{
	if (setup_redirections(current->redirections) != 0)
	{
		free_shell(shell);
		exit(1);
	}
	if (is_builtin(current->args[0]))
	{
		int status = execute_builtin(current->args, shell);
		free_shell(shell);
		exit(status);
	}
	execve(path, current->args, shell->env_array);
	print_error(current->args[0], NULL, strerror(errno));
	free_shell(shell);
	exit(1);
}

static int handle_empty_command(t_command *current, int *pipe_fd, int *prev_pipe_read)
{
	if (current->next)
	{
		close(pipe_fd[1]);
		*prev_pipe_read = pipe_fd[0];
	}
	return (0);
}

static int handle_builtin_single_command(t_command *current, t_shell *shell, char *path)
{
	if (path)
		free(path);
	int last_status = execute_builtin(current->args, shell);
	shell->exit_status = last_status;
	return (last_status);
}

static int handle_command_not_found(t_command *current, t_shell *shell, int *pipe_fd, int prev_pipe_read)
{
	print_command_not_found(current->args[0]);
	int last_status = 127;
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

int	execute_pipeline(t_command *cmds, t_shell *shell)
{
	t_command	*current;
	int			pipe_fd[2];
	int			prev_pipe_read;
	pid_t		pid;
	int			status;
	int			last_status;
	char		*path;
	int			last_cmd_not_found;

	current = cmds;
	path = NULL;
	prev_pipe_read = -1;
	last_status = 0;
	last_cmd_not_found = 0;
	while (current)
	{
		if (current->next)
			pipe(pipe_fd);
		if (!current->args || !current->args[0])
		{
			handle_empty_command(current, pipe_fd, &prev_pipe_read);
			current = current->next;
			continue ;
		}
		if (is_builtin(current->args[0]) && !current->next && !prev_pipe_read)
		{
			last_status = handle_builtin_single_command(current, shell, path);
			last_cmd_not_found = 0;
			current = current->next;
			continue ;
		}
		if (!is_builtin(current->args[0]))
			path = find_executable(current->args[0], shell);
		if (!path && !is_builtin(current->args[0]))
		{
			last_status = handle_command_not_found(current, shell, pipe_fd, prev_pipe_read);
			if (!current->next)
				last_cmd_not_found = 1;
			else
				last_cmd_not_found = 0;
			current = current->next;
			continue ;
		}
		last_cmd_not_found = 0;
		pid = fork();
		if (pid == 0)
		{
			setup_child_pipes(prev_pipe_read, pipe_fd, current);
			execute_child_process(current, shell, path);
		}
		if (prev_pipe_read != -1)
			close(prev_pipe_read);
		if (current->next)
		{
			close(pipe_fd[1]);
			prev_pipe_read = pipe_fd[0];
		}
		if (path)
			free(path);
		current = current->next;
	}
	while (wait(&status) > 0)
	{
		if (WIFEXITED(status))
		{
			if (!last_cmd_not_found)
			{
				last_status = WEXITSTATUS(status);
				shell->exit_status = last_status;
			}
		}
	}	
	return (last_status);
}
