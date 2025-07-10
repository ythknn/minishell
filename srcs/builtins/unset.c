/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:22:58 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/09 19:22:58 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/minishell.h"

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
	
	free(shell->env_array);
	shell->env_array = env_list_to_array(shell->env_list);
	
	return (0);
}
