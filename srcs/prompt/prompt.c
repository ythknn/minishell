/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:21:39 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/08 20:51:11 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define RESET "\033[0m"

static void	clean_resources(t_shell *shell)
{
	clear_current_tokens(shell);
	clear_current_commands(shell);
	gc_free_all(shell);
}

char	*display_prompt(t_shell *shell)
{
	char	*line;
	char	*prompt;
	char	*temp1;
	char	*temp2;
	char	*temp3;

	temp1 = ft_strjoin(GREEN, "minishell");
	temp2 = ft_strjoin(temp1, RESET);
	free(temp1);
	temp1 = ft_strjoin(temp2, YELLOW);
	free(temp2);
	temp2 = ft_strjoin(temp1, "$>");
	free(temp1);
	temp3 = ft_strjoin(temp2, RESET);
	free(temp2);
	prompt = ft_strjoin(temp3, " ");
	free(temp3);
	line = readline(prompt);
	free(prompt);
	if (!line)
		clean_resources(shell);
	return (line);
}
