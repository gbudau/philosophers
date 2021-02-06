/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 20:55:15 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/06 19:50:33 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_one.h"
#include <stdio.h>

/*
typedef struct	s_philo_one
{
	int		n_philos;
	int		n_forks;

}				t_philo_one;
*/

int check_args_n(int argc)
{
	if (argc < 5 || argc > 6)
	{
		ft_putstr_fd("Usage ./philo_one number_of_philosophers time_to_die "
				"time_to_eat time_to_sleep "
				"[number_of_times_each_philosopher_must_eat]\n", STDERR_FILENO);
		return (-1);
	}
	return (0);
}

int check_valid_args(char **argv)
{
	size_t	i;
	int		error;
	int		n;

	i = 0;
	while (argv[i])
	{
		error = TRUE;
		n = atoi_error(argv[i], &error);
		if (error)
		{
			ft_putstr_fd("Invalid argument ", STDERR_FILENO);
			ft_putstr_fd(argv[i], STDERR_FILENO);
			ft_putstr_fd("\n", STDERR_FILENO);
			return (-1);
		}
		printf("%d\n", n);
		i++;
	}
	return (0);
}

int	main(int argc, char **argv)
{
	if (check_args_n(argc) == -1)
		return (1);
	if (check_valid_args(argv + 1) == -1)
		return (1);
	return (0);
}
