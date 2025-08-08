/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:11:25 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/18 19:13:32 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "echo") == 0
		|| ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "env") == 0
		|| ft_strcmp(cmd, "exit") == 0
		|| ft_strcmp(cmd, "pills") == 0)
		return (1);
	return (0);
}

int	execute_builtin(char **args, t_shell *shell)
{
	if (!args || !args[0])
		return (1);
	if (ft_strcmp(args[0], "echo") == 0)
		return (ft_echo(args));
	else if (ft_strcmp(args[0], "cd") == 0)
		return (ft_cd(args, shell));
	else if (ft_strcmp(args[0], "pwd") == 0)
		return (ft_pwd());
	else if (ft_strcmp(args[0], "export") == 0)
		return (ft_export(args, shell));
	else if (ft_strcmp(args[0], "unset") == 0)
		return (ft_unset(args, shell));
	else if (ft_strcmp(args[0], "env") == 0)
		return (ft_env(shell));
	else if (ft_strcmp(args[0], "exit") == 0)
		return (ft_exit(args, shell));
	else if (ft_strcmp(args[0], "pills") == 0)
		return (ft_pills(args, shell));
	return (1);
}
