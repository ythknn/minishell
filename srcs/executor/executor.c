/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 16:29:27 by yihakan           #+#    #+#             */
/*   Updated: 2025/06/11 16:29:27 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/minishell.h"
# include "../libft/libft.h"

static int    execute_single_command(t_command *cmd, t_shell *shell)
{
    pid_t    pid;
    int      status;
    char     *path;

    if (!cmd->args || !cmd->args[0])
        return (0);
    if (is_builtin(cmd->args[0]))
        return (execute_builtin(cmd->args, shell));
    path = find_executable(cmd->args[0], shell);
    if (!path)
    {
        print_command_not_found(cmd->args[0]);
        return (127);
    }
    pid = fork();
    if (pid == 0)
    {
        setup_redirections(cmd->redirections);
        execve(path, cmd->args, shell->env_array);
        print_error(cmd->args[0], NULL, strerror(errno));
        exit(1);
    }
    else if (pid < 0)
    {
        print_error("fork", NULL, strerror(errno));
        free(path);
        return (1);
    }
    waitpid(pid, &status, 0);
    free(path);    
    if (WIFEXITED(status))
        return (WEXITSTATUS(status));
    return (1);
}

int    execute_commands(t_command *cmds, t_shell *shell)
{
    int          stdin_copy;
    int          stdout_copy;
    int          status;

    if (!cmds)
        return (0);
    stdin_copy = dup(STDIN_FILENO);
    stdout_copy = dup(STDOUT_FILENO);
    status = 0;
    if (!cmds->next)
    {
        setup_redirections(cmds->redirections);
        status = execute_single_command(cmds, shell);
        restore_redirections(stdin_copy, stdout_copy);
        return (status);
    }
    status = execute_pipeline(cmds, shell);
    restore_redirections(stdin_copy, stdout_copy);
    return (status);
}
