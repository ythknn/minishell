/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   static_vars.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 00:00:00 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/29 00:00:00 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

// Static parsing state for cleanup on interrupt  
static t_token *current_tokens = NULL;
static t_command *current_commands = NULL;

// Static değişkenlere erişim fonksiyonları
t_token *get_current_tokens(void)
{
	return (current_tokens);
}

t_command *get_current_commands(void)
{
	return (current_commands);
}

void set_current_tokens(t_token *tokens)
{
	current_tokens = tokens;
}

void set_current_commands(t_command *commands)
{
	current_commands = commands;
}

void clear_current_tokens(void)
{
	if (current_tokens)
	{
		free_tokens(current_tokens);
		current_tokens = NULL;
	}
}

void clear_current_commands(void)
{
	if (current_commands)
	{
		free_commands(current_commands);
		current_commands = NULL;
	}
}
