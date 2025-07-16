/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:21:39 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/15 18:36:11 by mdusunen         ###   ########.fr       */
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

char	*display_prompt(void)
{
	char	*line;
	char	*prompt;

	prompt = malloc(100);
	if (!prompt)
		return (NULL);
	sprintf(prompt, "%sminishell%s%s$>%s ", GREEN, RESET, YELLOW, RESET);
	line = readline(prompt);
	free(prompt);
	return (line);
}
