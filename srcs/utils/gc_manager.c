/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_manager.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 00:00:00 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/29 00:00:00 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

// Statik değişkenler - her type için ayrı pointer
static void *gc_line = NULL;
static void *gc_processed_line = NULL;
static void *gc_tokens = NULL;
static void *gc_commands = NULL;
static void *gc_args = NULL;
static void *gc_env_var = NULL;
static void *gc_path = NULL;
static void *gc_exec_path = NULL;
static void *gc_heredoc = NULL;
static void *gc_temp_str = NULL;
static void *gc_env_array = NULL;
static void *gc_redir = NULL;
static void *gc_general = NULL;

/**
 * Statik pointer'ı döndürür
 */
void *gc_get_static_ptr(t_gc_type type)
{
	if (type == GC_LINE)
		return (gc_line);
	else if (type == GC_PROCESSED_LINE)
		return (gc_processed_line);
	else if (type == GC_TOKENS)
		return (gc_tokens);
	else if (type == GC_COMMANDS)
		return (gc_commands);
	else if (type == GC_ARGS)
		return (gc_args);
	else if (type == GC_ENV_VAR)
		return (gc_env_var);
	else if (type == GC_PATH)
		return (gc_path);
	else if (type == GC_EXEC_PATH)
		return (gc_exec_path);
	else if (type == GC_HEREDOC)
		return (gc_heredoc);
	else if (type == GC_TEMP_STR)
		return (gc_temp_str);
	else if (type == GC_ENV_ARRAY)
		return (gc_env_array);
	else if (type == GC_REDIR)
		return (gc_redir);
	else if (type == GC_GENERAL)
		return (gc_general);
	return (NULL);
}

/**
 * Statik pointer'ı set eder
 */
void gc_set_static_ptr(void *ptr, t_gc_type type)
{
	if (type == GC_LINE)
		gc_line = ptr;
	else if (type == GC_PROCESSED_LINE)
		gc_processed_line = ptr;
	else if (type == GC_TOKENS)
		gc_tokens = ptr;
	else if (type == GC_COMMANDS)
		gc_commands = ptr;
	else if (type == GC_ARGS)
		gc_args = ptr;
	else if (type == GC_ENV_VAR)
		gc_env_var = ptr;
	else if (type == GC_PATH)
		gc_path = ptr;
	else if (type == GC_EXEC_PATH)
		gc_exec_path = ptr;
	else if (type == GC_HEREDOC)
		gc_heredoc = ptr;
	else if (type == GC_TEMP_STR)
		gc_temp_str = ptr;
	else if (type == GC_ENV_ARRAY)
		gc_env_array = ptr;
	else if (type == GC_REDIR)
		gc_redir = ptr;
	else if (type == GC_GENERAL)
		gc_general = ptr;
}

/**
 * Belirli tip için malloc yapar ve statik pointer'a atar
 */
void *gc_malloc(size_t size, t_gc_type type)
{
	void *ptr;

	// Önce eski pointer'ı free et
	gc_free_type(type);
	
	ptr = malloc(size);
	if (!ptr)
		return (NULL);
	
	// Yeni pointer'ı statik değişkene ata
	gc_set_static_ptr(ptr, type);
	return (ptr);
}

/**
 * Belirli tip için strdup yapar
 */
char *gc_strdup(const char *s, t_gc_type type)
{
	char *new_str;
	size_t len;

	if (!s)
		return (NULL);
		
	len = ft_strlen(s);
	new_str = gc_malloc(len + 1, type);
	if (!new_str)
		return (NULL);
		
	ft_strlcpy(new_str, s, len + 1);
	return (new_str);
}

/**
 * Belirli tip için strjoin yapar
 */
char *gc_strjoin(const char *s1, const char *s2, t_gc_type type)
{
	char *new_str;
	size_t len1, len2;

	if (!s1 || !s2)
		return (NULL);
		
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	new_str = gc_malloc(len1 + len2 + 1, type);
	if (!new_str)
		return (NULL);
		
	ft_strlcpy(new_str, s1, len1 + 1);
	ft_strlcat(new_str, s2, len1 + len2 + 1);
	return (new_str);
}

/**
 * Array için malloc yapar
 */
char **gc_malloc_array(size_t count, t_gc_type type)
{
	char **array;

	array = gc_malloc(sizeof(char *) * count, type);
	return (array);
}

/**
 * Belirli bir tip için memory'yi free eder
 */
void gc_free_type(t_gc_type type)
{
	void *ptr;

	ptr = gc_get_static_ptr(type);
	if (ptr)
	{
		free(ptr);
		// Statik pointer'ı NULL yap
		gc_set_static_ptr(NULL, type);
	}
}

/**
 * Tüm statik pointer'ları free eder
 */
void gc_free_all(void)
{
	gc_free_type(GC_LINE);
	gc_free_type(GC_PROCESSED_LINE);
	gc_free_type(GC_HEREDOC);
	gc_free_type(GC_TEMP_STR);
	gc_free_type(GC_GENERAL);
}
