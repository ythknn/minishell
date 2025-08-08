/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 00:00:00 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/07 00:00:00 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	has_heredoc(t_command *cmd)
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

int	handle_heredoc_for_pipe(t_command *cmd, int *pipe_fd)
{
	char	*heredoc_content;

	heredoc_content = handle_multiple_heredocs(cmd->redirections);
	if (!heredoc_content)
		return (-1);
	write(pipe_fd[1], heredoc_content, ft_strlen(heredoc_content));
	close(pipe_fd[1]);
	free(heredoc_content);
	return (0);
}

void	setup_child_pipes(int prev_pipe_read,
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

void	setup_child_pipes_with_heredoc(int prev_pipe_read,
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

void	close_heredoc_fds(int *heredoc_pipe_fd)
{
	close(heredoc_pipe_fd[0]);
	if (heredoc_pipe_fd[1] != -1)
		close(heredoc_pipe_fd[1]);
}
