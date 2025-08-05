/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   static_vars_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 20:26:36 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/05 20:26:38 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_token	*get_current_tokens(t_shell *shell)
{
	return (shell->current_tokens);
}

t_command	*get_current_commands(t_shell *shell)
{
	return (shell->current_commands);
}

void	clear_current_tokens(t_shell *shell)
{
	if (shell->current_tokens)
	{
		free_tokens(shell->current_tokens);
		shell->current_tokens = NULL;
	}
}

void	clear_current_commands(t_shell *shell)
{
	if (shell->current_commands)
	{
		free_commands(shell->current_commands);
		shell->current_commands = NULL;
	}
}
