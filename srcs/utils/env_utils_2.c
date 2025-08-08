/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils_2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 20:26:52 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/05 20:26:54 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static char	*envjoin(char *s1, char *s2)
{
	size_t	total_len;
	char	*result;
	int		i;
	int		j;

	i = 0;
	j = 0;
	total_len = ft_strlen(s1) + ft_strlen(s2);
	if (!s1 || !s2)
		return (NULL);
	result = (char *)malloc((total_len * sizeof(char)) + 1);
	if (!result)
		return (NULL);
	while (s1[i] != '\0')
	{
		result[i] = ((unsigned char *)s1)[i];
		i++;
	}
	while (s2[j] != '\0')
		result[i++] = ((unsigned char *)s2)[j++];
	result[i] = '\0';
	if (s1)
		free(s1);
	return (result);
}

static	char	**double_join(char **doublebase, char *tojoin)
{
	size_t	len;
	size_t	i;
	char	**ret;

	len = 0;
	i = 0;
	while (doublebase && doublebase[len])
		len++;
	ret = malloc(sizeof(char *) * (len + 2));
	while (doublebase && doublebase[i])
	{
		ret[i] = ft_strdup(doublebase[i]);
		free(doublebase[i]);
		doublebase[i] = NULL;
		i++;
	}
	ret[i] = ft_strdup(tojoin);
	i++;
	ret[i] = NULL;
	if (doublebase)
		free(doublebase);
	if (tojoin)
		free(tojoin);
	return (ret);
}

char	**env_list_to_array(t_env *env_list)
{
	char	**env_array;
	t_env	*current;
	char	*tojoin;

	current = env_list;
	env_array = NULL;
	while (current)
	{
		tojoin = ft_strjoin(current->key, "=");
		tojoin = envjoin(tojoin, current->value);
		env_array = double_join(env_array, tojoin);
		current = current->next;
	}
	return (env_array);
}

t_env	*add_env_var(t_env *env_list, char *key, char *value)
{
	t_env	*current;
	t_env	*new_node;

	current = env_list;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			free(current->value);
			current->value = ft_strdup(value);
			return (env_list);
		}
		current = current->next;
	}
	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return (env_list);
	new_node->key = ft_strdup(key);
	new_node->value = ft_strdup(value);
	new_node->next = env_list;
	return (new_node);
}

void	remove_env_var(t_env **env_list, char *key)
{
	t_env	*cur;
	t_env	*prev;

	cur = *env_list;
	prev = NULL;
	while (cur)
	{
		if (ft_strcmp(cur->key, key) == 0)
		{
			if (prev)
				prev->next = cur->next;
			else
				*env_list = cur->next;
			free(cur->key);
			free(cur->value);
			free(cur);
			return ;
		}
		prev = cur;
		cur = cur->next;
	}
}
