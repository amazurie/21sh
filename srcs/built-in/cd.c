/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amazurie <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/28 15:13:41 by amazurie          #+#    #+#             */
/*   Updated: 2017/10/09 16:55:10 by amazurie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static void	change_pwd(char *path, char ***env, char opt)
{
	char	*tmp;

	if (!ft_strncmp(path, "~/", 2))
	{
		if (!(tmp = ft_strjoin(get_elem(env, "HOME="), (path + 1))))
			return ;
		path = tmp;
		free(tmp);
	}
	if (!(tmp = check_path(path, env, opt)))
		return ;
	change_envpwd(tmp, env);
	free(tmp);
}

static void	check_pwd2(char *path, char ***env, char *rep, char *tmp)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = ft_strschr_len(tmp, path);
	while (tmp[j] == path[i++])
		ssupprchr(&tmp, j);
	i = 0;
	while (rep[i])
		saddchr(&tmp, rep[i++], j++);
	if (ft_strcmp(tmp, get_elem(env, "PWD=")) != 0)
	{
		ft_putstr_fd("cd: no such file or directory: ", 2);
		ft_putstr_fd(tmp, 2);
		ft_putchar_fd('\n', 2);
	}
	else
	{
		i = ft_strlen(get_elem(env, "HOME="));
		while (tmp[0] && i-- > 0)
			ssupprchr(&tmp, 0);
		ft_putchar('~');
		ft_putstr(tmp);
		ft_putchar('\n');
	}
}

static int	check_pwd(char *path, char ***env, char *rep)
{
	char	*tmp;
	char	*tmp2;

	if (!get_elem(env, "HOME="))
	{
		ft_putstr_fd("cd: HOME not set.\n", 2);
		return (0);
	}
	else if (!(tmp = ft_strdup(get_elem(env, "PWD"))))
		ft_putstr_fd("cd: PWD not set.\n", 2);
	else if ((tmp2 = ft_strschr(tmp, path)) == 0)
	{
		ft_putstr_fd("cd: string not in pwd: ", 2);
		ft_putstr_fd(path, 2);
		ft_putchar_fd('\n', 2);
	}
	else
		check_pwd2(path, env, rep, tmp);
	if (tmp)
		free(tmp);
	return (0);
}

static char	*cd2(char **path, char ***env, int i)
{
	char	*tmp;

	tmp = NULL;
	if (!path[i] || !ft_strcmp(path[i], "~") || (path[i][0] == '~'
			&& path[i][1] == '/' && !path[i][2])
			|| (path[i][0] == '-' && path[i][1] == '-'))
	{
		if (!get_elem(env, "HOME="))
			ft_putstr_fd("cd: HOME not set.\n", 2);
		else
			tmp = ft_strdup(get_elem(env, "HOME="));
	}
	else if (ft_strcmp(path[1], "-") == 0)
	{
		if (!get_elem(env, "OLDPWD="))
		{
			ft_putstr_fd("cd: OLDPWD not set.\n", 2);
			return (NULL);
		}
		else
			tmp = ft_strdup(get_elem(env, "OLDPWD="));
	}
	else
		tmp = ft_strdup(path[i]);
	return (tmp);
}

int			cd(char **path, char ***env)
{
	char	*tmp;
	char	opt;
	int		i;
	int		j;

	i = 0;
	tmp = NULL;
	if (!path || !path[0])
		return (1);
	opt = 0;
	j = 1;
	while (j > 0 && path[++i] && path[i][0] == '-')
	{
		j = 1;
		while (path[i][j] && (path[i][j] == 'P' || path[i][j] == 'L'))
			j++;
		opt = path[i][j - 1];
		if (j == 1 || (path[i][j] && path[i][j] != 'P' && path[i][j] != 'L'))
		{
			i--;
			j = -1;
			opt = 0;
		}
	}
	(i < 1) ? i = 1 : 0;
	j = 0;
	while (path[i + j])
		j++;
	if (j > 2)
		ft_putstr_fd("cd: too many arguments\n", 2);
	else if (j == 2)
		return (check_pwd(path[i], env, path[i + 1]));
	else
	{
		if (!(tmp = cd2(path, env, i)))
			return (-1);
		if (tmp)
			change_pwd(tmp, env, opt);
		free(tmp);
	}
	return (0);
}
