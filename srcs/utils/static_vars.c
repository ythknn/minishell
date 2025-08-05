/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   static_vars.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 00:00:00 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/04 19:06:56 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	set_current_tokens(t_shell *shell, t_token *tokens)
{
	shell->current_tokens = tokens;
}

void	set_current_commands(t_shell *shell, t_command *commands)
{
	shell->current_commands = commands;
}
