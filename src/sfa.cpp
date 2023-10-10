#include "sfa.h"
#include "utils.h"
#include <iostream>
#include <cmath>
#include <omp.h>

extern "C"{
    void SFA(double Ip, double Z, double n_prin, double *efield, double *t, const int nt, const int nthreads, double *dipole){
        double dt = t[1] -t[0];
        double *afield;
        double maxE0 = 0.0;
        afield = new double[nt];
        for(int i=0; i<nt;i++){
            afield[i] = 0.0;
        }
        omp_set_num_threads(nthreads);
        maxE0 = max_efield(efield, nt);
        calc_vecpot(efield, t, nt, afield);
        #pragma omp parallel for schedule(dynamic)
        for(int i=0; i<nt; i++){
            cdouble dipole_i = 0.0;
            cdouble tmp_value = 0.0;
            double action   = 0.0;
            double momentum = 0.0;
            cdouble matrix_dipole_ion = 0.0;
            cdouble matrix_dipole_rec = 0.0;
            for(int j=0; j<i; j++){
                tmp_value = 0.0;
                momentum = calc_momentum(afield, j, i, t);
                action = calc_action(afield, momentum, Ip, j, i, t);
                matrix_dipole_rec = calc_matrix_element(momentum+afield[i], Z, n_prin, maxE0, Ip);
                matrix_dipole_ion = calc_matrix_element(momentum+afield[j], Z, n_prin, maxE0, Ip);
                tmp_value  = pow(2.0*M_PI/( 0.00001 + I*(t[i]-t[j]) ), 1.5);
                tmp_value *= efield[j];
                tmp_value *= exp(-I*action);
                dipole_i += tmp_value*matrix_dipole_ion*std::conj(matrix_dipole_rec);
            }
            dipole_i *= I*dt;
            dipole[i] = dipole_i.real();
        }
        delete [] afield;
    }

    void SFA3D(double Ip, double Z, double n_prin, double *efield_x, double *efield_y, double *efield_z, double *t, const int nt, const int nthreads, double *dipole_x, double *dipole_y, double *dipole_z){
        double dt = t[1] - t[0];
        double *afield_x;
        double *afield_y;
        double *afield_z;
        double maxE0 = 0.0;
        double maxE0x = 0.0;
        double maxE0y = 0.0;
        double maxE0z = 0.0;
        afield_x = new double[nt];
        afield_y = new double[nt];
        afield_z = new double[nt];
        for(int i=0; i<nt; i++){
            afield_x[i] = 0.0;
            afield_y[i] = 0.0;
            afield_z[i] = 0.0;
        }
        maxE0x = max_efield(efield_x, nt);
        maxE0y = max_efield(efield_y, nt);
        maxE0z = max_efield(efield_z, nt);
        maxE0 = maxE0z;
        if (maxE0x>maxE0){
            maxE0 = maxE0x;
        }
        if (maxE0y>maxE0){
            maxE0 = maxE0y;
        }
        omp_set_num_threads(nthreads);
        calc_vecpot(efield_x, t, nt, afield_x);
        calc_vecpot(efield_y, t, nt, afield_y);
        calc_vecpot(efield_z, t, nt, afield_z);
        #pragma omp parallel for schedule(dynamic)
        for(int i=0; i<nt; i++){
            cdouble tmp_dipole_x = 0.0;
            cdouble tmp_dipole_y = 0.0;
            cdouble tmp_dipole_z = 0.0;
            cdouble tmp_value_x = 0.0;
            cdouble tmp_value_y = 0.0;
            cdouble tmp_value_z = 0.0;
            cdouble prefactor = 0.0;
            double action = 0.0;
            double *momentum = new double[3];
            cdouble *matrix_dipole_ion;
            cdouble *matrix_dipole_rec;
            for(int j=0; j<i; j++){
                tmp_value_x = 0.0;
                tmp_value_y = 0.0;
                tmp_value_z = 0.0;
                momentum = calc_momentum3D(afield_x, afield_y, afield_z, j, i, t);
                action = calc_action3D(afield_x, afield_y, afield_z, momentum, Ip, j, i, t);
                matrix_dipole_rec = calc_matrix_element3D(momentum, afield_x[i], 
                                                                    afield_y[i], 
                                                                    afield_z[i], Z, n_prin, maxE0, Ip);
                matrix_dipole_ion = calc_matrix_element3D(momentum, afield_x[j],
                                                                    afield_y[j],
                                                                    afield_z[j], Z, n_prin, maxE0, Ip);
                prefactor = pow(2.0*M_PI/(0.00001 + I*(t[i]-t[j])),1.5);
                tmp_value_x = prefactor;
                tmp_value_y = prefactor;
                tmp_value_z = prefactor;
                tmp_value_x *= efield_x[j];
                tmp_value_y *= efield_y[j];
                tmp_value_z *= efield_z[j];
                tmp_value_x *= exp(-I*action);
                tmp_value_y *= exp(-I*action);
                tmp_value_z *= exp(-I*action);
                tmp_dipole_x += tmp_value_x*matrix_dipole_ion[0]*std::conj(matrix_dipole_rec[0]);
                tmp_dipole_y += tmp_value_y*matrix_dipole_ion[1]*std::conj(matrix_dipole_rec[1]);
                tmp_dipole_z += tmp_value_z*matrix_dipole_ion[2]*std::conj(matrix_dipole_rec[2]);
            }
            tmp_dipole_x *= I*dt;
            tmp_dipole_y *= I*dt;
            tmp_dipole_z *= I*dt;
            dipole_x[i] = tmp_dipole_x.real();
            dipole_y[i] = tmp_dipole_y.real();
            dipole_z[i] = tmp_dipole_z.real();
        }
        delete[] afield_x;
        delete[] afield_y;
        delete[] afield_z;
    }
}
