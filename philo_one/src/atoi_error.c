/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atoi_error.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbudau <gbudau@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/16 21:03:23 by gbudau            #+#    #+#             */
/*   Updated: 2021/02/06 19:31:57 by gbudau           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_one.h"

static int	ft_isdigit(int c)
{
	if (c < '0' || c > '9')
		return (0);
	return (1);
}

static int	is_mult_overflow(unsigned int a, unsigned int b, int sign)
{
	if (sign > 0 && a > ((INT_MAX + 0U) / b))
		return (TRUE);
	else if (sign < 0 && a > ((INT_MAX + 1U) / b))
		return (TRUE);
	return (FALSE);
}

static int	is_add_overflow(unsigned int a, unsigned int b, int sign)
{
	if (sign > 0 && a > (INT_MAX + 0U - b))
		return (TRUE);
	else if (sign < 0 && a > (INT_MAX + 1U - b))
		return (TRUE);
	return (FALSE);
}

int			atoi_error(const char *str, int *error)
{
	unsigned int	n;
	int				sign;

	sign = 1;
	if (*str == '-' || *str == '+')
		sign = (*str++ == '-') ? -1 : sign;
	n = 0;
	while (ft_isdigit(*str))
	{
		*error = is_mult_overflow(n, 10, sign);
		if (*error == TRUE)
			break ;
		n = n * 10;
		*error = is_add_overflow(n, *str - '0', sign);
		if (*error == TRUE)
			break ;
		n = n + *str - '0';
		str++;
	}
	if (*str != '\0')
		*error = TRUE;
	return ((int)n * sign);
}
