#include "../includes/minishell.h"

void	*gc_malloc(t_shell *shell, size_t size, t_gc_type type)
{
	void	*ptr;

	gc_free_type(shell, type);
	ptr = malloc(size);
	if (!ptr)
		return (NULL);
	gc_set_static_ptr(shell, ptr, type);
	return (ptr);
}

char	*gc_strdup(t_shell *shell, const char *s, t_gc_type type)
{
	char	*new_str;
	size_t	len;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	new_str = gc_malloc(shell, len + 1, type);
	if (!new_str)
		return (NULL);
	ft_strlcpy(new_str, s, len + 1);
	return (new_str);
}

char	*gc_strjoin(t_shell *shell, const char *s1,
		const char *s2, t_gc_type type)
{
	char	*new_str;
	size_t	len1;
	size_t	len2;

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

char	**gc_malloc_array(t_shell *shell, size_t count, t_gc_type type)
{
	char	**array;

	array = gc_malloc(shell, sizeof(char *) * count, type);
	return (array);
}

void	gc_free_type(t_shell *shell, t_gc_type type)
{
	void	*ptr;

	ptr = gc_get_static_ptr(shell, type);
	if (ptr)
	{
		free(ptr);
		gc_set_static_ptr(shell, NULL, type);
	}
}
