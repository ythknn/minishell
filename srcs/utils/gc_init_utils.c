/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_init_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 21:01:30 by yihakan          #+#    #+#             */
/*   Updated: 2025/08/08 21:01:30 by yihakan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	gc_set_static_ptr_part1(t_shell *shell, void *ptr,
				t_gc_type type, int from_part2);

static void	gc_set_static_ptr_part2(t_shell *shell, void *ptr,
				t_gc_type type, int from_part1);

static void	gc_set_static_ptr_part2(t_shell *shell, void *ptr,
	t_gc_type type, int from_part1)
{
	if (type == GC_ENV_VAR)
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
	else if (!from_part1)
		gc_set_static_ptr_part1(shell, ptr, type, 1);
}

static void	gc_set_static_ptr_part1(t_shell *shell, void *ptr,
	t_gc_type type, int from_part2)
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
	else if (!from_part2)
		gc_set_static_ptr_part2(shell, ptr, type, 1);
}

void	gc_set_static_ptr(t_shell *shell, void *ptr, t_gc_type type)
{
	gc_set_static_ptr_part1(shell, ptr, type, 0);
}
