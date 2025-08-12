/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 19:30:47 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/12 21:10:45 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*handle_multiple_heredocs(t_redir *heredocs, t_shell *shell)
{
	char			*final_content;
	t_heredoc_state	state;

	if (!init_heredoc_state(&state, heredocs, shell))
		return (NULL);
	if (!process_heredoc_loop(heredocs, &state) || g_signal == SIGINT)
	{
		free(state.heredoc_content);
		close(state.stdin_copy);
		return (NULL);
	}
	final_content = finalize_heredoc(state.stdin_copy, state.heredoc_content);
	return (final_content);
}

int	setup_redirections(t_redir *redirs, t_shell *shell)
{
	if (apply_heredoc_redirections(redirs, shell))
		return (1);
	if (apply_file_redirections(redirs))
		return (1);
	return (0);
}

void	restore_redirections(int stdin_copy, int stdout_copy)
{
	dup2(stdin_copy, STDIN_FILENO);
	dup2(stdout_copy, STDOUT_FILENO);
	close(stdin_copy);
	close(stdout_copy);
}
