/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_manager.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 00:00:00 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/04 04:35:56 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/**
 * Get pointer from shell structure based on type
 */
void *gc_get_static_ptr(t_shell *shell, t_gc_type type)
{
	if (type == GC_LINE)
		return (shell->gc_line);
	else if (type == GC_PROCESSED_LINE)
		return (shell->gc_processed_line);
	else if (type == GC_TOKENS)
		return (shell->gc_tokens);
	else if (type == GC_COMMANDS)
		return (shell->gc_commands);
	else if (type == GC_ARGS)
		return (shell->gc_args);
	else if (type == GC_ENV_VAR)
		return (shell->gc_env_var);
	else if (type == GC_PATH)
		return (shell->gc_path);
	else if (type == GC_EXEC_PATH)
		return (shell->gc_exec_path);
	else if (type == GC_HEREDOC)
		return (shell->gc_heredoc);
	else if (type == GC_TEMP_STR)
		return (shell->gc_temp_str);
	else if (type == GC_ENV_ARRAY)
		return (shell->gc_env_array);
	else if (type == GC_REDIR)
		return (shell->gc_redir);
	else if (type == GC_GENERAL)
		return (shell->gc_general);
	return (NULL);
}

/**
 * Set pointer in shell structure based on type
 */
void gc_set_static_ptr(t_shell *shell, void *ptr, t_gc_type type)
{
	if (type == GC_LINE)
		shell->gc_line = ptr;
	else if (type == GC_PROCESSED_LINE)
		shell->gc_processed_line = ptr;
	else if (type == GC_TOKENS)
		shell->gc_tokens = ptr;
	else if (type == GC_COMMANDS)
		shell->gc_commands = ptr;
	else if (type == GC_ARGS)
		shell->gc_args = ptr;
	else if (type == GC_ENV_VAR)
		shell->gc_env_var = ptr;
	else if (type == GC_PATH)
		shell->gc_path = ptr;
	else if (type == GC_EXEC_PATH)
		shell->gc_exec_path = ptr;
	else if (type == GC_HEREDOC)
		shell->gc_heredoc = ptr;
	else if (type == GC_TEMP_STR)
		shell->gc_temp_str = ptr;
	else if (type == GC_ENV_ARRAY)
		shell->gc_env_array = ptr;
	else if (type == GC_REDIR)
		shell->gc_redir = ptr;
	else if (type == GC_GENERAL)
		shell->gc_general = ptr;
}

/**
 * Allocate memory and store pointer in shell structure
 */
void *gc_malloc(t_shell *shell, size_t size, t_gc_type type)
{
	void *ptr;

	gc_free_type(shell, type);
	
	ptr = malloc(size);
	if (!ptr)
		return (NULL);
	
	gc_set_static_ptr(shell, ptr, type);
	return (ptr);
}

/**
 * String duplication with garbage collection
 */
char *gc_strdup(t_shell *shell, const char *s, t_gc_type type)
{
	char *new_str;
	size_t len;

	if (!s)
		return (NULL);
		
	len = ft_strlen(s);
	new_str = gc_malloc(shell, len + 1, type);
	if (!new_str)
		return (NULL);
		
	ft_strlcpy(new_str, s, len + 1);
	return (new_str);
}

/**
 * String join with garbage collection
 */
char *gc_strjoin(t_shell *shell, const char *s1, const char *s2, t_gc_type type)
{
	char *new_str;
	size_t len1, len2;

	if (!s1 || !s2)
		return (NULL);
		
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	new_str = gc_malloc(shell, len1 + len2 + 1, type);
	if (!new_str)
		return (NULL);
		
	ft_strlcpy(new_str, s1, len1 + 1);
	ft_strlcat(new_str, s2, len1 + len2 + 1);
	return (new_str);
}

/**
 * Array allocation with garbage collection
 */
char **gc_malloc_array(t_shell *shell, size_t count, t_gc_type type)
{
	char **array;

	array = gc_malloc(shell, sizeof(char *) * count, type);
	return (array);
}

/**
 * Free memory for specific type
 */
void gc_free_type(t_shell *shell, t_gc_type type)
{
	void *ptr;

	ptr = gc_get_static_ptr(shell, type);
	if (ptr)
	{
		free(ptr);
		gc_set_static_ptr(shell, NULL, type);
	}
}

/**
 * Free all garbage collected memory
 */
void gc_free_all(t_shell *shell)
{
	gc_free_type(shell, GC_LINE);
	gc_free_type(shell, GC_PROCESSED_LINE);
	gc_free_type(shell, GC_HEREDOC);
	gc_free_type(shell, GC_TEMP_STR);
	gc_free_type(shell, GC_GENERAL);
}
