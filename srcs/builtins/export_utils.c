/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 10:54:00 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/03 10:56:09 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <string.h>

int	is_valid_env_name(const char *name)
{
	int	i;

	if (!name || !*name)
		return (0);
	if (name[0] != '_' && (name[0] < 'a' || name[0] > 'z')
		&& (name[0] < 'A' || name[0] > 'Z'))
		return (0);
	i = 1;
	while (name[i])
	{
		if (name[i] == '=')
			break ;
		if (name[i] != '_' && (name[i] < 'a' || name[i] > 'z')
			&& (name[i] < 'A' || name[i] > 'Z')
			&& (name[i] < '0' || name[i] > '9'))
			return (0);
		i++;
	}
	return (1);
}

void	print_export_error(char *arg)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
}

void	print_exported_vars(t_env *env)
{
	while (env)
	{
		printf("declare -x %s=\"%s\"\n", env->key, env->value);
		env = env->next;
	}
}

char	*extract_key(char *arg)
{
	int		j;
	char	*key;

	j = 0;
	while (arg[j] && arg[j] != '=')
		j++;
	key = malloc(j + 1);
	if (!key)
		return (NULL);
	ft_strlcpy(key, arg, j + 1);
	return (key);
}
