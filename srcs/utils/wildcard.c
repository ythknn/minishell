/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:30:00 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/30 17:27:40 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	has_wildcard(const char *str)
{
	int	i;
	int	in_quotes;
	char	quote_char;

	if (!str)
		return (0);
	i = 0;
	in_quotes = 0;
	quote_char = 0;
	while (str[i])
	{
		if (!in_quotes && (str[i] == '\'' || str[i] == '"'))
		{
			in_quotes = 1;
			quote_char = str[i];
		}
		else if (in_quotes && str[i] == quote_char)
		{
			in_quotes = 0;
			quote_char = 0;
		}
		else if (!in_quotes && (str[i] == '*' || str[i] == '?' || str[i] == '['))
			return (1);
		i++;
	}
	return (0);
}

static int	match_bracket(const char *pattern, char c, int *pattern_index)
{
	int	i;
	int	negate;
	int	match;

	i = *pattern_index + 1;
	negate = 0;
	match = 0;
	if (pattern[i] == '!')
	{
		negate = 1;
		i++;
	}
	while (pattern[i] && pattern[i] != ']')
	{
		if (pattern[i + 1] == '-' && pattern[i + 2] && pattern[i + 2] != ']')
		{
			if (c >= pattern[i] && c <= pattern[i + 2])
				match = 1;
			i += 3;
		}
		else
		{
			if (c == pattern[i])
				match = 1;
			i++;
		}
	}
	if (pattern[i] == ']')
		*pattern_index = i;
	else
		return (0);
	return (negate ? !match : match);
}

int	match_pattern(const char *pattern, const char *str)
{
	int	pi;
	int	si;

	pi = 0;
	si = 0;
	while (pattern[pi] && str[si])
	{
		if (pattern[pi] == '*')
		{
			pi++;
			if (!pattern[pi])
				return (1);
			while (str[si])
			{
				if (match_pattern(pattern + pi, str + si))
					return (1);
				si++;
			}
			return (0);
		}
		else if (pattern[pi] == '?')
		{
			pi++;
			si++;
		}
		else if (pattern[pi] == '[')
		{
			if (!match_bracket(pattern, str[si], &pi))
				return (0);
			pi++;
			si++;
		}
		else if (pattern[pi] == str[si])
		{
			pi++;
			si++;
		}
		else
			return (0);
	}
	while (pattern[pi] == '*')
		pi++;
	return (!pattern[pi] && !str[si]);
}

char	**expand_wildcard(const char *pattern)
{
	DIR				*dir;
	struct dirent	*entry;
	char			**matches;
	int				count;
	int				capacity;

	dir = opendir(".");
	if (!dir)
		return (NULL);
	matches = malloc(sizeof(char *) * 10);
	if (!matches)
	{
		closedir(dir);
		return (NULL);
	}
	count = 0;
	capacity = 10;
	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_name[0] == '.' && pattern[0] != '.')
			continue ;
		if (match_pattern(pattern, entry->d_name))
		{
			if (count >= capacity - 1)
			{
				capacity *= 2;
				matches = realloc(matches, sizeof(char *) * capacity);
				if (!matches)
				{
					closedir(dir);
					return (NULL);
				}
			}
			matches[count] = ft_strdup(entry->d_name);
			count++;
		}
	}
	closedir(dir);
	matches[count] = NULL;
	if (count == 0)
	{
		free(matches);
		return (NULL);
	}
	return (matches);
}

static int	compare_strings(const void *a, const void *b)
{
	return (ft_strcmp(*(const char **)a, *(const char **)b));
}

void	sort_matches(char **matches)
{
	int	count;

	if (!matches)
		return ;
	count = 0;
	while (matches[count])
		count++;
	qsort(matches, count, sizeof(char *), compare_strings);
} 