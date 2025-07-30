/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:30:00 by yihakan           #+#    #+#             */
/*   Updated: 2025/01/17 10:30:00 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	clear_current_commands(void)
{
	t_command	*commands;

	commands = get_current_commands();
	if (commands)
	{
		free_commands(commands);
		set_current_commands(NULL);
	}
}
