/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_setup.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 19:26:12 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/08 20:35:51 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	count_heredocs(t_redir *heredocs)
{
	t_redir	*current;
	int		heredoc_count;

	heredoc_count = 0;
	current = heredocs;
	while (current)
	{
		if (current->type == T_HEREDOC)
			heredoc_count++;
		current = current->next;
	}
	return (heredoc_count);
}

int	init_heredoc_state(t_heredoc_state *state, t_redir *heredocs)
{
	state->stdin_copy = dup(STDIN_FILENO);
	state->heredoc_count = count_heredocs(heredocs);
	if (state->heredoc_count == 0)
	{
		close(state->stdin_copy);
		return (0);
	}
	state->heredoc_content = ft_strdup("");
	if (!state->heredoc_content)
	{
		close(state->stdin_copy);
		return (0);
	}
	if (tcgetattr(STDIN_FILENO, &state->term) == 0)
	{
		state->term.c_lflag |= ECHO;
		tcsetattr(STDIN_FILENO, TCSANOW, &state->term);
	}
	setup_heredoc_signals();
	g_signal = 0;
	return (1);
}

char	*finalize_heredoc(int stdin_copy, char *heredoc_content)
{
	char	*final_content;

	dup2(stdin_copy, STDIN_FILENO);
	close(stdin_copy);
	rl_replace_line("", 0);
	rl_point = 0;
	rl_end = 0;
	rl_done = 0;
	setup_signals();
	final_content = heredoc_content;
	return (final_content);
}
