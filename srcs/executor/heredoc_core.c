/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_core.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 19:25:51 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/09 20:15:06 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

static int	process_heredoc_line(t_redir *current, char *delimiter,
		char **heredoc_content, t_shell *shell)
{
	char	*line;
	char	*expanded_line;

	line = read_heredoc_line();
	if (!line || g_signal == SIGINT)
	{
		if (line)
			free(line);
		return (0);
	}
	if (ft_strcmp(line, delimiter) == 0)
	{
		free(line);
		return (2);
	}
	if (heredoc_content)
	{
		expanded_line = expand_heredoc_line(current, line, delimiter, shell);
		return (append_heredoc_line(expanded_line, heredoc_content));
	}
	free(line);
	return (1);
}

static int	read_heredoc_until_delimiter(t_redir *current, char *delimiter,
		char **heredoc_content, t_shell *shell)
{
	int	res;

	while (g_signal != SIGINT)
	{
		if (heredoc_content)
			res = process_heredoc_line(current, delimiter, heredoc_content,
					shell);
		else
			res = process_heredoc_line(current, delimiter, NULL, NULL);
		if (res == 0 || res == 2)
			return (res);
	}
	return (0);
}

static int	process_single_heredoc_simple(t_redir *current, int is_last,
		char **heredoc_content, t_shell *shell)
{
	char	*delimiter;
	int		res;

	delimiter = strip_quotes(current->file);
	if (!delimiter)
		return (0);
	if (is_last)
		res = read_heredoc_until_delimiter(current, delimiter,
				heredoc_content, shell);
	else
		res = read_heredoc_until_delimiter(current, delimiter, NULL, NULL);
	free(delimiter);
	return (res != 0);
}

int	process_heredoc_loop(t_redir *heredocs, int heredoc_count,
		char **heredoc_content, t_shell *shell)
{
	t_redir	*current;
	int		current_heredoc;
	int		is_last;

	current = heredocs;
	current_heredoc = 0;
	while (current && current_heredoc < heredoc_count && g_signal != SIGINT)
	{
		if (current->type != T_HEREDOC)
		{
			current = current->next;
			continue ;
		}
		is_last = (current_heredoc == heredoc_count - 1);
		if (!process_single_heredoc_simple(current, is_last,
				heredoc_content, shell))
			return (0);
		current_heredoc++;
		current = current->next;
	}
	return (1);
}
