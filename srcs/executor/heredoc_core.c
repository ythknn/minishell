/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_core.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 19:25:51 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/08 19:43:11 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static char	*strip_quotes(char *str)
{
	char	*result;
	int		len;

	if (!str)
		return (NULL);
	len = strlen(str);
	if (len >= 2
		&& ((str[0] == '\'' && str[len - 1] == '\'')
			|| (str[0] == '"' && str[len - 1] == '"')))
	{
		result = malloc(len - 1);
		if (!result)
			return (NULL);
		strncpy(result, str + 1, len - 2);
		result[len - 2] = '\0';
		return (result);
	}
	return (ft_strdup(str));
}

static char	*read_heredoc_line(void)
{
	char	*line;

	if (g_signal == SIGINT)
		return (NULL);
	line = readline("> ");
	if (g_signal == SIGINT || !line)
	{
		if (line)
		{
			free(line);
			line = NULL;
		}
		return (NULL);
	}
	return (line);
}

static int	append_heredoc_line(t_redir *current, char *line, char *delimiter,
			char **heredoc_content)
{
	char	*expanded_line;
	char	*temp;
	size_t	content_size;

	if (strcmp(current->file, delimiter) == 0)
	{
		expanded_line = ft_strdup(line);
		free(line);
		if (!expanded_line)
			return (1);
	}
	else
		expanded_line = line;
	content_size = strlen(*heredoc_content) + strlen(expanded_line) + 2;
	temp = malloc(content_size);
	if (!temp)
	{
		free(expanded_line);
		return (0);
	}
	sprintf(temp, "%s%s\n", *heredoc_content, expanded_line);
	free(*heredoc_content);
	*heredoc_content = temp;
	free(expanded_line);
	return (1);
}

static int	process_heredoc_line(t_redir *current, char *delimiter, int is_last_heredoc,
			char **heredoc_content)
{
	char	*line;

	line = read_heredoc_line();
	if (!line || g_signal == SIGINT)
	{
		if (line)
			free(line);
		return (0);
	}
	if (strcmp(line, delimiter) == 0)
	{
		free(line);
		return (2);
	}
	if (is_last_heredoc)
		return (append_heredoc_line(current, line, delimiter, heredoc_content));
	free(line);
	return (1);
}

static int	process_single_heredoc_simple(t_redir *current, int current_heredoc,
			int heredoc_count, char **heredoc_content)
{
	char	*delimiter;
	int		is_last_heredoc;
	int		res;

	is_last_heredoc = (current_heredoc == heredoc_count - 1);
	delimiter = strip_quotes(current->file);
	if (!delimiter)
		return (0);
	while (g_signal != SIGINT)
	{
		res = process_heredoc_line(current, delimiter, is_last_heredoc,
				heredoc_content);
		if (res == 0)
		{
			free(delimiter);
			return (0);
		}
		if (res == 2)
			break ;
	}
	free(delimiter);
	return (1);
}

int	process_heredoc_loop(t_redir *heredocs, int heredoc_count,
			char **heredoc_content)
{
	t_redir	*current;
	int		current_heredoc;

	current = heredocs;
	current_heredoc = 0;
	while (current && current_heredoc < heredoc_count && g_signal != SIGINT)
	{
		if (current->type != T_HEREDOC)
		{
			current = current->next;
			continue ;
		}
		if (!process_single_heredoc_simple(current, current_heredoc, heredoc_count,
				heredoc_content))
			return (0);
		current_heredoc++;
		current = current->next;
	}
	return (1);
}


