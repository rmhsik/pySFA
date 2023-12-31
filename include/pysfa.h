#ifndef PYSFA_H
#define PYSFA_H

#include <complex>
#define cdouble std::complex<double>
#define I std::complex<double>(0.0,1.0)

extern "C" void pySFA(double Ip, double Z, double n_prin, double *efield, double *t, const int nt, const int nthreads, double *dipole);

extern "C" void pySFA3D(double Ip, double Z, double n_prin, double *efield_x, double *efield_y, double *efield_z, double *t, const int nt, const int nthreads, double *dipole_x, double *dipole_y, double *dipole_z);
#endif
