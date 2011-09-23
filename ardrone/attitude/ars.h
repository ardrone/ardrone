/***********************************************************************
 *                                                                     *
 * This file contains the code for the kalman filter that uses the     *
 * sensor data as inputs.                                              *
 *                                                                     *
 ***********************************************************************
 *                                                                     * 
 *    Author:         Tom Pycke                                        *
 *    Filename:       ars.h                                            *
 *    Date:           17/10/2007                                       *
 *    File Version:   1.00                                             *
 *                                                                     *
 ***********************************************************************/
/* 
 * Attitude reference system (no heading for a ahrs ;-) )
 */

struct ars_Gyro1DKalman
{
	/* These variables represent our state matrix x */
	double x_angle,
	      x_bias;

	/* Our error covariance matrix */
	double P_00,
	      P_01,
	      P_10,
	      P_11;	
	
	/* 
	 * Q is a 2x2 matrix of the covariance. Because we
	 * assume the gyro and accelero noise to be independend
	 * of eachother, the covariances on the / diagonal are 0.
	 *
	 * Covariance Q, the process noise, from the assumption
	 *    x = F x + B u + w
	 * with w having a normal distribution with covariance Q.
	 * (covariance = E[ (X - E[X])*(X - E[X])' ]
	 * We assume is linair with dt
	 */
	double Q_angle, Q_gyro;
	/*
	 * Covariance R, our observation noise (from the accelerometer)
	 * Also assumed to be linair with dt
	 */
	double R_angle;
};

// Initializing the struct
void ars_Init(struct ars_Gyro1DKalman *filterdata, double Q_angle, double Q_gyro, double R_angle);
// Kalman predict
void ars_predict(struct ars_Gyro1DKalman *filterdata, const double gyro, const double dt);
// Kalman update
double ars_update(struct ars_Gyro1DKalman *filterdata, const double angle_m);

