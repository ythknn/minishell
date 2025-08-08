/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 19:41:18 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/08 21:34:46 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <signal.h>
# include <dirent.h>
# include <string.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <termios.h>
# include "../libft/libft.h"

# define SUCCESS 0
# define ERROR 1
# define FALSE 0
# define TRUE 1

extern int	g_signal;

typedef enum e_gc_type
{
	GC_LINE,
	GC_PROCESSED_LINE,
	GC_TOKENS,
	GC_COMMANDS,
	GC_ARGS,
	GC_ENV_VAR,
	GC_PATH,
	GC_EXEC_PATH,
	GC_HEREDOC,
	GC_TEMP_STR,
	GC_ENV_ARRAY,
	GC_REDIR,
	GC_GENERAL
}	t_gc_type;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_REDIR_APPEND,
	T_HEREDOC,
	T_EOF
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

typedef struct s_redir
{
	t_token_type	type;
	char			*file;
	struct s_redir	*next;
}	t_redir;

typedef struct s_command
{
	char				**args;
	t_redir				*redirections;
	struct s_command	*next;
}	t_command;

typedef struct s_lexer
{
	char				*input;
	char				*processed;
	int					*i;
	int					*j;
	int					in_quotes;
	char				quote_char;
}	t_lexer;

typedef struct s_pipe_data
{
	int		pipe_fd[2];
	int		heredoc_pipe_fd[2];
	int		prev_pipe_read;
	int		last_status;
	int		last_cmd_not_found;
	int		has_heredoc_flag;
}	t_pipe_data;

typedef struct s_heredoc_state
{
	int				stdin_copy;
	char			*heredoc_content;
	struct termios	term;
	int				heredoc_count;
}	t_heredoc_state;

typedef struct s_shell
{
	t_env		*env_list;
	char		**env_array;
	t_token		*tokens;
	t_command	*commands;
	char		*processed_line;
	int			exit_status;
	int			interactive;
	void		*gc_line;
	void		*gc_processed_line;
	void		*gc_tokens;
	void		*gc_commands;
	void		*gc_args;
	void		*gc_env_var;
	void		*gc_path;
	void		*gc_exec_path;
	void		*gc_heredoc;
	void		*gc_temp_str;
	void		*gc_env_array;
	void		*gc_redir;
	void		*gc_general;
	t_token		*current_tokens;
	t_command	*current_commands;
}	t_shell;

typedef struct s_expand_ctx
{
	int			in_quotes;
	char		quote_char;
	t_shell		*shell;
	char		*result;
}	t_expand_ctx;

typedef struct s_expansion_out
{
	char		*result;
	int			*j;
}	t_expansion_out;

t_token		*get_current_tokens(t_shell *shell);
t_command	*get_current_commands(t_shell *shell);
void		set_current_tokens(t_shell *shell, t_token *tokens);
void		set_current_commands(t_shell *shell, t_command *commands);
void		clear_current_tokens(t_shell *shell);
void		clear_current_commands(t_shell *shell);

void		init_shell(t_shell *shell, char **env);
void		free_shell(t_shell *shell);
int			ft_strcmp(const char *s1, const char *s2);
void		cleanup_shell_resources(t_shell *shell);

char		*display_prompt(t_shell *shell);
void		add_to_history(char *line);
void		init_history(void);
void		save_history(void);

char		*lexer_analyze(char *input);
t_token		*tokenize(char *input);
t_command	*parse(t_token *tokens);
void		expand_variables(t_command *cmd, t_shell *shell);
void		free_tokens(t_token *tokens);
void		free_commands(t_command *cmds);

int			execute_commands(t_command *cmds, t_shell *shell);
int			execute_pipeline(t_command *cmds, t_shell *shell);
int			execute_builtin(char **args, t_shell *shell);
int			is_builtin(char *cmd);
int			setup_redirections(t_redir *redirs);
void		restore_redirections(int stdin_copy, int stdout_copy);
char		*find_executable(char *cmd, t_shell *shell);

int			ft_echo(char **args);
int			ft_cd(char **args, t_shell *shell);
int			ft_pwd(void);
int			ft_export(char **args, t_shell *shell);
int			ft_unset(char **args, t_shell *shell);
int			ft_env(t_shell *shell);
int			ft_exit(char **args, t_shell *shell);
int			ft_pills(char **args, t_shell *shell);

void		print_matrix_ascii(void);
void		print_morpheus_quote(void);
void		matrix_effect(void);

t_env		*create_env_list(char **env);
char		**env_list_to_array(t_env *env_list);
t_env		*add_env_var(t_env *env_list, char *key, char *value);
void		remove_env_var(t_env **env_list, char *key);
char		*get_env_value(t_env *env_list, char *key);
void		print_error(char *cmd, char *arg, char *msg);
void		print_command_not_found(char *cmd);
char		*get_path(char *cmd, t_shell *shell);

void		setup_signals(void);
void		reset_signals(void);
void		handle_sigint(int sig);
void		handle_sigquit(int sig);

void		handle_heredoc_sigint(int sig);
void		setup_heredoc_signals(void);

t_token		*create_token(t_token_type type, char *value);
void		add_token(t_token **tokens, t_token *new_token);
t_command	*parse_token_loop(t_token *tokens);
t_token		*handle_redirection(t_token *current, t_command *cmd);
void		add_redir(t_redir **redirs, t_redir *new_redir);
void		free_commands(t_command *cmds);
t_redir		*create_redir(t_token_type type, char *file);
void		free_args(char **args);

t_command	*get_commands_from_tokens(t_token *tokens);

char		*expand_env_vars(char *str, t_shell *shell);
char		*handle_exit_status(char *result, int *j, int *i, t_shell *shell);
char		*handle_env_var(char *str, int *i, t_expansion_out *out,
				t_shell *shell);
