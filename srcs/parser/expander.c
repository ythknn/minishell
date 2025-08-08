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

void	init_expand_ctx(t_expand_ctx *ctx, t_shell *shell, char *result)
{
	ctx->in_quotes = 0;
	ctx->quote_char = 0;
	ctx->shell = shell;
	ctx->result = result;
}

char	*handle_variable_expansion(char *str, int *i, t_shell *shell)
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
