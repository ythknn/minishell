/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 15:55:05 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/22 14:52:03 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

void	skip_whitespace(char *input, int *i)
{
	while (input[*i] && is_whitespace(input[*i]))
		(*i)++;
}

void	pipes(int *i, t_token **tokens)
{
	add_token(tokens, create_token(T_PIPE, ft_strdup("|")));
	(*i)++;
}

void	in_redirects(char *input, int *i, t_token **tokens)
{
	(*i)++;
	if (input[*i] == '<')
	{
		(*i)++;
		add_token(tokens, create_token(T_HEREDOC, ft_strdup("<<")));
	}
	else
		add_token(tokens, create_token(T_REDIR_IN, ft_strdup("<")));
}

void	out_redirects(char *input, int *i, t_token **tokens)
{
	(*i)++;
	if (input[*i] == '>')
	{
		(*i)++;
		add_token(tokens, create_token(T_REDIR_APPEND, ft_strdup(">>")));
	}
	else
		add_token(tokens, create_token(T_REDIR_OUT, ft_strdup(">")));
}

void	and_operator(char *input, int *i, t_token **tokens)
{
	if (input[*i + 1] == '&')
	{
		(*i) += 2;  // Skip both & characters
		add_token(tokens, create_token(T_AND, ft_strdup("&&")));
	}
	else
	{
		// Single & is not supported in this shell, skip it
		(*i)++;
	}
}

void	or_operator(char *input, int *i, t_token **tokens)
{
	if (input[*i + 1] == '|')
	{
		(*i) += 2;  // Skip both | characters
		add_token(tokens, create_token(T_OR, ft_strdup("||")));
	}
	else
	{
		// This is a regular pipe, handle it normally
		add_token(tokens, create_token(T_PIPE, ft_strdup("|")));
		(*i)++;
	}
}