char		*handle_dollar_sign(char *str, int *i, t_expansion_out *out,
				t_shell *shell);
void		expand_redirections(t_command *cmd, t_shell *shell);
void		expand_args(t_command *cmd, t_shell *shell);

void		print_error(char *cmd, char *arg, char *msg);
void		print_syntax_error(char *token);
void		print_command_not_found(char *cmd);
void		print_permission_denied(char *path);
void		print_no_such_file(char *path);
void		print_is_directory(char *path);
void		print_too_many_arguments_error(char *cmd);

void		out_redirects(char *input, int *i, t_token **tokens);
void		in_redirects(char *input, int *i, t_token **tokens);
void		pipes(int *i, t_token **tokens);
void		skip_whitespace(char *input, int *i);
int			is_whitespace(char c);

int			is_valid_env_name(const char *name);
void		print_export_error(char *arg);
void		print_exported_vars(t_env *env);
char		*extract_key(char *arg);

void		*gc_malloc(t_shell *shell, size_t size, t_gc_type type);
void		*gc_get_static_ptr(t_shell *shell, t_gc_type type);
void		gc_set_static_ptr(t_shell *shell, void *ptr, t_gc_type type);
void		gc_free_type(t_shell *shell, t_gc_type type);
void		gc_free_all(t_shell *shell);
char		*gc_strdup(t_shell *shell, const char *s, t_gc_type type);
char		*gc_strjoin(t_shell *shell, const char *s1, const char *s2,
				t_gc_type type);
char		**gc_malloc_array(t_shell *shell, size_t count, t_gc_type type);

void		print_morpheus_quote(void);
void		print_matrix_ascii(void);
void		matrix_effect(void);
char		*strip_quotes(char *str);
//main_utils.c
void		setup_terminal(void);
void		handle_signal_interrupt(t_shell *shell);

//path_utils.c
char		*search_in_path(char *cmd, char *path_env);
char		*build_exec_path(char *dir, char *cmd);
int			is_absolute_or_relative_path(char *cmd);
char		*check_relative_path(char *cmd);
char		*check_absolute_path(char *cmd);

void		add_command(t_command **cmds, t_command *new_cmd);
t_command	*create_command(void);
int			handle_pipe_error(t_command *current_cmd, t_token *current_token,
				t_command *commands);
int			is_redirection(t_token_type type);
void		add_arg(t_command *cmd, char *arg);
int			handle_redir_error(t_token *current_token, t_command *commands,
				t_command *current_cmd);
void		add_redir_to_command(t_command *cmd, t_redir *new_redir);
char		*handle_multiple_heredocs(t_redir *heredocs);
int			init_heredoc_state(t_heredoc_state *state, t_redir *heredocs);
int			process_heredoc_loop(t_redir *heredocs, int heredoc_count,
				char **heredoc_content);
char		*finalize_heredoc(int stdin_copy, char *heredoc_content);
int			apply_heredoc_redirections(t_redir *redirs);
int			apply_file_redirections(t_redir *redirs);

/* pipe_utils.c fonksiyonları */
int			has_heredoc(t_command *cmd);
int			handle_heredoc_for_pipe(t_command *cmd, int *pipe_fd);
void		setup_child_pipes(int prev_pipe_read,
				int *pipe_fd, t_command *current);
void		setup_child_pipes_with_heredoc(int prev_pipe_read, int *pipe_fd,
				t_command *current, int *heredoc_pipe_fd);
void		close_heredoc_fds(int *heredoc_pipe_fd);

/* pipe_handlers.c fonksiyonları */
void		execute_child_process(t_command *current,
				t_shell *shell, char *path);
void		execute_child_process_with_heredoc(t_command *current,
				t_shell *shell, char *path);
int			handle_empty_command(t_command *current,
				int *pipe_fd, int *prev_pipe_read);
int			handle_builtin_single_command(t_command *current,
				t_shell *shell, char *path);
int			handle_command_not_found(t_command *current,
				t_shell *shell, int *pipe_fd, int prev_pipe_read);

/* pipes_command_processing.c fonksiyonları */
int			process_single_command(t_command *cur, t_shell *shell,
				t_pipe_data *data);

/* pipes_heredoc_special.c fonksiyonları */
int			handle_heredoc_and_empty_cmd(t_command *current,
				int *heredoc_pipe_fd, int *pipe_fd, int *prev_pipe_read);
int			handle_cmd_not_found_and_heredoc(t_command *current,
				t_shell *shell, int *heredoc_pipe_fd, int *pipe_fd);
int			process_heredoc_command(t_command *cur, int *heredoc_pipe_fd,
				int *pipe_fd, int *prev_pipe_read);

/* pipes.c fonksiyonları */
void		handle_child_process(t_command *cur, t_shell *shell,
				char *path, t_pipe_data *data);
void		cleanup_after_fork(t_command *cur, t_pipe_data *data, char *path);
/*expander_utils_2-3.c fonksiyonlar*/
void		expand_args_in_place(char **args, t_shell *shell);
int			count_non_empty(char **args);
char		**build_filtered_args(char **args, int count);
char		*add_char_to_result(char *result, char c);
char		*ft_strjoin_free(char *s1, char *s2);
char		*expand_process_loop(char *str, t_expand_ctx *ctx);
void		init_expand_ctx(t_expand_ctx *ctx, t_shell *shell, char *result);
char		*handle_variable_expansion(char *str, int *i, t_shell *shell);

/*executor_utils.c fonksiyonları*/
int			check_executable_path(t_command *cmd, t_shell *shell);
int			handle_redirections_builtin(t_command *cmd, t_shell *shell);
#endif
