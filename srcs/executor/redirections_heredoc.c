/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_heredoc.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 19:27:01 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/08 19:27:01 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	handle_heredoc_redirection(t_redir *redirs, int *heredoc_processed)
{
	char	*heredoc_content;
	int		pipe_fd[2];

	if (g_signal == SIGINT)
	{
		g_signal = 0;
		return (1);
	}
	heredoc_content = handle_multiple_heredocs(redirs);
	if (!heredoc_content)
	{
		if (g_signal == SIGINT)
			g_signal = 0;
		return (1);
	}
	pipe(pipe_fd);
	write(pipe_fd[1], heredoc_content, strlen(heredoc_content));
	close(pipe_fd[1]);
	dup2(pipe_fd[0], STDIN_FILENO);
	close(pipe_fd[0]);
	free(heredoc_content);
	*heredoc_processed = 1;
	return (0);
}

int			apply_heredoc_redirections(t_redir *redirs)
{
	t_redir	*current;
	int		heredoc_processed;

	heredoc_processed = 0;
	current = redirs;
	while (current)
	{
		if (current->type == T_HEREDOC && !heredoc_processed)
		{
			if (handle_heredoc_redirection(redirs, &heredoc_processed))
				return (1);
		}
		current = current->next;
	}
	return (0);
}


