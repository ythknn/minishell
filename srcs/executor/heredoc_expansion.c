/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expansion.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 00:00:00 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/12 21:10:45 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*get_expanded_line(t_redir *current, char *line, t_shell *shell)
{
	char	*expanded_line;

	if (should_expand_heredoc(current->file))
	{
		expanded_line = expand_env_vars(line, shell);
		free(line);
		return (expanded_line);
	}
	return (line);
}

int	append_content_to_heredoc(char *expanded_line, t_heredoc_state *state)
{
	char	*temp;
	size_t	content_size;

	content_size = ft_strlen(state->heredoc_content)
		+ ft_strlen(expanded_line) + 2;
	temp = malloc(content_size);
	if (!temp)
		return (0);
	ft_strlcpy(temp, state->heredoc_content, content_size);
	ft_strlcat(temp, expanded_line, content_size);
	ft_strlcat(temp, "\n", content_size);
	free(state->heredoc_content);
	state->heredoc_content = temp;
	return (1);
}

int	append_heredoc_line(t_redir *current, char *line, char *delimiter,
		t_heredoc_state *state)
{
	char	*expanded_line;

	(void)delimiter;
	expanded_line = get_expanded_line(current, line, state->shell);
	if (!expanded_line)
		return (0);
	if (!append_content_to_heredoc(expanded_line, state))
	{
		if (should_expand_heredoc(current->file))
			free(expanded_line);
		return (0);
	}
	if (should_expand_heredoc(current->file))
		free(expanded_line);
	return (1);
}
