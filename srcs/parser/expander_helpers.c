/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_helpers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 20:53:00 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/09 20:53:00 by yihakan          ###   ########.fr       */
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

char	*handle_special_vars(char *str, int *i, t_shell *shell)
{
	if (str[*i] == '?')
	{
		(*i)++;
		return (ft_itoa(shell->exit_status));
	}
	else if (str[*i] == '$')
	{
		(*i)++;
		return (ft_itoa(getpid()));
	}
	return (NULL);
}
