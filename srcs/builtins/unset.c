/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:22:58 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/26 18:27:43 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	ft_unset(char **args, t_shell *shell)
{
	int	i;

	if (!args[1])
		return (0);
	i = 1;
	while (args[i])
	{
		remove_env_var(&shell->env_list, args[i]);
		i++;
	}
	if (shell->env_array)
	{
		i = 0;
		while (shell->env_array[i])
		{
			free(shell->env_array[i]);
			i++;
		}
		free(shell->env_array);
	}
	shell->env_array = env_list_to_array(shell->env_list);
	return (0);
}