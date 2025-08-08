/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:23:59 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/08 19:15:03 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static char	*process_expansion_or_char(char *str, int *i, t_expand_ctx *ctx)
{
	char	*temp;

	if (str[*i] == '$' && (!(ctx->in_quotes) || ctx->quote_char != '\''))
	{
		temp = handle_variable_expansion(str, i, ctx->shell);
		if (temp)
		{
			ctx->result = ft_strjoin_free(ctx->result, temp);
			free(temp);
		}
	}
	else
	{
		ctx->result = add_char_to_result(ctx->result, str[*i]);
		(*i)++;
	}
	return (ctx->result);
}

static void	update_quote_state(char c, int *in_quotes, char *quote_char)
{
	if ((c == '\'' || c == '"') && (!(*in_quotes) || *quote_char == c))
	{
		if (*in_quotes)
			*in_quotes = 0;
		else
		{
			*in_quotes = 1;
			*quote_char = c;
		}
	}
}

char	*expand_process_loop(char *str, t_expand_ctx *ctx)
{
	int		i;

	i = 0;
	while (str[i])
	{
		update_quote_state(str[i], &ctx->in_quotes, &ctx->quote_char);
		if ((str[i] == '\'' || str[i] == '"')
			&& (!ctx->in_quotes || ctx->quote_char == str[i]))
		{
			i++;
			continue ;
		}
		ctx->result = process_expansion_or_char(str, &i, ctx);
	}
	return (ctx->result);
}

char	*ft_strjoin_free(char *s1, char *s2)
{
	char	*result;

	if (!s1 || !s2)
		return (s1);
	result = ft_strjoin(s1, s2);
	free(s1);
	return (result);
}

char	*add_char_to_result(char *result, char c)
{
	char	temp[2];
	char	*new_result;

	temp[0] = c;
	temp[1] = '\0';
	new_result = ft_strjoin(result, temp);
	free(result);
	return (new_result);
}
