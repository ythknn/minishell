/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:48:31 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/12 20:32:15 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = value;
	token->next = NULL;
	return (token);
}

void	add_token(t_token **tokens, t_token *new_token)
{
	t_token	*current;

	if (!*tokens)
	{
		*tokens = new_token;
		return ;
	}
	current = *tokens;
	while (current->next)
		current = current->next;
	current->next = new_token;
}

static void	words(char *input, int *i, t_token **tokens)
{
	char	buffer[4096];
	int		j;
	char	quote;

	j = 0;
	while (input[*i] && !is_whitespace(input[*i])
		&& input[*i] != '|' && input[*i] != '<' && input[*i] != '>')
	{
		if (input[*i] == '\'' || input[*i] == '"')
		{
			quote = input[*i];
			buffer[j++] = input[(*i)++];
			while (input[*i] && input[*i] != quote)
				buffer[j++] = input[(*i)++];
			if (!input[*i])
			{
				print_error("minishell", NULL, "Unclosed quote");
				return ;
			}
			buffer[j++] = input[(*i)++];
		}
		else
			buffer[j++] = input[(*i)++];
	}
	buffer[j] = '\0';
	add_token(tokens, create_token(T_WORD, ft_strdup(buffer)));
}

t_token	*tokenize(char *input)
{
	t_token	*tokens;
	int		i;

	tokens = NULL;
	i = 0;
	while (input[i])
	{
		skip_whitespace(input, &i);
		if (!input[i])
			break ;
		if (input[i] == '|')
			pipes(&i, &tokens);
		else if (input[i] == '<')
			in_redirects(input, &i, &tokens);
		else if (input[i] == '>')
			out_redirects(input, &i, &tokens);
		else
			words(input, &i, &tokens);
	}
	return (tokens);
}

void	free_tokens(t_token *tokens)
{
	t_token	*current;
	t_token	*next;

	current = tokens;
	while (current)
	{
		next = current->next;
		free(current->value);
		free(current);
		current = next;
	}
}
