/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_command_processing.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 19:15:03 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/12 20:10:11 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	handle_empty_command_case(t_command *cur, t_pipe_data *data)
{
	if (data->has_heredoc_flag)
		handle_heredoc_and_empty_cmd(cur, data->heredoc_pipe_fd,
			data->pipe_fd, &data->prev_pipe_read);
	else
		handle_empty_command(cur, data->pipe_fd, &data->prev_pipe_read);
	return (1);
}

static int	handle_builtin_single_case(t_command *cur, t_shell *shell,
		char *path, t_pipe_data *data)
{
	if (is_builtin(cur->args[0]) && !cur->next
		&& data->prev_pipe_read == -1 && !data->has_heredoc_flag)
	{
		data->last_status = handle_builtin_single_command(cur, shell, path);
		data->last_cmd_not_found = 0;
		return (1);
	}
	return (0);
}

static int	handle_command_not_found_case(t_command *cur, t_shell *shell,
		char *path, t_pipe_data *data)
{
	if (!path && !is_builtin(cur->args[0]))
	{
		if (data->has_heredoc_flag)
		{
			data->last_status = handle_cmd_not_found_and_heredoc(cur,
					shell, data->heredoc_pipe_fd, data->pipe_fd);
			data->last_cmd_not_found = 1;
		}
		else
		{
			data->last_status = handle_command_not_found(cur, shell,
					data->pipe_fd, data->prev_pipe_read);
			if (cur->next)
				data->last_cmd_not_found = 0;
			else
				data->last_cmd_not_found = 1;
		}
		return (1);
	}
	return (0);
}

int	process_single_command(t_command *cur, t_shell *shell,
		t_pipe_data *data)
{
	char	*path;
	pid_t	pid;

	path = NULL;
	if (!cur->args || !cur->args[0])
		return (handle_empty_command_case(cur, data));
	if (handle_builtin_single_case(cur, shell, path, data))
		return (1);
	if (!is_builtin(cur->args[0]))
		path = find_executable(cur->args[0], shell);
	if (handle_command_not_found_case(cur, shell, path, data))
		return (1);
	data->last_cmd_not_found = 0;
	pid = fork();
	if (pid == 0)
		handle_child_process(cur, shell, path, data);
	data->pids[data->pid_count] = pid;
	data->pid_count++;
	cleanup_after_fork(cur, data, path);
	return (1);
}
