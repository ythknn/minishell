/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:58:29 by mdusunen          #+#    #+#             */
/*   Updated: 2025/06/29 18:37:49 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	init_lexer(t_lexer *lex, char *input, int *i, int *j)
{
	lex->in_quotes = 0;
	lex->quote_char = 0;
	lex->input = input;
	lex->i = i;
	lex->j = j;
}

static int	handle_quotes(t_lexer *lex)
{
	if (!lex->in_quotes && (lex->input[*lex->i] == '\''
			|| lex->input[*lex->i] == '"'))
	{
		lex->in_quotes = 1;
		lex->quote_char = lex->input[*lex->i];
		lex->processed[(*lex->j)++] = lex->input[(*lex->i)++];
		return (1);
	}
	else if (lex->in_quotes && lex->input[*lex->i] == lex->quote_char)
	{
		lex->in_quotes = 0;
		lex->processed[(*lex->j)++] = lex->input[(*lex->i)++];
		return (1);
	}
	return (0);
}

static int	handle_specials(t_lexer *lex)
{
	if (lex->in_quotes)
		return (0);
	if (lex->input[*lex->i] == '|' || lex->input[*lex->i] == '<'
		|| lex->input[*lex->i] == '>')
	{
		if (*lex->j > 0 && lex->processed[*lex->j - 1] != ' ')
			lex->processed[(*lex->j)++] = ' ';
		lex->processed[(*lex->j)++] = lex->input[(*lex->i)++];
		if ((lex->input[*lex->i - 1] == '<' && lex->input[*lex->i] == '<')
			|| (lex->input[*lex->i - 1] == '>' && lex->input[*lex->i] == '>'))
			lex->processed[(*lex->j)++] = lex->input[(*lex->i)++];
		if (lex->input[*lex->i] && lex->input[*lex->i] != ' ')
			lex->processed[(*lex->j)++] = ' ';
		return (1);
	}
	return (0);
}

char	*lexer_analyze(char *input)
{
	t_lexer	lex;
	int		i;
	int		j;

	if (!input)
		return (NULL);
	lex.processed = malloc(strlen(input) * 2 + 1);
	if (!(lex.processed))
		return (NULL);
	i = 0;
	j = 0;
	init_lexer(&lex, input, &i, &j);
	while (input[i])
	{
		if (handle_quotes(&lex))
			continue ;
		if (handle_specials(&lex))
			continue ;
		lex.processed[j++] = input[i++];
	}
	lex.processed[j] = '\0';
	return (lex.processed);
}
