/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:23:59 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/08 19:15:03 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	**build_filtered_args(char **args, int count)
{
	char	**new_args;
	int		i;
	int		j;

	new_args = malloc(sizeof(char *) * (count + 1));
	if (!new_args)
		return (NULL);
	i = 0;
	j = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')
		{
			new_args[j] = args[i];
			j++;
		}
		else
			free(args[i]);
		i++;
	}
	new_args[j] = NULL;
	return (new_args);
}

int	count_non_empty(char **args)
{
	int		i;
	int		count;

	i = 0;
	count = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')
			count++;
		i++;
	}
	return (count);
}

void	expand_args_in_place(char **args, t_shell *shell)
{
	int		i;
	char	*expanded;

	i = 0;
	while (args[i])
	{
		expanded = expand_env_vars(args[i], shell);
		free(args[i]);
		args[i] = expanded;
		i++;
	}
}
