/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_core.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 19:25:51 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/12 21:17:12 by yihakan          ###   ########.fr       */
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

static int	handle_line_result(t_line_context *ctx, t_heredoc_state *state)
{
	if (ft_strcmp(ctx->line, ctx->delimiter) == 0)
	{
		free(ctx->line);
		return (2);
	}
	if (ctx->is_last_heredoc)
		return (append_heredoc_line(ctx->current, ctx->line, ctx->delimiter,
				state));
	free(ctx->line);
	return (1);
}

static int	process_heredoc_line(t_redir *current, char *delimiter,
		int is_last_heredoc, t_heredoc_state *state)
{
	t_line_context	ctx;

	ctx.line = read_heredoc_line();
	if (g_signal == SIGINT)
	{
		if (ctx.line)
			free(ctx.line);
		return (0);
	}
	if (!ctx.line)
		return (3);
	ctx.current = current;
	ctx.delimiter = delimiter;
	ctx.is_last_heredoc = is_last_heredoc;
	return (handle_line_result(&ctx, state));
}

static int	process_single_heredoc_simple(t_redir *current, int current_heredoc,
			t_heredoc_state *state)
{
	char	*delimiter;
	int		is_last_heredoc;
	int		res;

	is_last_heredoc = (current_heredoc == state->heredoc_count - 1);
	delimiter = strip_quotes(current->file);
	if (!delimiter)
		return (0);
	while (g_signal != SIGINT)
	{
		res = process_heredoc_line(current, delimiter, is_last_heredoc, state);
		if (res == 0)
		{
			free(delimiter);
			return (0);
		}
		if (res == 2 || res == 3)
			break ;
	}
	free(delimiter);
	return (1);
}

int	process_heredoc_loop(t_redir *heredocs, t_heredoc_state *state)
{
	t_redir	*current;
	int		current_heredoc;

	current = heredocs;
	current_heredoc = 0;
	while (current && current_heredoc < state->heredoc_count
		&& g_signal != SIGINT)
	{
		if (current->type != T_HEREDOC)
		{
			current = current->next;
			continue ;
		}
		if (!process_single_heredoc_simple(current, current_heredoc, state))
			return (0);
		current_heredoc++;
		current = current->next;
	}
	return (1);
}
