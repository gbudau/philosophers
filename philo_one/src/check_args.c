/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/15 15:43:16 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/15 16:02:20 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_one.h"

static int check_args_n(int argc, char *str)
{
	if (argc < 5 || argc > 6)
	{
		ft_putstr_fd("Usage: ", STDERR_FILENO);
		ft_putstr_fd(str, STDERR_FILENO);
		ft_putstr_fd("number_of_philosophers time_to_die "
				"time_to_eat time_to_sleep "
				"[number_of_times_each_philosopher_must_eat]\n", STDERR_FILENO);
		return (-1);
	}
	return (0);
}

static int	fill_var(unsigned n, t_args *args, size_t i)
{
	if (i == 0)
		if (n < 2)
		{
			ft_putstr_fd("Atleast two philosophers are required\n", STDERR_FILENO);
			return (-1);
		}
		else
			args->n_philos = n;
	else if (i == 1)
		args->time_to_die = n;
	else if (i == 2)
		args->time_to_eat = n;
	else if (i == 3)
		args->time_to_sleep = n;
	else if (i == 4)
	{
		if (n == 0)
		{
			ft_putstr_fd("The number of times each philosopher must eat "
					"has to be a positive number\n", STDERR_FILENO);
			return (-1);
		}
		else
			args->n_times_to_eat = n;
	}
	return (0);
}

static int check_valid_args(char **argv, t_args *args)
{
	size_t		i;
	int			error;
	unsigned	n;

	i = 0;
	while (argv[i])
	{
		error = TRUE;
		n = atou_error(argv[i], &error);
		if (error || fill_var(n, args, i) == -1)
		{
			ft_putstr_fd("Invalid argument: ", STDERR_FILENO);
			ft_putstr_fd(argv[i], STDERR_FILENO);
			ft_putstr_fd("\n", STDERR_FILENO);
			return (-1);
		}
		i++;
	}
	return (0);
}

int	check_args(int argc, char **argv, t_args *args)
{
	if (check_args_n(argc, argv[0]) == -1)
		return (-1);
	if (argc == 6)
		args->limit_times_to_eat = TRUE;
	if (check_valid_args(argv + 1, args) == -1)
		return (-1);
	if (args->limit_times_to_eat && !args->n_times_to_eat)
		return (-1);
	return (0);
}
