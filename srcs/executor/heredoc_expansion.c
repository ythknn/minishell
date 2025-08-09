/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expansion.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 20:15:14 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/09 20:19:37 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	has_quotes(char *str)
{
	int	len;

	if (!str)
		return (0);
	len = ft_strlen(str);
	if (len >= 2 && ((str[0] == '\'' && str[len - 1] == '\'')
			|| (str[0] == '"' && str[len - 1] == '"')))
		return (1);
	return (0);
}

char	*expand_heredoc_line(t_redir *current, char *line,
		char *delimiter, t_shell *shell)
{
	char	*expanded_line;

	(void)delimiter;
	if (has_quotes(current->file))
	{
		expanded_line = ft_strdup(line);
		free(line);
		return (expanded_line);
	}
	else
	{
		expanded_line = expand_env_vars(line, shell);
		free(line);
		return (expanded_line);
	}
}

int	append_heredoc_line(char *expanded_line, char **heredoc_content)
{
	char	*temp;
	size_t	content_size;

	if (!expanded_line)
		return (0);
	content_size = ft_strlen(*heredoc_content) + ft_strlen(expanded_line) + 2;
	temp = malloc(content_size);
	if (!temp)
	{
		free(expanded_line);
		return (0);
	}
	ft_strlcpy(temp, *heredoc_content, content_size);
	ft_strlcat(temp, expanded_line, content_size);
	ft_strlcat(temp, "\n", content_size);
	free(*heredoc_content);
	*heredoc_content = temp;
	free(expanded_line);
	return (1);
}
