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
		add_token(tokens, create_token(T_REDIR_APPEND, strdup(">>")));
	}
	else
		add_token(tokens, create_token(T_REDIR_OUT, strdup(">")));
}
