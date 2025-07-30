/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_operators.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:30:00 by yihakan           #+#    #+#             */
/*   Updated: 2025/01/17 10:30:00 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	and_operator(char *input, int *i, t_token **tokens)
{
	if (input[*i + 1] == '&')
	{
		add_token(tokens, create_token(T_AND, ft_strdup("&&")));
		(*i) += 2;
	}
	else
		(*i)++;
}

void	or_operator(char *input, int *i, t_token **tokens)
{
	if (input[*i + 1] == '|')
	{
		add_token(tokens, create_token(T_OR, ft_strdup("||")));
		(*i) += 2;
	}
	else
	{
		add_token(tokens, create_token(T_PIPE, ft_strdup("|")));
		(*i)++;
	}
}
