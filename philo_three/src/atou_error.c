/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atou_error.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 21:03:23 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/14 22:53:07 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_three.h"

static int	ft_isdigit(int c)
{
	if (c < '0' || c > '9')
		return (FALSE);
	return (TRUE);
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
	unsigned	digit;

	n = 0;
	while (ft_isdigit(*str))
	{
		*error = is_mult_overflow(n, 10);
		if (*error == TRUE)
			return (n);
		n *= 10;
		digit = *str - '0';
		*error = is_add_overflow(n, digit);
		if (*error == TRUE)
			return (n);
		n += digit;
		str++;
	}
	if (*str != '\0')
		*error = TRUE;
	return (n);
}
