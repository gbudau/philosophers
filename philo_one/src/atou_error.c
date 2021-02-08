/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atou_error.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 21:03:23 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/08 20:43:07 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_one.h"

static int	ft_isdigit(int c)
{
	if (c < '0' || c > '9')
		return (0);
	return (1);
}

static int	is_mult_overflow(unsigned a, unsigned b)
{
	if (a > UINT_MAX / b)
		return (TRUE);
	return (FALSE);
}

static int	is_add_overflow(unsigned a, unsigned b)
{
	if (a > UINT_MAX - b)
		return (TRUE);
	return (FALSE);
}

unsigned	atou_error(const char *str, int *error)
{
	unsigned	n;

	n = 0;
	while (ft_isdigit(*str))
	{
		*error = is_mult_overflow(n, 10);
		if (*error == TRUE)
			break ;
		n = n * 10;
		*error = is_add_overflow(n, *str - '0');
		if (*error == TRUE)
			break ;
		n = n + *str - '0';
		str++;
	}
	if (*str != '\0')
		*error = TRUE;
	return (n);
}
