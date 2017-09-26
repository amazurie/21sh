/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amazurie <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/09/19 15:35:33 by amazurie          #+#    #+#             */
/*   Updated: 2017/09/20 17:55:42 by amazurie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

static int	nbr_perline(int maxlen, int w)
{
	int	i;

	i = 1;
	while (i * maxlen < w)
		i++;
	return ((--i > 0) ? i : 1);
}

static int	nbr_percol(int nbrargs, int nbrperline)
{
	int	i;

	i = 1;
	while (i * nbrperline < nbrargs)
		i++;
	return (i);
}

static int	maxrow_line(int lenline, int w)
{
	int	i;

	i = 1;
	while (i * w < lenline)
		i++;
	return (i);
}

int			*get_size(t_compl *compl, t_cmd *cmd)
{
	struct winsize	w;
	int				*size;

	if (!(size = (int*)ft_memalloc(sizeof(int) * 7)))
		return (NULL);
	ioctl(0, TIOCGWINSZ, &w);
	size[0] = w.ws_col;
	size[2] = maxrow_line(ft_strlen(cmd->str) + ft_strlen(cmd->prompt)
			+ compl->maxlen, w.ws_col);
	size[1] = w.ws_row - size[2];
	size[3] = nbr_perline(compl->maxlen, w.ws_col);
	size[4] = nbr_percol(compl->nbrargs, size[3]);
	size[5] = 0;
	size[6] = 0;
	return (size);
}

static void	print_args(t_compl *compl, int *size)
{
	t_coargs	*ar;
	int			i;

	ar = &compl->args;
	i = compl->toskip;
	while (ar && i-- > 0)
		ar = ar->next;
	while (ar)
	{
		ft_putstr(tgetstr("do", NULL));
		i = compl->maxlen * size[6];
		while (i--)
			ft_putstr(tgetstr("nd", NULL));
		if (ar->id == compl->curr)
			ft_putstr(tgetstr("mr", NULL));
		ft_putstr(ar->arg);
		i = 0;
		while ((size_t)++i < compl->maxlen - ft_strlen(ar->arg))
			ft_putchar(' ');
		ft_putstr(tgetstr("me", NULL));
		ft_putchar(' ');
		ar = ar->next;
		if (++size[5] == size[4] || size[5] == size[1])
		{
			size[6]++;
			while (size[5]-- > 0)
				tputs(tgetstr("up", NULL), 1, tputchar);
			i = compl->toskip + (size[4] - (compl->toskip + size[1]));
			while (ar && i-- > 0)
				ar = ar->next;
			size[5] = 0;
		}
	}
}

static void	print_complline(t_compl *compl, t_cmd *cmd, int *size)
{
	char	*tmp;
	int		len;

	choose_prompt(cmd);
	len = 0;
	if ((tmp = ft_strndup(cmd->str, cmd->col - 1 - cmd->prlen)))
	{
		ft_putstr(tmp);
		len = cmd->prlen + ft_strlen(tmp);
		len += len % size[0] == 0 ? 0 : 1;
		if (len % size[0] == 0)
			tputs(tgetstr("cr", NULL), 1, tputchar);
		if (len % size[0] == 0)
			tputs(tgetstr("do", NULL), 1, tputchar);
		len = cmd->prlen + ft_strlen(tmp);
		free(tmp);
	}
	if (compl->ar && compl->ar->arg
			&& (tmp = ft_strdup(compl->ar->arg + ft_strlen(compl->arg))))
	{
		ft_putstr(tmp);
		len += ft_strlen(tmp);
		len += len % size[0] == 0 ? 0 : 1;
		if (len % size[0] == 0)
			tputs(tgetstr("cr", NULL), 1, tputchar);
		if (len % size[0] == 0)
			tputs(tgetstr("do", NULL), 1, tputchar);
		free(tmp);
	}
	tputs(tgetstr("sc", NULL), 1, tputchar);
	ft_putstr(cmd->str + cmd->col - 1 - cmd->prlen);
	tputs(tgetstr("rc", NULL), 1, tputchar);
}

void		get_curr(t_compl *compl)
{
	t_coargs *ar;

	if (compl->curr < 0)
		return ;
	ar = &compl->args;
	while (ar)
	{
		if (compl->curr == ar->id)
		{
			compl->ar = ar;
			return ;
		}
		ar = ar->next;
	}
}

void		display_args(t_compl *compl, t_cmd *cmd)
{
	int				*size;

	if (!(size = get_size(compl, cmd)))
		return ;
	size[6] = (compl->ar) ? ft_strlen(compl->ar->arg) + ft_strlen(compl->arg) : 0;
	go_begin(cmd->col + size[6], cmd->sc_col);
	get_curr(compl);
	tputs(tgetstr("cd", NULL), 1, tputchar);
	if ((size_t)size[0] < compl->maxlen)
		print_complline(compl, cmd, size);
	if ((size_t)size[0] < compl->maxlen)
		return ;
//	ft_putstr_fd(tgetstr("vi", NULL), 0);
	size[6] = size[2];
	while (--size[6])
		tputs(tgetstr("do", NULL), 1, tputchar);
	size[6] = 0;
	print_args(compl, size);
	size[5] += size[2];
	while (--size[5])
		tputs(tgetstr("up", NULL), 1, tputchar);
	size[6] = compl->maxlen * size[3];
	while (size[6]--)
		ft_putstr(tgetstr("le", NULL));
	print_complline(compl, cmd, size);
	ft_putstr_fd(tgetstr("ve", NULL), 0);
}
