/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:19:51 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/08 21:41:54 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static char	*ft_strjoin_free(char *s1, char *s2)
{
	char	*result;

	if (!s1 || !s2)
		return (s1);
	result = ft_strjoin(s1, s2);
	free(s1);
	return (result);
}

static char	*add_char_to_result(char *result, char c)
{
	char	temp[2];
	char	*new_result;

	temp[0] = c;
	temp[1] = '\0';
	new_result = ft_strjoin(result, temp);
	free(result);
	return (new_result);
}

static char	*handle_variable_expansion(char *str, int *i, t_shell *shell)
{
	char	var_name[256];
	char	*env_value;
	int		j;

	(*i)++;
	if (str[*i] == '?')
	{
		(*i)++;
		return (ft_itoa(shell->exit_status));
	}
	else if (str[*i] && (str[*i] == '_' || ft_isalnum(str[*i])))
	{
		j = 0;
		while (str[*i] && (str[*i] == '_' || ft_isalnum(str[*i])))
			var_name[j++] = str[(*i)++];
		var_name[j] = '\0';
		env_value = get_env_value(shell->env_list, var_name);
		if (env_value)
			return (ft_strdup(env_value));
		return (ft_strdup(""));
	}
	else
		return (ft_strdup("$"));
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

typedef struct s_expand_ctx
{
	int		in_quotes;
	char	quote_char;
	t_shell	*shell;
	char	*result;
}	t_expand_ctx;

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

static void	init_expand_ctx(t_expand_ctx *ctx, t_shell *shell, char *result)
{
	ctx->in_quotes = 0;
	ctx->quote_char = 0;
	ctx->shell = shell;
	ctx->result = result;
}

static char	*expand_process_loop(char *str, t_expand_ctx *ctx)
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

char	*expand_env_vars(char *str, t_shell *shell)
{
	char			*result;
	t_expand_ctx	ctx;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	init_expand_ctx(&ctx, shell, result);
	return (expand_process_loop(str, &ctx));
}

void	expand_variables(t_command *cmd, t_shell *shell)
{
	t_command	*current_cmd;

	current_cmd = cmd;
	while (current_cmd)
	{
		expand_args(current_cmd, shell);
		expand_redirections(current_cmd, shell);
		current_cmd = current_cmd->next;
	}
}
