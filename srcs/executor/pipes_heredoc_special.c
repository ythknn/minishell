/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_heredoc_special.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 19:15:03 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/12 21:10:45 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	handle_heredoc_and_empty_cmd(t_command *current,
		int *heredoc_pipe_fd, int *pipe_fd, int *prev_pipe_read)
{
	close_heredoc_fds(heredoc_pipe_fd);
	handle_empty_command(current, pipe_fd, prev_pipe_read);
	return (0);
}

int	handle_cmd_not_found_and_heredoc(t_command *current,
		t_shell *shell, int *heredoc_pipe_fd, int *pipe_fd)
{
	close_heredoc_fds(heredoc_pipe_fd);
	return (handle_command_not_found(current, shell, pipe_fd, -1));
}

int	process_heredoc_command(t_command *cur, t_pipe_data *data)
{
	if (pipe(data->heredoc_pipe_fd) == -1)
		return (perror("pipe"), 1);
	if (handle_heredoc_for_pipe(cur, data->heredoc_pipe_fd, data->shell) == -1)
	{
		close_heredoc_fds(data->heredoc_pipe_fd);
		return (1);
	}
	return (0);
}
