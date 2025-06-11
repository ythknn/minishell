/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:24:57 by yihakan           #+#    #+#             */
/*   Updated: 2025/06/11 17:46:30 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/minishell.h"

static t_token	*create_token(t_token_type type, char *value)
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

static void	add_token(t_token **tokens, t_token *new_token)
{
	t_token	*current;

	if (!*tokens)
	{
		*tokens = new_token;
		return;
	}
	current = *tokens;
	while (current->next)
		current = current->next;
	current->next = new_token;
}

static int	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

static void skip_whitespace(char *input, int *i)
{
    while (input[*i] && is_whitespace(input[*i]))
        (*i)++;
}

static void pipes(int *i, t_token **tokens)
{
    add_token(tokens, create_token(T_PIPE, strdup("|")));
    (*i)++;
}

static void in_redirects(char *input, int *i, t_token **tokens)
{
    if (input[*i + 1] == '<')
    {
        add_token(tokens, create_token(T_HEREDOC, strdup("<<")));
        *i += 2;
    }
    else
    {
        add_token(tokens, create_token(T_REDIR_IN, strdup("<")));
        (*i)++;
    }
}

static void out_redirects(char *input, int *i, t_token **tokens)
{
    if (input[*i + 1] == '>')
    {
        add_token(tokens, create_token(T_REDIR_APPEND, strdup(">>")));
        *i += 2;
    }
    else
    {
        add_token(tokens, create_token(T_REDIR_OUT, strdup(">")));
        (*i)++;
    }
}

static void skip_quotes(char *input, int *i)
{
    char quote = input[(*i)++];
    while (input[*i] && input[*i] != quote)
        (*i)++;
    if (input[*i])
        (*i)++;
}

static void words(char *input, int *i, t_token **tokens)
{
    int start = *i;
    
    while (input[*i] && !is_whitespace(input[*i]) &&
           input[*i] != '|' && input[*i] != '<' && input[*i] != '>')
    {
        if (input[*i] == '\'' || input[*i] == '"')
            skip_quotes(input, i);
        else
            (*i)++;
    }
    add_token(tokens, create_token(T_WORD, strndup(input + start, *i - start)));
}

t_token *tokenize(char *input)
{
    t_token *tokens = NULL;
    int i = 0;

    while (input[i])
    {
		skip_whitespace(input, &i);
		if (!input[i])
			break;
        if (input[i] == '|')
			pipes(&i, &tokens);
		else if (input[i] == '<')
			in_redirects(input, &i, &tokens);
        else if (input[i] == '>')
			out_redirects(input, &i, &tokens);
        else
			words(input, &i, &tokens);
    }
    return tokens;
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
