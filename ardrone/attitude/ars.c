/***********************************************************************
 *                                                                     *
 * This file contains the code for the kalman filter that uses the     *
 * sensor data as inputs.                                              *
 *                                                                     *
 ***********************************************************************
 *                                                                     * 
 *    Author:         Tom Pycke                                        *
 *    Filename:       ars.c                                            *
 *    Date:           17/10/2007                                       *
 *    File Version:   1.00                                             *
 *                                                                     *
 ***********************************************************************
 *                                                                     *
 * Comments:                                                           *
 *   To help others to understand the kalman filter, I used one of     *
 *   the most accessible sources with information on it:               *
 *   http://en.wikipedia.org/wiki/Kalman_filter                        *
 *   The code is split into 2 parts: a Predict function and an         *
 *   Update function, just as the wikipedia page does.                 *
 *   The model I used in the kalman filter is the following:           *
 *   Our gyroscope measures the turnrate in degrees per second. This   *
 *   is the derivative of the angle, called dotAngle. The bias is the  *
 *   output of our gyro when the rotationrate is 0 degrees per second  *
 *   (not rotating). Because of drift it changes over time.            *
 *   So mathematically we just integrate the gyro (dt is timespan      *
 *   since last integration):                                          *
 *              angle = angle + (dotAngle - bias) * dt                 *
 *   When we include the bias in our model, the kalman filter will     *
 *   try to estimate the bias, thanks to our last input: the measured  *
 *   angle. This is just an estimate and comes from the accelerometer. *
 *   So the state used in our filter had 2 dimensions: [ angle, bias ] *
 *   Jump to the functions to read more about the actual               *
 *   implementation.                                                   *
 *                                                                     *
 ***********************************************************************/

#include "ars.h"

#include <math.h>


void ars_Init(struct ars_Gyro1DKalman *ars, double Q_angle, double Q_gyro, double R_angle)
{
	ars->Q_angle = Q_angle;
	ars->Q_gyro  = Q_gyro;
	ars->R_angle = R_angle;
}

/*
 * The predict function. Updates 2 variables:
 * our model-state x and the 2x2 matrix P
 *     
 * x = [ angle, bias ]' 
 * 
 *   = F x + B u
 *
 *   = [ 1 -dt, 0 1 ] [ angle, bias ] + [ dt, 0 ] [ dotAngle 0 ]
 *
 *   => angle = angle + dt (dotAngle - bias)
 *      bias  = bias
 *
 *
 * P = F P transpose(F) + Q
 *
 *   = [ 1 -dt, 0 1 ] * P * [ 1 0, -dt 1 ] + Q
 *
 *  P(0,0) = P(0,0) - dt * ( P(1,0) + P(0,1) ) + dt * P(1,1) + Q(0,0)
 *  P(0,1) = P(0,1) - dt * P(1,1) + Q(0,1)
 *  P(1,0) = P(1,0) - dt * P(1,1) + Q(1,0)
 *  P(1,1) = P(1,1) + Q(1,1)
 *
 *
 */
void ars_predict(struct ars_Gyro1DKalman *ars, const double dotAngle, const double dt)
{
	ars->x_angle += dt * (dotAngle - ars->x_bias);

	ars->P_00 +=  - dt * (ars->P_10 + ars->P_01) + ars->Q_angle * dt;
	ars->P_01 +=  - dt * ars->P_11;
	ars->P_10 +=  - dt * ars->P_11;
	ars->P_11 +=  + ars->Q_gyro * dt;
}

/*
 *  The update function updates our model using 
 *  the information from a 2nd measurement.
 *  Input angle_m is the angle measured by the accelerometer.
 *
 *  y = z - H x
 *
 *  S = H P transpose(H) + R
 *    = [ 1 0 ] P [ 1, 0 ] + R
 *    = P(0,0) + R
 * 
 *  K = P transpose(H) S^-1
 *    = [ P(0,0), P(1,0) ] / S
 *
 *  x = x + K y
 *
 *  P = (I - K H) P
 *
 *    = ( [ 1 0,    [ K(0),
 *          0 1 ] -   K(1) ] * [ 1 0 ] ) P
 *
 *    = [ P(0,0)-P(0,0)*K(0)  P(0,1)-P(0,1)*K(0),
 *        P(1,0)-P(0,0)*K(1)  P(1,1)-P(0,1)*K(1) ]
 */
double ars_update(struct ars_Gyro1DKalman *ars, const double angle_m)
{
	const double y = angle_m - ars->x_angle;
	
	const double S = ars->P_00 + ars->R_angle;
	const double K_0 = ars->P_00 / S;
	const double K_1 = ars->P_10 / S;
	
	ars->x_angle +=  K_0 * y;
	ars->x_bias  +=  K_1 * y;
	
	ars->P_00 -= K_0 * ars->P_00;
	ars->P_01 -= K_0 * ars->P_01;
	ars->P_10 -= K_1 * ars->P_00;
	ars->P_11 -= K_1 * ars->P_01;
	
	return ars->x_angle;
}
