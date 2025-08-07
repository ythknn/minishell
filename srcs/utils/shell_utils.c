/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 20:27:22 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/05 20:27:24 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	init_shell(t_shell *shell, char **env)
{
	shell->env_list = create_env_list(env);
	if (!shell->env_list)
		return ;
	shell->env_array = env_list_to_array(shell->env_list);
	if (!shell->env_array)
		return ;
	shell->exit_status = 0;
	shell->interactive = isatty(STDIN_FILENO);
	shell->gc_line = NULL;
	shell->gc_processed_line = NULL;
	shell->gc_tokens = NULL;
	shell->gc_commands = NULL;
	shell->gc_args = NULL;
	shell->gc_env_var = NULL;
	shell->gc_path = NULL;
	shell->gc_exec_path = NULL;
	shell->gc_heredoc = NULL;
	shell->gc_temp_str = NULL;
	shell->gc_env_array = NULL;
	shell->gc_redir = NULL;
	shell->gc_general = NULL;
	shell->current_tokens = NULL;
	shell->current_commands = NULL;
}

void	free_shell(t_shell *shell)
{
	t_env	*current;
	t_env	*next;
	int		i;

	i = 0;
	if (!shell)
		return ;
	clear_current_tokens(shell);
	clear_current_commands(shell);
	gc_free_all(shell);
	current = shell->env_list;
	while (current)
	{
		next = current->next;
		free(current->key);
		free(current->value);
		free(current);
		current = next;
	}
	if (shell->env_array)
	{
		while (shell->env_array[i])
			free(shell->env_array[i++]);
		free(shell->env_array);
	}
}

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char) s1[i] - (unsigned char) s2[i]);
}
