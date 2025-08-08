/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:23:59 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/08 19:15:03 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

void	cleanup_after_fork(t_command *cur, t_pipe_data *data, char *path)
{
	if (data->has_heredoc_flag)
		close_heredoc_fds(data->heredoc_pipe_fd);
	if (data->prev_pipe_read != -1)
		close(data->prev_pipe_read);
	if (cur->next)
	{
		close(data->pipe_fd[1]);
		data->prev_pipe_read = data->pipe_fd[0];
	}
	if (path)
		free(path);
}

void	handle_child_process(t_command *cur, t_shell *shell,
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

	cur = cmds;
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
		if (process_single_command(cur, shell, &data))
			cur = cur->next;
		else
			break ;
	}
	wait_for_children(&data.last_status, &data.last_cmd_not_found, shell);
	return (data.last_status);
}
