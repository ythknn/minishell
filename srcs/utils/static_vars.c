/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   static_vars.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 00:00:00 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/04 04:36:01 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_token *get_current_tokens(t_shell *shell)
{
	return (shell->current_tokens);
}

t_command *get_current_commands(t_shell *shell)
{
	return (shell->current_commands);
}

void set_current_tokens(t_shell *shell, t_token *tokens)
{
	shell->current_tokens = tokens;
}

void set_current_commands(t_shell *shell, t_command *commands)
{
	shell->current_commands = commands;
}

void clear_current_tokens(t_shell *shell)
{
	if (shell->current_tokens)
	{
		free_tokens(shell->current_tokens);
		shell->current_tokens = NULL;
	}
}

void clear_current_commands(t_shell *shell)
{
	if (shell->current_commands)
	{
		free_commands(shell->current_commands);
		shell->current_commands = NULL;
	}
}
