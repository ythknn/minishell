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

static int	has_heredoc(t_command *cmd)
{
	t_redir	*current;

	if (!cmd || !cmd->redirections)
		return (0);
	current = cmd->redirections;
	while (current)
	{
		if (current->type == T_HEREDOC)
			return (1);
		current = current->next;
	}
	return (0);
}

static int	handle_heredoc_for_pipe(t_command *cmd, int *pipe_fd)
{
	char	*heredoc_content;

	heredoc_content = handle_multiple_heredocs(cmd->redirections);
	if (!heredoc_content)
		return (-1);
	write(pipe_fd[1], heredoc_content, strlen(heredoc_content));
	close(pipe_fd[1]);
	free(heredoc_content);
	return (0);
}

static void	setup_child_pipes(int prev_pipe_read,
		int *pipe_fd, t_command *current)
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

static void	setup_child_pipes_with_heredoc(int prev_pipe_read,
		int *pipe_fd, t_command *current, int *heredoc_pipe_fd)
{
	if (heredoc_pipe_fd)
	{
		dup2(heredoc_pipe_fd[0], STDIN_FILENO);
		close(heredoc_pipe_fd[0]);
		if (heredoc_pipe_fd[1] != -1)
			close(heredoc_pipe_fd[1]);
	}
	else if (prev_pipe_read != -1)
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

static void	execute_child_process(t_command *current,
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

static void	execute_child_process_with_heredoc(t_command *current,
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

static int	handle_empty_command(t_command *current,
		int *pipe_fd, int *prev_pipe_read)
{
	if (current->next)
	{
		close(pipe_fd[1]);
		*prev_pipe_read = pipe_fd[0];
	}
	return (0);
}

static int	handle_builtin_single_command(t_command *current,
		t_shell *shell, char *path)
{
	int	last_status;

	if (path)
		free(path);
	last_status = execute_builtin(current->args, shell);
	shell->exit_status = last_status;
	return (last_status);
}

static int	handle_command_not_found(t_command *current,
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

static void	close_heredoc_fds(int *heredoc_pipe_fd)
{
	close(heredoc_pipe_fd[0]);
	if (heredoc_pipe_fd[1] != -1)
		close(heredoc_pipe_fd[1]);
}

static int	handle_heredoc_and_empty_cmd(t_command *current,
		int *heredoc_pipe_fd, int *pipe_fd, int *prev_pipe_read)
{
	close_heredoc_fds(heredoc_pipe_fd);
	handle_empty_command(current, pipe_fd, prev_pipe_read);
	return (0);
}

static int	handle_cmd_not_found_and_heredoc(t_command *current,
		t_shell *shell, int *heredoc_pipe_fd, int *pipe_fd)
{
	close_heredoc_fds(heredoc_pipe_fd);
	return (handle_command_not_found(current, shell, pipe_fd, -1));
}

static void	wait_for_children(int *last_status, int *last_cmd_not_found,
		t_shell *shell)
{
	int	status;

	while (wait(&status) > 0)
	{
		if (WIFEXITED(status) && !(*last_cmd_not_found))
		{
			*last_status = WEXITSTATUS(status);
			shell->exit_status = *last_status;
		}
	}
}

static int	process_heredoc_command(t_command *cur, int *heredoc_pipe_fd,
		int *pipe_fd, int *prev_pipe_read)
{
	(void)pipe_fd;
	(void)prev_pipe_read;
	if (pipe(heredoc_pipe_fd) == -1)
		return (perror("pipe"), 1);
	if (handle_heredoc_for_pipe(cur, heredoc_pipe_fd) == -1)
	{
		close_heredoc_fds(heredoc_pipe_fd);
		return (1);
	}
	return (0);
}

static void	handle_child_process(t_command *cur, t_shell *shell,
		char *path, t_pipe_data *data)
{
	if (data->has_heredoc_flag)
		setup_child_pipes_with_heredoc(data->prev_pipe_read,
			data->pipe_fd, cur, data->heredoc_pipe_fd);
	else
		setup_child_pipes(data->prev_pipe_read, data->pipe_fd, cur);
	if (data->has_heredoc_flag)
		execute_child_process_with_heredoc(cur, shell, path);
	else
		execute_child_process(cur, shell, path);
}

int	execute_pipeline(t_command *cmds, t_shell *shell)
{
	t_command		*cur;
	t_pipe_data		data;
	pid_t			pid;
	char			*path;

	cur = cmds;
	path = NULL;
	data.prev_pipe_read = -1;
	data.last_status = 0;
	data.last_cmd_not_found = 0;
	while (cur)
	{
		data.has_heredoc_flag = has_heredoc(cur);
		if (cur->next)
			pipe(data.pipe_fd);
		if (data.has_heredoc_flag)
		{
			if (process_heredoc_command(cur, data.heredoc_pipe_fd,
					data.pipe_fd, &data.prev_pipe_read))
				return (1);
		}
		if (!cur->args || !cur->args[0])
		{
			if (data.has_heredoc_flag)
				handle_heredoc_and_empty_cmd(cur, data.heredoc_pipe_fd,
					data.pipe_fd, &data.prev_pipe_read);
			else
				handle_empty_command(cur, data.pipe_fd,
					&data.prev_pipe_read);
			cur = cur->next;
			continue ;
		}
		if (is_builtin(cur->args[0]) && !cur->next
			&& !data.prev_pipe_read && !data.has_heredoc_flag)
		{
			data.last_status = handle_builtin_single_command(cur,
					shell, path);
			data.last_cmd_not_found = 0;
			cur = cur->next;
			continue ;
		}
		if (!is_builtin(cur->args[0]))
			path = find_executable(cur->args[0], shell);
		if (!path && !is_builtin(cur->args[0]))
		{
			if (data.has_heredoc_flag)
			{
				data.last_status = handle_cmd_not_found_and_heredoc(cur,
						shell, data.heredoc_pipe_fd, data.pipe_fd);
				data.last_cmd_not_found = 1;
			}
			else
			{
				data.last_status = handle_command_not_found(cur, shell,
						data.pipe_fd, data.prev_pipe_read);
				data.last_cmd_not_found = cur->next ? 0 : 1;
			}
			cur = cur->next;
			continue ;
		}
		data.last_cmd_not_found = 0;
		pid = fork();
		if (pid == 0)
			handle_child_process(cur, shell, path, &data);
		if (data.has_heredoc_flag)
			close_heredoc_fds(data.heredoc_pipe_fd);
		if (data.prev_pipe_read != -1)
			close(data.prev_pipe_read);
		if (cur->next)
		{
			close(data.pipe_fd[1]);
			data.prev_pipe_read = data.pipe_fd[0];
		}
		if (path)
			free(path);
		cur = cur->next;
	}
	wait_for_children(&data.last_status, &data.last_cmd_not_found, shell);
	return (data.last_status);
}
