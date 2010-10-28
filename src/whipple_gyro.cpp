#include "whipple_gyro.h"
int eomwrapper(double t, const double x[10], double f[10], void * params)
{
  Whipple * p = (Whipple *) params;
  // Assign the states of the Whipple object
  p->setState(x);
  // Evaluate the RHS of the ODE's representing the equations of motion
  p->eoms();
  // Assign the right hand sides of the ODE's to the array passed
  f[0] = p->q0p;
  f[1] = p->q1p;
  f[2] = p->q3p;
  f[3] = p->q4p;
  f[4] = p->q5p;
  f[5] = p->q6p;
  f[6] = p->q7p;
  f[7] = p->u1p;
  f[8] = p->u3p;
  f[9] = p->u5p;
  // Return the status
  return GSL_SUCCESS;
} // eomwrapper()

double hc_f(double q2, void * params)
{
  Whipple * p = (Whipple *) params;
  return
      p->rft + p->ls*cos(p->q1)*cos(q2) +
      p->lf*(sin(p->q1)*sin(p->q3)-sin(q2)*cos(p->q1)*cos(p->q3)) +
      p->rf*sqrt((1-pow((sin(p->q1)*cos(p->q3)+sin(q2)*sin(p->q3)*cos(p->q1)),2))) -
      p->rrt - p->rr*cos(p->q1) - p->lr*sin(q2)*cos(p->q1);
} // hc_f()

double hc_df(double q2, void * params)
{
  Whipple * p = (Whipple *) params;
  return
      -cos(p->q1)*(p->lr*cos(q2)+p->ls*sin(q2)+p->lf*cos(q2)*cos(p->q3)+p->rf*sin(p->q3)*cos(q2)*(
                  sin(p->q1)*cos(p->q3)+sin(q2)*sin(p->q3)*cos(p->q1))/sqrt((1-pow((sin(p->q1)*cos(p->q3)+sin(q2)*
                              sin(p->q3)*cos(p->q1)),2))));
} // hc_df()

void hc_fdf(double q2, void * params, double * f, double * df)
{
  *f = hc_f(q2, params);
  *df =  hc_df(q2, params);
} // hc_fdf()

ostream &operator<<(ostream &file, const Whipple * discs)
{
  file.write((char *) &(discs->t), sizeof discs->t);
  file.write((char *) &discs->q0, sizeof discs->q0);
  file.write((char *) &discs->q1, sizeof discs->q1);
  file.write((char *) &discs->q2, sizeof discs->q2);
  file.write((char *) &discs->q3, sizeof discs->q3);
  file.write((char *) &discs->q4, sizeof discs->q4);
  file.write((char *) &discs->q5, sizeof discs->q5);
  file.write((char *) &discs->q6, sizeof discs->q6);
  file.write((char *) &discs->q7, sizeof discs->q7);
  file.write((char *) &discs->u0, sizeof discs->u0);
  file.write((char *) &discs->u1, sizeof discs->u1);
  file.write((char *) &discs->u2, sizeof discs->u2);
  file.write((char *) &discs->u3, sizeof discs->u3);
  file.write((char *) &discs->u4, sizeof discs->u4);
  file.write((char *) &discs->u5, sizeof discs->u5);
  file.write((char *) discs->no_fn, sizeof discs->no_fn);
  file.write((char *) &discs->Rx, sizeof discs->Rx);
  file.write((char *) &discs->Ry, sizeof discs->Ry);
  file.write((char *) &discs->Rz, sizeof discs->Rz);
  file.write((char *) &discs->Fx, sizeof discs->Fx);
  file.write((char *) &discs->Fy, sizeof discs->Fy);
  file.write((char *) &discs->Fz, sizeof discs->Fz);
  file.write((char *) &discs->ke, sizeof discs->ke);
  file.write((char *) &discs->pe, sizeof discs->pe);
  file.write((char *) &discs->fa_yaw, sizeof discs->fa_yaw);
  file.write((char *) &discs->fa_lean, sizeof discs->fa_lean);
  file.write((char *) &discs->fa_pitch, sizeof discs->fa_pitch);
  file.write((char *) discs->constraints, sizeof discs->constraints);
  return file;
} // ostream &operator<<()

void Whipple::writeRecord_dt(void) const
{
  ofstream fp("./record.py", ios::out);
  fp << "#!/usr/bin/env python" << endl;
  fp << "import numpy as np" << endl;
  fp << "record_dt = np.dtype([('t', np.float64), " <<
        "('q0', np.float64), " <<
        "('q1', np.float64), " <<
        "('q2', np.float64), " <<
        "('q3', np.float64), " <<
        "('q4', np.float64), " <<
        "('q5', np.float64), " <<
        "('q6', np.float64), " <<
        "('q7', np.float64), " <<
        "('u0', np.float64), " <<
        "('u1', np.float64), " <<
        "('u2', np.float64), " <<
        "('u3', np.float64), " <<
        "('u4', np.float64), " <<
        "('u5', np.float64)," <<
        "('fnx', np.float64)," <<
        "('fny', np.float64)," <<
        "('fnz', np.float64)," <<
        "('Rx', np.float64)," <<
        "('Ry', np.float64)," <<
        "('Rz', np.float64)," <<
        "('Fx', np.float64)," <<
        "('Fy', np.float64)," <<
        "('Fz', np.float64)," <<
        "('ke', np.float64)," <<
        "('pe', np.float64)," <<
        "('fa_yaw', np.float64)," <<
        "('fa_lean', np.float64)," <<
        "('fa_pitch', np.float64)," << 
        "('nh1', np.float64)," << 
        "('nh2', np.float64)," << 
        "('nh3', np.float64)])" << 
        endl;
  fp.close();
} // writeRecord_dt()

void Whipple::printState(void) const
{
  cout << "q0 = " << q0 << "(ignorable)" << endl
       << "q1 = " << q1 << endl
       << "q2 = " << q2 << "(dependent)" << endl
       << "q3 = " << q3 << endl
       << "q4 = " << q4 << "(ignorable)" << endl
       << "q5 = " << q5 << "(ignorable)" << endl
       << "q6 = " << q6 << "(ignorable)" << endl
       << "q7 = " << q7 << "(ignorable)" << endl
       << "u0 = " << u0 << "(dependent)" << endl
       << "u1 = " << u1 << endl
       << "u2 = " << u2 << "(dependent)" << endl
       << "u3 = " << u3 << endl
       << "u4 = " << u4 << "(dependent)" << endl
       << "u5 = " << u5 << endl;
} // printState()

void Whipple::printParameters(void) const
{
  cout << "rr   = " << rr << endl
       << "rrt  = " << rrt << endl
       << "rf   = " << rf << endl
       << "rft  = " << rft << endl
       << "lr   = " << lr << endl
       << "ls   = " << ls << endl
       << "lf   = " << lf << endl
       << "mr   = " << mr << endl
       << "mf   = " << mf << endl
       << "ICyy = " << ICyy << endl
       << "IDxx = " << IDxx << endl
       << "IDyy = " << IDyy << endl
       << "IDzz = " << IDzz << endl
       << "IDxz = " << IDxz << endl
       << "IExx = " << IExx << endl
       << "IEyy = " << IEyy << endl
       << "IEzz = " << IEzz << endl
       << "IExz = " << IExz << endl
       << "IFyy = " << IFyy << endl
       << "lrx  = " << lrx << endl
       << "lrz  = " << lrz << endl
       << "lfx  = " << lfx << endl
       << "lfz  = " << lfz << endl
       << "g    = " << g << endl;
} // printParameters()

Whipple::Whipple()
{
  // Setup the root finder and the numerical integrator
  initRootFinder();
  initODESolver();
  // Default parameters
  setBenchmarkParameters();
  // Constants
  // zero out all the z's
  for (int i = 0; i < Z_MAX; ++i)
    z[i] = 0.0;

  setBenchmarkState();
  // zero out the input torques
  Trw = Tfw = Ts = 0.0;

  // Initialize the eigenvalues and eigenvectors
  evals = gsl_vector_complex_alloc(4);
  evecs = gsl_matrix_complex_alloc(4, 4);
  m = gsl_matrix_alloc(4, 4);
  gsl_matrix_set_zero(m);
  m->data[2] = 1.0;
  m->data[7] = 1.0;
  w = gsl_eigen_nonsymmv_alloc(4);

  // Camera settings
  theta = M_PI / 4.0;
  phi = 0.0;
  d = 1.0;
  ctx = .35;
  cty = .35; 
  ctz = 0.0;
  
  // Write data to file to define numpy data type so that plotting is easy
  writeRecord_dt();
} // constructor

Whipple::~Whipple()
{
  gsl_root_fdfsolver_free(fdf_s); 
  gsl_odeiv_evolve_free(e);
  gsl_odeiv_control_free(c);
  gsl_odeiv_step_free(s);
  gsl_vector_complex_free(evals);
  gsl_matrix_complex_free(evecs);
  gsl_matrix_free(m);
  gsl_eigen_nonsymmv_free(w);
} // destructor

void Whipple::setParameters(WhippleParams * p)
{
  ICyy = p->ICyy;
  IDxx = p->IDxx;
  IDxz = p->IDxz;
  IDyy = p->IDyy;
  IDzz = p->IDzz;
  IExx = p->IExx;
  IExz = p->IExz;
  IEyy = p->IEyy;
  IEzz = p->IEzz;
  IFyy = p->IFyy;
  g = p->g;
  lf = p->lf;
  lfx = p->lfx;
  lfz = p->lfz;
  lr = p->lr;
  lrx = p->lrx;
  lrz = p->lrz;
  ls = p->ls;
  mr = p->mr;
  mf = p->mf;
  rf = p->rf;
  rft = p->rft;
  rr = p->rr;
  rrt = p->rrt;
} // setParameters()

void Whipple::setState(const double state[10])
{
  q0 = state[0];  // Yaw
  q1 = state[1];  // Lean
  q3 = state[2];  // Steer
  calcPitch();    // Solve holonomic constraint for pitch angle
  q4 = state[3];
  q5 = state[4];
  q6 = state[5];
  q7 = state[6];
  u1 = state[7];
  u3 = state[8];
  u5 = state[9];
} // setState()

// TODO: fix inertias and cm distances
void Whipple::setBenchmarkParameters(void)
{
  rr=0.3;
  rrt=0.0;
  rf=0.35;
  rft=0.0;
  lr=0.9534570696121847;
  ls=0.2676445084476887;
  lf=0.0320714267276193;
  mr=87.0;
  mf=7.0;
  ICyy=0.0603;
  IDxx=7.178169776497895;
  IDyy=11;
  IDzz=4.821830223502103;
  IDxz=3.822553593835788;
  IExx=0.05841337700152973;
  IEyy=0.06;
  IEzz=0.007586622998470264;
  IExz=0.009119225261946296;
  IFyy=0.28;
  lrx=0.4707271515135145;
  lrz=-0.477928811464608;
  lfx=-0.005970833924186833;
  lfz=-0.3699518200282974;
  g=9.81;
  evalConstants();
} // setBenchmarkParameters()

void Whipple::setBenchmarkState(void)
{
  q0 = q1 = q3 = q4 = q5 = q6 = q7 = 0.0;
  q2 = M_PI/10.0;
  u1 = 0.9;
  u3 = 0.0;
  u5 = -3.6/(rf+rft);
  eoms();
  computeOutputs();
} // setBenchmarkState

void Whipple::initRootFinder(void)
{
  // Newton-Raphson setup
  FDF.f = &hc_f;
  FDF.df = &hc_df;
  FDF.fdf = &hc_fdf;
  FDF.params = this;
  fdf_T = gsl_root_fdfsolver_newton;
  fdf_s = gsl_root_fdfsolver_alloc(fdf_T);
} // initRootFinder()

void Whipple::initODESolver(void)
{
  // Set integration settings
  t = 0.0;
  tf = 5.0;
  h = 0.001;
  fps = 100;
  T = gsl_odeiv_step_rk8pd;
  s = gsl_odeiv_step_alloc(T, 10);
  c = gsl_odeiv_control_y_new(1e-6, 1e-9);
  e = gsl_odeiv_evolve_alloc(10);
  sys.function = eomwrapper;
  sys.jacobian = NULL;
  sys.dimension = 10;
  sys.params = this;
} // initODESolver()

void Whipple::calcPitch(void)
{ 
  iter = 0;
  // Newton's method
  gsl_root_fdfsolver_set(fdf_s, &FDF, q2);
  do
  {
    gsl_root_fdfsolver_iterate(fdf_s);
    status = gsl_root_test_residual(
              hc_f(gsl_root_fdfsolver_root(fdf_s), this), 1e-16);
  } while(status == GSL_CONTINUE && ++iter < 100);
  q2 = gsl_root_fdfsolver_root(fdf_s);
} // calcPitch

void Whipple::calcEvals(void)
{
  m->data[0] = m->data[1] = m->data[3] = 
    m->data[4] = m->data[5] = m->data[6] = 0.0;
  m->data[2] = m->data[7] = 1.0;  // set the top two rows
  // Evaluate the EOMS
  eoms();
  // Evaluate the 10x10 A matrix
  computeOutputs();

  // Get the 4x4 sub matrix of the 10x10 A matrix
  m->data[8] = A[71];
  m->data[9] = A[72];
  m->data[10] = A[77];
  m->data[11] = A[78];
  m->data[12] = A[81];
  m->data[13] = A[82];
  m->data[14] = A[87];
  m->data[15] = A[88];

  // Get the eigenvalues
  gsl_eigen_nonsymmv(m, evals, evecs, w);
  getFourValues();
} // calcEvals()

void Whipple::getFourValues(void)
{
  int i;
  double valr, vali;
  if (evalCase() == 0) {
    for (i = 0; i < 4; ++i)
      fourValues[i] = GSL_REAL(gsl_vector_complex_get(evals, i));
    return;
  } else
    for (i = 0; i < 4; ++i) {
      vali = GSL_IMAG(gsl_vector_complex_get(evals, i)); 
      valr = GSL_REAL(gsl_vector_complex_get(evals, i)); 
      if (vali == 0.0) {
        fourValues[i] = valr;
        continue;
      }
      fourValues[i] = valr;
      fourValues[++i] = vali;
    }
} // getFourValues()

int Whipple::evalCase(void)
{
  int i, real = 0;
  for (i = 0; i < 4; ++i)
   if (GSL_IMAG(gsl_vector_complex_get(evals, i)) == 0.0)
     ++real;

  if (real == 4)
    return 0;
  if (real == 2)
    return 1;
  // Final case, real == 0
  return 0;
} // evalCase()

void Whipple::evolve(double tj, double * state)
{
  gsl_odeiv_evolve_apply(e, c, s, &sys, &t, tj, &h, state);
} // evolve()

void Whipple::evalConstants(void)
{
  z[40] = pow(lf,2) + pow(lr,2) + pow(ls,2) + pow(rr,2);
  z[41] = lf*lr;
  z[42] = lf*rr;
  z[43] = lr*rr;
  z[44] = ls*rr;
  z[291] = g*mr;
  z[292] = g*mf;
  z[496] = lrx*z[291];
  z[497] = lrz*z[291];
  z[572] = rrt*z[291];
} // evalConstants()

void Whipple::eoms(void)
{
  calcPitch();
  z[13] = cos(q2);
  z[14] = sin(q2);
  z[15] = cos(q3);
  z[16] = sin(q3);
  z[11] = cos(q1);
  z[24] = z[14]*z[16];
  z[12] = sin(q1);
  z[28] = z[11]*z[24] + z[12]*z[15];
  z[31] = 1 - pow(z[28],2);
  z[32] = pow(z[31],0.5);
  z[34] = z[28]/z[32];
  z[36] = rf*z[34];
  z[33] = 1/z[32];
  z[35] = -rft - rf*z[33];
  z[65] = ls*z[13] - rr - lr*z[14] - lf*z[14]*z[15] - z[14]*z[16]*z[36] - 
  z[11]*(rrt+z[35]);
  z[30] = z[11]*z[13];
  z[22] = z[14]*z[15];
  z[26] = z[12]*z[16] - z[11]*z[22];
  z[56] = lr*z[30] + z[16]*(ls*z[28]+z[30]*z[36]) + z[15]*(lf*z[30]-ls*z[26]);
  z[74] = z[65]/z[56];
  z[66] = -lf*z[15] - z[16]*z[36] - z[11]*z[14]*z[35];
  z[75] = z[66]/z[56];
  z[23] = z[13]*z[16];
  z[69] = z[11]*z[23]*z[35];
  z[79] = z[69]/z[56];
  u0 = z[74]*u1 + z[75]*u3 + z[79]*u5;
  q0p = u0;
  q1p = u1;
  z[55] = z[13]*(rr+rrt*z[11]);
  z[21] = z[13]*z[15];
  z[29] = z[12]*z[13];
  z[70] = z[21]*z[36] + z[29]*(rrt+z[35]) - lf*z[23];
  z[57] = -lf*z[28] - lr*z[12] - z[26]*z[36] - rr*z[12]*z[14];
  z[90] = z[70] - z[57]*z[74];
  z[59] = z[14]*(rr+rrt*z[11]);
  z[38] = z[12]*z[14];
  z[61] = lf*z[14]*z[16] - z[14]*z[15]*z[36] - z[38]*(rrt+z[35]);
  z[53] = z[15]*(ls*z[28]+z[30]*z[36]) - rr*z[12]*z[13] - z[16]*(lf*z[30]-ls*
  z[26]);
  z[85] = z[61] - z[53]*z[74];
  z[58] = -lr - lf*z[15] - rr*z[14] - z[16]*z[36] - z[11]*z[14]*(rrt+z[35]);
  z[54] = ls*pow(z[15],2) + ls*pow(z[16],2) - rr*z[13] - z[11]*z[13]*(rrt+
  z[35]);
  z[81] = z[55]*z[58] - z[54]*z[59];
  z[101] = (z[55]*z[90]-z[59]*z[85])/z[81];
  z[27] = z[11]*z[15] - z[12]*z[24];
  z[73] = z[35]*(z[14]*z[27]-z[23]*z[29]);
  z[93] = z[73] - z[57]*z[79];
  z[64] = z[35]*(z[13]*z[27]+z[23]*z[38]);
  z[89] = z[64] - z[53]*z[79];
  z[105] = (z[55]*z[93]-z[59]*z[89])/z[81];
  z[97] = z[57]*z[75];
  z[60] = lf*z[16] - z[15]*z[36] - z[13]*z[29]*z[35] - z[14]*z[35]*z[38];
  z[84] = z[60] - z[53]*z[75];
  z[100] = (z[55]*z[97]+z[59]*z[84])/z[81];
  u2 = z[101]*u1 + z[105]*u5 - z[100]*u3;
  q2p = u2;
  q3p = u3;
  z[110] = (z[54]*z[90]-z[58]*z[85])/z[81];
  z[114] = (z[54]*z[93]-z[58]*z[89])/z[81];
  z[109] = (z[54]*z[97]+z[58]*z[84])/z[81];
  u4 = z[110]*u1 + z[114]*u5 - z[109]*u3;
  q4p = u4;
  q5p = u5;
  z[9] = cos(q0);
  q6p = -z[9]*(rr+rrt*z[11])*(q2p+q4p);
  z[10] = sin(q0);
  q7p = -z[10]*(rr+rrt*z[11])*(q2p+q4p);
  z[39] = z[11]*z[14];
  z[175] = z[39]*z[75];
  z[183] = z[30]*z[75];
  z[242] = IDxz*z[175] - IDzz*z[183];
  z[190] = z[26]*z[75] - z[16]*z[100];
  z[197] = 1 + z[30]*z[75];
  z[254] = -IExz*z[190] - IEzz*z[197];
  z[238] = IDxx*z[175] - IDxz*z[183];
  z[250] = -IExx*z[190] - IExz*z[197];
  z[194] = z[28]*z[75] - z[15]*z[100];
  z[258] = IEyy*z[194];
  z[262] = IFyy*z[194];
  z[208] = z[75]*(lrx*z[30]+lrz*z[39]);
  z[272] = mr*z[208];
  z[211] = lrx*(z[100]-z[12]*z[75]) + rrt*z[11]*z[14]*(z[100]+z[109]) + rr*
  z[14]*(z[100]+z[109]-z[12]*z[75]);
  z[275] = mr*z[211];
  z[229] = lfx*z[15]*z[100] - lfx*z[28]*z[75] - z[11]*z[14]*z[35]*z[100] - 
  z[36]*(z[16]*z[100]-z[26]*z[75]);
  z[289] = mf*z[229];
  z[180] = z[12]*z[75] - z[100];
  z[247] = IDyy*z[180];
  z[164] = z[12]*z[75] - z[100] - z[109];
  z[233] = ICyy*z[164];
  z[204] = rrt*z[11]*z[13]*(z[100]+z[109]) + rr*z[13]*(z[100]+z[109]-z[12]*
  z[75]) - lrz*(z[100]-z[12]*z[75]);
  z[268] = mr*z[204];
  z[25] = z[11]*z[16] + z[12]*z[22];
  z[220] = lfz*z[28]*z[75] - lfz*z[15]*z[100] - z[14]*z[25]*z[35] - z[36]*(1+
  z[30]*z[75]) - z[21]*z[35]*(z[29]+z[11]*z[100]);
  z[280] = mf*z[220];
  z[223] = lfx + lfx*z[30]*z[75] + lfz*z[16]*z[100] + z[23]*z[35]*(z[29]+
  z[11]*z[100]) - lfz*z[26]*z[75] - z[14]*z[27]*z[35];
  z[283] = mf*z[223];
  z[307] = z[242]*(z[14]+z[30]*z[74]) + z[254]*(z[14]+z[30]*z[74]) + z[238]*(
  z[13]-z[39]*z[74]) + z[250]*(z[21]+z[16]*z[101]+z[26]*z[74]) + z[258]*(
  z[23]-z[15]*z[101]-z[28]*z[74]) + z[262]*(z[23]-z[15]*z[101]-z[28]*z[74]) + 
  z[272]*(lrz*z[13]-rr-lrx*z[14]-rrt*z[11]-lrx*z[30]*z[74]-lrz*z[39]*z[74]) + 
  z[275]*(rrt*z[29]+lrx*(z[101]+z[12]*z[74])+rrt*z[11]*z[14]*(z[101]+z[110])+
  rr*z[14]*(z[101]+z[110]+z[12]*z[74])) + z[289]*(lfx*z[15]*z[101]+lfx*z[28]*
  z[74]-lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(z[21]+z[16]*
  z[101]+z[26]*z[74])) - z[247]*(z[101]+z[12]*z[74]) - z[233]*(z[101]+z[110]+
  z[12]*z[74]) - z[268]*(rrt*z[38]+lrz*(z[101]+z[12]*z[74])-rrt*z[11]*z[13]*(
  z[101]+z[110])-rr*z[13]*(z[101]+z[110]+z[12]*z[74])) - z[280]*(lfz*z[15]*
  z[101]+lfz*z[28]*z[74]+z[11]*z[21]*z[35]*z[101]-z[25]*z[35]-lfz*z[13]*z[16]-
  z[36]*(z[14]+z[30]*z[74])) - z[283]*(lfx*z[14]+lfx*z[30]*z[74]-z[27]*z[35]-
  lfz*z[13]*z[15]-lfz*z[16]*z[101]-lfz*z[26]*z[74]-z[11]*z[23]*z[35]*z[101]);
  z[176] = z[39]*z[79];
  z[184] = z[30]*z[79];
  z[243] = IDxz*z[176] - IDzz*z[184];
  z[191] = z[16]*z[105] + z[26]*z[79];
  z[255] = -IExz*z[191] - IEzz*z[184];
  z[179] = z[105] + z[12]*z[79];
  z[246] = IDyy*z[179];
  z[163] = z[105] + z[114] + z[12]*z[79];
  z[232] = ICyy*z[163];
  z[195] = z[15]*z[105] + z[28]*z[79];
  z[259] = IEyy*z[195];
  z[199] = 1 + z[15]*z[105] + z[28]*z[79];
  z[263] = IFyy*z[199];
  z[205] = lrz*(z[105]+z[12]*z[79]) - rrt*z[11]*z[13]*(z[105]+z[114]) - rr*
  z[13]*(z[105]+z[114]+z[12]*z[79]);
  z[269] = mr*z[205];
  z[225] = lfx*z[30]*z[79] - lfz*z[16]*z[105] - lfz*z[26]*z[79] - z[11]*z[23]*
  z[35]*z[105];
  z[285] = mf*z[225];
  z[239] = IDxx*z[176] - IDxz*z[184];
  z[209] = z[79]*(lrx*z[30]+lrz*z[39]);
  z[273] = mr*z[209];
  z[251] = -IExx*z[191] - IExz*z[184];
  z[213] = -lrx*(z[105]+z[12]*z[79]) - rrt*z[11]*z[14]*(z[105]+z[114]) - rr*
  z[14]*(z[105]+z[114]+z[12]*z[79]);
  z[277] = mr*z[213];
  z[228] = z[14]*z[35]*(z[27]+z[11]*z[105]) + z[36]*(z[16]*z[105]+z[26]*z[79]) - 
  lfx*z[15]*z[105] - lfx*z[28]*z[79] - z[23]*z[29]*z[35];
  z[288] = mf*z[228];
  z[221] = lfz*z[15]*z[105] + lfz*z[28]*z[79] + z[23]*z[25]*z[35] + z[21]*
  z[35]*(z[27]+z[11]*z[105]) - z[30]*z[36]*z[79];
  z[281] = mf*z[221];
  z[312] = z[30]*z[75]*z[243] + z[255]*(1+z[30]*z[75]) + z[246]*(z[100]-z[12]*
  z[75]) + z[232]*(z[100]+z[109]-z[12]*z[75]) + z[259]*(z[15]*z[100]-z[28]*
  z[75]) + z[263]*(z[15]*z[100]-z[28]*z[75]) + z[269]*(lrz*(z[100]-z[12]*
  z[75])-rrt*z[11]*z[13]*(z[100]+z[109])-rr*z[13]*(z[100]+z[109]-z[12]*z[75])) + 
  z[285]*(lfz*z[26]*z[75]+z[14]*z[27]*z[35]-lfx-lfx*z[30]*z[75]-lfz*z[16]*
  z[100]-z[23]*z[35]*(z[29]+z[11]*z[100])) - z[39]*z[75]*z[239] - z[75]*
  z[273]*(lrx*z[30]+lrz*z[39]) - z[251]*(z[16]*z[100]-z[26]*z[75]) - z[277]*(
  lrx*(z[100]-z[12]*z[75])+rrt*z[11]*z[14]*(z[100]+z[109])+rr*z[14]*(z[100]+
  z[109]-z[12]*z[75])) - z[288]*(lfx*z[15]*z[100]-lfx*z[28]*z[75]-z[11]*z[14]*
  z[35]*z[100]-z[36]*(z[16]*z[100]-z[26]*z[75])) - z[281]*(lfz*z[28]*z[75]-
  lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+z[30]*z[75])-z[21]*z[35]*(z[29]+
  z[11]*z[100]));
  z[308] = z[243]*(z[14]+z[30]*z[74]) + z[255]*(z[14]+z[30]*z[74]) + z[239]*(
  z[13]-z[39]*z[74]) + z[251]*(z[21]+z[16]*z[101]+z[26]*z[74]) + z[259]*(
  z[23]-z[15]*z[101]-z[28]*z[74]) + z[263]*(z[23]-z[15]*z[101]-z[28]*z[74]) + 
  z[273]*(lrz*z[13]-rr-lrx*z[14]-rrt*z[11]-lrx*z[30]*z[74]-lrz*z[39]*z[74]) + 
  z[277]*(rrt*z[29]+lrx*(z[101]+z[12]*z[74])+rrt*z[11]*z[14]*(z[101]+z[110])+
  rr*z[14]*(z[101]+z[110]+z[12]*z[74])) + z[288]*(lfx*z[15]*z[101]+lfx*z[28]*
  z[74]-lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(z[21]+z[16]*
  z[101]+z[26]*z[74])) - z[246]*(z[101]+z[12]*z[74]) - z[232]*(z[101]+z[110]+
  z[12]*z[74]) - z[269]*(rrt*z[38]+lrz*(z[101]+z[12]*z[74])-rrt*z[11]*z[13]*(
  z[101]+z[110])-rr*z[13]*(z[101]+z[110]+z[12]*z[74])) - z[281]*(lfz*z[15]*
  z[101]+lfz*z[28]*z[74]+z[11]*z[21]*z[35]*z[101]-z[25]*z[35]-lfz*z[13]*z[16]-
  z[36]*(z[14]+z[30]*z[74])) - z[285]*(lfx*z[14]+lfx*z[30]*z[74]-z[27]*z[35]-
  lfz*z[13]*z[15]-lfz*z[16]*z[101]-lfz*z[26]*z[74]-z[11]*z[23]*z[35]*z[101]);
  z[311] = z[30]*z[75]*z[242] + z[254]*(1+z[30]*z[75]) + z[247]*(z[100]-z[12]*
  z[75]) + z[233]*(z[100]+z[109]-z[12]*z[75]) + z[258]*(z[15]*z[100]-z[28]*
  z[75]) + z[262]*(z[15]*z[100]-z[28]*z[75]) + z[268]*(lrz*(z[100]-z[12]*
  z[75])-rrt*z[11]*z[13]*(z[100]+z[109])-rr*z[13]*(z[100]+z[109]-z[12]*z[75])) + 
  z[283]*(lfz*z[26]*z[75]+z[14]*z[27]*z[35]-lfx-lfx*z[30]*z[75]-lfz*z[16]*
  z[100]-z[23]*z[35]*(z[29]+z[11]*z[100])) - z[39]*z[75]*z[238] - z[75]*
  z[272]*(lrx*z[30]+lrz*z[39]) - z[250]*(z[16]*z[100]-z[26]*z[75]) - z[275]*(
  lrx*(z[100]-z[12]*z[75])+rrt*z[11]*z[14]*(z[100]+z[109])+rr*z[14]*(z[100]+
  z[109]-z[12]*z[75])) - z[289]*(lfx*z[15]*z[100]-lfx*z[28]*z[75]-z[11]*z[14]*
  z[35]*z[100]-z[36]*(z[16]*z[100]-z[26]*z[75])) - z[280]*(lfz*z[28]*z[75]-
  lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+z[30]*z[75])-z[21]*z[35]*(z[29]+
  z[11]*z[100]));
  z[347] = z[307]*z[312] - z[308]*z[311];
  z[297] = Tfw + Trw*z[114] + z[292]*z[26]*(lfz*z[15]*z[105]+z[23]*z[25]*
  z[35]+z[21]*z[35]*(z[27]+z[11]*z[105])) - z[292]*z[28]*z[105]*(lfz*z[16]+
  z[11]*z[23]*z[35]) - z[291]*z[30]*(lrx*z[105]+rrt*z[11]*z[14]*(z[105]+
  z[114])+rr*z[14]*(z[105]+z[114]+z[12]*z[79])) - z[291]*z[39]*(lrz*z[105]-
  rrt*z[11]*z[13]*(z[105]+z[114])-rr*z[13]*(z[105]+z[114]+z[12]*z[79])) - 
  z[292]*z[30]*(lfx*z[15]*z[105]+z[23]*z[29]*z[35]-z[16]*z[36]*z[105]-z[14]*
  z[35]*(z[27]+z[11]*z[105]));
  z[235] = ICyy*u1*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*
  u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5));
  z[266] = IFyy*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(u5+z[28]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5));
  z[173] = z[11]*z[13]*q2p - z[12]*z[14]*u1;
  z[120] = z[14]*z[15]*u3 + z[13]*z[16]*q2p;
  z[121] = z[11]*z[15]*u1 + z[11]*z[120] - z[12]*z[16]*u3 - z[12]*z[24]*u1;
  z[122] = z[28]*z[121]/pow(z[31],0.5);
  z[125] = rf*z[122]/pow(z[32],2);
  z[123] = (z[28]*z[122]+z[32]*z[121])/pow(z[32],2);
  z[131] = lf*z[14]*z[16]*u3 + z[12]*(rrt+z[35])*u1 + z[11]*z[125] - z[14]*
  z[15]*z[36]*u3 - lr*z[13]*q2p - ls*z[14]*q2p - lf*z[13]*z[15]*q2p - z[13]*
  z[16]*z[36]*q2p - rf*z[14]*z[16]*z[123];
  z[132] = -z[12]*z[13]*u1 - z[11]*z[14]*q2p;
  z[128] = z[13]*z[15]*q2p - z[14]*z[16]*u3;
  z[129] = z[11]*z[16]*u1 + z[12]*z[15]*u3 + z[12]*z[22]*u1 - z[11]*z[128];
  z[133] = z[15]*(ls*z[28]+z[30]*z[36])*u3 + lr*z[132] + z[15]*(lf*z[132]-ls*
  z[129]) + z[16]*(ls*z[121]+z[36]*z[132]+rf*z[30]*z[123]) - z[16]*(lf*z[30]-
  ls*z[26])*u3;
  z[134] = (z[56]*z[131]-z[65]*z[133])/pow(z[56],2);
  z[155] = lf*z[16]*u3 + z[12]*z[14]*z[35]*u1 + z[11]*z[14]*z[125] - z[15]*
  z[36]*u3 - z[11]*z[13]*z[35]*q2p - rf*z[16]*z[123];
  z[156] = (z[56]*z[155]-z[66]*z[133])/pow(z[56],2);
  z[126] = z[13]*z[15]*u3 - z[14]*z[16]*q2p;
  z[147] = z[11]*z[35]*z[126] - z[12]*z[23]*z[35]*u1 - z[11]*z[23]*z[125];
  z[148] = (z[56]*z[147]-z[69]*z[133])/pow(z[56],2);
  z[177] = -z[14]*u1*q2p - (z[74]*u1+z[75]*u3+z[79]*u5)*z[173] - z[39]*(u1*
  z[134]+u3*z[156]+u5*z[148]);
  z[185] = z[13]*u1*q2p + (z[74]*u1+z[75]*u3+z[79]*u5)*z[132] + z[30]*(u1*
  z[134]+u3*z[156]+u5*z[148]);
  z[244] = IDyy*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[100]*u3-
  z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IDxz*(z[14]*u1+
  z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5)) - IDxx*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+
  z[79]*u5))*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - 
  IDxz*z[177] - IDzz*z[185];
  z[119] = -z[13]*z[16]*u3 - z[14]*z[15]*q2p;
  z[141] = lf*z[16]*u3 + z[12]*z[14]*(rrt+z[35])*u1 + z[11]*z[14]*z[125] - 
  z[15]*z[36]*u3 - rr*z[13]*q2p - z[11]*z[13]*(rrt+z[35])*q2p - rf*z[16]*
  z[123];
  z[118] = -rrt*z[12]*z[13]*u1 - z[14]*(rr+rrt*z[11])*q2p;
  z[136] = z[13]*(rr+rrt*z[11])*q2p - rrt*z[12]*z[14]*u1;
  z[142] = z[12]*z[13]*(rrt+z[35])*u1 + rr*z[14]*q2p + z[11]*z[14]*(rrt+z[35])*
  q2p + z[11]*z[13]*z[125];
  z[143] = z[55]*z[141] + z[58]*z[118] - z[54]*z[136] - z[59]*z[142];
  z[130] = -lr*z[11]*u1 - rr*z[11]*z[14]*u1 - rr*z[12]*z[13]*q2p - lf*z[121] - 
  z[36]*z[129] - rf*z[26]*z[123];
  z[157] = z[57]*z[156] + z[75]*z[130];
  z[124] = z[11]*z[13]*u1 - z[12]*z[14]*q2p;
  z[137] = z[11]*z[14]*u1 + z[12]*z[13]*q2p;
  z[158] = lf*z[15]*u3 + z[16]*z[36]*u3 + z[14]*z[29]*z[35]*q2p + z[13]*z[29]*
  z[125] + z[14]*z[38]*z[125] - z[13]*z[35]*z[38]*q2p - rf*z[15]*z[123] - 
  z[13]*z[35]*z[124] - z[14]*z[35]*z[137];
  z[139] = rr*z[12]*z[14]*q2p + z[15]*(ls*z[121]+z[36]*z[132]+rf*z[30]*z[123]) - 
  rr*z[11]*z[13]*u1 - z[16]*(ls*z[28]+z[30]*z[36])*u3 - z[15]*(lf*z[30]-ls*
  z[26])*u3 - z[16]*(lf*z[132]-ls*z[129]);
  z[159] = z[158] - z[53]*z[156] - z[75]*z[139];
  z[160] = ((z[55]*z[97]+z[59]*z[84])*z[143]-z[81]*(z[55]*z[157]+z[59]*z[159]+
  z[84]*z[136]+z[97]*z[118]))/pow(z[81],2);
  z[138] = lf*z[14]*z[15]*u3 + z[14]*z[16]*z[36]*u3 + lf*z[13]*z[16]*q2p + 
  z[38]*z[125] - z[13]*z[15]*z[36]*q2p - (rrt+z[35])*z[137] - rf*z[14]*z[15]*
  z[123];
  z[140] = z[138] - z[53]*z[134] - z[74]*z[139];
  z[127] = z[36]*z[119] + rf*z[21]*z[123] + (rrt+z[35])*z[124] - lf*z[126] - 
  z[29]*z[125];
  z[135] = z[127] - z[57]*z[134] - z[74]*z[130];
  z[144] = ((z[55]*z[90]-z[59]*z[85])*z[143]+z[81]*(z[59]*z[140]+z[85]*z[136]-
  z[55]*z[135]-z[90]*z[118]))/pow(z[81],2);
  z[145] = -z[11]*z[16]*u3 - z[11]*z[24]*u1 - z[12]*z[15]*u1 - z[12]*z[120];
  z[150] = -(z[13]*z[27]+z[23]*z[38])*z[125] - z[35]*(z[14]*z[27]*q2p-z[13]*
  z[145]-z[23]*z[137]-z[38]*z[126]);
  z[151] = z[150] - z[53]*z[148] - z[79]*z[139];
  z[146] = z[35]*(z[13]*z[27]*q2p+z[14]*z[145]-z[23]*z[124]-z[29]*z[126]) - (
  z[14]*z[27]-z[23]*z[29])*z[125];
  z[149] = z[146] - z[57]*z[148] - z[79]*z[130];
  z[152] = ((z[55]*z[93]-z[59]*z[89])*z[143]+z[81]*(z[59]*z[151]+z[89]*z[136]-
  z[55]*z[149]-z[93]*z[118]))/pow(z[81],2);
  z[192] = u1*z[119] + (z[74]*u1+z[75]*u3+z[79]*u5)*z[129] + z[26]*(u1*z[134]+
  u3*z[156]+u5*z[148]) + z[16]*(u3*z[160]-u1*z[144]-u5*z[152]) - z[15]*u3*(
  z[100]*u3-z[101]*u1-z[105]*u5);
  z[256] = IExz*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[28]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) + 
  IExx*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*
  u1-z[105]*u5))*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*
  u3-z[101]*u1-z[105]*u5)) - IEyy*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IExz*z[192] - IEzz*z[185];
  z[181] = z[11]*u1*(z[74]*u1+z[75]*u3+z[79]*u5) + u3*z[160] + z[12]*(u1*
  z[134]+u3*z[156]+u5*z[148]) - u1*z[144] - u5*z[152];
  z[248] = -(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(IDxz*(z[14]*u1+
  z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-IDzz*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+
  z[79]*u5))) - (z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))*(IDxx*(z[14]*u1+
  z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-IDxz*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+
  z[79]*u5))) - IDyy*z[181];
  z[196] = z[16]*u3*(z[100]*u3-z[101]*u1-z[105]*u5) + (z[74]*u1+z[75]*u3+
  z[79]*u5)*z[121] + z[28]*(u1*z[134]+u3*z[156]+u5*z[148]) + z[15]*(u3*z[160]-
  u1*z[144]-u5*z[152]) - u1*z[126];
  z[260] = -(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(IExz*(u3+z[14]*
  u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-IEzz*(z[21]*u1+z[26]*(z[74]*u1+z[75]*
  u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))) - (z[21]*u1+z[26]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))*(IExx*(
  u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-IExz*(z[21]*u1+z[26]*(z[74]*
  u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))) - IEyy*z[196];
  z[252] = IEyy*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[28]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) - 
  IEzz*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[28]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IExz*(
  z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-
  z[105]*u5))*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-
  z[101]*u1-z[105]*u5)) - IExx*z[192] - IExz*z[185];
  z[210] = lrx*z[13]*u1*q2p + lrz*z[14]*u1*q2p + lrx*(z[74]*u1+z[75]*u3+z[79]*
  u5)*z[132] + lrx*z[30]*(u1*z[134]+u3*z[156]+u5*z[148]) + lrz*(z[74]*u1+
  z[75]*u3+z[79]*u5)*z[173] + lrz*z[39]*(u1*z[134]+u3*z[156]+u5*z[148]) - rrt*
  z[12]*pow(u1,2);
  z[274] = mr*((z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(lrz*(z[100]*u3-
  z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))-rrt*z[38]*u1-rrt*
  z[11]*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)-
  rr*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5)))-(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))*(
  rrt*z[29]*u1-lrx*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+
  z[79]*u5))-rrt*z[11]*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*
  u1-z[114]*u5)-rr*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))-z[210]);
  z[226] = lfz*z[13]*z[16]*u1*u3 + lfz*z[15]*u3*(z[100]*u3-z[101]*u1-z[105]*
  u5) + lfx*z[13]*u1*q2p + lfz*z[14]*z[15]*u1*q2p + z[27]*(u1+z[14]*u3)*
  z[125] + lfx*(z[74]*u1+z[75]*u3+z[79]*u5)*z[132] + lfx*z[30]*(u1*z[134]+u3*
  z[156]+u5*z[148]) + z[35]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))*
  z[126] + z[23]*z[35]*(u3*z[124]-z[12]*u1*(z[100]*u3-z[101]*u1-z[105]*u5)-
  z[11]*(u3*z[160]-u1*z[144]-u5*z[152])) - z[13]*z[27]*z[35]*u3*q2p - z[35]*(
  u1+z[14]*u3)*z[145] - lfz*(z[74]*u1+z[75]*u3+z[79]*u5)*z[129] - lfz*z[26]*(
  u1*z[134]+u3*z[156]+u5*z[148]) - lfz*z[16]*(u3*z[160]-u1*z[144]-u5*z[152]) - 
  z[23]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))*z[125];
  z[286] = mf*((u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(lfz*z[13]*
  z[16]*u1+z[25]*z[35]*(u1+z[14]*u3-z[23]*u5)+lfz*z[15]*(z[100]*u3-z[101]*u1-
  z[105]*u5)+z[36]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-lfz*z[28]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[21]*z[35]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-
  z[101]*u1-z[105]*u5)))-(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(
  z[100]*u3-z[101]*u1-z[105]*u5))*(lfx*z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*
  z[23]*u1-z[29]*z[35]*(u1-z[23]*u5)-lfx*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)-
  z[14]*z[35]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))-z[36]*(z[21]*
  u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)))-
  z[226]);
  z[265] = IFyy*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-
  z[101]*u1-z[105]*u5))*(u5+z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(
  z[100]*u3-z[101]*u1-z[105]*u5));
  z[240] = IDzz*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[100]*u3-
  z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) + IDxz*(z[13]*u1-
  z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5)) - IDyy*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+
  z[79]*u5))*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - 
  IDxx*z[177] - IDxz*z[185];
  z[161] = ((z[54]*z[97]+z[58]*z[84])*z[143]-z[81]*(z[54]*z[157]+z[58]*z[159]+
  z[84]*z[141]+z[97]*z[142]))/pow(z[81],2);
  z[153] = ((z[54]*z[90]-z[58]*z[85])*z[143]+z[81]*(z[58]*z[140]+z[85]*z[141]-
  z[54]*z[135]-z[90]*z[142]))/pow(z[81],2);
  z[154] = ((z[54]*z[93]-z[58]*z[89])*z[143]+z[81]*(z[58]*z[151]+z[89]*z[141]-
  z[54]*z[149]-z[93]*z[142]))/pow(z[81],2);
  z[165] = z[11]*u1*(z[74]*u1+z[75]*u3+z[79]*u5) + u3*z[160] + u3*z[161] + 
  z[12]*(u1*z[134]+u3*z[156]+u5*z[148]) - u1*z[144] - u1*z[153] - u5*z[152] - 
  u5*z[154];
  z[234] = ICyy*z[165];
  z[264] = IFyy*z[196];
  z[214] = rrt*z[11]*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5)*q2p + rr*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*
  u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))*q2p + lrx*(u1*z[144]+u5*
  z[152]-z[11]*u1*(z[74]*u1+z[75]*u3+z[79]*u5)-u3*z[160]-z[12]*(u1*z[134]+u3*
  z[156]+u5*z[148])) + rr*z[14]*(u1*z[144]+u1*z[153]+u5*z[152]+u5*z[154]-
  z[11]*u1*(z[74]*u1+z[75]*u3+z[79]*u5)-u3*z[160]-u3*z[161]-z[12]*(u1*z[134]+
  u3*z[156]+u5*z[148])) - rrt*z[12]*z[14]*u1*(z[100]*u3+z[109]*u3-z[101]*u1-
  z[105]*u5-z[110]*u1-z[114]*u5) - rrt*u1*z[124] - rrt*z[11]*z[14]*(u3*z[160]+
  u3*z[161]-u1*z[144]-u1*z[153]-u5*z[152]-u5*z[154]);
  z[278] = mr*((z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))*(lrz*z[13]*u1-rr*
  u1-lrx*z[14]*u1-rrt*z[11]*u1-lrx*z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-lrz*
  z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))+(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5))*(lrz*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5))-rrt*z[38]*u1-rrt*z[11]*z[13]*(z[100]*u3+z[109]*
  u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)-rr*z[13]*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))-
  z[214]);
  z[206] = rrt*u1*z[137] + rr*z[13]*(u1*z[144]+u1*z[153]+u5*z[152]+u5*z[154]-
  z[11]*u1*(z[74]*u1+z[75]*u3+z[79]*u5)-u3*z[160]-u3*z[161]-z[12]*(u1*z[134]+
  u3*z[156]+u5*z[148])) - rrt*z[12]*z[13]*u1*(z[100]*u3+z[109]*u3-z[101]*u1-
  z[105]*u5-z[110]*u1-z[114]*u5) - rrt*z[11]*z[14]*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)*q2p - rr*z[14]*(z[100]*u3+z[109]*
  u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*
  u5))*q2p - rrt*z[11]*z[13]*(u3*z[160]+u3*z[161]-u1*z[144]-u1*z[153]-u5*
  z[152]-u5*z[154]) - lrz*(u1*z[144]+u5*z[152]-z[11]*u1*(z[74]*u1+z[75]*u3+
  z[79]*u5)-u3*z[160]-z[12]*(u1*z[134]+u3*z[156]+u5*z[148]));
  z[270] = mr*((z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(lrz*z[13]*u1-rr*
  u1-lrx*z[14]*u1-rrt*z[11]*u1-lrx*z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-lrz*
  z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))+(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5))*(rrt*z[29]*u1-lrx*(z[100]*u3-z[101]*u1-z[105]*
  u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))-rrt*z[11]*z[14]*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)-rr*z[14]*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))+
  z[206]);
  z[218] = z[11]*z[15]*u3 + z[11]*z[22]*u1 + z[12]*z[128] - z[12]*z[16]*u1;
  z[222] = lfz*z[16]*u3*(z[100]*u3-z[101]*u1-z[105]*u5) + lfz*z[14]*z[16]*u1*
  q2p + lfz*(z[74]*u1+z[75]*u3+z[79]*u5)*z[121] + lfz*z[28]*(u1*z[134]+u3*
  z[156]+u5*z[148]) + z[25]*(u1+z[14]*u3-z[23]*u5)*z[125] + lfz*z[15]*(u3*
  z[160]-u1*z[144]-u5*z[152]) + z[35]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-
  z[101]*u1-z[105]*u5))*z[119] - lfz*z[13]*z[15]*u1*u3 - z[25]*z[35]*(z[13]*
  u3*q2p-u5*z[126]) - z[35]*(u1+z[14]*u3-z[23]*u5)*z[218] - rf*(u3+z[14]*u1+
  z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*z[123] - z[21]*(z[27]*u5-z[29]*u3-z[11]*(
  z[100]*u3-z[101]*u1-z[105]*u5))*z[125] - z[36]*(z[13]*u1*q2p+(z[74]*u1+
  z[75]*u3+z[79]*u5)*z[132]+z[30]*(u1*z[134]+u3*z[156]+u5*z[148])) - z[21]*
  z[35]*(u3*z[124]-z[12]*u1*(z[100]*u3-z[101]*u1-z[105]*u5)-u5*z[145]-z[11]*(
  u3*z[160]-u1*z[144]-u5*z[152]));
  z[282] = mf*((z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-
  z[101]*u1-z[105]*u5))*(lfx*z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*z[23]*u1-
  z[29]*z[35]*(u1-z[23]*u5)-lfx*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[14]*
  z[35]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))-z[36]*(z[21]*u1+
  z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)))-(
  u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(lfz*z[13]*z[15]*u1+z[27]*
  z[35]*(u1+z[14]*u3)+lfz*z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*u3-lfx*z[14]*
  u1-lfx*z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfz*z[16]*(z[100]*u3-z[101]*u1-
  z[105]*u5)-z[23]*z[35]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)))-
  z[222]);
  z[230] = z[13]*z[35]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))*q2p + 
  lfx*u1*z[126] + z[35]*(u1-z[23]*u5)*z[124] + rf*(z[21]*u1+z[26]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))*z[123] + z[14]*
  z[35]*(z[12]*u1*(z[100]*u3-z[101]*u1-z[105]*u5)+u5*z[145]+z[11]*(u3*z[160]-
  u1*z[144]-u5*z[152])) - lfx*z[16]*u3*(z[100]*u3-z[101]*u1-z[105]*u5) - 
  z[29]*z[35]*u5*z[126] - z[29]*(u1-z[23]*u5)*z[125] - lfx*(z[74]*u1+z[75]*u3+
  z[79]*u5)*z[121] - lfx*z[28]*(u1*z[134]+u3*z[156]+u5*z[148]) - lfx*z[15]*(
  u3*z[160]-u1*z[144]-u5*z[152]) - z[14]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5))*z[125] - z[36]*(z[15]*u3*(z[100]*u3-z[101]*u1-z[105]*u5)-u1*
  z[119]-(z[74]*u1+z[75]*u3+z[79]*u5)*z[129]-z[26]*(u1*z[134]+u3*z[156]+u5*
  z[148])-z[16]*(u3*z[160]-u1*z[144]-u5*z[152]));
  z[290] = mf*((z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-
  z[101]*u1-z[105]*u5))*(lfz*z[13]*z[15]*u1+z[27]*z[35]*(u1+z[14]*u3)+lfz*
  z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*u3-lfx*z[14]*u1-lfx*z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-lfz*z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[23]*z[35]*(
  z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)))-(z[28]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))*(lfz*z[13]*z[16]*
  u1+z[25]*z[35]*(u1+z[14]*u3-z[23]*u5)+lfz*z[15]*(z[100]*u3-z[101]*u1-z[105]*
  u5)+z[36]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-lfz*z[28]*(z[74]*
  u1+z[75]*u3+z[79]*u5)-z[21]*z[35]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-
  z[101]*u1-z[105]*u5)))-z[230]);
  z[317] = z[11]*z[79]*z[235] + (z[16]*z[105]+z[26]*z[79])*z[266] + z[30]*
  z[79]*z[244] + z[30]*z[79]*z[256] + (z[105]+z[12]*z[79])*z[248] + (z[15]*
  z[105]+z[28]*z[79])*z[260] + (z[16]*z[105]+z[26]*z[79])*z[252] + z[79]*(lrx*
  z[30]+lrz*z[39])*z[274] + (lfx*z[30]*z[79]-lfz*z[16]*z[105]-lfz*z[26]*z[79]-
  z[11]*z[23]*z[35]*z[105])*z[286] - z[30]*z[79]*z[265] - z[39]*z[79]*z[240] - (
  z[105]+z[114]+z[12]*z[79])*z[234] - (1+z[15]*z[105]+z[28]*z[79])*z[264] - (
  lrx*(z[105]+z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+z[114])+rr*z[14]*(z[105]+
  z[114]+z[12]*z[79]))*z[278] - (lrz*(z[105]+z[12]*z[79])-rrt*z[11]*z[13]*(
  z[105]+z[114])-rr*z[13]*(z[105]+z[114]+z[12]*z[79]))*z[270] - (z[30]*z[36]*
  z[79]-lfz*z[15]*z[105]-lfz*z[28]*z[79]-z[23]*z[25]*z[35]-z[21]*z[35]*(z[27]+
  z[11]*z[105]))*z[282] - (lfx*z[15]*z[105]+lfx*z[28]*z[79]+z[23]*z[29]*z[35]-
  z[14]*z[35]*(z[27]+z[11]*z[105])-z[36]*(z[16]*z[105]+z[26]*z[79]))*z[290];
  z[344] = -z[297] - z[317];
  z[316] = z[30]*z[79]*z[243] + z[30]*z[79]*z[255] + z[251]*(z[16]*z[105]+
  z[26]*z[79]) + z[277]*(lrx*(z[105]+z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+
  z[114])+rr*z[14]*(z[105]+z[114]+z[12]*z[79])) + z[281]*(z[30]*z[36]*z[79]-
  lfz*z[15]*z[105]-lfz*z[28]*z[79]-z[23]*z[25]*z[35]-z[21]*z[35]*(z[27]+z[11]*
  z[105])) + z[288]*(lfx*z[15]*z[105]+lfx*z[28]*z[79]+z[23]*z[29]*z[35]-z[14]*
  z[35]*(z[27]+z[11]*z[105])-z[36]*(z[16]*z[105]+z[26]*z[79])) - z[39]*z[79]*
  z[239] - z[246]*(z[105]+z[12]*z[79]) - z[232]*(z[105]+z[114]+z[12]*z[79]) - 
  z[259]*(z[15]*z[105]+z[28]*z[79]) - z[79]*z[273]*(lrx*z[30]+lrz*z[39]) - 
  z[263]*(1+z[15]*z[105]+z[28]*z[79]) - z[285]*(lfx*z[30]*z[79]-lfz*z[16]*
  z[105]-lfz*z[26]*z[79]-z[11]*z[23]*z[35]*z[105]) - z[269]*(lrz*(z[105]+
  z[12]*z[79])-rrt*z[11]*z[13]*(z[105]+z[114])-rr*z[13]*(z[105]+z[114]+z[12]*
  z[79]));
  z[315] = z[30]*z[79]*z[242] + z[30]*z[79]*z[254] + z[250]*(z[16]*z[105]+
  z[26]*z[79]) + z[275]*(lrx*(z[105]+z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+
  z[114])+rr*z[14]*(z[105]+z[114]+z[12]*z[79])) + z[280]*(z[30]*z[36]*z[79]-
  lfz*z[15]*z[105]-lfz*z[28]*z[79]-z[23]*z[25]*z[35]-z[21]*z[35]*(z[27]+z[11]*
  z[105])) + z[289]*(lfx*z[15]*z[105]+lfx*z[28]*z[79]+z[23]*z[29]*z[35]-z[14]*
  z[35]*(z[27]+z[11]*z[105])-z[36]*(z[16]*z[105]+z[26]*z[79])) - z[39]*z[79]*
  z[238] - z[247]*(z[105]+z[12]*z[79]) - z[233]*(z[105]+z[114]+z[12]*z[79]) - 
  z[258]*(z[15]*z[105]+z[28]*z[79]) - z[79]*z[272]*(lrx*z[30]+lrz*z[39]) - 
  z[262]*(1+z[15]*z[105]+z[28]*z[79]) - z[283]*(lfx*z[30]*z[79]-lfz*z[16]*
  z[105]-lfz*z[26]*z[79]-z[11]*z[23]*z[35]*z[105]) - z[268]*(lrz*(z[105]+
  z[12]*z[79])-rrt*z[11]*z[13]*(z[105]+z[114])-rr*z[13]*(z[105]+z[114]+z[12]*
  z[79]));
  z[349] = z[311]*z[316] - z[312]*z[315];
  z[295] = Trw*z[110] + z[292]*z[28]*(lfx*z[14]-z[27]*z[35]-lfz*z[13]*z[15]-
  lfz*z[16]*z[101]-z[11]*z[23]*z[35]*z[101]) - z[291]*z[12]*(lrz*z[13]-rr-lrx*
  z[14]-rrt*z[11]) - z[292]*z[26]*(z[14]*z[36]+z[25]*z[35]+lfz*z[13]*z[16]-
  lfz*z[15]*z[101]-z[11]*z[21]*z[35]*z[101]) - z[291]*z[30]*(lrx*z[101]+rrt*
  z[29]+rrt*z[11]*z[14]*(z[101]+z[110])+rr*z[14]*(z[101]+z[110]+z[12]*z[74])) - 
  z[291]*z[39]*(lrz*z[101]+rrt*z[38]-rrt*z[11]*z[13]*(z[101]+z[110])-rr*z[13]*(
  z[101]+z[110]+z[12]*z[74])) - z[292]*z[30]*(lfx*z[15]*z[101]-lfx*z[23]-
  z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(z[21]+z[16]*z[101]));
  z[236] = ICyy*z[11]*(z[74]*u1+z[75]*u3+z[79]*u5)*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5));
  z[309] = z[11]*z[74]*z[235] + (z[21]+z[16]*z[101]+z[26]*z[74])*z[266] + (
  z[14]+z[30]*z[74])*z[244] + (z[14]+z[30]*z[74])*z[256] + (z[101]+z[12]*
  z[74])*z[248] + (z[13]-z[39]*z[74])*z[240] + (z[21]+z[16]*z[101]+z[26]*
  z[74])*z[252] + (z[23]-z[15]*z[101]-z[28]*z[74])*z[264] + (lfz*z[15]*z[101]+
  lfz*z[28]*z[74]+z[11]*z[21]*z[35]*z[101]-z[25]*z[35]-lfz*z[13]*z[16]-z[36]*(
  z[14]+z[30]*z[74]))*z[282] + (lfx*z[14]+lfx*z[30]*z[74]-z[27]*z[35]-lfz*
  z[13]*z[15]-lfz*z[16]*z[101]-lfz*z[26]*z[74]-z[11]*z[23]*z[35]*z[101])*
  z[286] - z[236] - (z[14]+z[30]*z[74])*z[265] - (z[101]+z[110]+z[12]*z[74])*
  z[234] - (z[23]-z[15]*z[101]-z[28]*z[74])*z[260] - (lrz*z[13]-rr-lrx*z[14]-
  rrt*z[11]-lrx*z[30]*z[74]-lrz*z[39]*z[74])*z[274] - (rrt*z[29]+lrx*(z[101]+
  z[12]*z[74])+rrt*z[11]*z[14]*(z[101]+z[110])+rr*z[14]*(z[101]+z[110]+z[12]*
  z[74]))*z[278] - (rrt*z[38]+lrz*(z[101]+z[12]*z[74])-rrt*z[11]*z[13]*(
  z[101]+z[110])-rr*z[13]*(z[101]+z[110]+z[12]*z[74]))*z[270] - (lfx*z[15]*
  z[101]+lfx*z[28]*z[74]-lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(
  z[21]+z[16]*z[101]+z[26]*z[74]))*z[290];
  z[342] = -z[295] - z[309];
  z[352] = z[307]*z[316] - z[308]*z[315];
  z[296] = Ts + z[292]*z[30]*z[100]*(lfx*z[15]-z[16]*z[36]-z[11]*z[14]*z[35]) + 
  z[291]*z[30]*(lrx*z[100]+rrt*z[11]*z[14]*(z[100]+z[109])+rr*z[14]*(z[100]+
  z[109]-z[12]*z[75])) + z[291]*z[39]*(lrz*z[100]-rrt*z[11]*z[13]*(z[100]+
  z[109])-rr*z[13]*(z[100]+z[109]-z[12]*z[75])) - Trw*z[109] - z[292]*z[26]*(
  z[36]+lfz*z[15]*z[100]+z[14]*z[25]*z[35]+z[21]*z[35]*(z[29]+z[11]*z[100])) - 
  z[292]*z[28]*(z[14]*z[27]*z[35]-lfx-lfz*z[16]*z[100]-z[23]*z[35]*(z[29]+
  z[11]*z[100]));
  z[313] = z[11]*z[75]*z[235] + z[30]*z[75]*z[244] + (1+z[30]*z[75])*z[256] + (
  z[15]*z[100]-z[28]*z[75])*z[264] + (z[100]+z[109]-z[12]*z[75])*z[234] + 
  z[75]*(lrx*z[30]+lrz*z[39])*z[274] + (lfx*z[15]*z[100]-lfx*z[28]*z[75]-
  z[11]*z[14]*z[35]*z[100]-z[36]*(z[16]*z[100]-z[26]*z[75]))*z[290] + (lrx*(
  z[100]-z[12]*z[75])+rrt*z[11]*z[14]*(z[100]+z[109])+rr*z[14]*(z[100]+z[109]-
  z[12]*z[75]))*z[278] + (lrz*(z[100]-z[12]*z[75])-rrt*z[11]*z[13]*(z[100]+
  z[109])-rr*z[13]*(z[100]+z[109]-z[12]*z[75]))*z[270] + (lfz*z[28]*z[75]-lfz*
  z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+z[30]*z[75])-z[21]*z[35]*(z[29]+
  z[11]*z[100]))*z[282] - (1+z[30]*z[75])*z[265] - (z[16]*z[100]-z[26]*z[75])*
  z[266] - z[39]*z[75]*z[240] - (z[100]-z[12]*z[75])*z[248] - (z[15]*z[100]-
  z[28]*z[75])*z[260] - (z[16]*z[100]-z[26]*z[75])*z[252] - (lfz*z[26]*z[75]+
  z[14]*z[27]*z[35]-lfx-lfx*z[30]*z[75]-lfz*z[16]*z[100]-z[23]*z[35]*(z[29]+
  z[11]*z[100]))*z[286];
  z[343] = -z[296] - z[313];
  z[174] = z[13] - z[39]*z[74];
  z[182] = z[14] + z[30]*z[74];
  z[241] = -IDxz*z[174] - IDzz*z[182];
  z[189] = z[21] + z[16]*z[101] + z[26]*z[74];
  z[253] = -IExz*z[189] - IEzz*z[182];
  z[249] = -IExx*z[189] - IExz*z[182];
  z[212] = -rrt*z[29] - lrx*(z[101]+z[12]*z[74]) - rrt*z[11]*z[14]*(z[101]+
  z[110]) - rr*z[14]*(z[101]+z[110]+z[12]*z[74]);
  z[276] = mr*z[212];
  z[219] = lfz*z[15]*z[101] + lfz*z[28]*z[74] + z[11]*z[21]*z[35]*z[101] - 
  z[25]*z[35] - lfz*z[13]*z[16] - z[36]*(z[14]+z[30]*z[74]);
  z[279] = mf*z[219];
  z[227] = lfx*z[23] + z[29]*z[35] + z[11]*z[14]*z[35]*z[101] + z[36]*(z[21]+
  z[16]*z[101]+z[26]*z[74]) - lfx*z[15]*z[101] - lfx*z[28]*z[74];
  z[287] = mf*z[227];
  z[237] = -IDxx*z[174] - IDxz*z[182];
  z[178] = z[101] + z[12]*z[74];
  z[245] = IDyy*z[178];
  z[162] = z[101] + z[110] + z[12]*z[74];
  z[231] = ICyy*z[162];
  z[193] = z[15]*z[101] + z[28]*z[74] - z[23];
  z[257] = IEyy*z[193];
  z[207] = rr + lrx*z[14] + rrt*z[11] + lrx*z[30]*z[74] + lrz*z[39]*z[74] - 
  lrz*z[13];
  z[271] = mr*z[207];
  z[261] = IFyy*z[193];
  z[224] = lfx*z[14] + lfx*z[30]*z[74] - z[27]*z[35] - lfz*z[13]*z[15] - lfz*
  z[16]*z[101] - lfz*z[26]*z[74] - z[11]*z[23]*z[35]*z[101];
  z[284] = mf*z[224];
  z[203] = rrt*z[38] + lrz*(z[101]+z[12]*z[74]) - rrt*z[11]*z[13]*(z[101]+
  z[110]) - rr*z[13]*(z[101]+z[110]+z[12]*z[74]);
  z[267] = mr*z[203];
  z[314] = z[30]*z[79]*z[241] + z[30]*z[79]*z[253] + z[249]*(z[16]*z[105]+
  z[26]*z[79]) + z[276]*(lrx*(z[105]+z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+
  z[114])+rr*z[14]*(z[105]+z[114]+z[12]*z[79])) + z[279]*(z[30]*z[36]*z[79]-
  lfz*z[15]*z[105]-lfz*z[28]*z[79]-z[23]*z[25]*z[35]-z[21]*z[35]*(z[27]+z[11]*
  z[105])) + z[287]*(lfx*z[15]*z[105]+lfx*z[28]*z[79]+z[23]*z[29]*z[35]-z[14]*
  z[35]*(z[27]+z[11]*z[105])-z[36]*(z[16]*z[105]+z[26]*z[79])) - z[39]*z[79]*
  z[237] - z[245]*(z[105]+z[12]*z[79]) - z[231]*(z[105]+z[114]+z[12]*z[79]) - 
  z[257]*(z[15]*z[105]+z[28]*z[79]) - z[79]*z[271]*(lrx*z[30]+lrz*z[39]) - 
  z[261]*(1+z[15]*z[105]+z[28]*z[79]) - z[284]*(lfx*z[30]*z[79]-lfz*z[16]*
  z[105]-lfz*z[26]*z[79]-z[11]*z[23]*z[35]*z[105]) - z[267]*(lrz*(z[105]+
  z[12]*z[79])-rrt*z[11]*z[13]*(z[105]+z[114])-rr*z[13]*(z[105]+z[114]+z[12]*
  z[79]));
  z[306] = z[241]*(z[14]+z[30]*z[74]) + z[253]*(z[14]+z[30]*z[74]) + z[237]*(
  z[13]-z[39]*z[74]) + z[249]*(z[21]+z[16]*z[101]+z[26]*z[74]) + z[257]*(
  z[23]-z[15]*z[101]-z[28]*z[74]) + z[261]*(z[23]-z[15]*z[101]-z[28]*z[74]) + 
  z[271]*(lrz*z[13]-rr-lrx*z[14]-rrt*z[11]-lrx*z[30]*z[74]-lrz*z[39]*z[74]) + 
  z[276]*(rrt*z[29]+lrx*(z[101]+z[12]*z[74])+rrt*z[11]*z[14]*(z[101]+z[110])+
  rr*z[14]*(z[101]+z[110]+z[12]*z[74])) + z[287]*(lfx*z[15]*z[101]+lfx*z[28]*
  z[74]-lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(z[21]+z[16]*
  z[101]+z[26]*z[74])) - z[245]*(z[101]+z[12]*z[74]) - z[231]*(z[101]+z[110]+
  z[12]*z[74]) - z[267]*(rrt*z[38]+lrz*(z[101]+z[12]*z[74])-rrt*z[11]*z[13]*(
  z[101]+z[110])-rr*z[13]*(z[101]+z[110]+z[12]*z[74])) - z[279]*(lfz*z[15]*
  z[101]+lfz*z[28]*z[74]+z[11]*z[21]*z[35]*z[101]-z[25]*z[35]-lfz*z[13]*z[16]-
  z[36]*(z[14]+z[30]*z[74])) - z[284]*(lfx*z[14]+lfx*z[30]*z[74]-z[27]*z[35]-
  lfz*z[13]*z[15]-lfz*z[16]*z[101]-lfz*z[26]*z[74]-z[11]*z[23]*z[35]*z[101]);
  z[310] = z[30]*z[75]*z[241] + z[253]*(1+z[30]*z[75]) + z[245]*(z[100]-z[12]*
  z[75]) + z[231]*(z[100]+z[109]-z[12]*z[75]) + z[257]*(z[15]*z[100]-z[28]*
  z[75]) + z[261]*(z[15]*z[100]-z[28]*z[75]) + z[267]*(lrz*(z[100]-z[12]*
  z[75])-rrt*z[11]*z[13]*(z[100]+z[109])-rr*z[13]*(z[100]+z[109]-z[12]*z[75])) + 
  z[284]*(lfz*z[26]*z[75]+z[14]*z[27]*z[35]-lfx-lfx*z[30]*z[75]-lfz*z[16]*
  z[100]-z[23]*z[35]*(z[29]+z[11]*z[100])) - z[39]*z[75]*z[237] - z[75]*
  z[271]*(lrx*z[30]+lrz*z[39]) - z[249]*(z[16]*z[100]-z[26]*z[75]) - z[276]*(
  lrx*(z[100]-z[12]*z[75])+rrt*z[11]*z[14]*(z[100]+z[109])+rr*z[14]*(z[100]+
  z[109]-z[12]*z[75])) - z[287]*(lfx*z[15]*z[100]-lfx*z[28]*z[75]-z[11]*z[14]*
  z[35]*z[100]-z[36]*(z[16]*z[100]-z[26]*z[75])) - z[279]*(lfz*z[28]*z[75]-
  lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+z[30]*z[75])-z[21]*z[35]*(z[29]+
  z[11]*z[100]));
  z[345] = z[306]*z[311] - z[307]*z[310];
  z[346] = z[306]*z[312] - z[308]*z[310];
  z[348] = z[314]*z[347] + z[316]*z[345] - z[315]*z[346];
  z[355] = (z[347]*z[344]+z[349]*z[342]-z[352]*z[343])/z[348];
  u1p = z[355];
  z[350] = z[310]*z[316] - z[312]*z[314];
  z[353] = z[306]*z[316] - z[308]*z[314];
  z[356] = (z[346]*z[344]+z[350]*z[342]-z[353]*z[343])/z[348];
  u3p = -z[356];
  z[351] = z[310]*z[315] - z[311]*z[314];
  z[354] = z[306]*z[315] - z[307]*z[314];
  z[357] = (z[345]*z[344]+z[351]*z[342]-z[354]*z[343])/z[348];
  u5p = z[357];
} // eoms()

void Whipple::computeOutputs(void)
{
  z[115] = (z[13]*z[58]-z[14]*z[54])/z[81];
  z[106] = (z[13]*z[59]-z[14]*z[55])/z[81];
  z[298] = Trw*z[115] + z[292]*z[26]*z[106]*(lfz*z[15]+z[11]*z[21]*z[35]) + 
  z[291]*z[39]*(rr*z[13]*(z[106]+z[115])-lrz*z[106]-z[13]*(1-rrt*z[11]*(
  z[106]+z[115]))) - z[292]*z[28]*z[106]*(lfz*z[16]+z[11]*z[23]*z[35]) - 
  z[292]*z[30]*z[106]*(lfx*z[15]-z[16]*z[36]-z[11]*z[14]*z[35]) - z[291]*
  z[30]*(lrx*z[106]+rr*z[14]*(z[106]+z[115])-z[14]*(1-rrt*z[11]*(z[106]+
  z[115])));
  z[321] = z[16]*z[106]*z[266] + z[106]*z[248] + z[15]*z[106]*z[260] + z[16]*
  z[106]*z[252] + z[106]*(lfz*z[15]+z[11]*z[21]*z[35])*z[282] + (rr*z[13]*(
  z[106]+z[115])-lrz*z[106]-z[13]*(1-rrt*z[11]*(z[106]+z[115])))*z[270] - 
  z[15]*z[106]*z[264] - (z[106]+z[115])*z[234] - z[106]*(lfz*z[16]+z[11]*
  z[23]*z[35])*z[286] - z[106]*(lfx*z[15]-z[16]*z[36]-z[11]*z[14]*z[35])*
  z[290] - (lrx*z[106]+rr*z[14]*(z[106]+z[115])-z[14]*(1-rrt*z[11]*(z[106]+
  z[115])))*z[278];
  z[358] = -z[298] - z[321];
  z[318] = z[16]*z[106]*z[249] + z[106]*z[284]*(lfz*z[16]+z[11]*z[23]*z[35]) + 
  z[106]*z[287]*(lfx*z[15]-z[16]*z[36]-z[11]*z[14]*z[35]) + z[276]*(lrx*
  z[106]+rr*z[14]*(z[106]+z[115])-z[14]*(1-rrt*z[11]*(z[106]+z[115]))) + 
  z[267]*(rr*z[13]*(z[106]+z[115])-lrz*z[106]-z[13]*(1-rrt*z[11]*(z[106]+
  z[115]))) - z[106]*z[245] - z[15]*z[106]*z[257] - z[15]*z[106]*z[261] - 
  z[231]*(z[106]+z[115]) - z[106]*z[279]*(lfz*z[15]+z[11]*z[21]*z[35]);
  z[319] = z[16]*z[106]*z[250] + z[106]*z[283]*(lfz*z[16]+z[11]*z[23]*z[35]) + 
  z[106]*z[289]*(lfx*z[15]-z[16]*z[36]-z[11]*z[14]*z[35]) + z[275]*(lrx*
  z[106]+rr*z[14]*(z[106]+z[115])-z[14]*(1-rrt*z[11]*(z[106]+z[115]))) + 
  z[268]*(rr*z[13]*(z[106]+z[115])-lrz*z[106]-z[13]*(1-rrt*z[11]*(z[106]+
  z[115]))) - z[106]*z[247] - z[15]*z[106]*z[258] - z[15]*z[106]*z[262] - 
  z[233]*(z[106]+z[115]) - z[106]*z[280]*(lfz*z[15]+z[11]*z[21]*z[35]);
  z[320] = z[16]*z[106]*z[251] + z[106]*z[285]*(lfz*z[16]+z[11]*z[23]*z[35]) + 
  z[106]*z[288]*(lfx*z[15]-z[16]*z[36]-z[11]*z[14]*z[35]) + z[277]*(lrx*
  z[106]+rr*z[14]*(z[106]+z[115])-z[14]*(1-rrt*z[11]*(z[106]+z[115]))) + 
  z[269]*(rr*z[13]*(z[106]+z[115])-lrz*z[106]-z[13]*(1-rrt*z[11]*(z[106]+
  z[115]))) - z[106]*z[246] - z[15]*z[106]*z[259] - z[15]*z[106]*z[263] - 
  z[232]*(z[106]+z[115]) - z[106]*z[281]*(lfz*z[15]+z[11]*z[21]*z[35]);
  Rx = z[358] - z[318]*u1p - z[319]*u3p - z[320]*u5p;
  z[76] = z[11]/z[56];
  z[96] = z[29] + z[57]*z[76];
  z[86] = z[53]*z[76] - z[38];
  z[111] = (z[54]*z[96]-z[58]*z[86])/z[81];
  z[102] = (z[55]*z[96]-z[59]*z[86])/z[81];
  z[299] = Trw*z[111] + z[291]*z[11]*z[12] + z[292]*z[26]*z[102]*(lfz*z[15]+
  z[11]*z[21]*z[35]) - z[292]*z[28]*z[102]*(lfz*z[16]+z[11]*z[23]*z[35]) - 
  z[292]*z[30]*z[102]*(lfx*z[15]-z[16]*z[36]-z[11]*z[14]*z[35]) - z[291]*
  z[30]*(z[29]+lrx*z[102]+rrt*z[11]*z[14]*(z[102]+z[111])+rr*z[14]*(z[102]+
  z[111]-z[12]*z[76])) - z[291]*z[39]*(z[38]+lrz*z[102]-rrt*z[11]*z[13]*(
  z[102]+z[111])-rr*z[13]*(z[102]+z[111]-z[12]*z[76]));
  z[325] = z[30]*z[76]*z[265] + (z[16]*z[102]-z[26]*z[76])*z[266] + z[39]*
  z[76]*z[240] + (z[102]-z[12]*z[76])*z[248] + (z[15]*z[102]-z[28]*z[76])*
  z[260] + (z[16]*z[102]-z[26]*z[76])*z[252] + (z[11]-lrx*z[30]*z[76]-lrz*
  z[39]*z[76])*z[274] + (lfz*z[26]*z[76]-lfx*z[30]*z[76]-lfz*z[16]*z[102]-
  z[11]*z[23]*z[35]*z[102])*z[286] - z[11]*z[76]*z[235] - z[30]*z[76]*z[244] - 
  z[30]*z[76]*z[256] - (z[15]*z[102]-z[28]*z[76])*z[264] - (z[102]+z[111]-
  z[12]*z[76])*z[234] - (lfz*z[28]*z[76]-lfz*z[15]*z[102]-z[30]*z[36]*z[76]-
  z[11]*z[21]*z[35]*z[102])*z[282] - (lfx*z[15]*z[102]-lfx*z[28]*z[76]-z[11]*
  z[14]*z[35]*z[102]-z[36]*(z[16]*z[102]-z[26]*z[76]))*z[290] - (z[29]+lrx*(
  z[102]-z[12]*z[76])+rrt*z[11]*z[14]*(z[102]+z[111])+rr*z[14]*(z[102]+z[111]-
  z[12]*z[76]))*z[278] - (z[38]+lrz*(z[102]-z[12]*z[76])-rrt*z[11]*z[13]*(
  z[102]+z[111])-rr*z[13]*(z[102]+z[111]-z[12]*z[76]))*z[270];
  z[359] = -z[299] - z[325];
  z[322] = z[39]*z[76]*z[237] + z[249]*(z[16]*z[102]-z[26]*z[76]) + z[279]*(
  lfz*z[28]*z[76]-lfz*z[15]*z[102]-z[30]*z[36]*z[76]-z[11]*z[21]*z[35]*z[102]) + 
  z[287]*(lfx*z[15]*z[102]-lfx*z[28]*z[76]-z[11]*z[14]*z[35]*z[102]-z[36]*(
  z[16]*z[102]-z[26]*z[76])) + z[276]*(z[29]+lrx*(z[102]-z[12]*z[76])+rrt*
  z[11]*z[14]*(z[102]+z[111])+rr*z[14]*(z[102]+z[111]-z[12]*z[76])) - z[30]*
  z[76]*z[241] - z[30]*z[76]*z[253] - z[245]*(z[102]-z[12]*z[76]) - z[231]*(
  z[102]+z[111]-z[12]*z[76]) - z[257]*(z[15]*z[102]-z[28]*z[76]) - z[261]*(
  z[15]*z[102]-z[28]*z[76]) - z[271]*(z[11]-lrx*z[30]*z[76]-lrz*z[39]*z[76]) - 
  z[284]*(lfz*z[26]*z[76]-lfx*z[30]*z[76]-lfz*z[16]*z[102]-z[11]*z[23]*z[35]*
  z[102]) - z[267]*(z[38]+lrz*(z[102]-z[12]*z[76])-rrt*z[11]*z[13]*(z[102]+
  z[111])-rr*z[13]*(z[102]+z[111]-z[12]*z[76]));
  z[323] = z[39]*z[76]*z[238] + z[250]*(z[16]*z[102]-z[26]*z[76]) + z[280]*(
  lfz*z[28]*z[76]-lfz*z[15]*z[102]-z[30]*z[36]*z[76]-z[11]*z[21]*z[35]*z[102]) + 
  z[289]*(lfx*z[15]*z[102]-lfx*z[28]*z[76]-z[11]*z[14]*z[35]*z[102]-z[36]*(
  z[16]*z[102]-z[26]*z[76])) + z[275]*(z[29]+lrx*(z[102]-z[12]*z[76])+rrt*
  z[11]*z[14]*(z[102]+z[111])+rr*z[14]*(z[102]+z[111]-z[12]*z[76])) - z[30]*
  z[76]*z[242] - z[30]*z[76]*z[254] - z[247]*(z[102]-z[12]*z[76]) - z[233]*(
  z[102]+z[111]-z[12]*z[76]) - z[258]*(z[15]*z[102]-z[28]*z[76]) - z[262]*(
  z[15]*z[102]-z[28]*z[76]) - z[272]*(z[11]-lrx*z[30]*z[76]-lrz*z[39]*z[76]) - 
  z[283]*(lfz*z[26]*z[76]-lfx*z[30]*z[76]-lfz*z[16]*z[102]-z[11]*z[23]*z[35]*
  z[102]) - z[268]*(z[38]+lrz*(z[102]-z[12]*z[76])-rrt*z[11]*z[13]*(z[102]+
  z[111])-rr*z[13]*(z[102]+z[111]-z[12]*z[76]));
  z[324] = z[39]*z[76]*z[239] + z[251]*(z[16]*z[102]-z[26]*z[76]) + z[281]*(
  lfz*z[28]*z[76]-lfz*z[15]*z[102]-z[30]*z[36]*z[76]-z[11]*z[21]*z[35]*z[102]) + 
  z[288]*(lfx*z[15]*z[102]-lfx*z[28]*z[76]-z[11]*z[14]*z[35]*z[102]-z[36]*(
  z[16]*z[102]-z[26]*z[76])) + z[277]*(z[29]+lrx*(z[102]-z[12]*z[76])+rrt*
  z[11]*z[14]*(z[102]+z[111])+rr*z[14]*(z[102]+z[111]-z[12]*z[76])) - z[30]*
  z[76]*z[243] - z[30]*z[76]*z[255] - z[246]*(z[102]-z[12]*z[76]) - z[232]*(
  z[102]+z[111]-z[12]*z[76]) - z[259]*(z[15]*z[102]-z[28]*z[76]) - z[263]*(
  z[15]*z[102]-z[28]*z[76]) - z[273]*(z[11]-lrx*z[30]*z[76]-lrz*z[39]*z[76]) - 
  z[285]*(lfz*z[26]*z[76]-lfx*z[30]*z[76]-lfz*z[16]*z[102]-z[11]*z[23]*z[35]*
  z[102]) - z[269]*(z[38]+lrz*(z[102]-z[12]*z[76])-rrt*z[11]*z[13]*(z[102]+
  z[111])-rr*z[13]*(z[102]+z[111]-z[12]*z[76]));
  Ry = z[359] - z[322]*u1p - z[323]*u3p - z[324]*u5p;
  z[80] = z[12]/z[56];
  z[95] = z[57]*z[80] - z[30];
  z[83] = z[39] + z[53]*z[80];
  z[108] = (z[54]*z[95]-z[58]*z[83])/z[81];
  z[99] = (z[55]*z[95]-z[59]*z[83])/z[81];
  z[300] = Trw*z[108] + z[291]*pow(z[12],2) + z[292]*z[26]*z[99]*(lfz*z[15]+
  z[11]*z[21]*z[35]) + z[291]*z[30]*(z[30]-lrx*z[99]-rrt*z[11]*z[14]*(z[99]+
  z[108])-rr*z[14]*(z[99]+z[108]-z[12]*z[80])) - z[292]*z[28]*z[99]*(lfz*
  z[16]+z[11]*z[23]*z[35]) - z[292]*z[30]*z[99]*(lfx*z[15]-z[16]*z[36]-z[11]*
  z[14]*z[35]) - z[291]*z[39]*(lrz*z[99]-z[39]-rrt*z[11]*z[13]*(z[99]+z[108])-
  rr*z[13]*(z[99]+z[108]-z[12]*z[80]));
  z[329] = z[30]*z[80]*z[265] + (z[16]*z[99]-z[26]*z[80])*z[266] + z[39]*
  z[80]*z[240] + (z[99]-z[12]*z[80])*z[248] + (z[15]*z[99]-z[28]*z[80])*
  z[260] + (z[16]*z[99]-z[26]*z[80])*z[252] + (z[12]-lrx*z[30]*z[80]-lrz*
  z[39]*z[80])*z[274] + (lfz*z[26]*z[80]-lfx*z[30]*z[80]-lfz*z[16]*z[99]-
  z[11]*z[23]*z[35]*z[99])*z[286] + (z[30]-lrx*(z[99]-z[12]*z[80])-rrt*z[11]*
  z[14]*(z[99]+z[108])-rr*z[14]*(z[99]+z[108]-z[12]*z[80]))*z[278] - z[11]*
  z[80]*z[235] - z[30]*z[80]*z[244] - z[30]*z[80]*z[256] - (z[15]*z[99]-z[28]*
  z[80])*z[264] - (z[99]+z[108]-z[12]*z[80])*z[234] - (lfz*z[28]*z[80]-lfz*
  z[15]*z[99]-z[30]*z[36]*z[80]-z[11]*z[21]*z[35]*z[99])*z[282] - (lfx*z[15]*
  z[99]-lfx*z[28]*z[80]-z[11]*z[14]*z[35]*z[99]-z[36]*(z[16]*z[99]-z[26]*
  z[80]))*z[290] - (lrz*(z[99]-z[12]*z[80])-z[39]-rrt*z[11]*z[13]*(z[99]+
  z[108])-rr*z[13]*(z[99]+z[108]-z[12]*z[80]))*z[270];
  z[360] = -z[300] - z[329];
  z[326] = z[39]*z[80]*z[237] + z[249]*(z[16]*z[99]-z[26]*z[80]) + z[279]*(
  lfz*z[28]*z[80]-lfz*z[15]*z[99]-z[30]*z[36]*z[80]-z[11]*z[21]*z[35]*z[99]) + 
  z[287]*(lfx*z[15]*z[99]-lfx*z[28]*z[80]-z[11]*z[14]*z[35]*z[99]-z[36]*(
  z[16]*z[99]-z[26]*z[80])) - z[30]*z[80]*z[241] - z[30]*z[80]*z[253] - 
  z[245]*(z[99]-z[12]*z[80]) - z[231]*(z[99]+z[108]-z[12]*z[80]) - z[257]*(
  z[15]*z[99]-z[28]*z[80]) - z[261]*(z[15]*z[99]-z[28]*z[80]) - z[271]*(z[12]-
  lrx*z[30]*z[80]-lrz*z[39]*z[80]) - z[284]*(lfz*z[26]*z[80]-lfx*z[30]*z[80]-
  lfz*z[16]*z[99]-z[11]*z[23]*z[35]*z[99]) - z[267]*(lrz*(z[99]-z[12]*z[80])-
  z[39]-rrt*z[11]*z[13]*(z[99]+z[108])-rr*z[13]*(z[99]+z[108]-z[12]*z[80])) - 
  z[276]*(z[30]-lrx*(z[99]-z[12]*z[80])-rrt*z[11]*z[14]*(z[99]+z[108])-rr*
  z[14]*(z[99]+z[108]-z[12]*z[80]));
  z[327] = z[39]*z[80]*z[238] + z[250]*(z[16]*z[99]-z[26]*z[80]) + z[280]*(
  lfz*z[28]*z[80]-lfz*z[15]*z[99]-z[30]*z[36]*z[80]-z[11]*z[21]*z[35]*z[99]) + 
  z[289]*(lfx*z[15]*z[99]-lfx*z[28]*z[80]-z[11]*z[14]*z[35]*z[99]-z[36]*(
  z[16]*z[99]-z[26]*z[80])) - z[30]*z[80]*z[242] - z[30]*z[80]*z[254] - 
  z[247]*(z[99]-z[12]*z[80]) - z[233]*(z[99]+z[108]-z[12]*z[80]) - z[258]*(
  z[15]*z[99]-z[28]*z[80]) - z[262]*(z[15]*z[99]-z[28]*z[80]) - z[272]*(z[12]-
  lrx*z[30]*z[80]-lrz*z[39]*z[80]) - z[283]*(lfz*z[26]*z[80]-lfx*z[30]*z[80]-
  lfz*z[16]*z[99]-z[11]*z[23]*z[35]*z[99]) - z[268]*(lrz*(z[99]-z[12]*z[80])-
  z[39]-rrt*z[11]*z[13]*(z[99]+z[108])-rr*z[13]*(z[99]+z[108]-z[12]*z[80])) - 
  z[275]*(z[30]-lrx*(z[99]-z[12]*z[80])-rrt*z[11]*z[14]*(z[99]+z[108])-rr*
  z[14]*(z[99]+z[108]-z[12]*z[80]));
  z[328] = z[39]*z[80]*z[239] + z[251]*(z[16]*z[99]-z[26]*z[80]) + z[281]*(
  lfz*z[28]*z[80]-lfz*z[15]*z[99]-z[30]*z[36]*z[80]-z[11]*z[21]*z[35]*z[99]) + 
  z[288]*(lfx*z[15]*z[99]-lfx*z[28]*z[80]-z[11]*z[14]*z[35]*z[99]-z[36]*(
  z[16]*z[99]-z[26]*z[80])) - z[30]*z[80]*z[243] - z[30]*z[80]*z[255] - 
  z[246]*(z[99]-z[12]*z[80]) - z[232]*(z[99]+z[108]-z[12]*z[80]) - z[259]*(
  z[15]*z[99]-z[28]*z[80]) - z[263]*(z[15]*z[99]-z[28]*z[80]) - z[273]*(z[12]-
  lrx*z[30]*z[80]-lrz*z[39]*z[80]) - z[285]*(lfz*z[26]*z[80]-lfx*z[30]*z[80]-
  lfz*z[16]*z[99]-z[11]*z[23]*z[35]*z[99]) - z[269]*(lrz*(z[99]-z[12]*z[80])-
  z[39]-rrt*z[11]*z[13]*(z[99]+z[108])-rr*z[13]*(z[99]+z[108]-z[12]*z[80])) - 
  z[277]*(z[30]-lrx*(z[99]-z[12]*z[80])-rrt*z[11]*z[14]*(z[99]+z[108])-rr*
  z[14]*(z[99]+z[108]-z[12]*z[80]));
  Rz = z[360] - z[326]*u1p - z[327]*u3p - z[328]*u5p;
  z[294] = z[23]*z[33];
  z[293] = z[27]*z[33];
  z[301] = z[33]*(z[23]*z[294]+z[27]*z[293]);
  z[72] = z[33]*(z[14]*z[27]-z[23]*z[29]);
  z[68] = z[11]*z[23]*z[33];
  z[78] = z[68]/z[56];
  z[92] = z[72] - z[57]*z[78];
  z[63] = z[33]*(z[13]*z[27]+z[23]*z[38]);
  z[88] = z[63] - z[53]*z[78];
  z[113] = (z[54]*z[92]-z[58]*z[88])/z[81];
  z[104] = (z[55]*z[92]-z[59]*z[88])/z[81];
  z[303] = Trw*z[113] + z[292]*z[26]*(lfz*z[15]*z[104]+z[23]*z[25]*z[33]+
  z[21]*(z[27]*z[33]+z[11]*z[35]*z[104])) - z[292]*z[28]*z[104]*(lfz*z[16]+
  z[11]*z[23]*z[35]) - z[291]*z[30]*(lrx*z[104]+rrt*z[11]*z[14]*(z[104]+
  z[113])+rr*z[14]*(z[104]+z[113]+z[12]*z[78])) - z[291]*z[39]*(lrz*z[104]-
  rrt*z[11]*z[13]*(z[104]+z[113])-rr*z[13]*(z[104]+z[113]+z[12]*z[78])) - 
  z[292]*z[30]*(lfx*z[15]*z[104]+z[23]*z[29]*z[33]-z[16]*z[36]*z[104]-z[14]*(
  z[27]*z[33]+z[11]*z[35]*z[104]));
  z[333] = z[11]*z[78]*z[235] + (z[16]*z[104]+z[26]*z[78])*z[266] + z[30]*
  z[78]*z[244] + z[30]*z[78]*z[256] + (z[104]+z[12]*z[78])*z[248] + (z[15]*
  z[104]+z[28]*z[78])*z[260] + (z[16]*z[104]+z[26]*z[78])*z[252] + z[78]*(lrx*
  z[30]+lrz*z[39])*z[274] + (lfx*z[30]*z[78]-lfz*z[16]*z[104]-lfz*z[26]*z[78]-
  z[11]*z[23]*z[35]*z[104])*z[286] - z[30]*z[78]*z[265] - z[39]*z[78]*z[240] - (
  z[15]*z[104]+z[28]*z[78])*z[264] - (z[104]+z[113]+z[12]*z[78])*z[234] - (
  lrx*(z[104]+z[12]*z[78])+rrt*z[11]*z[14]*(z[104]+z[113])+rr*z[14]*(z[104]+
  z[113]+z[12]*z[78]))*z[278] - (lrz*(z[104]+z[12]*z[78])-rrt*z[11]*z[13]*(
  z[104]+z[113])-rr*z[13]*(z[104]+z[113]+z[12]*z[78]))*z[270] - (z[30]*z[36]*
  z[78]-lfz*z[15]*z[104]-lfz*z[28]*z[78]-z[23]*z[25]*z[33]-z[21]*(z[27]*z[33]+
  z[11]*z[35]*z[104]))*z[282] - (lfx*z[15]*z[104]+lfx*z[28]*z[78]+z[23]*z[29]*
  z[33]-z[36]*(z[16]*z[104]+z[26]*z[78])-z[14]*(z[27]*z[33]+z[11]*z[35]*
  z[104]))*z[290];
  z[361] = -z[303] - z[333];
  z[302] = z[33]*(z[23]*z[293]-z[27]*z[294]);
  z[71] = z[33]*(z[14]*z[23]+z[27]*z[29]);
  z[67] = z[11]*z[27]*z[33];
  z[77] = z[67]/z[56];
  z[91] = -z[71] - z[57]*z[77];
  z[62] = z[33]*(z[13]*z[23]-z[27]*z[38]);
  z[87] = -z[62] - z[53]*z[77];
  z[112] = (z[54]*z[91]-z[58]*z[87])/z[81];
  z[103] = (z[55]*z[91]-z[59]*z[87])/z[81];
  z[304] = Trw*z[112] + z[292]*z[26]*(lfz*z[15]*z[103]+z[25]*z[27]*z[33]-
  z[21]*(z[23]*z[33]-z[11]*z[35]*z[103])) + z[292]*z[30]*(z[16]*z[36]*z[103]-
  lfx*z[15]*z[103]-z[27]*z[29]*z[33]-z[14]*(z[23]*z[33]-z[11]*z[35]*z[103])) - 
  z[291]*z[30]*(lrx*z[103]+rrt*z[11]*z[14]*(z[103]+z[112])+rr*z[14]*(z[103]+
  z[112]+z[12]*z[77])) - z[292]*z[28]*(lfz*z[16]*z[103]-z[33]*pow(z[27],2)-
  z[23]*(z[23]*z[33]-z[11]*z[35]*z[103])) - z[291]*z[39]*(lrz*z[103]-rrt*
  z[11]*z[13]*(z[103]+z[112])-rr*z[13]*(z[103]+z[112]+z[12]*z[77]));
  z[337] = z[11]*z[77]*z[235] + (z[16]*z[103]+z[26]*z[77])*z[266] + z[30]*
  z[77]*z[244] + z[30]*z[77]*z[256] + (z[103]+z[12]*z[77])*z[248] + (z[15]*
  z[103]+z[28]*z[77])*z[260] + (z[16]*z[103]+z[26]*z[77])*z[252] + z[77]*(lrx*
  z[30]+lrz*z[39])*z[274] + (lfz*z[15]*z[103]+lfz*z[28]*z[77]+z[25]*z[27]*
  z[33]-z[30]*z[36]*z[77]-z[21]*(z[23]*z[33]-z[11]*z[35]*z[103]))*z[282] + (
  z[36]*(z[16]*z[103]+z[26]*z[77])-lfx*z[15]*z[103]-lfx*z[28]*z[77]-z[27]*
  z[29]*z[33]-z[14]*(z[23]*z[33]-z[11]*z[35]*z[103]))*z[290] - z[30]*z[77]*
  z[265] - z[39]*z[77]*z[240] - (z[15]*z[103]+z[28]*z[77])*z[264] - (z[103]+
  z[112]+z[12]*z[77])*z[234] - (lrx*(z[103]+z[12]*z[77])+rrt*z[11]*z[14]*(
  z[103]+z[112])+rr*z[14]*(z[103]+z[112]+z[12]*z[77]))*z[278] - (lrz*(z[103]+
  z[12]*z[77])-rrt*z[11]*z[13]*(z[103]+z[112])-rr*z[13]*(z[103]+z[112]+z[12]*
  z[77]))*z[270] - (lfz*z[16]*z[103]+lfz*z[26]*z[77]-lfx*z[30]*z[77]-z[33]*
  pow(z[27],2)-z[23]*(z[23]*z[33]-z[11]*z[35]*z[103]))*z[286];
  z[362] = -z[304] - z[337];
  z[364] = pow(z[301],2) + pow(z[302],2);
  z[368] = (z[301]*z[361]-z[302]*z[362])/z[364];
  z[330] = z[30]*z[78]*z[241] + z[30]*z[78]*z[253] + z[249]*(z[16]*z[104]+
  z[26]*z[78]) + z[276]*(lrx*(z[104]+z[12]*z[78])+rrt*z[11]*z[14]*(z[104]+
  z[113])+rr*z[14]*(z[104]+z[113]+z[12]*z[78])) + z[279]*(z[30]*z[36]*z[78]-
  lfz*z[15]*z[104]-lfz*z[28]*z[78]-z[23]*z[25]*z[33]-z[21]*(z[27]*z[33]+z[11]*
  z[35]*z[104])) + z[287]*(lfx*z[15]*z[104]+lfx*z[28]*z[78]+z[23]*z[29]*z[33]-
  z[36]*(z[16]*z[104]+z[26]*z[78])-z[14]*(z[27]*z[33]+z[11]*z[35]*z[104])) - 
  z[39]*z[78]*z[237] - z[245]*(z[104]+z[12]*z[78]) - z[231]*(z[104]+z[113]+
  z[12]*z[78]) - z[257]*(z[15]*z[104]+z[28]*z[78]) - z[261]*(z[15]*z[104]+
  z[28]*z[78]) - z[78]*z[271]*(lrx*z[30]+lrz*z[39]) - z[284]*(lfx*z[30]*z[78]-
  lfz*z[16]*z[104]-lfz*z[26]*z[78]-z[11]*z[23]*z[35]*z[104]) - z[267]*(lrz*(
  z[104]+z[12]*z[78])-rrt*z[11]*z[13]*(z[104]+z[113])-rr*z[13]*(z[104]+z[113]+
  z[12]*z[78]));
  z[334] = z[30]*z[77]*z[241] + z[30]*z[77]*z[253] + z[249]*(z[16]*z[103]+
  z[26]*z[77]) + z[276]*(lrx*(z[103]+z[12]*z[77])+rrt*z[11]*z[14]*(z[103]+
  z[112])+rr*z[14]*(z[103]+z[112]+z[12]*z[77])) + z[284]*(lfz*z[16]*z[103]+
  lfz*z[26]*z[77]-lfx*z[30]*z[77]-z[33]*pow(z[27],2)-z[23]*(z[23]*z[33]-z[11]*
  z[35]*z[103])) - z[39]*z[77]*z[237] - z[245]*(z[103]+z[12]*z[77]) - z[231]*(
  z[103]+z[112]+z[12]*z[77]) - z[257]*(z[15]*z[103]+z[28]*z[77]) - z[261]*(
  z[15]*z[103]+z[28]*z[77]) - z[77]*z[271]*(lrx*z[30]+lrz*z[39]) - z[267]*(
  lrz*(z[103]+z[12]*z[77])-rrt*z[11]*z[13]*(z[103]+z[112])-rr*z[13]*(z[103]+
  z[112]+z[12]*z[77])) - z[279]*(lfz*z[15]*z[103]+lfz*z[28]*z[77]+z[25]*z[27]*
  z[33]-z[30]*z[36]*z[77]-z[21]*(z[23]*z[33]-z[11]*z[35]*z[103])) - z[287]*(
  z[36]*(z[16]*z[103]+z[26]*z[77])-lfx*z[15]*z[103]-lfx*z[28]*z[77]-z[27]*
  z[29]*z[33]-z[14]*(z[23]*z[33]-z[11]*z[35]*z[103]));
  z[365] = (z[301]*z[330]-z[302]*z[334])/z[364];
  z[331] = z[30]*z[78]*z[242] + z[30]*z[78]*z[254] + z[250]*(z[16]*z[104]+
  z[26]*z[78]) + z[275]*(lrx*(z[104]+z[12]*z[78])+rrt*z[11]*z[14]*(z[104]+
  z[113])+rr*z[14]*(z[104]+z[113]+z[12]*z[78])) + z[280]*(z[30]*z[36]*z[78]-
  lfz*z[15]*z[104]-lfz*z[28]*z[78]-z[23]*z[25]*z[33]-z[21]*(z[27]*z[33]+z[11]*
  z[35]*z[104])) + z[289]*(lfx*z[15]*z[104]+lfx*z[28]*z[78]+z[23]*z[29]*z[33]-
  z[36]*(z[16]*z[104]+z[26]*z[78])-z[14]*(z[27]*z[33]+z[11]*z[35]*z[104])) - 
  z[39]*z[78]*z[238] - z[247]*(z[104]+z[12]*z[78]) - z[233]*(z[104]+z[113]+
  z[12]*z[78]) - z[258]*(z[15]*z[104]+z[28]*z[78]) - z[262]*(z[15]*z[104]+
  z[28]*z[78]) - z[78]*z[272]*(lrx*z[30]+lrz*z[39]) - z[283]*(lfx*z[30]*z[78]-
  lfz*z[16]*z[104]-lfz*z[26]*z[78]-z[11]*z[23]*z[35]*z[104]) - z[268]*(lrz*(
  z[104]+z[12]*z[78])-rrt*z[11]*z[13]*(z[104]+z[113])-rr*z[13]*(z[104]+z[113]+
  z[12]*z[78]));
  z[335] = z[30]*z[77]*z[242] + z[30]*z[77]*z[254] + z[250]*(z[16]*z[103]+
  z[26]*z[77]) + z[275]*(lrx*(z[103]+z[12]*z[77])+rrt*z[11]*z[14]*(z[103]+
  z[112])+rr*z[14]*(z[103]+z[112]+z[12]*z[77])) + z[283]*(lfz*z[16]*z[103]+
  lfz*z[26]*z[77]-lfx*z[30]*z[77]-z[33]*pow(z[27],2)-z[23]*(z[23]*z[33]-z[11]*
  z[35]*z[103])) - z[39]*z[77]*z[238] - z[247]*(z[103]+z[12]*z[77]) - z[233]*(
  z[103]+z[112]+z[12]*z[77]) - z[258]*(z[15]*z[103]+z[28]*z[77]) - z[262]*(
  z[15]*z[103]+z[28]*z[77]) - z[77]*z[272]*(lrx*z[30]+lrz*z[39]) - z[268]*(
  lrz*(z[103]+z[12]*z[77])-rrt*z[11]*z[13]*(z[103]+z[112])-rr*z[13]*(z[103]+
  z[112]+z[12]*z[77])) - z[280]*(lfz*z[15]*z[103]+lfz*z[28]*z[77]+z[25]*z[27]*
  z[33]-z[30]*z[36]*z[77]-z[21]*(z[23]*z[33]-z[11]*z[35]*z[103])) - z[289]*(
  z[36]*(z[16]*z[103]+z[26]*z[77])-lfx*z[15]*z[103]-lfx*z[28]*z[77]-z[27]*
  z[29]*z[33]-z[14]*(z[23]*z[33]-z[11]*z[35]*z[103]));
  z[366] = (z[301]*z[331]-z[302]*z[335])/z[364];
  z[332] = z[30]*z[78]*z[243] + z[30]*z[78]*z[255] + z[251]*(z[16]*z[104]+
  z[26]*z[78]) + z[277]*(lrx*(z[104]+z[12]*z[78])+rrt*z[11]*z[14]*(z[104]+
  z[113])+rr*z[14]*(z[104]+z[113]+z[12]*z[78])) + z[281]*(z[30]*z[36]*z[78]-
  lfz*z[15]*z[104]-lfz*z[28]*z[78]-z[23]*z[25]*z[33]-z[21]*(z[27]*z[33]+z[11]*
  z[35]*z[104])) + z[288]*(lfx*z[15]*z[104]+lfx*z[28]*z[78]+z[23]*z[29]*z[33]-
  z[36]*(z[16]*z[104]+z[26]*z[78])-z[14]*(z[27]*z[33]+z[11]*z[35]*z[104])) - 
  z[39]*z[78]*z[239] - z[246]*(z[104]+z[12]*z[78]) - z[232]*(z[104]+z[113]+
  z[12]*z[78]) - z[259]*(z[15]*z[104]+z[28]*z[78]) - z[263]*(z[15]*z[104]+
  z[28]*z[78]) - z[78]*z[273]*(lrx*z[30]+lrz*z[39]) - z[285]*(lfx*z[30]*z[78]-
  lfz*z[16]*z[104]-lfz*z[26]*z[78]-z[11]*z[23]*z[35]*z[104]) - z[269]*(lrz*(
  z[104]+z[12]*z[78])-rrt*z[11]*z[13]*(z[104]+z[113])-rr*z[13]*(z[104]+z[113]+
  z[12]*z[78]));
  z[336] = z[30]*z[77]*z[243] + z[30]*z[77]*z[255] + z[251]*(z[16]*z[103]+
  z[26]*z[77]) + z[277]*(lrx*(z[103]+z[12]*z[77])+rrt*z[11]*z[14]*(z[103]+
  z[112])+rr*z[14]*(z[103]+z[112]+z[12]*z[77])) + z[285]*(lfz*z[16]*z[103]+
  lfz*z[26]*z[77]-lfx*z[30]*z[77]-z[33]*pow(z[27],2)-z[23]*(z[23]*z[33]-z[11]*
  z[35]*z[103])) - z[39]*z[77]*z[239] - z[246]*(z[103]+z[12]*z[77]) - z[232]*(
  z[103]+z[112]+z[12]*z[77]) - z[259]*(z[15]*z[103]+z[28]*z[77]) - z[263]*(
  z[15]*z[103]+z[28]*z[77]) - z[77]*z[273]*(lrx*z[30]+lrz*z[39]) - z[269]*(
  lrz*(z[103]+z[12]*z[77])-rrt*z[11]*z[13]*(z[103]+z[112])-rr*z[13]*(z[103]+
  z[112]+z[12]*z[77])) - z[281]*(lfz*z[15]*z[103]+lfz*z[28]*z[77]+z[25]*z[27]*
  z[33]-z[30]*z[36]*z[77]-z[21]*(z[23]*z[33]-z[11]*z[35]*z[103])) - z[288]*(
  z[36]*(z[16]*z[103]+z[26]*z[77])-lfx*z[15]*z[103]-lfx*z[28]*z[77]-z[27]*
  z[29]*z[33]-z[14]*(z[23]*z[33]-z[11]*z[35]*z[103]));
  z[367] = (z[301]*z[332]-z[302]*z[336])/z[364];
  Fx = z[368] - z[365]*u1p - z[366]*u3p - z[367]*u5p;
  z[372] = (z[301]*z[362]+z[302]*z[361])/z[364];
  z[369] = (z[301]*z[334]+z[302]*z[330])/z[364];
  z[370] = (z[301]*z[335]+z[302]*z[331])/z[364];
  z[371] = (z[301]*z[336]+z[302]*z[332])/z[364];
  Fy = z[372] - z[369]*u1p - z[370]*u3p - z[371]*u5p;
  z[94] = z[30] - z[57]*z[80];
  z[82] = -z[39] - z[53]*z[80];
  z[107] = (z[54]*z[94]-z[58]*z[82])/z[81];
  z[98] = (z[55]*z[94]-z[59]*z[82])/z[81];
  z[305] = Trw*z[107] + z[292]*z[26]*(z[26]+lfz*z[15]*z[98]+z[11]*z[21]*z[35]*
  z[98]) + z[292]*z[28]*(z[28]-lfz*z[16]*z[98]-z[11]*z[23]*z[35]*z[98]) - 
  z[292]*z[30]*(lfx*z[15]*z[98]-z[30]-z[16]*z[36]*z[98]-z[11]*z[14]*z[35]*
  z[98]) - z[291]*z[30]*(lrx*z[98]+rrt*z[11]*z[14]*(z[98]+z[107])+rr*z[14]*(
  z[98]+z[107]+z[12]*z[80])) - z[291]*z[39]*(lrz*z[98]-rrt*z[11]*z[13]*(z[98]+
  z[107])-rr*z[13]*(z[98]+z[107]+z[12]*z[80]));
  z[341] = z[11]*z[80]*z[235] + (z[16]*z[98]+z[26]*z[80])*z[266] + z[30]*
  z[80]*z[244] + z[30]*z[80]*z[256] + (z[98]+z[12]*z[80])*z[248] + (z[15]*
  z[98]+z[28]*z[80])*z[260] + (z[16]*z[98]+z[26]*z[80])*z[252] + z[80]*(lrx*
  z[30]+lrz*z[39])*z[274] + (z[28]+lfx*z[30]*z[80]-lfz*z[16]*z[98]-lfz*z[26]*
  z[80]-z[11]*z[23]*z[35]*z[98])*z[286] - z[30]*z[80]*z[265] - z[39]*z[80]*
  z[240] - (z[15]*z[98]+z[28]*z[80])*z[264] - (z[98]+z[107]+z[12]*z[80])*
  z[234] - (lrx*(z[98]+z[12]*z[80])+rrt*z[11]*z[14]*(z[98]+z[107])+rr*z[14]*(
  z[98]+z[107]+z[12]*z[80]))*z[278] - (z[30]*z[36]*z[80]-z[26]-lfz*z[15]*
  z[98]-lfz*z[28]*z[80]-z[11]*z[21]*z[35]*z[98])*z[282] - (lfx*z[15]*z[98]+
  lfx*z[28]*z[80]-z[30]-z[11]*z[14]*z[35]*z[98]-z[36]*(z[16]*z[98]+z[26]*
  z[80]))*z[290] - (lrz*(z[98]+z[12]*z[80])-rrt*z[11]*z[13]*(z[98]+z[107])-rr*
  z[13]*(z[98]+z[107]+z[12]*z[80]))*z[270];
  z[363] = -z[305] - z[341];
  z[338] = z[30]*z[80]*z[241] + z[30]*z[80]*z[253] + z[249]*(z[16]*z[98]+
  z[26]*z[80]) + z[276]*(lrx*(z[98]+z[12]*z[80])+rrt*z[11]*z[14]*(z[98]+
  z[107])+rr*z[14]*(z[98]+z[107]+z[12]*z[80])) + z[279]*(z[30]*z[36]*z[80]-
  z[26]-lfz*z[15]*z[98]-lfz*z[28]*z[80]-z[11]*z[21]*z[35]*z[98]) + z[287]*(
  lfx*z[15]*z[98]+lfx*z[28]*z[80]-z[30]-z[11]*z[14]*z[35]*z[98]-z[36]*(z[16]*
  z[98]+z[26]*z[80])) - z[39]*z[80]*z[237] - z[245]*(z[98]+z[12]*z[80]) - 
  z[231]*(z[98]+z[107]+z[12]*z[80]) - z[257]*(z[15]*z[98]+z[28]*z[80]) - 
  z[261]*(z[15]*z[98]+z[28]*z[80]) - z[80]*z[271]*(lrx*z[30]+lrz*z[39]) - 
  z[284]*(z[28]+lfx*z[30]*z[80]-lfz*z[16]*z[98]-lfz*z[26]*z[80]-z[11]*z[23]*
  z[35]*z[98]) - z[267]*(lrz*(z[98]+z[12]*z[80])-rrt*z[11]*z[13]*(z[98]+
  z[107])-rr*z[13]*(z[98]+z[107]+z[12]*z[80]));
  z[339] = z[30]*z[80]*z[242] + z[30]*z[80]*z[254] + z[250]*(z[16]*z[98]+
  z[26]*z[80]) + z[275]*(lrx*(z[98]+z[12]*z[80])+rrt*z[11]*z[14]*(z[98]+
  z[107])+rr*z[14]*(z[98]+z[107]+z[12]*z[80])) + z[280]*(z[30]*z[36]*z[80]-
  z[26]-lfz*z[15]*z[98]-lfz*z[28]*z[80]-z[11]*z[21]*z[35]*z[98]) + z[289]*(
  lfx*z[15]*z[98]+lfx*z[28]*z[80]-z[30]-z[11]*z[14]*z[35]*z[98]-z[36]*(z[16]*
  z[98]+z[26]*z[80])) - z[39]*z[80]*z[238] - z[247]*(z[98]+z[12]*z[80]) - 
  z[233]*(z[98]+z[107]+z[12]*z[80]) - z[258]*(z[15]*z[98]+z[28]*z[80]) - 
  z[262]*(z[15]*z[98]+z[28]*z[80]) - z[80]*z[272]*(lrx*z[30]+lrz*z[39]) - 
  z[283]*(z[28]+lfx*z[30]*z[80]-lfz*z[16]*z[98]-lfz*z[26]*z[80]-z[11]*z[23]*
  z[35]*z[98]) - z[268]*(lrz*(z[98]+z[12]*z[80])-rrt*z[11]*z[13]*(z[98]+
  z[107])-rr*z[13]*(z[98]+z[107]+z[12]*z[80]));
  z[340] = z[30]*z[80]*z[243] + z[30]*z[80]*z[255] + z[251]*(z[16]*z[98]+
  z[26]*z[80]) + z[277]*(lrx*(z[98]+z[12]*z[80])+rrt*z[11]*z[14]*(z[98]+
  z[107])+rr*z[14]*(z[98]+z[107]+z[12]*z[80])) + z[281]*(z[30]*z[36]*z[80]-
  z[26]-lfz*z[15]*z[98]-lfz*z[28]*z[80]-z[11]*z[21]*z[35]*z[98]) + z[288]*(
  lfx*z[15]*z[98]+lfx*z[28]*z[80]-z[30]-z[11]*z[14]*z[35]*z[98]-z[36]*(z[16]*
  z[98]+z[26]*z[80])) - z[39]*z[80]*z[239] - z[246]*(z[98]+z[12]*z[80]) - 
  z[232]*(z[98]+z[107]+z[12]*z[80]) - z[259]*(z[15]*z[98]+z[28]*z[80]) - 
  z[263]*(z[15]*z[98]+z[28]*z[80]) - z[80]*z[273]*(lrx*z[30]+lrz*z[39]) - 
  z[285]*(z[28]+lfx*z[30]*z[80]-lfz*z[16]*z[98]-lfz*z[26]*z[80]-z[11]*z[23]*
  z[35]*z[98]) - z[269]*(lrz*(z[98]+z[12]*z[80])-rrt*z[11]*z[13]*(z[98]+
  z[107])-rr*z[13]*(z[98]+z[107]+z[12]*z[80]));
  Fz = z[363] - z[338]*u1p - z[339]*u3p - z[340]*u5p;
  fa_yaw = q0 + asin(z[23]*z[33]);
  fa_lean = asin(z[28]);
  fa_pitch = asin(z[33]*(z[14]*z[27]-z[23]*z[29]));
  z[37] = -rrt - z[35];
  z[45] = z[40] + pow(z[36],2) + pow(z[37],2) + 2*z[41]*z[15] + 2*z[42]*z[22] + 
  2*z[43]*z[14] + 2*lf*z[26]*z[37] + 2*lr*z[16]*z[36] + 2*ls*z[30]*z[37] + 2*
  rr*z[24]*z[36] - 2*z[44]*z[13] - 2*lr*z[37]*z[39] - 2*rr*z[11]*z[37] - 2*
  z[28]*z[36]*z[37];
  z[46] = pow(z[45],0.5);
  z[47] = z[37]/z[46];
  z[48] = rr/z[46];
  z[49] = lr/z[46];
  z[50] = lf/z[46];
  z[51] = ls/z[46];
  z[52] = z[36]/z[46];
  ke = 0.5*IDxx*pow((z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5)),2) + 0.5*
  IDyy*pow((z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)),
  2) + 0.5*IExx*pow((z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(
  z[100]*u3-z[101]*u1-z[105]*u5)),2) + 0.5*IEyy*pow((z[28]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)),2) + 0.5*IFyy*
  pow((u5+z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*
  u1-z[105]*u5)),2) + 0.5*ICyy*pow((z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-
  z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)),2) + 0.5*(z[14]*u1+
  z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(IDzz*(z[14]*u1+z[30]*(z[74]*u1+z[75]*
  u3+z[79]*u5))+2*IDxz*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))) + 0.5*(
  u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(IEzz*(u3+z[14]*u1+z[30]*(
  z[74]*u1+z[75]*u3+z[79]*u5))+2*IExz*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))) + 0.5*mr*(pow((lrz*z[13]*
  u1-rr*u1-lrx*z[14]*u1-rrt*z[11]*u1-lrx*z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  lrz*z[39]*(z[74]*u1+z[75]*u3+z[79]*u5)),2)+pow((lrz*(z[100]*u3-z[101]*u1-
  z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))-rrt*z[38]*u1-rrt*z[11]*z[13]*(
  z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)-rr*z[13]*(
  z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+
  z[75]*u3+z[79]*u5))),2)+pow((rrt*z[29]*u1-lrx*(z[100]*u3-z[101]*u1-z[105]*
  u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))-rrt*z[11]*z[14]*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)-rr*z[14]*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))),
  2)) + 0.5*mf*(pow((lfz*z[13]*z[15]*u1+z[27]*z[35]*(u1+z[14]*u3)+lfz*z[26]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-lfx*u3-lfx*z[14]*u1-lfx*z[30]*(z[74]*u1+z[75]*
  u3+z[79]*u5)-lfz*z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[23]*z[35]*(z[29]*
  u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))),2)+pow((lfz*z[13]*z[16]*u1+z[25]*
  z[35]*(u1+z[14]*u3-z[23]*u5)+lfz*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)+
  z[36]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-lfz*z[28]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[21]*z[35]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*
  u1-z[105]*u5))),2)+pow((lfx*z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*z[23]*u1-
  z[29]*z[35]*(u1-z[23]*u5)-lfx*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[14]*
  z[35]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))-z[36]*(z[21]*u1+
  z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))),
  2));
  pe = -g*(mf*(z[35]+lfx*z[26]+lfz*z[30]+z[28]*z[36])+mr*(lrz*z[30]-rrt-lrx*
  z[39]-rr*z[11]));
  z[373] = z[10]*z[11];
  z[374] = z[9]*z[11];
  z[375] = z[10]*z[12];
  z[376] = z[9]*z[12];
  z[377] = z[9]*z[13] - z[14]*z[375];
  z[378] = z[10]*z[13] + z[14]*z[376];
  z[379] = z[9]*z[14] + z[13]*z[375];
  z[380] = z[10]*z[14] - z[13]*z[376];
  z[381] = z[15]*z[377] - z[16]*z[373];
  z[382] = z[15]*z[378] + z[16]*z[374];
  z[384] = -z[15]*z[373] - z[16]*z[377];
  z[385] = z[15]*z[374] - z[16]*z[378];
  z[387] = z[27]*(z[32]+pow(z[28],2)/pow(z[31],0.5))/pow(z[32],2);
  z[388] = z[27]*z[28]/(pow(z[31],0.5)*pow(z[32],2));
  z[389] = rf*z[11]*z[388] + z[12]*(rrt+z[35]) - rf*z[14]*z[16]*z[387];
  z[390] = z[16]*(ls*z[27]+rf*z[30]*z[387]-z[12]*z[13]*z[36]) - lr*z[12]*
  z[13] - z[15]*(ls*z[25]+lf*z[12]*z[13]);
  z[391] = (z[56]*z[389]-z[65]*z[390])/pow(z[56],2);
  z[392] = z[12]*z[14]*z[35] + rf*z[11]*z[14]*z[388] - rf*z[16]*z[387];
  z[393] = (z[56]*z[392]-z[66]*z[390])/pow(z[56],2);
  z[394] = z[23]*(z[12]*z[35]+rf*z[11]*z[388]);
  z[395] = (z[56]*z[394]+z[69]*z[390])/pow(z[56],2);
  z[396] = z[391]*u1 + z[393]*u3 - z[395]*u5;
  z[397] = z[11]*z[14]*z[15] - z[12]*z[16];
  z[398] = z[397]*(z[32]+pow(z[28],2)/pow(z[31],0.5))/pow(z[32],2);
  z[399] = z[28]*z[397]/(pow(z[31],0.5)*pow(z[32],2));
  z[400] = lf*z[14]*z[16] + rf*z[11]*z[399] - z[14]*z[15]*z[36] - rf*z[14]*
  z[16]*z[398];
  z[401] = z[12]*z[15] + z[11]*z[14]*z[16];
  z[402] = z[15]*(ls*z[28]+z[30]*z[36]) + z[16]*(ls*z[397]+rf*z[30]*z[398]) - 
  ls*z[15]*z[401] - z[16]*(lf*z[30]-ls*z[26]);
  z[403] = (z[56]*z[400]-z[65]*z[402])/pow(z[56],2);
  z[404] = lf*z[16] + rf*z[11]*z[14]*z[399] - z[15]*z[36] - rf*z[16]*z[398];
  z[405] = (z[56]*z[404]-z[66]*z[402])/pow(z[56],2);
  z[406] = z[11]*(rf*z[23]*z[399]-z[13]*z[15]*z[35]);
  z[407] = (z[56]*z[406]+z[69]*z[402])/pow(z[56],2);
  z[408] = z[403]*u1 + z[405]*u3 - z[407]*u5;
  z[409] = z[13]*(rf*z[11]*z[388]+z[12]*(rrt+z[35]));
  z[410] = rf*z[21]*z[387] + z[11]*z[13]*(rrt+z[35]) - rf*z[29]*z[388];
  z[411] = -lf*z[27] - lr*z[11] - z[25]*z[36] - rf*z[26]*z[387] - rr*z[11]*
  z[14];
  z[412] = z[410] - z[57]*z[391] - z[74]*z[411];
  z[413] = rf*z[11]*z[14]*z[388] + z[12]*z[14]*(rrt+z[35]) - rf*z[16]*z[387];
  z[414] = rf*z[38]*z[388] - rf*z[14]*z[15]*z[387] - z[11]*z[14]*(rrt+z[35]);
  z[415] = z[16]*(ls*z[25]+lf*z[12]*z[13]) + z[15]*(ls*z[27]+rf*z[30]*z[387]-
  z[12]*z[13]*z[36]) - rr*z[11]*z[13];
  z[416] = z[414] - z[53]*z[391] - z[74]*z[415];
  z[417] = z[55]*z[413] + rrt*z[12]*z[14]*z[54] - z[59]*z[409] - rrt*z[12]*
  z[13]*z[58];
  z[418] = (z[417]*(z[54]*z[90]-z[58]*z[85])+z[81]*(z[58]*z[416]+z[85]*z[413]-
  z[54]*z[412]-z[90]*z[409]))/pow(z[81],2);
  z[419] = -z[11]*z[24] - z[12]*z[15];
  z[420] = z[35]*(z[14]*z[419]-z[11]*z[13]*z[23]) - rf*z[388]*(z[14]*z[27]-
  z[23]*z[29]);
  z[421] = z[420] + z[57]*z[395] - z[79]*z[411];
  z[422] = z[35]*(z[13]*z[419]+z[11]*z[14]*z[23]) - rf*z[388]*(z[13]*z[27]+
  z[23]*z[38]);
  z[423] = z[422] + z[53]*z[395] - z[79]*z[415];
  z[424] = (z[417]*(z[54]*z[93]-z[58]*z[89])+z[81]*(z[58]*z[423]+z[89]*z[413]-
  z[54]*z[421]-z[93]*z[409]))/pow(z[81],2);
  z[425] = z[57]*z[393] + z[75]*z[411];
  z[426] = rf*z[13]*z[29]*z[388] + rf*z[14]*z[38]*z[388] - rf*z[15]*z[387] - 
  z[11]*z[35]*pow(z[13],2) - z[11]*z[35]*pow(z[14],2);
  z[427] = z[426] - z[53]*z[393] - z[75]*z[415];
  z[428] = (z[417]*(z[54]*z[97]+z[58]*z[84])-z[81]*(z[54]*z[425]+z[58]*z[427]+
  z[84]*z[413]+z[97]*z[409]))/pow(z[81],2);
  z[429] = z[428]*u3 - z[418]*u1 - z[424]*u5;
  z[430] = rf*z[11]*z[13]*z[399];
  z[431] = rf*z[21]*z[398] - lf*z[13]*z[15] - rf*z[29]*z[399] - z[13]*z[16]*
  z[36];
  z[432] = -lf*z[397] - z[36]*z[401] - rf*z[26]*z[398];
  z[433] = z[431] - z[57]*z[403] - z[74]*z[432];
  z[434] = lf*z[14]*z[15] + rf*z[38]*z[399] + z[14]*z[16]*z[36] - rf*z[14]*
  z[15]*z[398];
  z[435] = ls*z[16]*z[401] + z[15]*(ls*z[397]+rf*z[30]*z[398]) - z[16]*(ls*
  z[28]+z[30]*z[36]) - z[15]*(lf*z[30]-ls*z[26]);
  z[436] = z[434] - z[53]*z[403] - z[74]*z[435];
  z[437] = z[55]*z[404] - z[59]*z[430];
  z[438] = (z[437]*(z[54]*z[90]-z[58]*z[85])+z[81]*(z[58]*z[436]+z[85]*z[404]-
  z[54]*z[433]-z[90]*z[430]))/pow(z[81],2);
  z[439] = -z[11]*z[16] - z[12]*z[14]*z[15];
  z[440] = z[35]*(z[14]*z[439]-z[13]*z[15]*z[29]) - rf*z[399]*(z[14]*z[27]-
  z[23]*z[29]);
  z[441] = z[440] + z[57]*z[407] - z[79]*z[432];
  z[442] = z[13]*z[35]*(z[439]+z[15]*z[38]) - rf*z[399]*(z[13]*z[27]+z[23]*
  z[38]);
  z[443] = z[442] + z[53]*z[407] - z[79]*z[435];
  z[444] = (z[437]*(z[54]*z[93]-z[58]*z[89])+z[81]*(z[58]*z[443]+z[89]*z[404]-
  z[54]*z[441]-z[93]*z[430]))/pow(z[81],2);
  z[445] = z[57]*z[405] + z[75]*z[432];
  z[446] = lf*z[15] + z[16]*z[36] + rf*z[13]*z[29]*z[399] + rf*z[14]*z[38]*
  z[399] - rf*z[15]*z[398];
  z[447] = z[446] - z[53]*z[405] - z[75]*z[435];
  z[448] = (z[437]*(z[54]*z[97]+z[58]*z[84])-z[81]*(z[54]*z[445]+z[58]*z[447]+
  z[84]*z[404]+z[97]*z[430]))/pow(z[81],2);
  z[449] = z[448]*u3 - z[438]*u1 - z[444]*u5;
  z[450] = (z[417]*(z[55]*z[90]-z[59]*z[85])+z[81]*(z[59]*z[416]+rrt*z[12]*
  z[13]*z[90]-z[55]*z[412]-rrt*z[12]*z[14]*z[85]))/pow(z[81],2);
  z[451] = (z[417]*(z[55]*z[93]-z[59]*z[89])+z[81]*(z[59]*z[423]+rrt*z[12]*
  z[13]*z[93]-z[55]*z[421]-rrt*z[12]*z[14]*z[89]))/pow(z[81],2);
  z[452] = (z[417]*(z[55]*z[97]+z[59]*z[84])-z[81]*(z[55]*z[425]+z[59]*z[427]-
  rrt*z[12]*z[13]*z[97]-rrt*z[12]*z[14]*z[84]))/pow(z[81],2);
  z[453] = z[452]*u3 - z[450]*u1 - z[451]*u5;
  z[454] = (z[81]*(z[55]*z[433]-z[59]*z[436])-z[437]*(z[55]*z[90]-z[59]*z[85]))/
  pow(z[81],2);
  z[455] = (z[81]*(z[55]*z[441]-z[59]*z[443])-z[437]*(z[55]*z[93]-z[59]*z[89]))/
  pow(z[81],2);
  z[456] = (z[81]*(z[55]*z[445]+z[59]*z[447])-z[437]*(z[55]*z[97]+z[59]*z[84]))/
  pow(z[81],2);
  z[457] = z[454]*u1 + z[455]*u5 - z[456]*u3;
  z[458] = z[39]*z[393] - z[12]*z[14]*z[75];
  z[459] = z[30]*z[393] - z[12]*z[13]*z[75];
  z[460] = IDxz*z[458] - IDzz*z[459];
  z[461] = z[16]*z[452] + z[25]*z[75] + z[26]*z[393];
  z[462] = -IExz*z[461] - IEzz*z[459];
  z[463] = IDxx*z[458] - IDxz*z[459];
  z[464] = -IExx*z[461] - IExz*z[459];
  z[465] = z[15]*z[452] + z[27]*z[75] + z[28]*z[393];
  z[466] = z[393]*(lrx*z[30]+lrz*z[39]) - z[12]*z[75]*(lrx*z[13]+lrz*z[14]);
  z[467] = -rrt*z[11]*z[14]*(z[428]+z[452]) - rrt*z[12]*z[14]*(z[100]+z[109]) - 
  lrx*(z[452]+z[11]*z[75]+z[12]*z[393]) - rr*z[14]*(z[428]+z[452]+z[11]*z[75]+
  z[12]*z[393]);
  z[468] = z[11]*z[14]*z[35]*z[452] + z[12]*z[14]*z[35]*z[100] + rf*z[11]*
  z[14]*z[100]*z[388] + z[36]*(z[16]*z[452]+z[25]*z[75]+z[26]*z[393]) - lfx*
  z[15]*z[452] - lfx*z[27]*z[75] - lfx*z[28]*z[393] - rf*z[387]*(z[16]*z[100]-
  z[26]*z[75]);
  z[469] = z[452] + z[11]*z[75] + z[12]*z[393];
  z[470] = z[428] + z[452] + z[11]*z[75] + z[12]*z[393];
  z[471] = lrz*(z[452]+z[11]*z[75]+z[12]*z[393]) - rrt*z[11]*z[13]*(z[428]+
  z[452]) - rrt*z[12]*z[13]*(z[100]+z[109]) - rr*z[13]*(z[428]+z[452]+z[11]*
  z[75]+z[12]*z[393]);
  z[472] = z[11]*z[22] - z[12]*z[16];
  z[473] = lfz*z[15]*z[452] + lfz*z[27]*z[75] + lfz*z[28]*z[393] + rf*z[14]*
  z[25]*z[388] + rf*z[21]*z[388]*(z[29]+z[11]*z[100]) - z[14]*z[35]*z[472] - 
  rf*z[387]*(1+z[30]*z[75]) - z[36]*(z[30]*z[393]-z[12]*z[13]*z[75]) - z[21]*
  z[35]*(z[11]*z[13]-z[11]*z[452]-z[12]*z[100]);
  z[474] = lfx*z[30]*z[393] + rf*z[14]*z[27]*z[388] + z[23]*z[35]*(z[11]*
  z[13]-z[11]*z[452]-z[12]*z[100]) - lfz*z[16]*z[452] - lfz*z[25]*z[75] - lfz*
  z[26]*z[393] - z[14]*z[35]*z[419] - lfx*z[12]*z[13]*z[75] - rf*z[23]*z[388]*(
  z[29]+z[11]*z[100]);
  z[475] = z[460]*(z[14]+z[30]*z[74]) + z[462]*(z[14]+z[30]*z[74]) + z[463]*(
  z[13]-z[39]*z[74]) + z[464]*(z[21]+z[16]*z[101]+z[26]*z[74]) + z[242]*(
  z[30]*z[391]-z[12]*z[13]*z[74]) + z[254]*(z[30]*z[391]-z[12]*z[13]*z[74]) + 
  z[247]*(z[450]-z[11]*z[74]-z[12]*z[391]) + IEyy*z[465]*(z[23]-z[15]*z[101]-
  z[28]*z[74]) + IFyy*z[465]*(z[23]-z[15]*z[101]-z[28]*z[74]) + z[233]*(
  z[418]+z[450]-z[11]*z[74]-z[12]*z[391]) + z[258]*(z[15]*z[450]-z[27]*z[74]-
  z[28]*z[391]) + z[262]*(z[15]*z[450]-z[27]*z[74]-z[28]*z[391]) + mr*z[466]*(
  lrz*z[13]-rr-lrx*z[14]-rrt*z[11]-lrx*z[30]*z[74]-lrz*z[39]*z[74]) + mr*
  z[467]*(rrt*z[29]+lrx*(z[101]+z[12]*z[74])+rrt*z[11]*z[14]*(z[101]+z[110])+
  rr*z[14]*(z[101]+z[110]+z[12]*z[74])) + mf*z[468]*(lfx*z[15]*z[101]+lfx*
  z[28]*z[74]-lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(z[21]+
  z[16]*z[101]+z[26]*z[74])) + z[268]*(lrz*(z[450]-z[11]*z[74]-z[12]*z[391])-
  rrt*z[11]*z[14]-rrt*z[11]*z[13]*(z[418]+z[450])-rrt*z[12]*z[13]*(z[101]+
  z[110])-rr*z[13]*(z[418]+z[450]-z[11]*z[74]-z[12]*z[391])) + z[275]*(rrt*
  z[11]*z[13]-rrt*z[11]*z[14]*(z[418]+z[450])-rrt*z[12]*z[14]*(z[101]+z[110])-
  lrx*(z[450]-z[11]*z[74]-z[12]*z[391])-rr*z[14]*(z[418]+z[450]-z[11]*z[74]-
  z[12]*z[391])) + z[283]*(z[35]*z[419]+lfz*z[25]*z[74]+lfz*z[26]*z[391]+lfx*
  z[12]*z[13]*z[74]-lfx*z[30]*z[391]-lfz*z[16]*z[450]-rf*z[27]*z[388]-z[11]*
  z[23]*z[35]*z[450]-z[12]*z[23]*z[35]*z[101]-rf*z[11]*z[23]*z[101]*z[388]) - 
  IDyy*z[469]*(z[101]+z[12]*z[74]) - ICyy*z[470]*(z[101]+z[110]+z[12]*z[74]) - 
  z[238]*(z[39]*z[391]-z[12]*z[14]*z[74]) - z[250]*(z[16]*z[450]-z[25]*z[74]-
  z[26]*z[391]) - z[272]*(lrx*z[30]*z[391]+lrz*z[39]*z[391]-rrt*z[12]-lrx*
  z[12]*z[13]*z[74]-lrz*z[12]*z[14]*z[74]) - mr*z[471]*(rrt*z[38]+lrz*(z[101]+
  z[12]*z[74])-rrt*z[11]*z[13]*(z[101]+z[110])-rr*z[13]*(z[101]+z[110]+z[12]*
  z[74])) - mf*z[473]*(lfz*z[15]*z[101]+lfz*z[28]*z[74]+z[11]*z[21]*z[35]*
  z[101]-z[25]*z[35]-lfz*z[13]*z[16]-z[36]*(z[14]+z[30]*z[74])) - mf*z[474]*(
  lfx*z[14]+lfx*z[30]*z[74]-z[27]*z[35]-lfz*z[13]*z[15]-lfz*z[16]*z[101]-lfz*
  z[26]*z[74]-z[11]*z[23]*z[35]*z[101]) - z[280]*(lfz*z[27]*z[74]+lfz*z[28]*
  z[391]+rf*z[25]*z[388]-z[35]*z[472]-lfz*z[15]*z[450]-z[11]*z[21]*z[35]*
  z[450]-z[12]*z[21]*z[35]*z[101]-rf*z[11]*z[21]*z[101]*z[388]-rf*z[387]*(
  z[14]+z[30]*z[74])-z[36]*(z[30]*z[391]-z[12]*z[13]*z[74])) - z[289]*(lfx*
  z[15]*z[450]+z[11]*z[13]*z[35]+rf*z[387]*(z[21]+z[16]*z[101]+z[26]*z[74])-
  lfx*z[27]*z[74]-lfx*z[28]*z[391]-rf*z[29]*z[388]-z[11]*z[14]*z[35]*z[450]-
  z[12]*z[14]*z[35]*z[101]-rf*z[11]*z[14]*z[101]*z[388]-z[36]*(z[16]*z[450]-
  z[25]*z[74]-z[26]*z[391]));
  z[476] = -z[39]*z[395] - z[12]*z[14]*z[79];
  z[477] = -z[30]*z[395] - z[12]*z[13]*z[79];
  z[478] = IDxz*z[476] - IDzz*z[477];
  z[479] = z[25]*z[79] - z[16]*z[451] - z[26]*z[395];
  z[480] = -IExz*z[479] - IEzz*z[477];
  z[481] = z[11]*z[79] - z[451] - z[12]*z[395];
  z[482] = z[11]*z[79] - z[424] - z[451] - z[12]*z[395];
  z[483] = z[27]*z[79] - z[15]*z[451] - z[28]*z[395];
  z[484] = rrt*z[11]*z[13]*(z[424]+z[451]) + rrt*z[12]*z[13]*(z[105]+z[114]) + 
  lrz*(z[11]*z[79]-z[451]-z[12]*z[395]) - rr*z[13]*(z[11]*z[79]-z[424]-z[451]-
  z[12]*z[395]);
  z[485] = lfz*z[16]*z[451] + lfz*z[26]*z[395] + z[11]*z[23]*z[35]*z[451] + 
  z[12]*z[23]*z[35]*z[105] + rf*z[11]*z[23]*z[105]*z[388] - lfx*z[30]*z[395] - 
  lfz*z[25]*z[79] - lfx*z[12]*z[13]*z[79];
  z[486] = IDxx*z[476] - IDxz*z[477];
  z[487] = -z[395]*(lrx*z[30]+lrz*z[39]) - z[12]*z[79]*(lrx*z[13]+lrz*z[14]);
  z[488] = -IExx*z[479] - IExz*z[477];
  z[489] = rrt*z[11]*z[14]*(z[424]+z[451]) + rrt*z[12]*z[14]*(z[105]+z[114]) - 
  lrx*(z[11]*z[79]-z[451]-z[12]*z[395]) - rr*z[14]*(z[11]*z[79]-z[424]-z[451]-
  z[12]*z[395]);
  z[490] = lfx*z[15]*z[451] + lfx*z[28]*z[395] + rf*z[23]*z[29]*z[388] + rf*
  z[387]*(z[16]*z[105]+z[26]*z[79]) + z[14]*z[35]*(z[419]-z[11]*z[451]-z[12]*
  z[105]) + z[36]*(z[25]*z[79]-z[16]*z[451]-z[26]*z[395]) - lfx*z[27]*z[79] - 
  z[11]*z[13]*z[23]*z[35] - rf*z[14]*z[388]*(z[27]+z[11]*z[105]);
  z[491] = lfz*z[27]*z[79] + z[23]*z[35]*z[472] + z[30]*z[36]*z[395] + z[12]*
  z[13]*z[36]*z[79] + z[21]*z[35]*(z[419]-z[11]*z[451]-z[12]*z[105]) - lfz*
  z[15]*z[451] - lfz*z[28]*z[395] - rf*z[23]*z[25]*z[388] - rf*z[30]*z[79]*
  z[387] - rf*z[21]*z[388]*(z[27]+z[11]*z[105]);
  z[492] = z[30]*z[75]*z[478] + z[30]*z[243]*z[393] + z[12]*z[14]*z[75]*
  z[239] + z[480]*(1+z[30]*z[75]) + IDyy*z[481]*(z[100]-z[12]*z[75]) + ICyy*
  z[482]*(z[100]+z[109]-z[12]*z[75]) + IEyy*z[483]*(z[15]*z[100]-z[28]*z[75]) + 
  IFyy*z[483]*(z[15]*z[100]-z[28]*z[75]) + z[12]*z[75]*z[273]*(lrx*z[13]+lrz*
  z[14]) + z[251]*(z[16]*z[452]+z[25]*z[75]+z[26]*z[393]) + z[255]*(z[30]*
  z[393]-z[12]*z[13]*z[75]) + mr*z[484]*(lrz*(z[100]-z[12]*z[75])-rrt*z[11]*
  z[13]*(z[100]+z[109])-rr*z[13]*(z[100]+z[109]-z[12]*z[75])) + mf*z[485]*(
  lfz*z[26]*z[75]+z[14]*z[27]*z[35]-lfx-lfx*z[30]*z[75]-lfz*z[16]*z[100]-
  z[23]*z[35]*(z[29]+z[11]*z[100])) + z[277]*(rrt*z[11]*z[14]*(z[428]+z[452])+
  rrt*z[12]*z[14]*(z[100]+z[109])+lrx*(z[452]+z[11]*z[75]+z[12]*z[393])+rr*
  z[14]*(z[428]+z[452]+z[11]*z[75]+z[12]*z[393])) + z[288]*(lfx*z[15]*z[452]+
  lfx*z[27]*z[75]+lfx*z[28]*z[393]+rf*z[387]*(z[16]*z[100]-z[26]*z[75])-z[11]*
  z[14]*z[35]*z[452]-z[12]*z[14]*z[35]*z[100]-rf*z[11]*z[14]*z[100]*z[388]-
  z[36]*(z[16]*z[452]+z[25]*z[75]+z[26]*z[393])) + z[285]*(lfz*z[16]*z[452]+
  lfz*z[25]*z[75]+lfz*z[26]*z[393]+z[14]*z[35]*z[419]+lfx*z[12]*z[13]*z[75]+
  rf*z[23]*z[388]*(z[29]+z[11]*z[100])-lfx*z[30]*z[393]-rf*z[14]*z[27]*z[388]-
  z[23]*z[35]*(z[11]*z[13]-z[11]*z[452]-z[12]*z[100])) - z[39]*z[75]*z[486] - 
  z[39]*z[239]*z[393] - z[12]*z[13]*z[75]*z[243] - z[246]*(z[452]+z[11]*z[75]+
  z[12]*z[393]) - z[273]*z[393]*(lrx*z[30]+lrz*z[39]) - z[488]*(z[16]*z[100]-
  z[26]*z[75]) - mr*z[75]*z[487]*(lrx*z[30]+lrz*z[39]) - z[232]*(z[428]+
  z[452]+z[11]*z[75]+z[12]*z[393]) - z[259]*(z[15]*z[452]+z[27]*z[75]+z[28]*
  z[393]) - z[263]*(z[15]*z[452]+z[27]*z[75]+z[28]*z[393]) - mf*z[490]*(lfx*
  z[15]*z[100]-lfx*z[28]*z[75]-z[11]*z[14]*z[35]*z[100]-z[36]*(z[16]*z[100]-
  z[26]*z[75])) - mr*z[489]*(lrx*(z[100]-z[12]*z[75])+rrt*z[11]*z[14]*(z[100]+
  z[109])+rr*z[14]*(z[100]+z[109]-z[12]*z[75])) - mf*z[491]*(lfz*z[28]*z[75]-
  lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+z[30]*z[75])-z[21]*z[35]*(z[29]+
  z[11]*z[100])) - z[269]*(lrz*(z[452]+z[11]*z[75]+z[12]*z[393])-rrt*z[11]*
  z[13]*(z[428]+z[452])-rrt*z[12]*z[13]*(z[100]+z[109])-rr*z[13]*(z[428]+
  z[452]+z[11]*z[75]+z[12]*z[393])) - z[281]*(lfz*z[15]*z[452]+lfz*z[27]*
  z[75]+lfz*z[28]*z[393]+rf*z[14]*z[25]*z[388]+rf*z[21]*z[388]*(z[29]+z[11]*
  z[100])-z[14]*z[35]*z[472]-rf*z[387]*(1+z[30]*z[75])-z[36]*(z[30]*z[393]-
  z[12]*z[13]*z[75])-z[21]*z[35]*(z[11]*z[13]-z[11]*z[452]-z[12]*z[100]));
  z[493] = z[478]*(z[14]+z[30]*z[74]) + z[480]*(z[14]+z[30]*z[74]) + z[486]*(
  z[13]-z[39]*z[74]) + z[488]*(z[21]+z[16]*z[101]+z[26]*z[74]) + z[243]*(
  z[30]*z[391]-z[12]*z[13]*z[74]) + z[255]*(z[30]*z[391]-z[12]*z[13]*z[74]) + 
  z[246]*(z[450]-z[11]*z[74]-z[12]*z[391]) + IEyy*z[483]*(z[23]-z[15]*z[101]-
  z[28]*z[74]) + IFyy*z[483]*(z[23]-z[15]*z[101]-z[28]*z[74]) + z[232]*(
  z[418]+z[450]-z[11]*z[74]-z[12]*z[391]) + z[259]*(z[15]*z[450]-z[27]*z[74]-
  z[28]*z[391]) + z[263]*(z[15]*z[450]-z[27]*z[74]-z[28]*z[391]) + mr*z[487]*(
  lrz*z[13]-rr-lrx*z[14]-rrt*z[11]-lrx*z[30]*z[74]-lrz*z[39]*z[74]) + mr*
  z[489]*(rrt*z[29]+lrx*(z[101]+z[12]*z[74])+rrt*z[11]*z[14]*(z[101]+z[110])+
  rr*z[14]*(z[101]+z[110]+z[12]*z[74])) + mf*z[490]*(lfx*z[15]*z[101]+lfx*
  z[28]*z[74]-lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(z[21]+
  z[16]*z[101]+z[26]*z[74])) + z[269]*(lrz*(z[450]-z[11]*z[74]-z[12]*z[391])-
  rrt*z[11]*z[14]-rrt*z[11]*z[13]*(z[418]+z[450])-rrt*z[12]*z[13]*(z[101]+
  z[110])-rr*z[13]*(z[418]+z[450]-z[11]*z[74]-z[12]*z[391])) + z[277]*(rrt*
  z[11]*z[13]-rrt*z[11]*z[14]*(z[418]+z[450])-rrt*z[12]*z[14]*(z[101]+z[110])-
  lrx*(z[450]-z[11]*z[74]-z[12]*z[391])-rr*z[14]*(z[418]+z[450]-z[11]*z[74]-
  z[12]*z[391])) + z[285]*(z[35]*z[419]+lfz*z[25]*z[74]+lfz*z[26]*z[391]+lfx*
  z[12]*z[13]*z[74]-lfx*z[30]*z[391]-lfz*z[16]*z[450]-rf*z[27]*z[388]-z[11]*
  z[23]*z[35]*z[450]-z[12]*z[23]*z[35]*z[101]-rf*z[11]*z[23]*z[101]*z[388]) - 
  IDyy*z[481]*(z[101]+z[12]*z[74]) - ICyy*z[482]*(z[101]+z[110]+z[12]*z[74]) - 
  z[239]*(z[39]*z[391]-z[12]*z[14]*z[74]) - z[251]*(z[16]*z[450]-z[25]*z[74]-
  z[26]*z[391]) - z[273]*(lrx*z[30]*z[391]+lrz*z[39]*z[391]-rrt*z[12]-lrx*
  z[12]*z[13]*z[74]-lrz*z[12]*z[14]*z[74]) - mr*z[484]*(rrt*z[38]+lrz*(z[101]+
  z[12]*z[74])-rrt*z[11]*z[13]*(z[101]+z[110])-rr*z[13]*(z[101]+z[110]+z[12]*
  z[74])) - mf*z[491]*(lfz*z[15]*z[101]+lfz*z[28]*z[74]+z[11]*z[21]*z[35]*
  z[101]-z[25]*z[35]-lfz*z[13]*z[16]-z[36]*(z[14]+z[30]*z[74])) - mf*z[485]*(
  lfx*z[14]+lfx*z[30]*z[74]-z[27]*z[35]-lfz*z[13]*z[15]-lfz*z[16]*z[101]-lfz*
  z[26]*z[74]-z[11]*z[23]*z[35]*z[101]) - z[281]*(lfz*z[27]*z[74]+lfz*z[28]*
  z[391]+rf*z[25]*z[388]-z[35]*z[472]-lfz*z[15]*z[450]-z[11]*z[21]*z[35]*
  z[450]-z[12]*z[21]*z[35]*z[101]-rf*z[11]*z[21]*z[101]*z[388]-rf*z[387]*(
  z[14]+z[30]*z[74])-z[36]*(z[30]*z[391]-z[12]*z[13]*z[74])) - z[288]*(lfx*
  z[15]*z[450]+z[11]*z[13]*z[35]+rf*z[387]*(z[21]+z[16]*z[101]+z[26]*z[74])-
  lfx*z[27]*z[74]-lfx*z[28]*z[391]-rf*z[29]*z[388]-z[11]*z[14]*z[35]*z[450]-
  z[12]*z[14]*z[35]*z[101]-rf*z[11]*z[14]*z[101]*z[388]-z[36]*(z[16]*z[450]-
  z[25]*z[74]-z[26]*z[391]));
  z[494] = z[30]*z[75]*z[460] + z[30]*z[242]*z[393] + z[12]*z[14]*z[75]*
  z[238] + z[462]*(1+z[30]*z[75]) + IDyy*z[469]*(z[100]-z[12]*z[75]) + ICyy*
  z[470]*(z[100]+z[109]-z[12]*z[75]) + IEyy*z[465]*(z[15]*z[100]-z[28]*z[75]) + 
  IFyy*z[465]*(z[15]*z[100]-z[28]*z[75]) + z[12]*z[75]*z[272]*(lrx*z[13]+lrz*
  z[14]) + z[250]*(z[16]*z[452]+z[25]*z[75]+z[26]*z[393]) + z[254]*(z[30]*
  z[393]-z[12]*z[13]*z[75]) + mr*z[471]*(lrz*(z[100]-z[12]*z[75])-rrt*z[11]*
  z[13]*(z[100]+z[109])-rr*z[13]*(z[100]+z[109]-z[12]*z[75])) + mf*z[474]*(
  lfz*z[26]*z[75]+z[14]*z[27]*z[35]-lfx-lfx*z[30]*z[75]-lfz*z[16]*z[100]-
  z[23]*z[35]*(z[29]+z[11]*z[100])) + z[275]*(rrt*z[11]*z[14]*(z[428]+z[452])+
  rrt*z[12]*z[14]*(z[100]+z[109])+lrx*(z[452]+z[11]*z[75]+z[12]*z[393])+rr*
  z[14]*(z[428]+z[452]+z[11]*z[75]+z[12]*z[393])) + z[289]*(lfx*z[15]*z[452]+
  lfx*z[27]*z[75]+lfx*z[28]*z[393]+rf*z[387]*(z[16]*z[100]-z[26]*z[75])-z[11]*
  z[14]*z[35]*z[452]-z[12]*z[14]*z[35]*z[100]-rf*z[11]*z[14]*z[100]*z[388]-
  z[36]*(z[16]*z[452]+z[25]*z[75]+z[26]*z[393])) + z[283]*(lfz*z[16]*z[452]+
  lfz*z[25]*z[75]+lfz*z[26]*z[393]+z[14]*z[35]*z[419]+lfx*z[12]*z[13]*z[75]+
  rf*z[23]*z[388]*(z[29]+z[11]*z[100])-lfx*z[30]*z[393]-rf*z[14]*z[27]*z[388]-
  z[23]*z[35]*(z[11]*z[13]-z[11]*z[452]-z[12]*z[100])) - z[39]*z[75]*z[463] - 
  z[39]*z[238]*z[393] - z[12]*z[13]*z[75]*z[242] - z[247]*(z[452]+z[11]*z[75]+
  z[12]*z[393]) - z[272]*z[393]*(lrx*z[30]+lrz*z[39]) - z[464]*(z[16]*z[100]-
  z[26]*z[75]) - mr*z[75]*z[466]*(lrx*z[30]+lrz*z[39]) - z[233]*(z[428]+
  z[452]+z[11]*z[75]+z[12]*z[393]) - z[258]*(z[15]*z[452]+z[27]*z[75]+z[28]*
  z[393]) - z[262]*(z[15]*z[452]+z[27]*z[75]+z[28]*z[393]) - mf*z[468]*(lfx*
  z[15]*z[100]-lfx*z[28]*z[75]-z[11]*z[14]*z[35]*z[100]-z[36]*(z[16]*z[100]-
  z[26]*z[75])) - mr*z[467]*(lrx*(z[100]-z[12]*z[75])+rrt*z[11]*z[14]*(z[100]+
  z[109])+rr*z[14]*(z[100]+z[109]-z[12]*z[75])) - mf*z[473]*(lfz*z[28]*z[75]-
  lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+z[30]*z[75])-z[21]*z[35]*(z[29]+
  z[11]*z[100])) - z[268]*(lrz*(z[452]+z[11]*z[75]+z[12]*z[393])-rrt*z[11]*
  z[13]*(z[428]+z[452])-rrt*z[12]*z[13]*(z[100]+z[109])-rr*z[13]*(z[428]+
  z[452]+z[11]*z[75]+z[12]*z[393])) - z[280]*(lfz*z[15]*z[452]+lfz*z[27]*
  z[75]+lfz*z[28]*z[393]+rf*z[14]*z[25]*z[388]+rf*z[21]*z[388]*(z[29]+z[11]*
  z[100])-z[14]*z[35]*z[472]-rf*z[387]*(1+z[30]*z[75])-z[36]*(z[30]*z[393]-
  z[12]*z[13]*z[75])-z[21]*z[35]*(z[11]*z[13]-z[11]*z[452]-z[12]*z[100]));
  z[495] = z[307]*z[492] + z[312]*z[475] - z[308]*z[494] - z[311]*z[493];
  z[498] = z[496]*z[12]*z[13]*z[105] + z[497]*z[12]*z[14]*z[105] + z[292]*
  z[28]*z[451]*(lfz*z[16]+z[11]*z[23]*z[35]) + z[292]*z[23]*z[28]*z[105]*(
  z[12]*z[35]+rf*z[11]*z[388]) + z[292]*z[25]*(lfz*z[15]*z[105]+z[23]*z[25]*
  z[35]+z[21]*z[35]*(z[27]+z[11]*z[105])) + z[292]*z[12]*z[13]*(lfx*z[15]*
  z[105]+z[23]*z[29]*z[35]-z[16]*z[36]*z[105]-z[14]*z[35]*(z[27]+z[11]*z[105])) + 
  z[291]*z[30]*(lrx*z[451]+rrt*z[11]*z[14]*(z[424]+z[451])+rrt*z[12]*z[14]*(
  z[105]+z[114])-rr*z[14]*(z[11]*z[79]-z[424]-z[451]-z[12]*z[395])) - Trw*
  z[424] - z[292]*z[27]*z[105]*(lfz*z[16]+z[11]*z[23]*z[35]) - z[291]*z[39]*(
  rrt*z[11]*z[13]*(z[424]+z[451])+rrt*z[12]*z[13]*(z[105]+z[114])-lrz*z[451]-
  rr*z[13]*(z[11]*z[79]-z[424]-z[451]-z[12]*z[395])) - z[292]*z[26]*(lfz*
  z[15]*z[451]+rf*z[23]*z[25]*z[388]+rf*z[21]*z[388]*(z[27]+z[11]*z[105])-
  z[23]*z[35]*z[472]-z[21]*z[35]*(z[419]-z[11]*z[451]-z[12]*z[105])) - z[292]*
  z[30]*(z[16]*z[36]*z[451]+z[11]*z[13]*z[23]*z[35]+rf*z[14]*z[388]*(z[27]+
  z[11]*z[105])-lfx*z[15]*z[451]-rf*z[16]*z[105]*z[387]-rf*z[23]*z[29]*z[388]-
  z[14]*z[35]*(z[419]-z[11]*z[451]-z[12]*z[105]));
  z[499] = ICyy*u1*(z[418]*u1+z[424]*u5+z[450]*u1+z[451]*u5-z[428]*u3-z[452]*
  u3-z[11]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[12]*(z[391]*u1+z[393]*u3-z[395]*u5));
  z[500] = IFyy*((u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[15]*(
  z[450]*u1+z[451]*u5-z[452]*u3)-z[27]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[28]*(
  z[391]*u1+z[393]*u3-z[395]*u5))+(z[12]*z[13]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[30]*(z[391]*u1+z[393]*u3-z[395]*u5))*(u5+z[28]*(z[74]*u1+z[75]*u3+z[79]*
  u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)));
  z[501] = z[11]*z[13]*z[453] - z[11]*z[14]*u1 - z[12]*z[13]*q2p;
  z[502] = z[11]*z[13]*z[16]*z[453] - z[11]*z[16]*u3 - z[11]*z[24]*u1 - z[12]*
  z[15]*u1 - z[12]*z[120];
  z[503] = (z[27]*z[31]*z[121]+z[28]*z[31]*z[502]+z[27]*pow(z[28],2)*z[121])/
  pow(z[31],1.5);
  z[504] = rf*(z[32]*z[503]+2*z[27]*z[28]*z[122]/pow(z[31],0.5))/pow(z[32],3);
  z[505] = (z[32]*(z[27]*z[122]+z[28]*z[503]+z[32]*z[502])+z[27]*z[28]*(z[32]*
  z[121]+2*z[28]*z[122])/pow(z[31],0.5))/pow(z[32],3);
  z[506] = z[11]*(rrt+z[35])*u1 + z[11]*z[504] - rf*z[12]*z[388]*u1 - rf*
  z[14]*z[15]*z[387]*u3 - lr*z[13]*z[453] - ls*z[14]*z[453] - lf*z[13]*z[15]*
  z[453] - z[13]*z[16]*z[36]*z[453] - rf*z[13]*z[16]*z[387]*q2p - z[12]*
  z[125] - rf*z[14]*z[16]*z[505];
  z[507] = z[12]*z[14]*q2p - z[11]*z[13]*u1 - z[11]*z[14]*z[453];
  z[508] = z[11]*z[15]*u3 + z[11]*z[22]*u1 + z[12]*z[128] - z[12]*z[16]*u1 - 
  z[11]*z[13]*z[15]*z[453];
  z[509] = z[16]*(ls*z[25]+lf*z[12]*z[13])*u3 + z[15]*(ls*z[27]+rf*z[30]*
  z[387]-z[12]*z[13]*z[36])*u3 + lr*z[507] + z[15]*(lf*z[507]-ls*z[508]) + 
  z[16]*(ls*z[502]+z[36]*z[507]+rf*z[30]*z[505]+rf*z[387]*z[132]-rf*z[12]*
  z[13]*z[123]);
  z[510] = (z[390]*(z[56]*z[131]-2*z[65]*z[133])-z[56]*(z[56]*z[506]-z[65]*
  z[509]-z[389]*z[133]))/pow(z[56],3);
  z[511] = z[11]*z[14]*z[35]*u1 + z[12]*z[13]*z[35]*q2p + rf*z[11]*z[13]*
  z[388]*q2p + z[11]*z[14]*z[504] - rf*z[15]*z[387]*u3 - rf*z[12]*z[14]*
  z[388]*u1 - z[11]*z[13]*z[35]*z[453] - rf*z[16]*z[505] - z[12]*z[14]*z[125];
  z[512] = (2*z[66]*z[390]*z[133]+z[56]*(z[56]*z[511]-z[66]*z[509]-z[390]*
  z[155]-z[392]*z[133]))/pow(z[56],3);
  z[513] = rf*z[12]*z[23]*z[388]*u1 + z[12]*z[23]*z[125] - z[11]*z[23]*z[35]*
  u1 - z[11]*z[14]*z[16]*z[35]*z[453] - z[11]*z[23]*z[504] - z[12]*z[35]*
  z[126] - rf*z[11]*z[388]*z[126];
  z[514] = (z[390]*(z[56]*z[147]-2*z[69]*z[133])+z[56]*(z[69]*z[509]-z[56]*
  z[513]-z[394]*z[133]))/pow(z[56],3);
  z[515] = z[12]*z[14]*(u1*z[134]+u3*z[156]+u5*z[148]) - z[14]*u1*z[453] - (
  z[74]*u1+z[75]*u3+z[79]*u5)*z[501] - (z[391]*u1+z[393]*u3-z[395]*u5)*z[173] - 
  z[39]*(u3*z[512]-u1*z[510]-u5*z[514]);
  z[516] = z[13]*u1*z[453] + (z[74]*u1+z[75]*u3+z[79]*u5)*z[507] + (z[391]*u1+
  z[393]*u3-z[395]*u5)*z[132] + z[30]*(u3*z[512]-u1*z[510]-u5*z[514]) - z[12]*
  z[13]*(u1*z[134]+u3*z[156]+u5*z[148]);
  z[517] = IDxz*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*
  u5))*(z[12]*z[13]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[30]*(z[391]*u1+z[393]*u3-
  z[395]*u5)) + IDyy*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+
  z[79]*u5))*(z[12]*z[14]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[39]*(z[391]*u1+
  z[393]*u3-z[395]*u5)) + IDyy*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))*(
  z[450]*u1+z[451]*u5-z[452]*u3-z[11]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[12]*(
  z[391]*u1+z[393]*u3-z[395]*u5)) - IDxx*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5))*(z[12]*z[14]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[39]*(z[391]*u1+z[393]*u3-z[395]*u5)) - IDxz*(z[14]*u1+z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5))*(z[450]*u1+z[451]*u5-z[452]*u3-z[11]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[12]*(z[391]*u1+z[393]*u3-z[395]*u5)) - IDxx*(z[13]*u1-z[39]*(
  z[74]*u1+z[75]*u3+z[79]*u5))*(z[450]*u1+z[451]*u5-z[452]*u3-z[11]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[12]*(z[391]*u1+z[393]*u3-z[395]*u5)) - IDxz*z[515] - 
  IDzz*z[516];
  z[518] = z[11]*z[14]*(rrt+z[35])*u1 + rf*z[11]*z[13]*z[388]*q2p + z[12]*
  z[13]*(rrt+z[35])*q2p + z[11]*z[14]*z[504] - rf*z[15]*z[387]*u3 - rf*z[12]*
  z[14]*z[388]*u1 - rr*z[13]*z[453] - z[11]*z[13]*(rrt+z[35])*z[453] - rf*
  z[16]*z[505] - z[12]*z[14]*z[125];
  z[519] = rrt*z[12]*z[14]*q2p - rrt*z[11]*z[13]*u1 - z[14]*(rr+rrt*z[11])*
  z[453];
  z[520] = z[13]*(rr+rrt*z[11])*z[453] - rrt*z[11]*z[14]*u1 - rrt*z[12]*z[13]*
  q2p;
  z[521] = z[11]*z[13]*(rrt+z[35])*u1 + rr*z[14]*z[453] + z[11]*z[14]*(rrt+
  z[35])*z[453] + z[11]*z[13]*z[504] - rf*z[12]*z[13]*z[388]*u1 - rf*z[11]*
  z[14]*z[388]*q2p - z[12]*z[14]*(rrt+z[35])*q2p - z[12]*z[13]*z[125];
  z[522] = z[55]*z[518] + z[58]*z[519] + z[413]*z[118] + rrt*z[12]*z[14]*
  z[142] - z[54]*z[520] - z[59]*z[521] - z[409]*z[136] - rrt*z[12]*z[13]*
  z[141];
  z[523] = lr*z[12]*u1 + rr*z[12]*z[14]*u1 - rr*z[12]*z[13]*z[453] - rr*z[11]*
  z[13]*q2p - lf*z[502] - z[36]*z[508] - rf*z[25]*z[123] - rf*z[26]*z[505] - 
  rf*z[387]*z[129];
  z[524] = z[57]*z[512] + z[75]*z[523] + z[393]*z[130] + z[411]*z[156];
  z[525] = -z[12]*z[13]*u1 - z[12]*z[14]*z[453] - z[11]*z[14]*q2p;
  z[526] = z[12]*z[13]*z[453] + z[11]*z[13]*q2p - z[12]*z[14]*u1;
  z[527] = rf*z[16]*z[387]*u3 + z[14]*z[29]*z[35]*z[453] + rf*z[13]*z[38]*
  z[388]*q2p + z[13]*z[29]*z[504] + z[14]*z[38]*z[504] + rf*z[13]*z[388]*
  z[124] + rf*z[14]*z[388]*z[137] + z[11]*pow(z[13],2)*z[125] + z[11]*pow(
  z[14],2)*z[125] - z[13]*z[35]*z[38]*z[453] - rf*z[14]*z[29]*z[388]*q2p - rf*
  z[15]*z[505] - z[13]*z[35]*z[525] - z[14]*z[35]*z[526];
  z[528] = rr*z[12]*z[13]*u1 + z[15]*(ls*z[25]+lf*z[12]*z[13])*u3 + rr*z[12]*
  z[14]*z[453] + rr*z[11]*z[14]*q2p + z[15]*(ls*z[502]+z[36]*z[507]+rf*z[30]*
  z[505]+rf*z[387]*z[132]-rf*z[12]*z[13]*z[123]) - z[16]*(ls*z[27]+rf*z[30]*
  z[387]-z[12]*z[13]*z[36])*u3 - z[16]*(lf*z[507]-ls*z[508]);
  z[529] = z[527] - z[53]*z[512] - z[75]*z[528] - z[393]*z[139] - z[415]*
  z[156];
  z[530] = (2*z[417]*(z[55]*z[97]+z[59]*z[84])*z[143]-z[81]*((z[55]*z[97]+
  z[59]*z[84])*z[522]+z[417]*(z[55]*z[157]+z[59]*z[159]+z[84]*z[136]+z[97]*
  z[118])+(z[55]*z[425]+z[59]*z[427]-rrt*z[12]*z[13]*z[97]-rrt*z[12]*z[14]*
  z[84])*z[143]-z[81]*(z[55]*z[524]+z[59]*z[529]+z[84]*z[520]+z[97]*z[519]+
  z[425]*z[118]+z[427]*z[136]-rrt*z[12]*z[13]*z[157]-rrt*z[12]*z[14]*z[159])))/
  pow(z[81],3);
  z[531] = rf*z[14]*z[16]*z[387]*u3 + lf*z[13]*z[16]*z[453] + z[38]*z[504] + 
  rf*z[388]*z[137] + z[11]*z[14]*z[125] - z[13]*z[15]*z[36]*z[453] - rf*z[13]*
  z[15]*z[387]*q2p - (rrt+z[35])*z[526] - rf*z[14]*z[15]*z[505];
  z[532] = z[531] + z[53]*z[510] - z[74]*z[528] - z[391]*z[139] - z[415]*
  z[134];
  z[533] = lf*z[14]*z[16]*z[453] + rf*z[21]*z[505] + rf*z[387]*z[119] + (rrt+
  z[35])*z[525] - z[14]*z[15]*z[36]*z[453] - z[29]*z[504] - rf*z[388]*z[124] - 
  z[11]*z[13]*z[125];
  z[534] = z[533] + z[57]*z[510] - z[74]*z[523] - z[391]*z[130] - z[411]*
  z[134];
  z[535] = (z[417]*(2*(z[55]*z[90]-z[59]*z[85])*z[143]+z[81]*(z[59]*z[140]+
  z[85]*z[136]-z[55]*z[135]-z[90]*z[118]))+z[81]*((z[59]*z[416]+rrt*z[12]*
  z[13]*z[90]-z[55]*z[412]-rrt*z[12]*z[14]*z[85])*z[143]-(z[55]*z[90]-z[59]*
  z[85])*z[522]-z[81]*(z[59]*z[532]+z[85]*z[520]+z[416]*z[136]+rrt*z[12]*
  z[13]*z[135]-z[55]*z[534]-z[90]*z[519]-z[412]*z[118]-rrt*z[12]*z[14]*z[140])))/
  pow(z[81],3);
  z[536] = z[12]*z[16]*u3 + z[12]*z[24]*u1 - z[11]*z[15]*u1 - z[12]*z[13]*
  z[16]*z[453] - z[11]*z[120];
  z[537] = rf*z[388]*(z[14]*z[27]*q2p-z[13]*z[145]-z[23]*z[137]-z[38]*z[126]) - (
  z[13]*z[27]+z[23]*z[38])*z[504] - (z[13]*z[419]+z[11]*z[14]*z[23])*z[125] - 
  z[35]*(z[14]*z[27]*z[453]+z[14]*z[16]*z[38]*z[453]+z[14]*z[419]*q2p-z[13]*
  z[536]-z[23]*z[526]-z[11]*z[14]*z[126]);
  z[538] = z[537] + z[53]*z[514] + z[395]*z[139] - z[79]*z[528] - z[415]*
  z[148];
  z[539] = z[35]*(z[13]*z[27]*z[453]+z[14]*z[16]*z[29]*z[453]+z[13]*z[419]*
  q2p+z[14]*z[536]-z[23]*z[525]-z[11]*z[13]*z[126]) - (z[14]*z[27]-z[23]*
  z[29])*z[504] - (z[14]*z[419]-z[11]*z[13]*z[23])*z[125] - rf*z[388]*(z[13]*
  z[27]*q2p+z[14]*z[145]-z[23]*z[124]-z[29]*z[126]);
  z[540] = z[539] + z[57]*z[514] + z[395]*z[130] - z[79]*z[523] - z[411]*
  z[148];
  z[541] = (z[417]*(2*(z[55]*z[93]-z[59]*z[89])*z[143]+z[81]*(z[59]*z[151]+
  z[89]*z[136]-z[55]*z[149]-z[93]*z[118]))+z[81]*((z[59]*z[423]+rrt*z[12]*
  z[13]*z[93]-z[55]*z[421]-rrt*z[12]*z[14]*z[89])*z[143]-(z[55]*z[93]-z[59]*
  z[89])*z[522]-z[81]*(z[59]*z[538]+z[89]*z[520]+z[423]*z[136]+rrt*z[12]*
  z[13]*z[149]-z[55]*z[540]-z[93]*z[519]-z[421]*z[118]-rrt*z[12]*z[14]*z[151])))/
  pow(z[81],3);
  z[542] = (z[74]*u1+z[75]*u3+z[79]*u5)*z[508] + z[25]*(u1*z[134]+u3*z[156]+
  u5*z[148]) + (z[391]*u1+z[393]*u3-z[395]*u5)*z[129] + z[26]*(u3*z[512]-u1*
  z[510]-u5*z[514]) - z[15]*u3*(z[450]*u1+z[451]*u5-z[452]*u3) - z[14]*z[15]*
  u1*z[453] - z[16]*(u3*z[530]-u1*z[535]-u5*z[541]);
  z[543] = IEyy*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-
  z[101]*u1-z[105]*u5))*(z[15]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[27]*(z[74]*
  u1+z[75]*u3+z[79]*u5)-z[28]*(z[391]*u1+z[393]*u3-z[395]*u5)) + IEyy*(z[28]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))*(
  z[16]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[25]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[26]*(z[391]*u1+z[393]*u3-z[395]*u5)) - IExz*(u3+z[14]*u1+z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5))*(z[15]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[27]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[28]*(z[391]*u1+z[393]*u3-z[395]*u5)) - IExz*(z[12]*
  z[13]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[30]*(z[391]*u1+z[393]*u3-z[395]*u5))*(
  z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-
  z[105]*u5)) - IExx*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(
  z[100]*u3-z[101]*u1-z[105]*u5))*(z[15]*(z[450]*u1+z[451]*u5-z[452]*u3)-
  z[27]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[28]*(z[391]*u1+z[393]*u3-z[395]*u5)) - 
  IExx*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*
  u1-z[105]*u5))*(z[16]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[25]*(z[74]*u1+z[75]*
  u3+z[79]*u5)-z[26]*(z[391]*u1+z[393]*u3-z[395]*u5)) - IExz*z[542] - IEzz*
  z[516];
  z[544] = z[11]*u1*(z[391]*u1+z[393]*u3-z[395]*u5) + u1*z[535] + u5*z[541] + 
  z[11]*(u1*z[134]+u3*z[156]+u5*z[148]) + z[12]*(u3*z[512]-u1*z[510]-u5*
  z[514]) - z[12]*u1*(z[74]*u1+z[75]*u3+z[79]*u5) - u3*z[530];
  z[545] = IDzz*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[12]*z[14]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[39]*(z[391]*u1+z[393]*u3-z[395]*u5)) + IDxx*(
  z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[12]*z[13]*(z[74]*u1+z[75]*
  u3+z[79]*u5)-z[30]*(z[391]*u1+z[393]*u3-z[395]*u5)) + (z[12]*z[13]*(z[74]*
  u1+z[75]*u3+z[79]*u5)-z[30]*(z[391]*u1+z[393]*u3-z[395]*u5))*(2*IDxz*(z[14]*
  u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-IDzz*(z[13]*u1-z[39]*(z[74]*u1+z[75]*
  u3+z[79]*u5))) - (z[12]*z[14]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[39]*(z[391]*u1+
  z[393]*u3-z[395]*u5))*(IDxx*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-2*
  IDxz*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))) - IDyy*z[544];
  z[546] = z[16]*u3*(z[450]*u1+z[451]*u5-z[452]*u3) + z[14]*z[16]*u1*z[453] + (
  z[74]*u1+z[75]*u3+z[79]*u5)*z[502] + z[27]*(u1*z[134]+u3*z[156]+u5*z[148]) + (
  z[391]*u1+z[393]*u3-z[395]*u5)*z[121] + z[28]*(u3*z[512]-u1*z[510]-u5*
  z[514]) - z[15]*(u3*z[530]-u1*z[535]-u5*z[541]);
  z[547] = IExx*(z[12]*z[13]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[30]*(z[391]*u1+
  z[393]*u3-z[395]*u5))*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(
  z[100]*u3-z[101]*u1-z[105]*u5)) + (z[12]*z[13]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[30]*(z[391]*u1+z[393]*u3-z[395]*u5))*(2*IExz*(u3+z[14]*u1+z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5))-IEzz*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(
  z[100]*u3-z[101]*u1-z[105]*u5))) + (z[16]*(z[450]*u1+z[451]*u5-z[452]*u3)-
  z[25]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[26]*(z[391]*u1+z[393]*u3-z[395]*u5))*(
  IExx*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-2*IExz*(z[21]*u1+
  z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))) - 
  IEzz*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[16]*(z[450]*u1+
  z[451]*u5-z[452]*u3)-z[25]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[26]*(z[391]*u1+
  z[393]*u3-z[395]*u5)) - IEyy*z[546];
  z[548] = IEzz*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[15]*(
  z[450]*u1+z[451]*u5-z[452]*u3)-z[27]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[28]*(
  z[391]*u1+z[393]*u3-z[395]*u5)) + IEzz*(z[12]*z[13]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[30]*(z[391]*u1+z[393]*u3-z[395]*u5))*(z[28]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) + IExz*(z[21]*u1+
  z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))*(
  z[15]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[27]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[28]*(z[391]*u1+z[393]*u3-z[395]*u5)) + IExz*(z[28]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))*(z[16]*(z[450]*u1+
  z[451]*u5-z[452]*u3)-z[25]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[26]*(z[391]*u1+
  z[393]*u3-z[395]*u5)) - IEyy*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(
  z[15]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[27]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[28]*(z[391]*u1+z[393]*u3-z[395]*u5)) - IEyy*(z[12]*z[13]*(z[74]*u1+z[75]*
  u3+z[79]*u5)-z[30]*(z[391]*u1+z[393]*u3-z[395]*u5))*(z[28]*(z[74]*u1+z[75]*
  u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IExx*z[542] - 
  IExz*z[516];
  z[549] = lrx*z[13]*u1*z[453] + lrz*z[14]*u1*z[453] + lrx*(z[74]*u1+z[75]*u3+
  z[79]*u5)*z[507] + lrz*(z[74]*u1+z[75]*u3+z[79]*u5)*z[501] + lrx*(z[391]*u1+
  z[393]*u3-z[395]*u5)*z[132] + lrz*(z[391]*u1+z[393]*u3-z[395]*u5)*z[173] + 
  lrx*z[30]*(u3*z[512]-u1*z[510]-u5*z[514]) + lrz*z[39]*(u3*z[512]-u1*z[510]-
  u5*z[514]) - rrt*z[11]*pow(u1,2) - lrx*z[12]*z[13]*(u1*z[134]+u3*z[156]+u5*
  z[148]) - lrz*z[12]*z[14]*(u1*z[134]+u3*z[156]+u5*z[148]);
  z[550] = mr*((z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(rrt*z[12]*z[13]*(
  z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)+lrz*(z[450]*u1+
  z[451]*u5-z[452]*u3-z[11]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[12]*(z[391]*u1+
  z[393]*u3-z[395]*u5))-rrt*z[11]*z[14]*u1-rrt*z[11]*z[13]*(z[418]*u1+z[424]*
  u5+z[450]*u1+z[451]*u5-z[428]*u3-z[452]*u3)-rr*z[13]*(z[418]*u1+z[424]*u5+
  z[450]*u1+z[451]*u5-z[428]*u3-z[452]*u3-z[11]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[12]*(z[391]*u1+z[393]*u3-z[395]*u5)))-(z[12]*z[13]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[30]*(z[391]*u1+z[393]*u3-z[395]*u5))*(lrz*(z[100]*u3-z[101]*u1-
  z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))-rrt*z[38]*u1-rrt*z[11]*z[13]*(
  z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)-rr*z[13]*(
  z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+
  z[75]*u3+z[79]*u5)))-(z[12]*z[14]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[39]*(
  z[391]*u1+z[393]*u3-z[395]*u5))*(rrt*z[29]*u1-lrx*(z[100]*u3-z[101]*u1-
  z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))-rrt*z[11]*z[14]*(z[100]*u3+
  z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)-rr*z[14]*(z[100]*u3+
  z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+
  z[79]*u5)))-(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))*(rrt*z[11]*z[13]*
  u1+rrt*z[12]*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5)-rrt*z[11]*z[14]*(z[418]*u1+z[424]*u5+z[450]*u1+z[451]*u5-z[428]*
  u3-z[452]*u3)-lrx*(z[450]*u1+z[451]*u5-z[452]*u3-z[11]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[12]*(z[391]*u1+z[393]*u3-z[395]*u5))-rr*z[14]*(z[418]*u1+z[424]*
  u5+z[450]*u1+z[451]*u5-z[428]*u3-z[452]*u3-z[11]*(z[74]*u1+z[75]*u3+z[79]*
  u5)-z[12]*(z[391]*u1+z[393]*u3-z[395]*u5)))-z[549]);
  z[551] = lfz*z[15]*u3*(z[450]*u1+z[451]*u5-z[452]*u3) + lfx*z[13]*u1*z[453] + 
  lfz*z[14]*z[15]*u1*z[453] + rf*z[13]*z[27]*z[388]*u3*q2p + z[27]*(u1+z[14]*
  u3)*z[504] + z[419]*(u1+z[14]*u3)*z[125] + rf*z[388]*(u1+z[14]*u3)*z[145] + 
  lfx*(z[74]*u1+z[75]*u3+z[79]*u5)*z[507] + lfx*(z[391]*u1+z[393]*u3-z[395]*
  u5)*z[132] + lfx*z[30]*(u3*z[512]-u1*z[510]-u5*z[514]) + lfz*z[16]*(u3*
  z[530]-u1*z[535]-u5*z[541]) + z[35]*(z[11]*z[13]*u3+z[11]*(z[450]*u1+z[451]*
  u5-z[452]*u3)-z[12]*(z[100]*u3-z[101]*u1-z[105]*u5))*z[126] - z[13]*z[27]*
  z[35]*u3*z[453] - z[14]*z[16]*z[35]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5))*z[453] - z[13]*z[35]*z[419]*u3*q2p - z[35]*(u1+z[14]*u3)*z[536] - 
  lfz*(z[74]*u1+z[75]*u3+z[79]*u5)*z[508] - lfz*z[25]*(u1*z[134]+u3*z[156]+u5*
  z[148]) - lfx*z[12]*z[13]*(u1*z[134]+u3*z[156]+u5*z[148]) - lfz*(z[391]*u1+
  z[393]*u3-z[395]*u5)*z[129] - lfz*z[26]*(u3*z[512]-u1*z[510]-u5*z[514]) - 
  z[23]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))*z[504] - rf*z[388]*(
  z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))*z[126] - z[23]*(z[11]*z[13]*
  u3+z[11]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[12]*(z[100]*u3-z[101]*u1-z[105]*
  u5))*z[125] - rf*z[23]*z[388]*(u3*z[124]-z[12]*u1*(z[100]*u3-z[101]*u1-
  z[105]*u5)-z[11]*(u3*z[160]-u1*z[144]-u5*z[152])) - z[23]*z[35]*(z[12]*u1*(
  z[450]*u1+z[451]*u5-z[452]*u3)+z[11]*u1*(z[100]*u3-z[101]*u1-z[105]*u5)-u3*
  z[525]-z[11]*(u3*z[530]-u1*z[535]-u5*z[541])-z[12]*(u3*z[160]-u1*z[144]-u5*
  z[152]));
  z[552] = mf*((z[16]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[25]*(z[74]*u1+z[75]*
  u3+z[79]*u5)-z[26]*(z[391]*u1+z[393]*u3-z[395]*u5))*(lfx*z[28]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-lfx*z[23]*u1-z[29]*z[35]*(u1-z[23]*u5)-lfx*z[15]*(z[100]*
  u3-z[101]*u1-z[105]*u5)-z[14]*z[35]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5))-2*z[36]*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(
  z[100]*u3-z[101]*u1-z[105]*u5)))+(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*
  u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))*(z[11]*z[13]*z[35]*(u1-z[23]*u5)+
  lfx*z[15]*(z[450]*u1+z[451]*u5-z[452]*u3)+rf*z[387]*(z[21]*u1+z[26]*(z[74]*
  u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))-rf*z[29]*
  z[388]*(u1-z[23]*u5)-lfx*z[27]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*z[28]*(
  z[391]*u1+z[393]*u3-z[395]*u5)-rf*z[14]*z[388]*(z[27]*u5-z[11]*(z[100]*u3-
  z[101]*u1-z[105]*u5))-z[14]*z[35]*(z[11]*(z[450]*u1+z[451]*u5-z[452]*u3)-
  z[419]*u5-z[12]*(z[100]*u3-z[101]*u1-z[105]*u5)))-(z[12]*z[13]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[30]*(z[391]*u1+z[393]*u3-z[395]*u5))*(lfz*z[13]*z[16]*
  u1+z[25]*z[35]*(u1+z[14]*u3-z[23]*u5)+lfz*z[15]*(z[100]*u3-z[101]*u1-z[105]*
  u5)+2*z[36]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-lfz*z[28]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[21]*z[35]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-
  z[101]*u1-z[105]*u5)))-(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(
  lfz*z[27]*(z[74]*u1+z[75]*u3+z[79]*u5)+lfz*z[28]*(z[391]*u1+z[393]*u3-
  z[395]*u5)+rf*z[25]*z[388]*(u1+z[14]*u3-z[23]*u5)-z[35]*z[472]*(u1+z[14]*u3-
  z[23]*u5)-lfz*z[15]*(z[450]*u1+z[451]*u5-z[452]*u3)-rf*z[387]*(u3+z[14]*u1+
  z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-rf*z[21]*z[388]*(z[27]*u5-z[29]*u3-
  z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))-z[21]*z[35]*(z[11]*z[13]*u3+z[11]*(
  z[450]*u1+z[451]*u5-z[452]*u3)-z[419]*u5-z[12]*(z[100]*u3-z[101]*u1-z[105]*
  u5)))-z[551]);
  z[553] = IFyy*((z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*
  u3-z[101]*u1-z[105]*u5))*(z[15]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[27]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[28]*(z[391]*u1+z[393]*u3-z[395]*u5))+(u5+
  z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-
  z[105]*u5))*(z[16]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[25]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[26]*(z[391]*u1+z[393]*u3-z[395]*u5)));
  z[554] = IDxz*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*
  u5))*(z[12]*z[14]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[39]*(z[391]*u1+z[393]*u3-
  z[395]*u5)) + IDyy*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+
  z[79]*u5))*(z[12]*z[13]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[30]*(z[391]*u1+
  z[393]*u3-z[395]*u5)) + IDzz*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(
  z[450]*u1+z[451]*u5-z[452]*u3-z[11]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[12]*(
  z[391]*u1+z[393]*u3-z[395]*u5)) + IDxz*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+
  z[79]*u5))*(z[450]*u1+z[451]*u5-z[452]*u3-z[11]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[12]*(z[391]*u1+z[393]*u3-z[395]*u5)) - IDyy*(z[14]*u1+z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5))*(z[450]*u1+z[451]*u5-z[452]*u3-z[11]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[12]*(z[391]*u1+z[393]*u3-z[395]*u5)) - IDzz*(z[100]*u3-z[101]*
  u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[12]*z[13]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[30]*(z[391]*u1+z[393]*u3-z[395]*u5)) - IDxx*z[515] - 
  IDxz*z[516];
  z[555] = (2*z[417]*(z[54]*z[97]+z[58]*z[84])*z[143]-z[81]*((z[54]*z[97]+
  z[58]*z[84])*z[522]+(z[54]*z[425]+z[58]*z[427]+z[84]*z[413]+z[97]*z[409])*
  z[143]+z[417]*(z[54]*z[157]+z[58]*z[159]+z[84]*z[141]+z[97]*z[142])-z[81]*(
  z[54]*z[524]+z[58]*z[529]+z[84]*z[518]+z[97]*z[521]+z[409]*z[157]+z[413]*
  z[159]+z[425]*z[142]+z[427]*z[141])))/pow(z[81],3);
  z[556] = (2*z[417]*(z[54]*z[90]-z[58]*z[85])*z[143]-z[81]*((z[54]*z[90]-
  z[58]*z[85])*z[522]-(z[58]*z[416]+z[85]*z[413]-z[54]*z[412]-z[90]*z[409])*
  z[143]-z[417]*(z[58]*z[140]+z[85]*z[141]-z[54]*z[135]-z[90]*z[142])-z[81]*(
  z[54]*z[534]+z[90]*z[521]+z[409]*z[135]+z[412]*z[142]-z[58]*z[532]-z[85]*
  z[518]-z[413]*z[140]-z[416]*z[141])))/pow(z[81],3);
  z[557] = (2*z[417]*(z[54]*z[93]-z[58]*z[89])*z[143]-z[81]*((z[54]*z[93]-
  z[58]*z[89])*z[522]-(z[58]*z[423]+z[89]*z[413]-z[54]*z[421]-z[93]*z[409])*
  z[143]-z[417]*(z[58]*z[151]+z[89]*z[141]-z[54]*z[149]-z[93]*z[142])-z[81]*(
  z[54]*z[540]+z[93]*z[521]+z[409]*z[149]+z[421]*z[142]-z[58]*z[538]-z[89]*
  z[518]-z[413]*z[151]-z[423]*z[141])))/pow(z[81],3);
  z[558] = z[11]*u1*(z[391]*u1+z[393]*u3-z[395]*u5) + u1*z[535] + u1*z[556] + 
  u5*z[541] + u5*z[557] + z[11]*(u1*z[134]+u3*z[156]+u5*z[148]) + z[12]*(u3*
  z[512]-u1*z[510]-u5*z[514]) - z[12]*u1*(z[74]*u1+z[75]*u3+z[79]*u5) - u3*
  z[530] - u3*z[555];
  z[559] = rrt*z[11]*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5)*z[453] + rr*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-
  z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))*z[453] + rrt*z[11]*
  z[13]*(z[418]*u1+z[424]*u5+z[450]*u1+z[451]*u5-z[428]*u3-z[452]*u3)*q2p + 
  rr*z[13]*(z[418]*u1+z[424]*u5+z[450]*u1+z[451]*u5-z[428]*u3-z[452]*u3-z[11]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[12]*(z[391]*u1+z[393]*u3-z[395]*u5))*q2p + 
  rrt*z[11]*z[14]*(u3*z[530]+u3*z[555]-u1*z[535]-u1*z[556]-u5*z[541]-u5*
  z[557]) + rrt*z[12]*z[14]*(u3*z[160]+u3*z[161]-u1*z[144]-u1*z[153]-u5*
  z[152]-u5*z[154]) + lrx*(z[12]*u1*(z[74]*u1+z[75]*u3+z[79]*u5)+u3*z[530]-
  z[11]*u1*(z[391]*u1+z[393]*u3-z[395]*u5)-u1*z[535]-u5*z[541]-z[11]*(u1*
  z[134]+u3*z[156]+u5*z[148])-z[12]*(u3*z[512]-u1*z[510]-u5*z[514])) + rr*
  z[14]*(z[12]*u1*(z[74]*u1+z[75]*u3+z[79]*u5)+u3*z[530]+u3*z[555]-z[11]*u1*(
  z[391]*u1+z[393]*u3-z[395]*u5)-u1*z[535]-u1*z[556]-u5*z[541]-u5*z[557]-
  z[11]*(u1*z[134]+u3*z[156]+u5*z[148])-z[12]*(u3*z[512]-u1*z[510]-u5*z[514])) - 
  rrt*z[12]*z[14]*u1*(z[418]*u1+z[424]*u5+z[450]*u1+z[451]*u5-z[428]*u3-
  z[452]*u3) - rrt*z[11]*z[14]*u1*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-
  z[110]*u1-z[114]*u5) - rrt*z[12]*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-
  z[105]*u5-z[110]*u1-z[114]*u5)*q2p - rrt*u1*z[525];
  z[560] = mr*((z[12]*z[14]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[39]*(z[391]*u1+
  z[393]*u3-z[395]*u5))*(lrz*z[13]*u1-rr*u1-lrx*z[14]*u1-rrt*z[11]*u1-lrx*
  z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-lrz*z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))+(
  z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))*(rrt*z[12]*u1+lrx*z[12]*z[13]*(
  z[74]*u1+z[75]*u3+z[79]*u5)+lrz*z[12]*z[14]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  lrx*z[30]*(z[391]*u1+z[393]*u3-z[395]*u5)-lrz*z[39]*(z[391]*u1+z[393]*u3-
  z[395]*u5))+(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*
  u5))*(rrt*z[12]*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5)-rrt*z[11]*z[14]*u1-rrt*z[11]*z[13]*(z[418]*u1+z[424]*u5+z[450]*
  u1+z[451]*u5-z[428]*u3-z[452]*u3)-rr*z[13]*(z[418]*u1+z[424]*u5+z[450]*u1+
  z[451]*u5-z[428]*u3-z[452]*u3-z[11]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[12]*(
  z[391]*u1+z[393]*u3-z[395]*u5)))+(z[450]*u1+z[451]*u5-z[452]*u3-z[11]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[12]*(z[391]*u1+z[393]*u3-z[395]*u5))*(2*lrz*(
  z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))-rrt*z[38]*
  u1-rrt*z[11]*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5)-rr*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))-z[559]);
  z[561] = rrt*z[12]*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5)*q2p + rrt*u1*z[526] + rrt*z[11]*z[13]*(u3*z[530]+u3*z[555]-u1*
  z[535]-u1*z[556]-u5*z[541]-u5*z[557]) + rrt*z[12]*z[13]*(u3*z[160]+u3*
  z[161]-u1*z[144]-u1*z[153]-u5*z[152]-u5*z[154]) + rr*z[13]*(z[12]*u1*(z[74]*
  u1+z[75]*u3+z[79]*u5)+u3*z[530]+u3*z[555]-z[11]*u1*(z[391]*u1+z[393]*u3-
  z[395]*u5)-u1*z[535]-u1*z[556]-u5*z[541]-u5*z[557]-z[11]*(u1*z[134]+u3*
  z[156]+u5*z[148])-z[12]*(u3*z[512]-u1*z[510]-u5*z[514])) - rrt*z[12]*z[13]*
  u1*(z[418]*u1+z[424]*u5+z[450]*u1+z[451]*u5-z[428]*u3-z[452]*u3) - rrt*
  z[11]*z[13]*u1*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5) - 
  rrt*z[11]*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*
  u5)*z[453] - rr*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))*z[453] - rrt*z[11]*z[14]*(
  z[418]*u1+z[424]*u5+z[450]*u1+z[451]*u5-z[428]*u3-z[452]*u3)*q2p - rr*z[14]*(
  z[418]*u1+z[424]*u5+z[450]*u1+z[451]*u5-z[428]*u3-z[452]*u3-z[11]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[12]*(z[391]*u1+z[393]*u3-z[395]*u5))*q2p - lrz*(z[12]*
  u1*(z[74]*u1+z[75]*u3+z[79]*u5)+u3*z[530]-z[11]*u1*(z[391]*u1+z[393]*u3-
  z[395]*u5)-u1*z[535]-u5*z[541]-z[11]*(u1*z[134]+u3*z[156]+u5*z[148])-z[12]*(
  u3*z[512]-u1*z[510]-u5*z[514]));
  z[562] = mr*((z[12]*z[13]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[30]*(z[391]*u1+
  z[393]*u3-z[395]*u5))*(lrz*z[13]*u1-rr*u1-lrx*z[14]*u1-rrt*z[11]*u1-lrx*
  z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-lrz*z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))-(
  z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(rrt*z[12]*u1+lrx*z[12]*z[13]*(
  z[74]*u1+z[75]*u3+z[79]*u5)+lrz*z[12]*z[14]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  lrx*z[30]*(z[391]*u1+z[393]*u3-z[395]*u5)-lrz*z[39]*(z[391]*u1+z[393]*u3-
  z[395]*u5))-(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*
  u5))*(rrt*z[11]*z[13]*u1+rrt*z[12]*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-
  z[105]*u5-z[110]*u1-z[114]*u5)-rrt*z[11]*z[14]*(z[418]*u1+z[424]*u5+z[450]*
  u1+z[451]*u5-z[428]*u3-z[452]*u3)-rr*z[14]*(z[418]*u1+z[424]*u5+z[450]*u1+
  z[451]*u5-z[428]*u3-z[452]*u3-z[11]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[12]*(
  z[391]*u1+z[393]*u3-z[395]*u5)))-(z[450]*u1+z[451]*u5-z[452]*u3-z[11]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[12]*(z[391]*u1+z[393]*u3-z[395]*u5))*(rrt*
  z[29]*u1-2*lrx*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+
  z[79]*u5))-rrt*z[11]*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*
  u1-z[114]*u5)-rr*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))-z[561]);
  z[563] = z[12]*z[13]*z[15]*z[453] + z[11]*z[128] - z[11]*z[16]*u1 - z[12]*
  z[15]*u3 - z[12]*z[22]*u1;
  z[564] = lfz*z[16]*u3*(z[450]*u1+z[451]*u5-z[452]*u3) + lfz*z[14]*z[16]*u1*
  z[453] + lfz*(z[74]*u1+z[75]*u3+z[79]*u5)*z[502] + lfz*z[27]*(u1*z[134]+u3*
  z[156]+u5*z[148]) + z[25]*(u1+z[14]*u3-z[23]*u5)*z[504] + z[472]*(u1+z[14]*
  u3-z[23]*u5)*z[125] + lfz*(z[391]*u1+z[393]*u3-z[395]*u5)*z[121] + rf*z[25]*
  z[388]*(z[13]*u3*q2p-u5*z[126]) + rf*z[388]*(u1+z[14]*u3-z[23]*u5)*z[218] + 
  lfz*z[28]*(u3*z[512]-u1*z[510]-u5*z[514]) + rf*(z[12]*z[13]*(z[74]*u1+z[75]*
  u3+z[79]*u5)-z[30]*(z[391]*u1+z[393]*u3-z[395]*u5))*z[123] + z[21]*(z[11]*
  z[13]*u3+z[11]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[419]*u5-z[12]*(z[100]*u3-
  z[101]*u1-z[105]*u5))*z[125] + rf*z[21]*z[388]*(u3*z[124]-z[12]*u1*(z[100]*
  u3-z[101]*u1-z[105]*u5)-u5*z[145]-z[11]*(u3*z[160]-u1*z[144]-u5*z[152])) + 
  z[36]*(z[12]*z[13]*(u1*z[134]+u3*z[156]+u5*z[148])-z[13]*u1*z[453]-(z[74]*
  u1+z[75]*u3+z[79]*u5)*z[507]-(z[391]*u1+z[393]*u3-z[395]*u5)*z[132]-z[30]*(
  u3*z[512]-u1*z[510]-u5*z[514])) + z[21]*z[35]*(z[12]*u1*(z[450]*u1+z[451]*
  u5-z[452]*u3)+z[11]*u1*(z[100]*u3-z[101]*u1-z[105]*u5)+u5*z[536]-u3*z[525]-
  z[11]*(u3*z[530]-u1*z[535]-u5*z[541])-z[12]*(u3*z[160]-u1*z[144]-u5*z[152])) - 
  z[25]*z[35]*(z[13]*u3+z[14]*z[16]*u5)*z[453] - z[14]*z[15]*z[35]*(z[27]*u5-
  z[29]*u3-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))*z[453] - z[35]*(u1+z[14]*u3-
  z[23]*u5)*z[563] - z[35]*z[472]*(z[13]*u3*q2p-u5*z[126]) - lfz*z[15]*(u3*
  z[530]-u1*z[535]-u5*z[541]) - rf*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+
  z[79]*u5))*z[505] - z[21]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5))*z[504] - rf*z[388]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*
  u1-z[105]*u5))*z[119] - rf*z[387]*(z[13]*u1*q2p+(z[74]*u1+z[75]*u3+z[79]*u5)*
  z[132]+z[30]*(u1*z[134]+u3*z[156]+u5*z[148])) - z[35]*(z[11]*z[13]*u3+z[11]*(
  z[450]*u1+z[451]*u5-z[452]*u3)-z[419]*u5-z[12]*(z[100]*u3-z[101]*u1-z[105]*
  u5))*z[119];
  z[565] = mf*((z[12]*z[13]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[30]*(z[391]*u1+
  z[393]*u3-z[395]*u5))*(lfz*z[13]*z[15]*u1+z[27]*z[35]*(u1+z[14]*u3)+lfz*
  z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*u3-lfx*z[14]*u1-lfx*z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-lfz*z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[23]*z[35]*(
  z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)))-(z[15]*(z[450]*u1+z[451]*
  u5-z[452]*u3)-z[27]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[28]*(z[391]*u1+z[393]*u3-
  z[395]*u5))*(lfx*z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*z[23]*u1-z[29]*
  z[35]*(u1-z[23]*u5)-lfx*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[14]*z[35]*(
  z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))-z[36]*(z[21]*u1+z[26]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)))-(u3+
  z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[35]*z[419]*(u1+z[14]*u3)+
  lfz*z[25]*(z[74]*u1+z[75]*u3+z[79]*u5)+lfx*z[12]*z[13]*(z[74]*u1+z[75]*u3+
  z[79]*u5)+lfz*z[26]*(z[391]*u1+z[393]*u3-z[395]*u5)+rf*z[23]*z[388]*(z[29]*
  u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))-rf*z[27]*z[388]*(u1+z[14]*u3)-lfx*
  z[30]*(z[391]*u1+z[393]*u3-z[395]*u5)-lfz*z[16]*(z[450]*u1+z[451]*u5-z[452]*
  u3)-z[23]*z[35]*(z[11]*z[13]*u3+z[11]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[12]*(
  z[100]*u3-z[101]*u1-z[105]*u5)))-(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*
  u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))*(z[11]*z[13]*z[35]*(u1-z[23]*u5)+
  lfx*z[15]*(z[450]*u1+z[451]*u5-z[452]*u3)+rf*z[387]*(z[21]*u1+z[26]*(z[74]*
  u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))-rf*z[29]*
  z[388]*(u1-z[23]*u5)-lfx*z[27]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*z[28]*(
  z[391]*u1+z[393]*u3-z[395]*u5)-rf*z[14]*z[388]*(z[27]*u5-z[11]*(z[100]*u3-
  z[101]*u1-z[105]*u5))-z[14]*z[35]*(z[11]*(z[450]*u1+z[451]*u5-z[452]*u3)-
  z[419]*u5-z[12]*(z[100]*u3-z[101]*u1-z[105]*u5))-z[36]*(z[16]*(z[450]*u1+
  z[451]*u5-z[452]*u3)-z[25]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[26]*(z[391]*u1+
  z[393]*u3-z[395]*u5)))-z[564]);
  z[566] = z[14]*z[16]*z[29]*z[35]*u5*z[453] + z[13]*z[35]*(z[27]*u5-z[11]*(
  z[100]*u3-z[101]*u1-z[105]*u5))*z[453] + rf*z[29]*z[388]*u5*z[126] + z[35]*(
  u1-z[23]*u5)*z[525] + lfx*z[15]*(u3*z[530]-u1*z[535]-u5*z[541]) + rf*(z[21]*
  u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))*
  z[505] + z[14]*(z[11]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[419]*u5-z[12]*(
  z[100]*u3-z[101]*u1-z[105]*u5))*z[125] + z[14]*z[35]*(z[12]*u1*(z[450]*u1+
  z[451]*u5-z[452]*u3)+z[11]*u1*(z[100]*u3-z[101]*u1-z[105]*u5)+u5*z[536]-
  z[11]*(u3*z[530]-u1*z[535]-u5*z[541])-z[12]*(u3*z[160]-u1*z[144]-u5*z[152])) - 
  lfx*z[16]*u3*(z[450]*u1+z[451]*u5-z[452]*u3) - lfx*z[14]*z[16]*u1*z[453] - 
  rf*z[13]*z[388]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))*q2p - 
  z[13]*z[35]*(z[11]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[419]*u5-z[12]*(z[100]*
  u3-z[101]*u1-z[105]*u5))*q2p - z[11]*z[13]*z[35]*u5*z[126] - z[29]*(u1-
  z[23]*u5)*z[504] - rf*z[388]*(u1-z[23]*u5)*z[124] - z[11]*z[13]*(u1-z[23]*
  u5)*z[125] - lfx*(z[74]*u1+z[75]*u3+z[79]*u5)*z[502] - lfx*z[27]*(u1*z[134]+
  u3*z[156]+u5*z[148]) - lfx*(z[391]*u1+z[393]*u3-z[395]*u5)*z[121] - lfx*
  z[28]*(u3*z[512]-u1*z[510]-u5*z[514]) - z[14]*(z[27]*u5-z[11]*(z[100]*u3-
  z[101]*u1-z[105]*u5))*z[504] - rf*z[14]*z[388]*(z[12]*u1*(z[100]*u3-z[101]*
  u1-z[105]*u5)+u5*z[145]+z[11]*(u3*z[160]-u1*z[144]-u5*z[152])) - rf*(z[16]*(
  z[450]*u1+z[451]*u5-z[452]*u3)-z[25]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[26]*(
  z[391]*u1+z[393]*u3-z[395]*u5))*z[123] - rf*z[387]*(z[15]*u3*(z[100]*u3-
  z[101]*u1-z[105]*u5)-u1*z[119]-(z[74]*u1+z[75]*u3+z[79]*u5)*z[129]-z[26]*(
  u1*z[134]+u3*z[156]+u5*z[148])-z[16]*(u3*z[160]-u1*z[144]-u5*z[152])) - 
  z[36]*(z[15]*u3*(z[450]*u1+z[451]*u5-z[452]*u3)+z[14]*z[15]*u1*z[453]+z[16]*(
  u3*z[530]-u1*z[535]-u5*z[541])-(z[74]*u1+z[75]*u3+z[79]*u5)*z[508]-z[25]*(
  u1*z[134]+u3*z[156]+u5*z[148])-(z[391]*u1+z[393]*u3-z[395]*u5)*z[129]-z[26]*(
  u3*z[512]-u1*z[510]-u5*z[514]));
  z[567] = mf*((z[15]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[27]*(z[74]*u1+z[75]*
  u3+z[79]*u5)-z[28]*(z[391]*u1+z[393]*u3-z[395]*u5))*(lfz*z[13]*z[16]*u1+
  z[25]*z[35]*(u1+z[14]*u3-z[23]*u5)+lfz*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)+
  z[36]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-lfz*z[28]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[21]*z[35]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*
  u1-z[105]*u5)))+(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*
  u3-z[101]*u1-z[105]*u5))*(z[35]*z[419]*(u1+z[14]*u3)+lfz*z[25]*(z[74]*u1+
  z[75]*u3+z[79]*u5)+lfx*z[12]*z[13]*(z[74]*u1+z[75]*u3+z[79]*u5)+lfz*z[26]*(
  z[391]*u1+z[393]*u3-z[395]*u5)+rf*z[23]*z[388]*(z[29]*u3+z[11]*(z[100]*u3-
  z[101]*u1-z[105]*u5))-rf*z[27]*z[388]*(u1+z[14]*u3)-lfx*z[30]*(z[391]*u1+
  z[393]*u3-z[395]*u5)-lfz*z[16]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[23]*z[35]*(
  z[11]*z[13]*u3+z[11]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[12]*(z[100]*u3-
  z[101]*u1-z[105]*u5)))+(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(
  z[100]*u3-z[101]*u1-z[105]*u5))*(lfz*z[27]*(z[74]*u1+z[75]*u3+z[79]*u5)+lfz*
  z[28]*(z[391]*u1+z[393]*u3-z[395]*u5)+rf*z[25]*z[388]*(u1+z[14]*u3-z[23]*u5)+
  z[36]*(z[12]*z[13]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[30]*(z[391]*u1+z[393]*u3-
  z[395]*u5))-z[35]*z[472]*(u1+z[14]*u3-z[23]*u5)-lfz*z[15]*(z[450]*u1+z[451]*
  u5-z[452]*u3)-rf*z[387]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-rf*
  z[21]*z[388]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))-
  z[21]*z[35]*(z[11]*z[13]*u3+z[11]*(z[450]*u1+z[451]*u5-z[452]*u3)-z[419]*u5-
  z[12]*(z[100]*u3-z[101]*u1-z[105]*u5)))-(z[16]*(z[450]*u1+z[451]*u5-z[452]*
  u3)-z[25]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[26]*(z[391]*u1+z[393]*u3-z[395]*u5))*(
  lfz*z[13]*z[15]*u1+z[27]*z[35]*(u1+z[14]*u3)+lfz*z[26]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-lfx*u3-lfx*z[14]*u1-lfx*z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfz*
  z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[23]*z[35]*(z[29]*u3+z[11]*(z[100]*
  u3-z[101]*u1-z[105]*u5)))-z[566]);
  z[568] = z[11]*z[79]*z[499] + z[30]*z[79]*z[553] + z[30]*z[395]*z[265] + 
  z[12]*z[13]*z[79]*z[265] + (z[25]*z[79]-z[16]*z[451]-z[26]*z[395])*z[266] + 
  z[30]*z[79]*z[517] + z[30]*z[79]*z[543] + z[39]*z[395]*z[240] + z[12]*z[14]*
  z[79]*z[240] + (z[105]+z[12]*z[79])*z[545] + (z[15]*z[105]+z[28]*z[79])*
  z[547] + (z[16]*z[105]+z[26]*z[79])*z[548] + z[79]*(lrx*z[30]+lrz*z[39])*
  z[550] + (z[11]*z[79]-z[451]-z[12]*z[395])*z[248] + (z[25]*z[79]-z[16]*
  z[451]-z[26]*z[395])*z[252] + (z[27]*z[79]-z[15]*z[451]-z[28]*z[395])*
  z[260] + (lfx*z[30]*z[79]-lfz*z[16]*z[105]-lfz*z[26]*z[79]-z[11]*z[23]*
  z[35]*z[105])*z[552] + (lrz*(z[105]+z[12]*z[79])-rrt*z[11]*z[13]*(z[105]+
  z[114])-rr*z[13]*(z[105]+z[114]+z[12]*z[79]))*z[562] + (rrt*z[11]*z[14]*(
  z[424]+z[451])+rrt*z[12]*z[14]*(z[105]+z[114])-lrx*(z[11]*z[79]-z[451]-
  z[12]*z[395])-rr*z[14]*(z[11]*z[79]-z[424]-z[451]-z[12]*z[395]))*z[278] - 
  z[11]*z[395]*z[235] - z[12]*z[79]*z[235] - (z[16]*z[105]+z[26]*z[79])*
  z[500] - z[30]*z[395]*z[244] - z[30]*z[395]*z[256] - z[39]*z[79]*z[554] - 
  z[12]*z[13]*z[79]*z[244] - z[12]*z[13]*z[79]*z[256] - ICyy*(z[105]+z[114]+
  z[12]*z[79])*z[558] - z[395]*(lrx*z[30]+lrz*z[39])*z[274] - IFyy*(1+z[15]*
  z[105]+z[28]*z[79])*z[546] - z[12]*z[79]*(lrx*z[13]+lrz*z[14])*z[274] - (
  z[27]*z[79]-z[15]*z[451]-z[28]*z[395])*z[264] - (z[11]*z[79]-z[424]-z[451]-
  z[12]*z[395])*z[234] - (lrx*(z[105]+z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+
  z[114])+rr*z[14]*(z[105]+z[114]+z[12]*z[79]))*z[560] - (z[30]*z[36]*z[79]-
  lfz*z[15]*z[105]-lfz*z[28]*z[79]-z[23]*z[25]*z[35]-z[21]*z[35]*(z[27]+z[11]*
  z[105]))*z[565] - (lfx*z[15]*z[105]+lfx*z[28]*z[79]+z[23]*z[29]*z[35]-z[14]*
  z[35]*(z[27]+z[11]*z[105])-z[36]*(z[16]*z[105]+z[26]*z[79]))*z[567] - (lfx*
  z[30]*z[395]+lfz*z[25]*z[79]+lfx*z[12]*z[13]*z[79]-lfz*z[16]*z[451]-lfz*
  z[26]*z[395]-z[11]*z[23]*z[35]*z[451]-z[12]*z[23]*z[35]*z[105]-rf*z[11]*
  z[23]*z[105]*z[388])*z[286] - (rrt*z[11]*z[13]*(z[424]+z[451])+rrt*z[12]*
  z[13]*(z[105]+z[114])+lrz*(z[11]*z[79]-z[451]-z[12]*z[395])-rr*z[13]*(z[11]*
  z[79]-z[424]-z[451]-z[12]*z[395]))*z[270] - (lfz*z[15]*z[451]+lfz*z[28]*
  z[395]+rf*z[23]*z[25]*z[388]+rf*z[30]*z[79]*z[387]+rf*z[21]*z[388]*(z[27]+
  z[11]*z[105])-lfz*z[27]*z[79]-z[23]*z[35]*z[472]-z[30]*z[36]*z[395]-z[12]*
  z[13]*z[36]*z[79]-z[21]*z[35]*(z[419]-z[11]*z[451]-z[12]*z[105]))*z[282] - (
  lfx*z[27]*z[79]+z[11]*z[13]*z[23]*z[35]+rf*z[14]*z[388]*(z[27]+z[11]*z[105])-
  lfx*z[15]*z[451]-lfx*z[28]*z[395]-rf*z[23]*z[29]*z[388]-rf*z[387]*(z[16]*
  z[105]+z[26]*z[79])-z[14]*z[35]*(z[419]-z[11]*z[451]-z[12]*z[105])-z[36]*(
  z[25]*z[79]-z[16]*z[451]-z[26]*z[395]))*z[290];
  z[569] = z[30]*z[79]*z[478] + z[30]*z[79]*z[480] + z[39]*z[239]*z[395] + 
  z[12]*z[14]*z[79]*z[239] + z[488]*(z[16]*z[105]+z[26]*z[79]) + z[273]*
  z[395]*(lrx*z[30]+lrz*z[39]) + z[12]*z[79]*z[273]*(lrx*z[13]+lrz*z[14]) + 
  z[251]*(z[25]*z[79]-z[16]*z[451]-z[26]*z[395]) + mr*z[489]*(lrx*(z[105]+
  z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+z[114])+rr*z[14]*(z[105]+z[114]+z[12]*
  z[79])) + mf*z[491]*(z[30]*z[36]*z[79]-lfz*z[15]*z[105]-lfz*z[28]*z[79]-
  z[23]*z[25]*z[35]-z[21]*z[35]*(z[27]+z[11]*z[105])) + mf*z[490]*(lfx*z[15]*
  z[105]+lfx*z[28]*z[79]+z[23]*z[29]*z[35]-z[14]*z[35]*(z[27]+z[11]*z[105])-
  z[36]*(z[16]*z[105]+z[26]*z[79])) + z[285]*(lfx*z[30]*z[395]+lfz*z[25]*
  z[79]+lfx*z[12]*z[13]*z[79]-lfz*z[16]*z[451]-lfz*z[26]*z[395]-z[11]*z[23]*
  z[35]*z[451]-z[12]*z[23]*z[35]*z[105]-rf*z[11]*z[23]*z[105]*z[388]) + 
  z[281]*(lfz*z[15]*z[451]+lfz*z[28]*z[395]+rf*z[23]*z[25]*z[388]+rf*z[30]*
  z[79]*z[387]+rf*z[21]*z[388]*(z[27]+z[11]*z[105])-lfz*z[27]*z[79]-z[23]*
  z[35]*z[472]-z[30]*z[36]*z[395]-z[12]*z[13]*z[36]*z[79]-z[21]*z[35]*(z[419]-
  z[11]*z[451]-z[12]*z[105])) + z[288]*(lfx*z[27]*z[79]+z[11]*z[13]*z[23]*
  z[35]+rf*z[14]*z[388]*(z[27]+z[11]*z[105])-lfx*z[15]*z[451]-lfx*z[28]*
  z[395]-rf*z[23]*z[29]*z[388]-rf*z[387]*(z[16]*z[105]+z[26]*z[79])-z[14]*
  z[35]*(z[419]-z[11]*z[451]-z[12]*z[105])-z[36]*(z[25]*z[79]-z[16]*z[451]-
  z[26]*z[395])) - z[30]*z[243]*z[395] - z[30]*z[255]*z[395] - z[39]*z[79]*
  z[486] - z[12]*z[13]*z[79]*z[243] - z[12]*z[13]*z[79]*z[255] - IDyy*z[481]*(
  z[105]+z[12]*z[79]) - ICyy*z[482]*(z[105]+z[114]+z[12]*z[79]) - IEyy*z[483]*(
  z[15]*z[105]+z[28]*z[79]) - IFyy*z[483]*(1+z[15]*z[105]+z[28]*z[79]) - mr*
  z[79]*z[487]*(lrx*z[30]+lrz*z[39]) - z[246]*(z[11]*z[79]-z[451]-z[12]*
  z[395]) - z[259]*(z[27]*z[79]-z[15]*z[451]-z[28]*z[395]) - z[263]*(z[27]*
  z[79]-z[15]*z[451]-z[28]*z[395]) - z[232]*(z[11]*z[79]-z[424]-z[451]-z[12]*
  z[395]) - mf*z[485]*(lfx*z[30]*z[79]-lfz*z[16]*z[105]-lfz*z[26]*z[79]-z[11]*
  z[23]*z[35]*z[105]) - mr*z[484]*(lrz*(z[105]+z[12]*z[79])-rrt*z[11]*z[13]*(
  z[105]+z[114])-rr*z[13]*(z[105]+z[114]+z[12]*z[79])) - z[269]*(rrt*z[11]*
  z[13]*(z[424]+z[451])+rrt*z[12]*z[13]*(z[105]+z[114])+lrz*(z[11]*z[79]-
  z[451]-z[12]*z[395])-rr*z[13]*(z[11]*z[79]-z[424]-z[451]-z[12]*z[395])) - 
  z[277]*(rrt*z[11]*z[14]*(z[424]+z[451])+rrt*z[12]*z[14]*(z[105]+z[114])-lrx*(
  z[11]*z[79]-z[451]-z[12]*z[395])-rr*z[14]*(z[11]*z[79]-z[424]-z[451]-z[12]*
  z[395]));
  z[570] = z[30]*z[79]*z[460] + z[30]*z[79]*z[462] + z[39]*z[238]*z[395] + 
  z[12]*z[14]*z[79]*z[238] + z[464]*(z[16]*z[105]+z[26]*z[79]) + z[272]*
  z[395]*(lrx*z[30]+lrz*z[39]) + z[12]*z[79]*z[272]*(lrx*z[13]+lrz*z[14]) + 
  z[250]*(z[25]*z[79]-z[16]*z[451]-z[26]*z[395]) + mr*z[467]*(lrx*(z[105]+
  z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+z[114])+rr*z[14]*(z[105]+z[114]+z[12]*
  z[79])) + mf*z[473]*(z[30]*z[36]*z[79]-lfz*z[15]*z[105]-lfz*z[28]*z[79]-
  z[23]*z[25]*z[35]-z[21]*z[35]*(z[27]+z[11]*z[105])) + mf*z[468]*(lfx*z[15]*
  z[105]+lfx*z[28]*z[79]+z[23]*z[29]*z[35]-z[14]*z[35]*(z[27]+z[11]*z[105])-
  z[36]*(z[16]*z[105]+z[26]*z[79])) + z[283]*(lfx*z[30]*z[395]+lfz*z[25]*
  z[79]+lfx*z[12]*z[13]*z[79]-lfz*z[16]*z[451]-lfz*z[26]*z[395]-z[11]*z[23]*
  z[35]*z[451]-z[12]*z[23]*z[35]*z[105]-rf*z[11]*z[23]*z[105]*z[388]) + 
  z[280]*(lfz*z[15]*z[451]+lfz*z[28]*z[395]+rf*z[23]*z[25]*z[388]+rf*z[30]*
  z[79]*z[387]+rf*z[21]*z[388]*(z[27]+z[11]*z[105])-lfz*z[27]*z[79]-z[23]*
  z[35]*z[472]-z[30]*z[36]*z[395]-z[12]*z[13]*z[36]*z[79]-z[21]*z[35]*(z[419]-
  z[11]*z[451]-z[12]*z[105])) + z[289]*(lfx*z[27]*z[79]+z[11]*z[13]*z[23]*
  z[35]+rf*z[14]*z[388]*(z[27]+z[11]*z[105])-lfx*z[15]*z[451]-lfx*z[28]*
  z[395]-rf*z[23]*z[29]*z[388]-rf*z[387]*(z[16]*z[105]+z[26]*z[79])-z[14]*
  z[35]*(z[419]-z[11]*z[451]-z[12]*z[105])-z[36]*(z[25]*z[79]-z[16]*z[451]-
  z[26]*z[395])) - z[30]*z[242]*z[395] - z[30]*z[254]*z[395] - z[39]*z[79]*
  z[463] - z[12]*z[13]*z[79]*z[242] - z[12]*z[13]*z[79]*z[254] - IDyy*z[469]*(
  z[105]+z[12]*z[79]) - ICyy*z[470]*(z[105]+z[114]+z[12]*z[79]) - IEyy*z[465]*(
  z[15]*z[105]+z[28]*z[79]) - IFyy*z[465]*(1+z[15]*z[105]+z[28]*z[79]) - mr*
  z[79]*z[466]*(lrx*z[30]+lrz*z[39]) - z[247]*(z[11]*z[79]-z[451]-z[12]*
  z[395]) - z[258]*(z[27]*z[79]-z[15]*z[451]-z[28]*z[395]) - z[262]*(z[27]*
  z[79]-z[15]*z[451]-z[28]*z[395]) - z[233]*(z[11]*z[79]-z[424]-z[451]-z[12]*
  z[395]) - mf*z[474]*(lfx*z[30]*z[79]-lfz*z[16]*z[105]-lfz*z[26]*z[79]-z[11]*
  z[23]*z[35]*z[105]) - mr*z[471]*(lrz*(z[105]+z[12]*z[79])-rrt*z[11]*z[13]*(
  z[105]+z[114])-rr*z[13]*(z[105]+z[114]+z[12]*z[79])) - z[268]*(rrt*z[11]*
  z[13]*(z[424]+z[451])+rrt*z[12]*z[13]*(z[105]+z[114])+lrz*(z[11]*z[79]-
  z[451]-z[12]*z[395])-rr*z[13]*(z[11]*z[79]-z[424]-z[451]-z[12]*z[395])) - 
  z[275]*(rrt*z[11]*z[14]*(z[424]+z[451])+rrt*z[12]*z[14]*(z[105]+z[114])-lrx*(
  z[11]*z[79]-z[451]-z[12]*z[395])-rr*z[14]*(z[11]*z[79]-z[424]-z[451]-z[12]*
  z[395]));
  z[571] = z[311]*z[569] + z[316]*z[494] - z[312]*z[570] - z[315]*z[492];
  z[573] = z[291]*z[12]*z[13]*(lrx*z[101]+rrt*z[29]) + z[291]*z[12]*z[14]*(
  lrz*z[101]+rrt*z[38]) + z[292]*z[27]*(lfx*z[14]-z[27]*z[35]-lfz*z[13]*z[15]-
  lfz*z[16]*z[101]-z[11]*z[23]*z[35]*z[101]) + z[292]*z[12]*z[13]*(lfx*z[15]*
  z[101]-lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(z[21]+z[16]*
  z[101])) + z[292]*z[26]*(rf*z[25]*z[388]-z[35]*z[472]-lfz*z[15]*z[450]-rf*
  z[14]*z[387]-z[11]*z[21]*z[35]*z[450]-z[12]*z[21]*z[35]*z[101]-rf*z[11]*
  z[21]*z[101]*z[388]) + z[291]*z[39]*(lrz*z[450]-rrt*z[11]*z[14]-rrt*z[11]*
  z[13]*(z[418]+z[450])-rrt*z[12]*z[13]*(z[101]+z[110])-rr*z[13]*(z[418]+
  z[450]-z[11]*z[74]-z[12]*z[391])) - Trw*z[418] - z[572]*pow(z[12],2) - 
  z[291]*z[11]*(lrz*z[13]-rr-lrx*z[14]-rrt*z[11]) - z[292]*z[25]*(z[14]*z[36]+
  z[25]*z[35]+lfz*z[13]*z[16]-lfz*z[15]*z[101]-z[11]*z[21]*z[35]*z[101]) - 
  z[292]*z[28]*(z[35]*z[419]-lfz*z[16]*z[450]-rf*z[27]*z[388]-z[11]*z[23]*
  z[35]*z[450]-z[12]*z[23]*z[35]*z[101]-rf*z[11]*z[23]*z[101]*z[388]) - 
  z[291]*z[30]*(rrt*z[11]*z[13]-lrx*z[450]-rrt*z[11]*z[14]*(z[418]+z[450])-
  rrt*z[12]*z[14]*(z[101]+z[110])-rr*z[14]*(z[418]+z[450]-z[11]*z[74]-z[12]*
  z[391])) - z[292]*z[30]*(rf*z[29]*z[388]+z[16]*z[36]*z[450]+z[11]*z[14]*
  z[35]*z[450]+z[12]*z[14]*z[35]*z[101]+rf*z[11]*z[14]*z[101]*z[388]-lfx*
  z[15]*z[450]-z[11]*z[13]*z[35]-rf*z[387]*(z[21]+z[16]*z[101]));
  z[574] = ICyy*(z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))-
  z[11]*(z[74]*u1+z[75]*u3+z[79]*u5)*(z[418]*u1+z[424]*u5+z[450]*u1+z[451]*u5-
  z[428]*u3-z[452]*u3-z[11]*(z[74]*u1+z[75]*u3+z[79]*u5))-z[11]*(z[391]*u1+
  z[393]*u3-z[395]*u5)*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5-2*z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)));
  z[575] = z[574] + z[11]*z[74]*z[499] + z[11]*z[391]*z[235] + (z[14]+z[30]*
  z[74])*z[553] + (z[14]+z[30]*z[74])*z[517] + (z[14]+z[30]*z[74])*z[543] + (
  z[101]+z[12]*z[74])*z[545] + (z[13]-z[39]*z[74])*z[554] + (z[21]+z[16]*
  z[101]+z[26]*z[74])*z[548] + (z[30]*z[391]-z[12]*z[13]*z[74])*z[244] + (
  z[30]*z[391]-z[12]*z[13]*z[74])*z[256] + (z[15]*z[450]-z[27]*z[74]-z[28]*
  z[391])*z[264] + (z[418]+z[450]-z[11]*z[74]-z[12]*z[391])*z[234] + IFyy*(
  z[23]-z[15]*z[101]-z[28]*z[74])*z[546] + (lrx*z[30]*z[391]+lrz*z[39]*z[391]-
  rrt*z[12]-lrx*z[12]*z[13]*z[74]-lrz*z[12]*z[14]*z[74])*z[274] + (rrt*z[38]+
  lrz*(z[101]+z[12]*z[74])-rrt*z[11]*z[13]*(z[101]+z[110])-rr*z[13]*(z[101]+
  z[110]+z[12]*z[74]))*z[562] + (lfz*z[15]*z[101]+lfz*z[28]*z[74]+z[11]*z[21]*
  z[35]*z[101]-z[25]*z[35]-lfz*z[13]*z[16]-z[36]*(z[14]+z[30]*z[74]))*z[565] + (
  lfx*z[14]+lfx*z[30]*z[74]-z[27]*z[35]-lfz*z[13]*z[15]-lfz*z[16]*z[101]-lfz*
  z[26]*z[74]-z[11]*z[23]*z[35]*z[101])*z[552] + (lrz*(z[450]-z[11]*z[74]-
  z[12]*z[391])-rrt*z[11]*z[14]-rrt*z[11]*z[13]*(z[418]+z[450])-rrt*z[12]*
  z[13]*(z[101]+z[110])-rr*z[13]*(z[418]+z[450]-z[11]*z[74]-z[12]*z[391]))*
  z[270] + (lfz*z[27]*z[74]+lfz*z[28]*z[391]+rf*z[25]*z[388]-z[35]*z[472]-lfz*
  z[15]*z[450]-z[11]*z[21]*z[35]*z[450]-z[12]*z[21]*z[35]*z[101]-rf*z[11]*
  z[21]*z[101]*z[388]-rf*z[387]*(z[14]+z[30]*z[74])-z[36]*(z[30]*z[391]-z[12]*
  z[13]*z[74]))*z[282] + (lfx*z[15]*z[450]+z[11]*z[13]*z[35]+rf*z[387]*(z[21]+
  z[16]*z[101]+z[26]*z[74])-lfx*z[27]*z[74]-lfx*z[28]*z[391]-rf*z[29]*z[388]-
  z[11]*z[14]*z[35]*z[450]-z[12]*z[14]*z[35]*z[101]-rf*z[11]*z[14]*z[101]*
  z[388]-z[36]*(z[16]*z[450]-z[25]*z[74]-z[26]*z[391]))*z[290] - z[12]*z[74]*
  z[235] - (z[21]+z[16]*z[101]+z[26]*z[74])*z[500] - (z[30]*z[391]-z[12]*
  z[13]*z[74])*z[265] - (z[16]*z[450]-z[25]*z[74]-z[26]*z[391])*z[266] - ICyy*(
  z[101]+z[110]+z[12]*z[74])*z[558] - (z[39]*z[391]-z[12]*z[14]*z[74])*z[240] - (
  z[23]-z[15]*z[101]-z[28]*z[74])*z[547] - (z[450]-z[11]*z[74]-z[12]*z[391])*
  z[248] - (z[15]*z[450]-z[27]*z[74]-z[28]*z[391])*z[260] - (z[16]*z[450]-
  z[25]*z[74]-z[26]*z[391])*z[252] - (lrz*z[13]-rr-lrx*z[14]-rrt*z[11]-lrx*
  z[30]*z[74]-lrz*z[39]*z[74])*z[550] - (rrt*z[29]+lrx*(z[101]+z[12]*z[74])+
  rrt*z[11]*z[14]*(z[101]+z[110])+rr*z[14]*(z[101]+z[110]+z[12]*z[74]))*
  z[560] - (lfx*z[15]*z[101]+lfx*z[28]*z[74]-lfx*z[23]-z[29]*z[35]-z[11]*
  z[14]*z[35]*z[101]-z[36]*(z[21]+z[16]*z[101]+z[26]*z[74]))*z[567] - (rrt*
  z[11]*z[13]-rrt*z[11]*z[14]*(z[418]+z[450])-rrt*z[12]*z[14]*(z[101]+z[110])-
  lrx*(z[450]-z[11]*z[74]-z[12]*z[391])-rr*z[14]*(z[418]+z[450]-z[11]*z[74]-
  z[12]*z[391]))*z[278] - (z[35]*z[419]+lfz*z[25]*z[74]+lfz*z[26]*z[391]+lfx*
  z[12]*z[13]*z[74]-lfx*z[30]*z[391]-lfz*z[16]*z[450]-rf*z[27]*z[388]-z[11]*
  z[23]*z[35]*z[450]-z[12]*z[23]*z[35]*z[101]-rf*z[11]*z[23]*z[101]*z[388])*
  z[286];
  z[576] = z[307]*z[569] + z[316]*z[475] - z[308]*z[570] - z[315]*z[493];
  z[577] = Trw*z[428] + z[292]*z[26]*(lfz*z[15]*z[452]+rf*z[14]*z[25]*z[388]+
  rf*z[21]*z[388]*(z[29]+z[11]*z[100])-rf*z[387]-z[14]*z[35]*z[472]-z[21]*
  z[35]*(z[11]*z[13]-z[11]*z[452]-z[12]*z[100])) - z[496]*z[12]*z[13]*z[100] - 
  z[497]*z[12]*z[14]*z[100] - z[292]*z[30]*z[452]*(lfx*z[15]-z[16]*z[36]-
  z[11]*z[14]*z[35]) - z[292]*z[12]*z[13]*z[100]*(lfx*z[15]-z[16]*z[36]-z[11]*
  z[14]*z[35]) - z[292]*z[25]*(z[36]+lfz*z[15]*z[100]+z[14]*z[25]*z[35]+z[21]*
  z[35]*(z[29]+z[11]*z[100])) - z[292]*z[30]*z[100]*(rf*z[16]*z[387]-z[12]*
  z[14]*z[35]-rf*z[11]*z[14]*z[388]) - z[292]*z[27]*(z[14]*z[27]*z[35]-lfx-
  lfz*z[16]*z[100]-z[23]*z[35]*(z[29]+z[11]*z[100])) - z[291]*z[30]*(lrx*
  z[452]+rrt*z[11]*z[14]*(z[428]+z[452])+rrt*z[12]*z[14]*(z[100]+z[109])+rr*
  z[14]*(z[428]+z[452]+z[11]*z[75]+z[12]*z[393])) - z[291]*z[39]*(lrz*z[452]-
  rrt*z[11]*z[13]*(z[428]+z[452])-rrt*z[12]*z[13]*(z[100]+z[109])-rr*z[13]*(
  z[428]+z[452]+z[11]*z[75]+z[12]*z[393])) - z[292]*z[28]*(lfz*z[16]*z[452]+
  z[14]*z[35]*z[419]+rf*z[23]*z[388]*(z[29]+z[11]*z[100])-rf*z[14]*z[27]*
  z[388]-z[23]*z[35]*(z[11]*z[13]-z[11]*z[452]-z[12]*z[100]));
  z[578] = z[11]*z[75]*z[499] + z[11]*z[393]*z[235] + (1+z[30]*z[75])*z[553] + (
  z[16]*z[100]-z[26]*z[75])*z[500] + (z[16]*z[452]+z[25]*z[75]+z[26]*z[393])*
  z[266] + z[30]*z[75]*z[517] + z[30]*z[393]*z[244] + z[12]*z[14]*z[75]*
  z[240] + (1+z[30]*z[75])*z[543] + (z[452]+z[11]*z[75]+z[12]*z[393])*z[248] + 
  z[75]*(lrx*z[30]+lrz*z[39])*z[550] + z[393]*(lrx*z[30]+lrz*z[39])*z[274] + (
  z[15]*z[452]+z[27]*z[75]+z[28]*z[393])*z[260] + (z[16]*z[452]+z[25]*z[75]+
  z[26]*z[393])*z[252] + (z[30]*z[393]-z[12]*z[13]*z[75])*z[256] + ICyy*(
  z[100]+z[109]-z[12]*z[75])*z[558] + IFyy*(z[15]*z[100]-z[28]*z[75])*z[546] + (
  lfx*z[15]*z[100]-lfx*z[28]*z[75]-z[11]*z[14]*z[35]*z[100]-z[36]*(z[16]*
  z[100]-z[26]*z[75]))*z[567] + (lrx*(z[100]-z[12]*z[75])+rrt*z[11]*z[14]*(
  z[100]+z[109])+rr*z[14]*(z[100]+z[109]-z[12]*z[75]))*z[560] + (lfz*z[28]*
  z[75]-lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+z[30]*z[75])-z[21]*z[35]*(
  z[29]+z[11]*z[100]))*z[565] + (lfz*z[15]*z[452]+lfz*z[27]*z[75]+lfz*z[28]*
  z[393]+rf*z[14]*z[25]*z[388]+rf*z[21]*z[388]*(z[29]+z[11]*z[100])-z[14]*
  z[35]*z[472]-rf*z[387]*(1+z[30]*z[75])-z[36]*(z[30]*z[393]-z[12]*z[13]*
  z[75])-z[21]*z[35]*(z[11]*z[13]-z[11]*z[452]-z[12]*z[100]))*z[282] - z[12]*
  z[75]*z[235] - (z[30]*z[393]-z[12]*z[13]*z[75])*z[265] - z[39]*z[75]*z[554] - 
  z[39]*z[393]*z[240] - z[12]*z[13]*z[75]*z[244] - (z[100]-z[12]*z[75])*
  z[545] - (z[15]*z[100]-z[28]*z[75])*z[547] - (z[16]*z[100]-z[26]*z[75])*
  z[548] - (z[15]*z[452]+z[27]*z[75]+z[28]*z[393])*z[264] - (z[428]+z[452]+
  z[11]*z[75]+z[12]*z[393])*z[234] - z[12]*z[75]*(lrx*z[13]+lrz*z[14])*z[274] - (
  lrz*(z[100]-z[12]*z[75])-rrt*z[11]*z[13]*(z[100]+z[109])-rr*z[13]*(z[100]+
  z[109]-z[12]*z[75]))*z[562] - (lfz*z[26]*z[75]+z[14]*z[27]*z[35]-lfx-lfx*
  z[30]*z[75]-lfz*z[16]*z[100]-z[23]*z[35]*(z[29]+z[11]*z[100]))*z[552] - (
  rrt*z[11]*z[14]*(z[428]+z[452])+rrt*z[12]*z[14]*(z[100]+z[109])+lrx*(z[452]+
  z[11]*z[75]+z[12]*z[393])+rr*z[14]*(z[428]+z[452]+z[11]*z[75]+z[12]*z[393]))*
  z[278] - (lrz*(z[452]+z[11]*z[75]+z[12]*z[393])-rrt*z[11]*z[13]*(z[428]+
  z[452])-rrt*z[12]*z[13]*(z[100]+z[109])-rr*z[13]*(z[428]+z[452]+z[11]*z[75]+
  z[12]*z[393]))*z[270] - (lfx*z[15]*z[452]+lfx*z[27]*z[75]+lfx*z[28]*z[393]+
  rf*z[387]*(z[16]*z[100]-z[26]*z[75])-z[11]*z[14]*z[35]*z[452]-z[12]*z[14]*
  z[35]*z[100]-rf*z[11]*z[14]*z[100]*z[388]-z[36]*(z[16]*z[452]+z[25]*z[75]+
  z[26]*z[393]))*z[290] - (lfz*z[16]*z[452]+lfz*z[25]*z[75]+lfz*z[26]*z[393]+
  z[14]*z[35]*z[419]+lfx*z[12]*z[13]*z[75]+rf*z[23]*z[388]*(z[29]+z[11]*
  z[100])-lfx*z[30]*z[393]-rf*z[14]*z[27]*z[388]-z[23]*z[35]*(z[11]*z[13]-
  z[11]*z[452]-z[12]*z[100]))*z[286];
  z[579] = z[12]*z[14]*z[74] - z[39]*z[391];
  z[580] = z[30]*z[391] - z[12]*z[13]*z[74];
  z[581] = -IDxz*z[579] - IDzz*z[580];
  z[582] = z[25]*z[74] + z[26]*z[391] - z[16]*z[450];
  z[583] = -IExz*z[582] - IEzz*z[580];
  z[584] = -IExx*z[582] - IExz*z[580];
  z[585] = rrt*z[11]*z[14]*(z[418]+z[450]) + rrt*z[12]*z[14]*(z[101]+z[110]) + 
  lrx*(z[450]-z[11]*z[74]-z[12]*z[391]) + rr*z[14]*(z[418]+z[450]-z[11]*z[74]-
  z[12]*z[391]) - rrt*z[11]*z[13];
  z[586] = lfz*z[27]*z[74] + lfz*z[28]*z[391] + rf*z[25]*z[388] - z[35]*
  z[472] - lfz*z[15]*z[450] - z[11]*z[21]*z[35]*z[450] - z[12]*z[21]*z[35]*
  z[101] - rf*z[11]*z[21]*z[101]*z[388] - rf*z[387]*(z[14]+z[30]*z[74]) - 
  z[36]*(z[30]*z[391]-z[12]*z[13]*z[74]);
  z[587] = lfx*z[15]*z[450] + z[11]*z[13]*z[35] + rf*z[387]*(z[21]+z[16]*
  z[101]+z[26]*z[74]) - lfx*z[27]*z[74] - lfx*z[28]*z[391] - rf*z[29]*z[388] - 
  z[11]*z[14]*z[35]*z[450] - z[12]*z[14]*z[35]*z[101] - rf*z[11]*z[14]*z[101]*
  z[388] - z[36]*(z[16]*z[450]-z[25]*z[74]-z[26]*z[391]);
  z[588] = -IDxx*z[579] - IDxz*z[580];
  z[589] = z[11]*z[74] + z[12]*z[391] - z[450];
  z[590] = z[11]*z[74] + z[12]*z[391] - z[418] - z[450];
  z[591] = z[27]*z[74] + z[28]*z[391] - z[15]*z[450];
  z[592] = lrx*z[30]*z[391] + lrz*z[39]*z[391] - rrt*z[12] - lrx*z[12]*z[13]*
  z[74] - lrz*z[12]*z[14]*z[74];
  z[593] = lfx*z[30]*z[391] + lfz*z[16]*z[450] + rf*z[27]*z[388] + z[11]*
  z[23]*z[35]*z[450] + z[12]*z[23]*z[35]*z[101] + rf*z[11]*z[23]*z[101]*
  z[388] - z[35]*z[419] - lfz*z[25]*z[74] - lfz*z[26]*z[391] - lfx*z[12]*
  z[13]*z[74];
  z[594] = rrt*z[11]*z[14] + rrt*z[11]*z[13]*(z[418]+z[450]) + rrt*z[12]*
  z[13]*(z[101]+z[110]) + rr*z[13]*(z[418]+z[450]-z[11]*z[74]-z[12]*z[391]) - 
  lrz*(z[450]-z[11]*z[74]-z[12]*z[391]);
  z[595] = z[30]*z[79]*z[581] + z[30]*z[79]*z[583] + z[39]*z[237]*z[395] + 
  z[12]*z[14]*z[79]*z[237] + z[584]*(z[16]*z[105]+z[26]*z[79]) + z[271]*
  z[395]*(lrx*z[30]+lrz*z[39]) + z[12]*z[79]*z[271]*(lrx*z[13]+lrz*z[14]) + 
  z[249]*(z[25]*z[79]-z[16]*z[451]-z[26]*z[395]) + mr*z[585]*(lrx*(z[105]+
  z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+z[114])+rr*z[14]*(z[105]+z[114]+z[12]*
  z[79])) + mf*z[586]*(z[30]*z[36]*z[79]-lfz*z[15]*z[105]-lfz*z[28]*z[79]-
  z[23]*z[25]*z[35]-z[21]*z[35]*(z[27]+z[11]*z[105])) + mf*z[587]*(lfx*z[15]*
  z[105]+lfx*z[28]*z[79]+z[23]*z[29]*z[35]-z[14]*z[35]*(z[27]+z[11]*z[105])-
  z[36]*(z[16]*z[105]+z[26]*z[79])) + z[284]*(lfx*z[30]*z[395]+lfz*z[25]*
  z[79]+lfx*z[12]*z[13]*z[79]-lfz*z[16]*z[451]-lfz*z[26]*z[395]-z[11]*z[23]*
  z[35]*z[451]-z[12]*z[23]*z[35]*z[105]-rf*z[11]*z[23]*z[105]*z[388]) + 
  z[279]*(lfz*z[15]*z[451]+lfz*z[28]*z[395]+rf*z[23]*z[25]*z[388]+rf*z[30]*
  z[79]*z[387]+rf*z[21]*z[388]*(z[27]+z[11]*z[105])-lfz*z[27]*z[79]-z[23]*
  z[35]*z[472]-z[30]*z[36]*z[395]-z[12]*z[13]*z[36]*z[79]-z[21]*z[35]*(z[419]-
  z[11]*z[451]-z[12]*z[105])) + z[287]*(lfx*z[27]*z[79]+z[11]*z[13]*z[23]*
  z[35]+rf*z[14]*z[388]*(z[27]+z[11]*z[105])-lfx*z[15]*z[451]-lfx*z[28]*
  z[395]-rf*z[23]*z[29]*z[388]-rf*z[387]*(z[16]*z[105]+z[26]*z[79])-z[14]*
  z[35]*(z[419]-z[11]*z[451]-z[12]*z[105])-z[36]*(z[25]*z[79]-z[16]*z[451]-
  z[26]*z[395])) - z[30]*z[241]*z[395] - z[30]*z[253]*z[395] - z[39]*z[79]*
  z[588] - z[12]*z[13]*z[79]*z[241] - z[12]*z[13]*z[79]*z[253] - IDyy*z[589]*(
  z[105]+z[12]*z[79]) - ICyy*z[590]*(z[105]+z[114]+z[12]*z[79]) - IEyy*z[591]*(
  z[15]*z[105]+z[28]*z[79]) - IFyy*z[591]*(1+z[15]*z[105]+z[28]*z[79]) - mr*
  z[79]*z[592]*(lrx*z[30]+lrz*z[39]) - z[245]*(z[11]*z[79]-z[451]-z[12]*
  z[395]) - z[257]*(z[27]*z[79]-z[15]*z[451]-z[28]*z[395]) - z[261]*(z[27]*
  z[79]-z[15]*z[451]-z[28]*z[395]) - z[231]*(z[11]*z[79]-z[424]-z[451]-z[12]*
  z[395]) - mf*z[593]*(lfx*z[30]*z[79]-lfz*z[16]*z[105]-lfz*z[26]*z[79]-z[11]*
  z[23]*z[35]*z[105]) - mr*z[594]*(lrz*(z[105]+z[12]*z[79])-rrt*z[11]*z[13]*(
  z[105]+z[114])-rr*z[13]*(z[105]+z[114]+z[12]*z[79])) - z[267]*(rrt*z[11]*
  z[13]*(z[424]+z[451])+rrt*z[12]*z[13]*(z[105]+z[114])+lrz*(z[11]*z[79]-
  z[451]-z[12]*z[395])-rr*z[13]*(z[11]*z[79]-z[424]-z[451]-z[12]*z[395])) - 
  z[276]*(rrt*z[11]*z[14]*(z[424]+z[451])+rrt*z[12]*z[14]*(z[105]+z[114])-lrx*(
  z[11]*z[79]-z[451]-z[12]*z[395])-rr*z[14]*(z[11]*z[79]-z[424]-z[451]-z[12]*
  z[395]));
  z[596] = z[581]*(z[14]+z[30]*z[74]) + z[583]*(z[14]+z[30]*z[74]) + z[588]*(
  z[13]-z[39]*z[74]) + z[584]*(z[21]+z[16]*z[101]+z[26]*z[74]) + z[241]*(
  z[30]*z[391]-z[12]*z[13]*z[74]) + z[253]*(z[30]*z[391]-z[12]*z[13]*z[74]) + 
  z[245]*(z[450]-z[11]*z[74]-z[12]*z[391]) + IEyy*z[591]*(z[23]-z[15]*z[101]-
  z[28]*z[74]) + IFyy*z[591]*(z[23]-z[15]*z[101]-z[28]*z[74]) + z[231]*(
  z[418]+z[450]-z[11]*z[74]-z[12]*z[391]) + z[257]*(z[15]*z[450]-z[27]*z[74]-
  z[28]*z[391]) + z[261]*(z[15]*z[450]-z[27]*z[74]-z[28]*z[391]) + mr*z[592]*(
  lrz*z[13]-rr-lrx*z[14]-rrt*z[11]-lrx*z[30]*z[74]-lrz*z[39]*z[74]) + mr*
  z[585]*(rrt*z[29]+lrx*(z[101]+z[12]*z[74])+rrt*z[11]*z[14]*(z[101]+z[110])+
  rr*z[14]*(z[101]+z[110]+z[12]*z[74])) + mf*z[587]*(lfx*z[15]*z[101]+lfx*
  z[28]*z[74]-lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(z[21]+
  z[16]*z[101]+z[26]*z[74])) + z[267]*(lrz*(z[450]-z[11]*z[74]-z[12]*z[391])-
  rrt*z[11]*z[14]-rrt*z[11]*z[13]*(z[418]+z[450])-rrt*z[12]*z[13]*(z[101]+
  z[110])-rr*z[13]*(z[418]+z[450]-z[11]*z[74]-z[12]*z[391])) + z[276]*(rrt*
  z[11]*z[13]-rrt*z[11]*z[14]*(z[418]+z[450])-rrt*z[12]*z[14]*(z[101]+z[110])-
  lrx*(z[450]-z[11]*z[74]-z[12]*z[391])-rr*z[14]*(z[418]+z[450]-z[11]*z[74]-
  z[12]*z[391])) + z[284]*(z[35]*z[419]+lfz*z[25]*z[74]+lfz*z[26]*z[391]+lfx*
  z[12]*z[13]*z[74]-lfx*z[30]*z[391]-lfz*z[16]*z[450]-rf*z[27]*z[388]-z[11]*
  z[23]*z[35]*z[450]-z[12]*z[23]*z[35]*z[101]-rf*z[11]*z[23]*z[101]*z[388]) - 
  IDyy*z[589]*(z[101]+z[12]*z[74]) - ICyy*z[590]*(z[101]+z[110]+z[12]*z[74]) - 
  z[237]*(z[39]*z[391]-z[12]*z[14]*z[74]) - z[249]*(z[16]*z[450]-z[25]*z[74]-
  z[26]*z[391]) - z[271]*(lrx*z[30]*z[391]+lrz*z[39]*z[391]-rrt*z[12]-lrx*
  z[12]*z[13]*z[74]-lrz*z[12]*z[14]*z[74]) - mr*z[594]*(rrt*z[38]+lrz*(z[101]+
  z[12]*z[74])-rrt*z[11]*z[13]*(z[101]+z[110])-rr*z[13]*(z[101]+z[110]+z[12]*
  z[74])) - mf*z[586]*(lfz*z[15]*z[101]+lfz*z[28]*z[74]+z[11]*z[21]*z[35]*
  z[101]-z[25]*z[35]-lfz*z[13]*z[16]-z[36]*(z[14]+z[30]*z[74])) - mf*z[593]*(
  lfx*z[14]+lfx*z[30]*z[74]-z[27]*z[35]-lfz*z[13]*z[15]-lfz*z[16]*z[101]-lfz*
  z[26]*z[74]-z[11]*z[23]*z[35]*z[101]) - z[279]*(lfz*z[27]*z[74]+lfz*z[28]*
  z[391]+rf*z[25]*z[388]-z[35]*z[472]-lfz*z[15]*z[450]-z[11]*z[21]*z[35]*
  z[450]-z[12]*z[21]*z[35]*z[101]-rf*z[11]*z[21]*z[101]*z[388]-rf*z[387]*(
  z[14]+z[30]*z[74])-z[36]*(z[30]*z[391]-z[12]*z[13]*z[74])) - z[287]*(lfx*
  z[15]*z[450]+z[11]*z[13]*z[35]+rf*z[387]*(z[21]+z[16]*z[101]+z[26]*z[74])-
  lfx*z[27]*z[74]-lfx*z[28]*z[391]-rf*z[29]*z[388]-z[11]*z[14]*z[35]*z[450]-
  z[12]*z[14]*z[35]*z[101]-rf*z[11]*z[14]*z[101]*z[388]-z[36]*(z[16]*z[450]-
  z[25]*z[74]-z[26]*z[391]));
  z[597] = z[30]*z[75]*z[581] + z[30]*z[241]*z[393] + z[12]*z[14]*z[75]*
  z[237] + z[583]*(1+z[30]*z[75]) + IDyy*z[589]*(z[100]-z[12]*z[75]) + ICyy*
  z[590]*(z[100]+z[109]-z[12]*z[75]) + IEyy*z[591]*(z[15]*z[100]-z[28]*z[75]) + 
  IFyy*z[591]*(z[15]*z[100]-z[28]*z[75]) + z[12]*z[75]*z[271]*(lrx*z[13]+lrz*
  z[14]) + z[249]*(z[16]*z[452]+z[25]*z[75]+z[26]*z[393]) + z[253]*(z[30]*
  z[393]-z[12]*z[13]*z[75]) + mr*z[594]*(lrz*(z[100]-z[12]*z[75])-rrt*z[11]*
  z[13]*(z[100]+z[109])-rr*z[13]*(z[100]+z[109]-z[12]*z[75])) + mf*z[593]*(
  lfz*z[26]*z[75]+z[14]*z[27]*z[35]-lfx-lfx*z[30]*z[75]-lfz*z[16]*z[100]-
  z[23]*z[35]*(z[29]+z[11]*z[100])) + z[276]*(rrt*z[11]*z[14]*(z[428]+z[452])+
  rrt*z[12]*z[14]*(z[100]+z[109])+lrx*(z[452]+z[11]*z[75]+z[12]*z[393])+rr*
  z[14]*(z[428]+z[452]+z[11]*z[75]+z[12]*z[393])) + z[287]*(lfx*z[15]*z[452]+
  lfx*z[27]*z[75]+lfx*z[28]*z[393]+rf*z[387]*(z[16]*z[100]-z[26]*z[75])-z[11]*
  z[14]*z[35]*z[452]-z[12]*z[14]*z[35]*z[100]-rf*z[11]*z[14]*z[100]*z[388]-
  z[36]*(z[16]*z[452]+z[25]*z[75]+z[26]*z[393])) + z[284]*(lfz*z[16]*z[452]+
  lfz*z[25]*z[75]+lfz*z[26]*z[393]+z[14]*z[35]*z[419]+lfx*z[12]*z[13]*z[75]+
  rf*z[23]*z[388]*(z[29]+z[11]*z[100])-lfx*z[30]*z[393]-rf*z[14]*z[27]*z[388]-
  z[23]*z[35]*(z[11]*z[13]-z[11]*z[452]-z[12]*z[100])) - z[39]*z[75]*z[588] - 
  z[39]*z[237]*z[393] - z[12]*z[13]*z[75]*z[241] - z[245]*(z[452]+z[11]*z[75]+
  z[12]*z[393]) - z[271]*z[393]*(lrx*z[30]+lrz*z[39]) - z[584]*(z[16]*z[100]-
  z[26]*z[75]) - mr*z[75]*z[592]*(lrx*z[30]+lrz*z[39]) - z[231]*(z[428]+
  z[452]+z[11]*z[75]+z[12]*z[393]) - z[257]*(z[15]*z[452]+z[27]*z[75]+z[28]*
  z[393]) - z[261]*(z[15]*z[452]+z[27]*z[75]+z[28]*z[393]) - mf*z[587]*(lfx*
  z[15]*z[100]-lfx*z[28]*z[75]-z[11]*z[14]*z[35]*z[100]-z[36]*(z[16]*z[100]-
  z[26]*z[75])) - mr*z[585]*(lrx*(z[100]-z[12]*z[75])+rrt*z[11]*z[14]*(z[100]+
  z[109])+rr*z[14]*(z[100]+z[109]-z[12]*z[75])) - mf*z[586]*(lfz*z[28]*z[75]-
  lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+z[30]*z[75])-z[21]*z[35]*(z[29]+
  z[11]*z[100])) - z[267]*(lrz*(z[452]+z[11]*z[75]+z[12]*z[393])-rrt*z[11]*
  z[13]*(z[428]+z[452])-rrt*z[12]*z[13]*(z[100]+z[109])-rr*z[13]*(z[428]+
  z[452]+z[11]*z[75]+z[12]*z[393])) - z[279]*(lfz*z[15]*z[452]+lfz*z[27]*
  z[75]+lfz*z[28]*z[393]+rf*z[14]*z[25]*z[388]+rf*z[21]*z[388]*(z[29]+z[11]*
  z[100])-z[14]*z[35]*z[472]-rf*z[387]*(1+z[30]*z[75])-z[36]*(z[30]*z[393]-
  z[12]*z[13]*z[75])-z[21]*z[35]*(z[11]*z[13]-z[11]*z[452]-z[12]*z[100]));
  z[598] = z[306]*z[494] + z[311]*z[596] - z[307]*z[597] - z[310]*z[475];
  z[599] = z[306]*z[492] + z[312]*z[596] - z[308]*z[597] - z[310]*z[493];
  z[600] = z[314]*z[495] + z[316]*z[598] + z[345]*z[569] + z[347]*z[595] - 
  z[315]*z[599] - z[346]*z[570];
  z[601] = (z[600]*(z[347]*z[344]+z[349]*z[342]-z[352]*z[343])+z[348]*(z[576]*
  z[343]+z[347]*(z[498]+z[568])+z[349]*(z[573]+z[575])-z[495]*z[344]-z[571]*
  z[342]-z[352]*(z[577]+z[578])))/pow(z[348],2);
  z[602] = z[405]*(IDxz*z[39]-IDzz*z[30]);
  z[603] = z[26]*z[405] + z[75]*z[401] - z[15]*z[100] - z[16]*z[456];
  z[604] = -IExz*z[603] - IEzz*z[30]*z[405];
  z[605] = z[405]*(IDxx*z[39]-IDxz*z[30]);
  z[606] = -IExx*z[603] - IExz*z[30]*z[405];
  z[607] = z[16]*z[100] + z[28]*z[405] + z[75]*z[397] - z[15]*z[456];
  z[608] = z[405]*(lrx*z[30]+lrz*z[39]);
  z[609] = lrx*(z[456]-z[12]*z[405]) + rr*z[14]*(z[456]-z[448]-z[12]*z[405]) - 
  rrt*z[11]*z[14]*(z[448]-z[456]);
  z[610] = lfx*z[15]*z[456] + rf*z[11]*z[14]*z[100]*z[399] - lfx*z[16]*z[100] - 
  lfx*z[28]*z[405] - lfx*z[75]*z[397] - z[11]*z[14]*z[35]*z[456] - rf*z[398]*(
  z[16]*z[100]-z[26]*z[75]) - z[36]*(z[15]*z[100]+z[16]*z[456]-z[26]*z[405]-
  z[75]*z[401]);
  z[611] = IDyy*(z[456]-z[12]*z[405]);
  z[612] = z[448] + z[12]*z[405] - z[456];
  z[613] = rr*z[13]*(z[456]-z[448]-z[12]*z[405]) - lrz*(z[456]-z[12]*z[405]) - 
  rrt*z[11]*z[13]*(z[448]-z[456]);
  z[614] = z[11]*z[15] - z[12]*z[14]*z[16];
  z[615] = lfz*z[16]*z[100] + lfz*z[28]*z[405] + lfz*z[75]*z[397] + rf*z[14]*
  z[25]*z[399] + rf*z[21]*z[399]*(z[29]+z[11]*z[100]) + z[13]*z[16]*z[35]*(
  z[29]+z[11]*z[100]) - lfz*z[15]*z[456] - z[14]*z[35]*z[614] - z[30]*z[36]*
  z[405] - z[11]*z[21]*z[35]*z[456] - rf*z[398]*(1+z[30]*z[75]);
  z[616] = lfx*z[30]*z[405] + lfz*z[15]*z[100] + lfz*z[16]*z[456] + rf*z[14]*
  z[27]*z[399] + z[11]*z[23]*z[35]*z[456] + z[13]*z[15]*z[35]*(z[29]+z[11]*
  z[100]) - lfz*z[26]*z[405] - lfz*z[75]*z[401] - z[14]*z[35]*z[439] - rf*
  z[23]*z[399]*(z[29]+z[11]*z[100]);
  z[617] = z[30]*z[242]*z[403] + z[30]*z[254]*z[403] + z[602]*(z[14]+z[30]*
  z[74]) + z[604]*(z[14]+z[30]*z[74]) + z[611]*(z[101]+z[12]*z[74]) + z[605]*(
  z[13]-z[39]*z[74]) + z[606]*(z[21]+z[16]*z[101]+z[26]*z[74]) + z[233]*(
  z[438]-z[454]-z[12]*z[403]) + IEyy*z[607]*(z[23]-z[15]*z[101]-z[28]*z[74]) + 
  IFyy*z[607]*(z[23]-z[15]*z[101]-z[28]*z[74]) + z[258]*(z[13]*z[15]+z[16]*
  z[101]-z[15]*z[454]-z[28]*z[403]-z[74]*z[397]) + z[262]*(z[13]*z[15]+z[16]*
  z[101]-z[15]*z[454]-z[28]*z[403]-z[74]*z[397]) + mr*z[608]*(lrz*z[13]-rr-
  lrx*z[14]-rrt*z[11]-lrx*z[30]*z[74]-lrz*z[39]*z[74]) + mr*z[609]*(rrt*z[29]+
  lrx*(z[101]+z[12]*z[74])+rrt*z[11]*z[14]*(z[101]+z[110])+rr*z[14]*(z[101]+
  z[110]+z[12]*z[74])) + z[275]*(lrx*(z[454]+z[12]*z[403])-rrt*z[11]*z[14]*(
  z[438]-z[454])-rr*z[14]*(z[438]-z[454]-z[12]*z[403])) + mf*z[610]*(lfx*
  z[15]*z[101]+lfx*z[28]*z[74]-lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-
  z[36]*(z[21]+z[16]*z[101]+z[26]*z[74])) - z[39]*z[238]*z[403] - z[247]*(
  z[454]+z[12]*z[403]) - ICyy*z[612]*(z[101]+z[110]+z[12]*z[74]) - z[272]*
  z[403]*(lrx*z[30]+lrz*z[39]) - z[250]*(z[13]*z[16]-z[15]*z[101]-z[16]*
  z[454]-z[26]*z[403]-z[74]*z[401]) - z[268]*(lrz*(z[454]+z[12]*z[403])+rrt*
  z[11]*z[13]*(z[438]-z[454])+rr*z[13]*(z[438]-z[454]-z[12]*z[403])) - mr*
  z[613]*(rrt*z[38]+lrz*(z[101]+z[12]*z[74])-rrt*z[11]*z[13]*(z[101]+z[110])-
  rr*z[13]*(z[101]+z[110]+z[12]*z[74])) - mf*z[615]*(lfz*z[15]*z[101]+lfz*
  z[28]*z[74]+z[11]*z[21]*z[35]*z[101]-z[25]*z[35]-lfz*z[13]*z[16]-z[36]*(
  z[14]+z[30]*z[74])) - mf*z[616]*(lfx*z[14]+lfx*z[30]*z[74]-z[27]*z[35]-lfz*
  z[13]*z[15]-lfz*z[16]*z[101]-lfz*z[26]*z[74]-z[11]*z[23]*z[35]*z[101]) - 
  z[283]*(lfx*z[30]*z[403]+lfz*z[13]*z[16]+rf*z[27]*z[399]+rf*z[11]*z[23]*
  z[101]*z[399]-z[35]*z[439]-lfz*z[15]*z[101]-lfz*z[16]*z[454]-lfz*z[26]*
  z[403]-lfz*z[74]*z[401]-z[11]*z[23]*z[35]*z[454]-z[11]*z[13]*z[15]*z[35]*
  z[101]) - z[280]*(lfz*z[15]*z[454]+lfz*z[28]*z[403]+lfz*z[74]*z[397]+rf*
  z[25]*z[399]+z[11]*z[21]*z[35]*z[454]-z[35]*z[614]-lfz*z[13]*z[15]-lfz*
  z[16]*z[101]-z[30]*z[36]*z[403]-rf*z[11]*z[21]*z[101]*z[399]-z[11]*z[13]*
  z[16]*z[35]*z[101]-rf*z[398]*(z[14]+z[30]*z[74])) - z[289]*(lfx*z[13]*z[15]+
  lfx*z[16]*z[101]+z[11]*z[14]*z[35]*z[454]+rf*z[398]*(z[21]+z[16]*z[101]+
  z[26]*z[74])-lfx*z[15]*z[454]-lfx*z[28]*z[403]-lfx*z[74]*z[397]-rf*z[29]*
  z[399]-rf*z[11]*z[14]*z[101]*z[399]-z[36]*(z[13]*z[16]-z[15]*z[101]-z[16]*
  z[454]-z[26]*z[403]-z[74]*z[401]));
  z[618] = z[407]*(IDxz*z[39]-IDzz*z[30]);
  z[619] = z[15]*z[105] + z[16]*z[455] + z[79]*z[401] - z[26]*z[407];
  z[620] = IEzz*z[30]*z[407] - IExz*z[619];
  z[621] = IDyy*(z[455]-z[12]*z[407]);
  z[622] = z[455] - z[444] - z[12]*z[407];
  z[623] = z[15]*z[455] + z[79]*z[397] - z[16]*z[105] - z[28]*z[407];
  z[624] = lrz*(z[455]-z[12]*z[407]) + rrt*z[11]*z[13]*(z[444]-z[455]) - rr*
  z[13]*(z[455]-z[444]-z[12]*z[407]);
  z[625] = lfz*z[26]*z[407] + rf*z[11]*z[23]*z[105]*z[399] - lfx*z[30]*z[407] - 
  lfz*z[15]*z[105] - lfz*z[16]*z[455] - lfz*z[79]*z[401] - z[11]*z[23]*z[35]*
  z[455] - z[11]*z[13]*z[15]*z[35]*z[105];
  z[626] = z[407]*(IDxx*z[39]-IDxz*z[30]);
  z[627] = z[407]*(lrx*z[30]+lrz*z[39]);
  z[628] = IExz*z[30]*z[407] - IExx*z[619];
  z[629] = rrt*z[11]*z[14]*(z[444]-z[455]) - lrx*(z[455]-z[12]*z[407]) - rr*
  z[14]*(z[455]-z[444]-z[12]*z[407]);
  z[630] = lfx*z[16]*z[105] + lfx*z[28]*z[407] + rf*z[23]*z[29]*z[399] + 
  z[14]*z[35]*(z[439]+z[11]*z[455]) + rf*z[398]*(z[16]*z[105]+z[26]*z[79]) - 
  lfx*z[15]*z[455] - lfx*z[79]*z[397] - z[13]*z[15]*z[29]*z[35] - rf*z[14]*
  z[399]*(z[27]+z[11]*z[105]) - z[36]*(z[26]*z[407]-z[15]*z[105]-z[16]*z[455]-
  z[79]*z[401]);
  z[631] = lfz*z[15]*z[455] + lfz*z[79]*z[397] + z[23]*z[35]*z[614] + z[30]*
  z[36]*z[407] + z[13]*z[15]*z[25]*z[35] + z[21]*z[35]*(z[439]+z[11]*z[455]) - 
  lfz*z[16]*z[105] - lfz*z[28]*z[407] - rf*z[23]*z[25]*z[399] - rf*z[30]*
  z[79]*z[398] - rf*z[21]*z[399]*(z[27]+z[11]*z[105]) - z[13]*z[16]*z[35]*(
  z[27]+z[11]*z[105]);
  z[632] = z[30]*z[243]*z[405] + z[30]*z[255]*z[405] + z[39]*z[75]*z[626] + 
  z[620]*(1+z[30]*z[75]) + z[246]*(z[456]-z[12]*z[405]) + z[621]*(z[100]-
  z[12]*z[75]) + ICyy*z[622]*(z[100]+z[109]-z[12]*z[75]) + IEyy*z[623]*(z[15]*
  z[100]-z[28]*z[75]) + IFyy*z[623]*(z[15]*z[100]-z[28]*z[75]) + mr*z[75]*
  z[627]*(lrx*z[30]+lrz*z[39]) + z[232]*(z[456]-z[448]-z[12]*z[405]) + z[259]*(
  z[15]*z[456]-z[16]*z[100]-z[28]*z[405]-z[75]*z[397]) + z[263]*(z[15]*z[456]-
  z[16]*z[100]-z[28]*z[405]-z[75]*z[397]) + mr*z[624]*(lrz*(z[100]-z[12]*
  z[75])-rrt*z[11]*z[13]*(z[100]+z[109])-rr*z[13]*(z[100]+z[109]-z[12]*z[75])) + 
  z[269]*(lrz*(z[456]-z[12]*z[405])+rrt*z[11]*z[13]*(z[448]-z[456])-rr*z[13]*(
  z[456]-z[448]-z[12]*z[405])) + mf*z[625]*(lfz*z[26]*z[75]+z[14]*z[27]*z[35]-
  lfx-lfx*z[30]*z[75]-lfz*z[16]*z[100]-z[23]*z[35]*(z[29]+z[11]*z[100])) + 
  z[277]*(rrt*z[11]*z[14]*(z[448]-z[456])-lrx*(z[456]-z[12]*z[405])-rr*z[14]*(
  z[456]-z[448]-z[12]*z[405])) + z[285]*(lfz*z[26]*z[405]+lfz*z[75]*z[401]+
  z[14]*z[35]*z[439]+rf*z[23]*z[399]*(z[29]+z[11]*z[100])-lfx*z[30]*z[405]-
  lfz*z[15]*z[100]-lfz*z[16]*z[456]-rf*z[14]*z[27]*z[399]-z[11]*z[23]*z[35]*
  z[456]-z[13]*z[15]*z[35]*(z[29]+z[11]*z[100])) + z[281]*(lfz*z[15]*z[456]+
  z[14]*z[35]*z[614]+z[30]*z[36]*z[405]+z[11]*z[21]*z[35]*z[456]+rf*z[398]*(1+
  z[30]*z[75])-lfz*z[16]*z[100]-lfz*z[28]*z[405]-lfz*z[75]*z[397]-rf*z[14]*
  z[25]*z[399]-rf*z[21]*z[399]*(z[29]+z[11]*z[100])-z[13]*z[16]*z[35]*(z[29]+
  z[11]*z[100])) - z[30]*z[75]*z[618] - z[39]*z[239]*z[405] - z[273]*z[405]*(
  lrx*z[30]+lrz*z[39]) - z[628]*(z[16]*z[100]-z[26]*z[75]) - z[251]*(z[15]*
  z[100]+z[16]*z[456]-z[26]*z[405]-z[75]*z[401]) - mf*z[630]*(lfx*z[15]*
  z[100]-lfx*z[28]*z[75]-z[11]*z[14]*z[35]*z[100]-z[36]*(z[16]*z[100]-z[26]*
  z[75])) - mr*z[629]*(lrx*(z[100]-z[12]*z[75])+rrt*z[11]*z[14]*(z[100]+
  z[109])+rr*z[14]*(z[100]+z[109]-z[12]*z[75])) - mf*z[631]*(lfz*z[28]*z[75]-
  lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+z[30]*z[75])-z[21]*z[35]*(z[29]+
  z[11]*z[100])) - z[288]*(lfx*z[15]*z[456]+rf*z[11]*z[14]*z[100]*z[399]-lfx*
  z[16]*z[100]-lfx*z[28]*z[405]-lfx*z[75]*z[397]-z[11]*z[14]*z[35]*z[456]-rf*
  z[398]*(z[16]*z[100]-z[26]*z[75])-z[36]*(z[15]*z[100]+z[16]*z[456]-z[26]*
  z[405]-z[75]*z[401]));
  z[633] = z[30]*z[243]*z[403] + z[30]*z[255]*z[403] + z[620]*(z[14]+z[30]*
  z[74]) + z[628]*(z[21]+z[16]*z[101]+z[26]*z[74]) + z[232]*(z[438]-z[454]-
  z[12]*z[403]) + IEyy*z[623]*(z[23]-z[15]*z[101]-z[28]*z[74]) + IFyy*z[623]*(
  z[23]-z[15]*z[101]-z[28]*z[74]) + z[259]*(z[13]*z[15]+z[16]*z[101]-z[15]*
  z[454]-z[28]*z[403]-z[74]*z[397]) + z[263]*(z[13]*z[15]+z[16]*z[101]-z[15]*
  z[454]-z[28]*z[403]-z[74]*z[397]) + mr*z[629]*(rrt*z[29]+lrx*(z[101]+z[12]*
  z[74])+rrt*z[11]*z[14]*(z[101]+z[110])+rr*z[14]*(z[101]+z[110]+z[12]*z[74])) + 
  z[277]*(lrx*(z[454]+z[12]*z[403])-rrt*z[11]*z[14]*(z[438]-z[454])-rr*z[14]*(
  z[438]-z[454]-z[12]*z[403])) + mf*z[630]*(lfx*z[15]*z[101]+lfx*z[28]*z[74]-
  lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(z[21]+z[16]*z[101]+
  z[26]*z[74])) - z[39]*z[239]*z[403] - z[246]*(z[454]+z[12]*z[403]) - z[618]*(
  z[14]+z[30]*z[74]) - z[621]*(z[101]+z[12]*z[74]) - z[626]*(z[13]-z[39]*
  z[74]) - ICyy*z[622]*(z[101]+z[110]+z[12]*z[74]) - z[273]*z[403]*(lrx*z[30]+
  lrz*z[39]) - z[251]*(z[13]*z[16]-z[15]*z[101]-z[16]*z[454]-z[26]*z[403]-
  z[74]*z[401]) - mr*z[627]*(lrz*z[13]-rr-lrx*z[14]-rrt*z[11]-lrx*z[30]*z[74]-
  lrz*z[39]*z[74]) - z[269]*(lrz*(z[454]+z[12]*z[403])+rrt*z[11]*z[13]*(
  z[438]-z[454])+rr*z[13]*(z[438]-z[454]-z[12]*z[403])) - mr*z[624]*(rrt*
  z[38]+lrz*(z[101]+z[12]*z[74])-rrt*z[11]*z[13]*(z[101]+z[110])-rr*z[13]*(
  z[101]+z[110]+z[12]*z[74])) - mf*z[631]*(lfz*z[15]*z[101]+lfz*z[28]*z[74]+
  z[11]*z[21]*z[35]*z[101]-z[25]*z[35]-lfz*z[13]*z[16]-z[36]*(z[14]+z[30]*
  z[74])) - mf*z[625]*(lfx*z[14]+lfx*z[30]*z[74]-z[27]*z[35]-lfz*z[13]*z[15]-
  lfz*z[16]*z[101]-lfz*z[26]*z[74]-z[11]*z[23]*z[35]*z[101]) - z[285]*(lfx*
  z[30]*z[403]+lfz*z[13]*z[16]+rf*z[27]*z[399]+rf*z[11]*z[23]*z[101]*z[399]-
  z[35]*z[439]-lfz*z[15]*z[101]-lfz*z[16]*z[454]-lfz*z[26]*z[403]-lfz*z[74]*
  z[401]-z[11]*z[23]*z[35]*z[454]-z[11]*z[13]*z[15]*z[35]*z[101]) - z[281]*(
  lfz*z[15]*z[454]+lfz*z[28]*z[403]+lfz*z[74]*z[397]+rf*z[25]*z[399]+z[11]*
  z[21]*z[35]*z[454]-z[35]*z[614]-lfz*z[13]*z[15]-lfz*z[16]*z[101]-z[30]*
  z[36]*z[403]-rf*z[11]*z[21]*z[101]*z[399]-z[11]*z[13]*z[16]*z[35]*z[101]-rf*
  z[398]*(z[14]+z[30]*z[74])) - z[288]*(lfx*z[13]*z[15]+lfx*z[16]*z[101]+
  z[11]*z[14]*z[35]*z[454]+rf*z[398]*(z[21]+z[16]*z[101]+z[26]*z[74])-lfx*
  z[15]*z[454]-lfx*z[28]*z[403]-lfx*z[74]*z[397]-rf*z[29]*z[399]-rf*z[11]*
  z[14]*z[101]*z[399]-z[36]*(z[13]*z[16]-z[15]*z[101]-z[16]*z[454]-z[26]*
  z[403]-z[74]*z[401]));
  z[634] = z[30]*z[75]*z[602] + z[30]*z[242]*z[405] + z[30]*z[254]*z[405] + 
  z[604]*(1+z[30]*z[75]) + z[247]*(z[456]-z[12]*z[405]) + ICyy*z[612]*(z[100]+
  z[109]-z[12]*z[75]) + IEyy*z[607]*(z[15]*z[100]-z[28]*z[75]) + IFyy*z[607]*(
  z[15]*z[100]-z[28]*z[75]) + z[233]*(z[456]-z[448]-z[12]*z[405]) + z[258]*(
  z[15]*z[456]-z[16]*z[100]-z[28]*z[405]-z[75]*z[397]) + z[262]*(z[15]*z[456]-
  z[16]*z[100]-z[28]*z[405]-z[75]*z[397]) + mr*z[613]*(lrz*(z[100]-z[12]*
  z[75])-rrt*z[11]*z[13]*(z[100]+z[109])-rr*z[13]*(z[100]+z[109]-z[12]*z[75])) + 
  z[268]*(lrz*(z[456]-z[12]*z[405])+rrt*z[11]*z[13]*(z[448]-z[456])-rr*z[13]*(
  z[456]-z[448]-z[12]*z[405])) + mf*z[616]*(lfz*z[26]*z[75]+z[14]*z[27]*z[35]-
  lfx-lfx*z[30]*z[75]-lfz*z[16]*z[100]-z[23]*z[35]*(z[29]+z[11]*z[100])) + 
  z[275]*(rrt*z[11]*z[14]*(z[448]-z[456])-lrx*(z[456]-z[12]*z[405])-rr*z[14]*(
  z[456]-z[448]-z[12]*z[405])) + z[283]*(lfz*z[26]*z[405]+lfz*z[75]*z[401]+
  z[14]*z[35]*z[439]+rf*z[23]*z[399]*(z[29]+z[11]*z[100])-lfx*z[30]*z[405]-
  lfz*z[15]*z[100]-lfz*z[16]*z[456]-rf*z[14]*z[27]*z[399]-z[11]*z[23]*z[35]*
  z[456]-z[13]*z[15]*z[35]*(z[29]+z[11]*z[100])) + z[280]*(lfz*z[15]*z[456]+
  z[14]*z[35]*z[614]+z[30]*z[36]*z[405]+z[11]*z[21]*z[35]*z[456]+rf*z[398]*(1+
  z[30]*z[75])-lfz*z[16]*z[100]-lfz*z[28]*z[405]-lfz*z[75]*z[397]-rf*z[14]*
  z[25]*z[399]-rf*z[21]*z[399]*(z[29]+z[11]*z[100])-z[13]*z[16]*z[35]*(z[29]+
  z[11]*z[100])) - z[39]*z[75]*z[605] - z[39]*z[238]*z[405] - z[611]*(z[100]-
  z[12]*z[75]) - z[272]*z[405]*(lrx*z[30]+lrz*z[39]) - z[606]*(z[16]*z[100]-
  z[26]*z[75]) - mr*z[75]*z[608]*(lrx*z[30]+lrz*z[39]) - z[250]*(z[15]*z[100]+
  z[16]*z[456]-z[26]*z[405]-z[75]*z[401]) - mf*z[610]*(lfx*z[15]*z[100]-lfx*
  z[28]*z[75]-z[11]*z[14]*z[35]*z[100]-z[36]*(z[16]*z[100]-z[26]*z[75])) - mr*
  z[609]*(lrx*(z[100]-z[12]*z[75])+rrt*z[11]*z[14]*(z[100]+z[109])+rr*z[14]*(
  z[100]+z[109]-z[12]*z[75])) - mf*z[615]*(lfz*z[28]*z[75]-lfz*z[15]*z[100]-
  z[14]*z[25]*z[35]-z[36]*(1+z[30]*z[75])-z[21]*z[35]*(z[29]+z[11]*z[100])) - 
  z[289]*(lfx*z[15]*z[456]+rf*z[11]*z[14]*z[100]*z[399]-lfx*z[16]*z[100]-lfx*
  z[28]*z[405]-lfx*z[75]*z[397]-z[11]*z[14]*z[35]*z[456]-rf*z[398]*(z[16]*
  z[100]-z[26]*z[75])-z[36]*(z[15]*z[100]+z[16]*z[456]-z[26]*z[405]-z[75]*
  z[401]));
  z[635] = z[307]*z[632] + z[312]*z[617] - z[308]*z[634] - z[311]*z[633];
  z[636] = z[292]*z[401]*(lfz*z[15]*z[105]+z[23]*z[25]*z[35]+z[21]*z[35]*(
  z[27]+z[11]*z[105])) + z[292]*z[28]*z[105]*(rf*z[11]*z[23]*z[399]-lfz*z[15]-
  z[11]*z[13]*z[15]*z[35]) + z[291]*z[30]*(rrt*z[11]*z[14]*(z[444]-z[455])-
  lrx*z[455]-rr*z[14]*(z[455]-z[444]-z[12]*z[407])) + z[292]*z[30]*(lfx*z[16]*
  z[105]+z[15]*z[36]*z[105]+z[16]*z[36]*z[455]+rf*z[16]*z[105]*z[398]+rf*
  z[23]*z[29]*z[399]+z[14]*z[35]*(z[439]+z[11]*z[455])-lfx*z[15]*z[455]-z[13]*
  z[15]*z[29]*z[35]-rf*z[14]*z[399]*(z[27]+z[11]*z[105])) + z[292]*z[26]*(lfz*
  z[15]*z[455]+z[23]*z[35]*z[614]+z[13]*z[15]*z[25]*z[35]+z[21]*z[35]*(z[439]+
  z[11]*z[455])-lfz*z[16]*z[105]-rf*z[23]*z[25]*z[399]-rf*z[21]*z[399]*(z[27]+
  z[11]*z[105])-z[13]*z[16]*z[35]*(z[27]+z[11]*z[105])) - Trw*z[444] - z[292]*
  z[28]*z[455]*(lfz*z[16]+z[11]*z[23]*z[35]) - z[292]*z[105]*z[397]*(lfz*
  z[16]+z[11]*z[23]*z[35]) - z[291]*z[39]*(lrz*z[455]+rrt*z[11]*z[13]*(z[444]-
  z[455])-rr*z[13]*(z[455]-z[444]-z[12]*z[407]));
  z[637] = ICyy*u1*(z[438]*u1+z[444]*u5+z[456]*u3-z[448]*u3-z[454]*u1-z[455]*
  u5-z[12]*(z[403]*u1+z[405]*u3-z[407]*u5));
  z[638] = IFyy*(z[30]*(z[403]*u1+z[405]*u3-z[407]*u5)*(u5+z[28]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))-(u3+
  z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[13]*z[15]*u1-z[397]*(z[74]*
  u1+z[75]*u3+z[79]*u5)-z[15]*(z[454]*u1+z[455]*u5-z[456]*u3)-z[28]*(z[403]*
  u1+z[405]*u3-z[407]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)));
  z[639] = z[13]*z[16]*z[457] + z[13]*z[15]*q2p - z[14]*z[16]*u3;
  z[640] = z[11]*z[639] - z[11]*z[16]*u1 - z[12]*z[15]*u3 - z[12]*z[14]*z[15]*
  u1;
  z[641] = (z[28]*z[31]*z[640]+z[31]*z[397]*z[121]+z[397]*pow(z[28],2)*z[121])/
  pow(z[31],1.5);
  z[642] = rf*(z[32]*z[641]+2*z[28]*z[397]*z[122]/pow(z[31],0.5))/pow(z[32],3);
  z[643] = (z[32]*(z[28]*z[641]+z[32]*z[640]+z[397]*z[122])+z[28]*z[397]*(
  z[32]*z[121]+2*z[28]*z[122])/pow(z[31],0.5))/pow(z[32],3);
  z[644] = lf*z[14]*z[15]*u3 + z[14]*z[16]*z[36]*u3 + lf*z[13]*z[16]*q2p + 
  z[11]*z[642] - rf*z[12]*z[399]*u1 - rf*z[14]*z[15]*z[398]*u3 - lr*z[13]*
  z[457] - ls*z[14]*z[457] - lf*z[13]*z[15]*z[457] - z[13]*z[16]*z[36]*z[457] - 
  z[13]*z[15]*z[36]*q2p - rf*z[13]*z[16]*z[398]*q2p - rf*z[14]*z[15]*z[123] - 
  rf*z[14]*z[16]*z[643];
  z[645] = z[13]*z[15]*z[457] - z[14]*z[15]*u3 - z[13]*z[16]*q2p;
  z[646] = z[11]*z[15]*u1 - z[12]*z[16]*u3 - z[12]*z[14]*z[16]*u1 - z[11]*
  z[645];
  z[647] = ls*z[16]*z[401]*u3 + z[15]*(ls*z[397]+rf*z[30]*z[398])*u3 + z[15]*(
  ls*z[121]+z[36]*z[132]+rf*z[30]*z[123]) - z[16]*(ls*z[28]+z[30]*z[36])*u3 - 
  z[15]*(lf*z[30]-ls*z[26])*u3 - lr*z[11]*z[14]*z[457] - z[16]*(lf*z[132]-ls*
  z[129]) - z[15]*(lf*z[11]*z[14]*z[457]+ls*z[646]) - z[16]*(z[11]*z[14]*
  z[36]*z[457]-ls*z[640]-rf*z[30]*z[643]-rf*z[398]*z[132]);
  z[648] = (z[402]*(z[56]*z[131]-2*z[65]*z[133])-z[56]*(z[56]*z[644]-z[65]*
  z[647]-z[400]*z[133]))/pow(z[56],3);
  z[649] = lf*z[15]*u3 + z[16]*z[36]*u3 + rf*z[11]*z[13]*z[399]*q2p + z[11]*
  z[14]*z[642] - rf*z[15]*z[398]*u3 - rf*z[12]*z[14]*z[399]*u1 - z[11]*z[13]*
  z[35]*z[457] - rf*z[15]*z[123] - rf*z[16]*z[643];
  z[650] = (2*z[66]*z[402]*z[133]+z[56]*(z[56]*z[649]-z[66]*z[647]-z[402]*
  z[155]-z[404]*z[133]))/pow(z[56],3);
  z[651] = -z[13]*z[16]*u3 - z[14]*z[16]*z[457] - z[14]*z[15]*q2p;
  z[652] = rf*z[12]*z[23]*z[399]*u1 + z[11]*z[35]*z[651] - z[12]*z[13]*z[15]*
  z[35]*u1 - z[11]*z[23]*z[642] - rf*z[11]*z[399]*z[126] - z[11]*z[13]*z[15]*
  z[125];
  z[653] = (z[402]*(z[56]*z[147]-2*z[69]*z[133])+z[56]*(z[69]*z[647]-z[56]*
  z[652]-z[406]*z[133]))/pow(z[56],3);
  z[654] = -z[14]*u1*z[457] - z[11]*z[13]*(z[74]*u1+z[75]*u3+z[79]*u5)*z[457] - (
  z[403]*u1+z[405]*u3-z[407]*u5)*z[173] - z[39]*(u3*z[650]-u1*z[648]-u5*
  z[653]);
  z[655] = z[13]*u1*z[457] + (z[403]*u1+z[405]*u3-z[407]*u5)*z[132] + z[30]*(
  u3*z[650]-u1*z[648]-u5*z[653]) - z[11]*z[14]*(z[74]*u1+z[75]*u3+z[79]*u5)*
  z[457];
  z[656] = IDxx*z[39]*(z[403]*u1+z[405]*u3-z[407]*u5)*(z[100]*u3-z[101]*u1-
  z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) + IDyy*(z[13]*u1-z[39]*(z[74]*
  u1+z[75]*u3+z[79]*u5))*(z[456]*u3-z[454]*u1-z[455]*u5-z[12]*(z[403]*u1+
  z[405]*u3-z[407]*u5)) - IDxz*z[30]*(z[403]*u1+z[405]*u3-z[407]*u5)*(z[100]*
  u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IDyy*z[39]*(
  z[403]*u1+z[405]*u3-z[407]*u5)*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*
  u1+z[75]*u3+z[79]*u5)) - IDxz*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(
  z[456]*u3-z[454]*u1-z[455]*u5-z[12]*(z[403]*u1+z[405]*u3-z[407]*u5)) - IDxx*(
  z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[456]*u3-z[454]*u1-z[455]*u5-
  z[12]*(z[403]*u1+z[405]*u3-z[407]*u5)) - IDxz*z[654] - IDzz*z[655];
  z[657] = z[14]*z[16]*q2p - z[13]*z[15]*u3 - z[14]*z[15]*z[457];
  z[658] = lf*z[15]*u3 + z[16]*z[36]*u3 + rf*z[11]*z[13]*z[399]*q2p + z[11]*
  z[14]*z[642] - rf*z[15]*z[398]*u3 - rf*z[12]*z[14]*z[399]*u1 - rr*z[13]*
  z[457] - z[11]*z[13]*(rrt+z[35])*z[457] - rf*z[15]*z[123] - rf*z[16]*z[643];
  z[659] = z[14]*(rr+rrt*z[11])*z[457];
  z[660] = z[13]*(rr+rrt*z[11])*z[457];
  z[661] = rr*z[14]*z[457] + z[11]*z[14]*(rrt+z[35])*z[457] + z[11]*z[13]*
  z[642] - rf*z[12]*z[13]*z[399]*u1 - rf*z[11]*z[14]*z[399]*q2p;
  z[662] = z[55]*z[658] + z[404]*z[118] - z[54]*z[660] - z[58]*z[659] - z[59]*
  z[661] - z[430]*z[136];
  z[663] = -rr*z[12]*z[13]*z[457] - lf*z[640] - z[36]*z[646] - rf*z[26]*
  z[643] - rf*z[398]*z[129] - rf*z[401]*z[123];
  z[664] = z[57]*z[650] + z[75]*z[663] + z[405]*z[130] + z[432]*z[156];
  z[665] = z[15]*z[36]*u3 + rf*z[16]*z[398]*u3 + z[14]*z[29]*z[35]*z[457] + 
  rf*z[13]*z[38]*z[399]*q2p + rf*z[16]*z[123] + z[13]*z[29]*z[642] + z[14]*
  z[38]*z[642] + rf*z[13]*z[399]*z[124] + rf*z[14]*z[399]*z[137] - lf*z[16]*
  u3 - z[13]*z[35]*z[38]*z[457] - rf*z[14]*z[29]*z[399]*q2p - rf*z[15]*z[643];
  z[666] = ls*z[15]*z[401]*u3 + z[16]*(lf*z[30]-ls*z[26])*u3 + rr*z[12]*z[14]*
  z[457] + z[16]*(lf*z[11]*z[14]*z[457]+ls*z[646]) - z[15]*(ls*z[28]+z[30]*
  z[36])*u3 - z[16]*(ls*z[397]+rf*z[30]*z[398])*u3 - z[15]*(lf*z[132]-ls*
  z[129]) - z[16]*(ls*z[121]+z[36]*z[132]+rf*z[30]*z[123]) - z[15]*(z[11]*
  z[14]*z[36]*z[457]-ls*z[640]-rf*z[30]*z[643]-rf*z[398]*z[132]);
  z[667] = z[665] - z[53]*z[650] - z[75]*z[666] - z[405]*z[139] - z[435]*
  z[156];
  z[668] = (z[437]*(2*(z[55]*z[97]+z[59]*z[84])*z[143]-z[81]*(z[55]*z[157]+
  z[59]*z[159]+z[84]*z[136]+z[97]*z[118]))-z[81]*((z[55]*z[97]+z[59]*z[84])*
  z[662]+(z[55]*z[445]+z[59]*z[447])*z[143]+z[81]*(z[97]*z[659]-z[84]*z[660]-
  z[55]*z[664]-z[59]*z[667]-z[445]*z[118]-z[447]*z[136])))/pow(z[81],3);
  z[669] = z[14]*z[15]*z[36]*u3 + rf*z[14]*z[16]*z[398]*u3 + lf*z[13]*z[16]*
  z[457] + lf*z[13]*z[15]*q2p + z[13]*z[16]*z[36]*q2p + z[38]*z[642] + rf*
  z[399]*z[137] + rf*z[14]*z[16]*z[123] - lf*z[14]*z[16]*u3 - z[13]*z[15]*
  z[36]*z[457] - z[12]*z[13]*(rrt+z[35])*z[457] - rf*z[13]*z[15]*z[398]*q2p - 
  rf*z[14]*z[15]*z[643];
  z[670] = z[669] + z[53]*z[648] - z[74]*z[666] - z[403]*z[139] - z[435]*
  z[134];
  z[671] = z[36]*z[657] + rf*z[21]*z[643] + rf*z[398]*z[119] - z[12]*z[14]*(
  rrt+z[35])*z[457] - lf*z[651] - z[29]*z[642] - rf*z[399]*z[124] - rf*z[13]*
  z[16]*z[123];
  z[672] = z[671] + z[57]*z[648] - z[74]*z[663] - z[403]*z[130] - z[432]*
  z[134];
  z[673] = (2*z[437]*(z[55]*z[90]-z[59]*z[85])*z[143]-z[81]*((z[55]*z[90]-
  z[59]*z[85])*z[662]+(z[55]*z[433]-z[59]*z[436])*z[143]-z[437]*(z[59]*z[140]+
  z[85]*z[136]-z[55]*z[135]-z[90]*z[118])-z[81]*(z[55]*z[672]+z[433]*z[118]-
  z[85]*z[660]-z[90]*z[659]-z[59]*z[670]-z[436]*z[136])))/pow(z[81],3);
  z[674] = z[12]*z[16]*u1 - z[11]*z[15]*u3 - z[11]*z[14]*z[15]*u1 - z[12]*
  z[639];
  z[675] = rf*z[399]*(z[14]*z[27]*q2p-z[13]*z[145]-z[23]*z[137]-z[38]*z[126]) - (
  z[13]*z[27]+z[23]*z[38])*z[642] - z[13]*(z[439]+z[15]*z[38])*z[125] - z[35]*(
  z[14]*z[27]*z[457]+z[14]*z[439]*q2p-z[12]*z[13]*z[23]*z[457]-z[13]*z[674]-
  z[38]*z[651]-z[13]*z[15]*z[137]);
  z[676] = z[675] + z[53]*z[653] + z[407]*z[139] - z[79]*z[666] - z[435]*
  z[148];
  z[677] = z[35]*(z[13]*z[27]*z[457]+z[12]*z[14]*z[23]*z[457]+z[13]*z[439]*
  q2p+z[14]*z[674]-z[29]*z[651]-z[13]*z[15]*z[124]) - (z[14]*z[27]-z[23]*
  z[29])*z[642] - (z[14]*z[439]-z[13]*z[15]*z[29])*z[125] - rf*z[399]*(z[13]*
  z[27]*q2p+z[14]*z[145]-z[23]*z[124]-z[29]*z[126]);
  z[678] = z[677] + z[57]*z[653] + z[407]*z[130] - z[79]*z[663] - z[432]*
  z[148];
  z[679] = (2*z[437]*(z[55]*z[93]-z[59]*z[89])*z[143]-z[81]*((z[55]*z[93]-
  z[59]*z[89])*z[662]+(z[55]*z[441]-z[59]*z[443])*z[143]-z[437]*(z[59]*z[151]+
  z[89]*z[136]-z[55]*z[149]-z[93]*z[118])-z[81]*(z[55]*z[678]+z[441]*z[118]-
  z[89]*z[660]-z[93]*z[659]-z[59]*z[676]-z[443]*z[136])))/pow(z[81],3);
  z[680] = z[15]*u3*(z[454]*u1+z[455]*u5-z[456]*u3) + z[16]*u3*(z[100]*u3-
  z[101]*u1-z[105]*u5) + u1*z[657] + (z[74]*u1+z[75]*u3+z[79]*u5)*z[646] + 
  z[401]*(u1*z[134]+u3*z[156]+u5*z[148]) + (z[403]*u1+z[405]*u3-z[407]*u5)*
  z[129] + z[15]*(u3*z[160]-u1*z[144]-u5*z[152]) + z[26]*(u3*z[650]-u1*z[648]-
  u5*z[653]) - z[16]*(u3*z[668]-u1*z[673]-u5*z[679]);
  z[681] = IExz*z[30]*(z[403]*u1+z[405]*u3-z[407]*u5)*(z[28]*(z[74]*u1+z[75]*
  u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) + IExx*(z[28]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))*(
  z[401]*(z[74]*u1+z[75]*u3+z[79]*u5)+z[16]*(z[454]*u1+z[455]*u5-z[456]*u3)+
  z[26]*(z[403]*u1+z[405]*u3-z[407]*u5)-z[13]*z[16]*u1-z[15]*(z[100]*u3-
  z[101]*u1-z[105]*u5)) + IEyy*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))*(z[13]*z[15]*u1-z[397]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[15]*(z[454]*u1+z[455]*u5-z[456]*u3)-z[28]*(z[403]*u1+
  z[405]*u3-z[407]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IExz*(u3+
  z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[13]*z[15]*u1-z[397]*(z[74]*
  u1+z[75]*u3+z[79]*u5)-z[15]*(z[454]*u1+z[455]*u5-z[456]*u3)-z[28]*(z[403]*
  u1+z[405]*u3-z[407]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IEyy*(
  z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-
  z[105]*u5))*(z[401]*(z[74]*u1+z[75]*u3+z[79]*u5)+z[16]*(z[454]*u1+z[455]*u5-
  z[456]*u3)+z[26]*(z[403]*u1+z[405]*u3-z[407]*u5)-z[13]*z[16]*u1-z[15]*(
  z[100]*u3-z[101]*u1-z[105]*u5)) - IExx*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))*(z[13]*z[15]*u1-z[397]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[15]*(z[454]*u1+z[455]*u5-z[456]*u3)-z[28]*(
  z[403]*u1+z[405]*u3-z[407]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) - 
  IExz*z[680] - IEzz*z[655];
  z[682] = z[11]*u1*(z[403]*u1+z[405]*u3-z[407]*u5) + u1*z[673] + u5*z[679] + 
  z[12]*(u3*z[650]-u1*z[648]-u5*z[653]) - u3*z[668];
  z[683] = IDzz*z[30]*(z[403]*u1+z[405]*u3-z[407]*u5)*(z[13]*u1-z[39]*(z[74]*
  u1+z[75]*u3+z[79]*u5)) + z[39]*(z[403]*u1+z[405]*u3-z[407]*u5)*(IDxx*(z[14]*
  u1+2*z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-2*IDxz*(z[13]*u1-z[39]*(z[74]*u1+
  z[75]*u3+z[79]*u5))) - IDxx*z[13]*z[30]*u1*(z[403]*u1+z[405]*u3-z[407]*u5) - (
  IDzz*z[39]+2*IDxz*z[30])*(z[403]*u1+z[405]*u3-z[407]*u5)*(z[14]*u1+z[30]*(
  z[74]*u1+z[75]*u3+z[79]*u5)) - IDyy*z[682];
  z[684] = z[15]*u3*(z[100]*u3-z[101]*u1-z[105]*u5) + (z[74]*u1+z[75]*u3+
  z[79]*u5)*z[640] + z[397]*(u1*z[134]+u3*z[156]+u5*z[148]) + (z[403]*u1+
  z[405]*u3-z[407]*u5)*z[121] + z[28]*(u3*z[650]-u1*z[648]-u5*z[653]) - z[16]*
  u3*(z[454]*u1+z[455]*u5-z[456]*u3) - u1*z[651] - z[15]*(u3*z[668]-u1*z[673]-
  u5*z[679]) - z[16]*(u3*z[160]-u1*z[144]-u5*z[152]);
  z[685] = IEzz*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[401]*(
  z[74]*u1+z[75]*u3+z[79]*u5)+z[16]*(z[454]*u1+z[455]*u5-z[456]*u3)+z[26]*(
  z[403]*u1+z[405]*u3-z[407]*u5)-z[13]*z[16]*u1-z[15]*(z[100]*u3-z[101]*u1-
  z[105]*u5)) - z[30]*(z[403]*u1+z[405]*u3-z[407]*u5)*(2*IExz*(u3+z[14]*u1+
  z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-IEzz*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))) - IExx*(u3+z[14]*u1+z[30]*(
  z[74]*u1+z[75]*u3+z[79]*u5))*(z[401]*(z[74]*u1+z[75]*u3+z[79]*u5)+z[16]*(
  z[454]*u1+z[455]*u5-z[456]*u3)+z[26]*(z[403]*u1+z[405]*u3-z[407]*u5)-z[13]*
  z[16]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) - (z[21]*u1+z[26]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))*(IExx*z[30]*(
  z[403]*u1+z[405]*u3-z[407]*u5)-2*IExz*(z[401]*(z[74]*u1+z[75]*u3+z[79]*u5)+
  z[16]*(z[454]*u1+z[455]*u5-z[456]*u3)+z[26]*(z[403]*u1+z[405]*u3-z[407]*u5)-
  z[13]*z[16]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))) - IEyy*z[684];
  z[686] = IEyy*z[30]*(z[403]*u1+z[405]*u3-z[407]*u5)*(z[28]*(z[74]*u1+z[75]*
  u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) + IEzz*(u3+
  z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[13]*z[15]*u1-z[397]*(z[74]*
  u1+z[75]*u3+z[79]*u5)-z[15]*(z[454]*u1+z[455]*u5-z[456]*u3)-z[28]*(z[403]*
  u1+z[405]*u3-z[407]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) + IExz*(
  z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-
  z[105]*u5))*(z[13]*z[15]*u1-z[397]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[15]*(
  z[454]*u1+z[455]*u5-z[456]*u3)-z[28]*(z[403]*u1+z[405]*u3-z[407]*u5)-z[16]*(
  z[100]*u3-z[101]*u1-z[105]*u5)) - IEzz*z[30]*(z[403]*u1+z[405]*u3-z[407]*u5)*(
  z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-
  z[105]*u5)) - IEyy*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[13]*
  z[15]*u1-z[397]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[15]*(z[454]*u1+z[455]*u5-
  z[456]*u3)-z[28]*(z[403]*u1+z[405]*u3-z[407]*u5)-z[16]*(z[100]*u3-z[101]*u1-
  z[105]*u5)) - IExz*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(
  z[100]*u3-z[101]*u1-z[105]*u5))*(z[401]*(z[74]*u1+z[75]*u3+z[79]*u5)+z[16]*(
  z[454]*u1+z[455]*u5-z[456]*u3)+z[26]*(z[403]*u1+z[405]*u3-z[407]*u5)-z[13]*
  z[16]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IExx*z[680] - IExz*z[655];
  z[687] = lrx*z[13]*u1*z[457] + lrz*z[14]*u1*z[457] + lrz*z[11]*z[13]*(z[74]*
  u1+z[75]*u3+z[79]*u5)*z[457] + lrx*(z[403]*u1+z[405]*u3-z[407]*u5)*z[132] + 
  lrz*(z[403]*u1+z[405]*u3-z[407]*u5)*z[173] + lrx*z[30]*(u3*z[650]-u1*z[648]-
  u5*z[653]) + lrz*z[39]*(u3*z[650]-u1*z[648]-u5*z[653]) - lrx*z[11]*z[14]*(
  z[74]*u1+z[75]*u3+z[79]*u5)*z[457];
  z[688] = mr*((z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(rrt*z[11]*z[13]*(
  z[448]*u3+z[454]*u1+z[455]*u5-z[438]*u1-z[444]*u5-z[456]*u3)+lrz*(z[456]*u3-
  z[454]*u1-z[455]*u5-z[12]*(z[403]*u1+z[405]*u3-z[407]*u5))-rr*z[13]*(z[438]*
  u1+z[444]*u5+z[456]*u3-z[448]*u3-z[454]*u1-z[455]*u5-z[12]*(z[403]*u1+
  z[405]*u3-z[407]*u5)))+z[30]*(z[403]*u1+z[405]*u3-z[407]*u5)*(lrz*(z[100]*
  u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))-rrt*z[38]*u1-rrt*
  z[11]*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)-
  rr*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5)))+z[39]*(z[403]*u1+z[405]*u3-z[407]*u5)*(rrt*
  z[29]*u1-lrx*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*
  u5))-rrt*z[11]*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5)-rr*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))-(z[13]*u1-z[39]*(z[74]*u1+
  z[75]*u3+z[79]*u5))*(rrt*z[11]*z[14]*(z[448]*u3+z[454]*u1+z[455]*u5-z[438]*
  u1-z[444]*u5-z[456]*u3)-lrx*(z[456]*u3-z[454]*u1-z[455]*u5-z[12]*(z[403]*u1+
  z[405]*u3-z[407]*u5))-rr*z[14]*(z[438]*u1+z[444]*u5+z[456]*u3-z[448]*u3-
  z[454]*u1-z[455]*u5-z[12]*(z[403]*u1+z[405]*u3-z[407]*u5)))-z[687]);
  z[689] = lfz*z[13]*z[15]*u1*u3 + lfx*z[13]*u1*z[457] + lfz*z[14]*z[15]*u1*
  z[457] + rf*z[13]*z[27]*z[399]*u3*q2p + z[27]*(u1+z[14]*u3)*z[642] + z[439]*(
  u1+z[14]*u3)*z[125] + rf*z[399]*(u1+z[14]*u3)*z[145] + lfx*(z[403]*u1+
  z[405]*u3-z[407]*u5)*z[132] + lfx*z[30]*(u3*z[650]-u1*z[648]-u5*z[653]) + 
  lfz*z[16]*(u3*z[668]-u1*z[673]-u5*z[679]) + z[11]*z[23]*(z[454]*u1+z[455]*
  u5-z[456]*u3)*z[125] + z[35]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))*
  z[651] + z[13]*z[15]*z[35]*(u3*z[124]-z[12]*u1*(z[100]*u3-z[101]*u1-z[105]*
  u5)-z[11]*(u3*z[160]-u1*z[144]-u5*z[152])) - lfz*z[15]*u3*(z[454]*u1+z[455]*
  u5-z[456]*u3) - lfz*z[16]*u3*(z[100]*u3-z[101]*u1-z[105]*u5) - z[13]*z[27]*
  z[35]*u3*z[457] - lfx*z[11]*z[14]*(z[74]*u1+z[75]*u3+z[79]*u5)*z[457] - lfz*
  z[14]*z[16]*u1*q2p - z[13]*z[35]*z[439]*u3*q2p - z[35]*(u1+z[14]*u3)*z[674] - 
  lfz*(z[74]*u1+z[75]*u3+z[79]*u5)*z[646] - lfz*z[401]*(u1*z[134]+u3*z[156]+
  u5*z[148]) - lfz*(z[403]*u1+z[405]*u3-z[407]*u5)*z[129] - lfz*z[15]*(u3*
  z[160]-u1*z[144]-u5*z[152]) - lfz*z[26]*(u3*z[650]-u1*z[648]-u5*z[653]) - 
  z[11]*z[35]*(z[454]*u1+z[455]*u5-z[456]*u3)*z[126] - z[23]*(z[29]*u3+z[11]*(
  z[100]*u3-z[101]*u1-z[105]*u5))*z[642] - rf*z[399]*(z[29]*u3+z[11]*(z[100]*
  u3-z[101]*u1-z[105]*u5))*z[126] - z[13]*z[15]*(z[29]*u3+z[11]*(z[100]*u3-
  z[101]*u1-z[105]*u5))*z[125] - rf*z[23]*z[399]*(u3*z[124]-z[12]*u1*(z[100]*
  u3-z[101]*u1-z[105]*u5)-z[11]*(u3*z[160]-u1*z[144]-u5*z[152])) - z[23]*
  z[35]*(z[12]*z[14]*u3*z[457]-z[12]*u1*(z[454]*u1+z[455]*u5-z[456]*u3)-z[11]*(
  u3*z[668]-u1*z[673]-u5*z[679]));
  z[690] = mf*(z[30]*(z[403]*u1+z[405]*u3-z[407]*u5)*(lfz*z[13]*z[16]*u1+
  z[25]*z[35]*(u1+z[14]*u3-z[23]*u5)+lfz*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)+
  2*z[36]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-lfz*z[28]*(z[74]*
  u1+z[75]*u3+z[79]*u5)-z[21]*z[35]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-
  z[101]*u1-z[105]*u5)))-(z[401]*(z[74]*u1+z[75]*u3+z[79]*u5)+z[16]*(z[454]*
  u1+z[455]*u5-z[456]*u3)+z[26]*(z[403]*u1+z[405]*u3-z[407]*u5)-z[13]*z[16]*
  u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))*(lfx*z[28]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-lfx*z[23]*u1-z[29]*z[35]*(u1-z[23]*u5)-lfx*z[15]*(z[100]*u3-
  z[101]*u1-z[105]*u5)-z[14]*z[35]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5))-2*z[36]*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(
  z[100]*u3-z[101]*u1-z[105]*u5)))-(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*
  u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))*(z[13]*z[15]*z[29]*z[35]*u5+rf*
  z[29]*z[399]*(u1-z[23]*u5)+lfx*z[397]*(z[74]*u1+z[75]*u3+z[79]*u5)+lfx*
  z[15]*(z[454]*u1+z[455]*u5-z[456]*u3)+lfx*z[28]*(z[403]*u1+z[405]*u3-z[407]*
  u5)+lfx*z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)+rf*z[14]*z[399]*(z[27]*u5-
  z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))-lfx*z[13]*z[15]*u1-z[14]*z[35]*(
  z[439]*u5+z[11]*(z[454]*u1+z[455]*u5-z[456]*u3))-rf*z[398]*(z[21]*u1+z[26]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)))-(u3+
  z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[13]*z[15]*z[25]*z[35]*u5+
  lfz*z[397]*(z[74]*u1+z[75]*u3+z[79]*u5)+lfz*z[15]*(z[454]*u1+z[455]*u5-
  z[456]*u3)+lfz*z[28]*(z[403]*u1+z[405]*u3-z[407]*u5)+rf*z[25]*z[399]*(u1+
  z[14]*u3-z[23]*u5)+lfz*z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)+z[21]*z[35]*(
  z[439]*u5+z[11]*(z[454]*u1+z[455]*u5-z[456]*u3))-lfz*z[13]*z[15]*u1-z[35]*
  z[614]*(u1+z[14]*u3-z[23]*u5)-rf*z[398]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*
  u3+z[79]*u5))-rf*z[21]*z[399]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5))-z[13]*z[16]*z[35]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5)))-z[689]);
  z[691] = IFyy*((z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*
  u3-z[101]*u1-z[105]*u5))*(z[13]*z[15]*u1-z[397]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[15]*(z[454]*u1+z[455]*u5-z[456]*u3)-z[28]*(z[403]*u1+z[405]*u3-z[407]*u5)-
  z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))-(u5+z[28]*(z[74]*u1+z[75]*u3+z[79]*
  u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))*(z[401]*(z[74]*u1+z[75]*
  u3+z[79]*u5)+z[16]*(z[454]*u1+z[455]*u5-z[456]*u3)+z[26]*(z[403]*u1+z[405]*
  u3-z[407]*u5)-z[13]*z[16]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)));
  z[692] = IDzz*z[30]*(z[403]*u1+z[405]*u3-z[407]*u5)*(z[100]*u3-z[101]*u1-
  z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) + IDzz*(z[14]*u1+z[30]*(z[74]*
  u1+z[75]*u3+z[79]*u5))*(z[456]*u3-z[454]*u1-z[455]*u5-z[12]*(z[403]*u1+
  z[405]*u3-z[407]*u5)) + IDxz*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))*(
  z[456]*u3-z[454]*u1-z[455]*u5-z[12]*(z[403]*u1+z[405]*u3-z[407]*u5)) - IDxz*
  z[39]*(z[403]*u1+z[405]*u3-z[407]*u5)*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5)) - IDyy*z[30]*(z[403]*u1+z[405]*u3-z[407]*u5)*(
  z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IDyy*(
  z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*(z[456]*u3-z[454]*u1-z[455]*u5-
  z[12]*(z[403]*u1+z[405]*u3-z[407]*u5)) - IDxx*z[654] - IDxz*z[655];
  z[693] = (2*z[437]*(z[54]*z[97]+z[58]*z[84])*z[143]-z[81]*((z[54]*z[97]+
  z[58]*z[84])*z[662]+(z[54]*z[445]+z[58]*z[447]+z[84]*z[404]+z[97]*z[430])*
  z[143]+z[437]*(z[54]*z[157]+z[58]*z[159]+z[84]*z[141]+z[97]*z[142])-z[81]*(
  z[54]*z[664]+z[58]*z[667]+z[84]*z[658]+z[97]*z[661]+z[404]*z[159]+z[430]*
  z[157]+z[445]*z[142]+z[447]*z[141])))/pow(z[81],3);
  z[694] = (2*z[437]*(z[54]*z[90]-z[58]*z[85])*z[143]-z[81]*((z[54]*z[90]-
  z[58]*z[85])*z[662]-(z[58]*z[436]+z[85]*z[404]-z[54]*z[433]-z[90]*z[430])*
  z[143]-z[437]*(z[58]*z[140]+z[85]*z[141]-z[54]*z[135]-z[90]*z[142])-z[81]*(
  z[54]*z[672]+z[90]*z[661]+z[430]*z[135]+z[433]*z[142]-z[58]*z[670]-z[85]*
  z[658]-z[404]*z[140]-z[436]*z[141])))/pow(z[81],3);
  z[695] = (2*z[437]*(z[54]*z[93]-z[58]*z[89])*z[143]-z[81]*((z[54]*z[93]-
  z[58]*z[89])*z[662]-(z[58]*z[443]+z[89]*z[404]-z[54]*z[441]-z[93]*z[430])*
  z[143]-z[437]*(z[58]*z[151]+z[89]*z[141]-z[54]*z[149]-z[93]*z[142])-z[81]*(
  z[54]*z[678]+z[93]*z[661]+z[430]*z[149]+z[441]*z[142]-z[58]*z[676]-z[89]*
  z[658]-z[404]*z[151]-z[443]*z[141])))/pow(z[81],3);
  z[696] = z[11]*u1*(z[403]*u1+z[405]*u3-z[407]*u5) + u1*z[673] + u1*z[694] + 
  u5*z[679] + u5*z[695] + z[12]*(u3*z[650]-u1*z[648]-u5*z[653]) - u3*z[668] - 
  u3*z[693];
  z[697] = rrt*z[12]*z[14]*u1*(z[448]*u3+z[454]*u1+z[455]*u5-z[438]*u1-z[444]*
  u5-z[456]*u3) + rrt*z[12]*z[14]*u1*z[457] + rrt*z[11]*z[13]*(z[100]*u3+
  z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)*z[457] + rr*z[13]*(
  z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+
  z[75]*u3+z[79]*u5))*z[457] + rr*z[13]*(z[438]*u1+z[444]*u5+z[456]*u3-z[448]*
  u3-z[454]*u1-z[455]*u5-z[12]*(z[403]*u1+z[405]*u3-z[407]*u5))*q2p + rrt*
  z[11]*z[14]*(u3*z[668]+u3*z[693]-u1*z[673]-u1*z[694]-u5*z[679]-u5*z[695]) + 
  lrx*(u3*z[668]-z[11]*u1*(z[403]*u1+z[405]*u3-z[407]*u5)-u1*z[673]-u5*z[679]-
  z[12]*(u3*z[650]-u1*z[648]-u5*z[653])) + rr*z[14]*(u3*z[668]+u3*z[693]-
  z[11]*u1*(z[403]*u1+z[405]*u3-z[407]*u5)-u1*z[673]-u1*z[694]-u5*z[679]-u5*
  z[695]-z[12]*(u3*z[650]-u1*z[648]-u5*z[653])) - rrt*z[11]*z[13]*(z[448]*u3+
  z[454]*u1+z[455]*u5-z[438]*u1-z[444]*u5-z[456]*u3)*q2p;
  z[698] = mr*((z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*
  u5))*(rrt*z[11]*z[13]*(z[448]*u3+z[454]*u1+z[455]*u5-z[438]*u1-z[444]*u5-
  z[456]*u3)+2*lrz*(z[456]*u3-z[454]*u1-z[455]*u5-z[12]*(z[403]*u1+z[405]*u3-
  z[407]*u5))-rr*z[13]*(z[438]*u1+z[444]*u5+z[456]*u3-z[448]*u3-z[454]*u1-
  z[455]*u5-z[12]*(z[403]*u1+z[405]*u3-z[407]*u5)))-(lrx*z[30]+lrz*z[39])*(
  z[403]*u1+z[405]*u3-z[407]*u5)*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))-
  z[39]*(z[403]*u1+z[405]*u3-z[407]*u5)*(lrz*z[13]*u1-rr*u1-lrx*z[14]*u1-rrt*
  z[11]*u1-lrx*z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-lrz*z[39]*(z[74]*u1+z[75]*
  u3+z[79]*u5))-(z[456]*u3-z[454]*u1-z[455]*u5-z[12]*(z[403]*u1+z[405]*u3-
  z[407]*u5))*(rrt*z[38]*u1+rrt*z[11]*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-
  z[105]*u5-z[110]*u1-z[114]*u5)+rr*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-
  z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))-z[697]);
  z[699] = rrt*z[12]*z[13]*u1*(z[448]*u3+z[454]*u1+z[455]*u5-z[438]*u1-z[444]*
  u5-z[456]*u3) + rrt*z[12]*z[13]*u1*z[457] + rrt*z[11]*z[14]*(z[448]*u3+
  z[454]*u1+z[455]*u5-z[438]*u1-z[444]*u5-z[456]*u3)*q2p + rrt*z[11]*z[13]*(
  u3*z[668]+u3*z[693]-u1*z[673]-u1*z[694]-u5*z[679]-u5*z[695]) + rr*z[13]*(u3*
  z[668]+u3*z[693]-z[11]*u1*(z[403]*u1+z[405]*u3-z[407]*u5)-u1*z[673]-u1*
  z[694]-u5*z[679]-u5*z[695]-z[12]*(u3*z[650]-u1*z[648]-u5*z[653])) - rrt*
  z[11]*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)*
  z[457] - rr*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*
  u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))*z[457] - rr*z[14]*(z[438]*u1+z[444]*
  u5+z[456]*u3-z[448]*u3-z[454]*u1-z[455]*u5-z[12]*(z[403]*u1+z[405]*u3-
  z[407]*u5))*q2p - lrz*(u3*z[668]-z[11]*u1*(z[403]*u1+z[405]*u3-z[407]*u5)-
  u1*z[673]-u5*z[679]-z[12]*(u3*z[650]-u1*z[648]-u5*z[653]));
  z[700] = mr*((lrx*z[30]+lrz*z[39])*(z[403]*u1+z[405]*u3-z[407]*u5)*(z[14]*
  u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-z[30]*(z[403]*u1+z[405]*u3-z[407]*u5)*(
  lrz*z[13]*u1-rr*u1-lrx*z[14]*u1-rrt*z[11]*u1-lrx*z[30]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-lrz*z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))-(z[456]*u3-z[454]*u1-
  z[455]*u5-z[12]*(z[403]*u1+z[405]*u3-z[407]*u5))*(rrt*z[29]*u1-rrt*z[11]*
  z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)-rr*
  z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5)))-(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*
  u1+z[75]*u3+z[79]*u5))*(rrt*z[11]*z[14]*(z[448]*u3+z[454]*u1+z[455]*u5-
  z[438]*u1-z[444]*u5-z[456]*u3)-2*lrx*(z[456]*u3-z[454]*u1-z[455]*u5-z[12]*(
  z[403]*u1+z[405]*u3-z[407]*u5))-rr*z[14]*(z[438]*u1+z[444]*u5+z[456]*u3-
  z[448]*u3-z[454]*u1-z[455]*u5-z[12]*(z[403]*u1+z[405]*u3-z[407]*u5)))-
  z[699]);
  z[701] = z[12]*z[645] - z[11]*z[16]*u3 - z[12]*z[15]*u1 - z[11]*z[14]*z[16]*
  u1;
  z[702] = lfz*z[13]*z[16]*u1*u3 + lfz*z[15]*u3*(z[100]*u3-z[101]*u1-z[105]*
  u5) + lfz*z[14]*z[16]*u1*z[457] + lfz*z[14]*z[15]*u1*q2p + z[13]*z[15]*
  z[35]*u5*z[218] + lfz*(z[74]*u1+z[75]*u3+z[79]*u5)*z[640] + lfz*z[397]*(u1*
  z[134]+u3*z[156]+u5*z[148]) + z[25]*(u1+z[14]*u3-z[23]*u5)*z[642] + z[614]*(
  u1+z[14]*u3-z[23]*u5)*z[125] + lfz*(z[403]*u1+z[405]*u3-z[407]*u5)*z[121] + 
  rf*z[25]*z[399]*(z[13]*u3*q2p-u5*z[126]) + rf*z[399]*(u1+z[14]*u3-z[23]*u5)*
  z[218] + lfz*z[28]*(u3*z[650]-u1*z[648]-u5*z[653]) + z[35]*(z[439]*u5+z[11]*(
  z[454]*u1+z[455]*u5-z[456]*u3))*z[119] + z[35]*(z[27]*u5-z[29]*u3-z[11]*(
  z[100]*u3-z[101]*u1-z[105]*u5))*z[657] + z[13]*z[16]*(z[27]*u5-z[29]*u3-
  z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))*z[125] + z[21]*z[35]*(z[12]*z[14]*u3*
  z[457]+u5*z[674]-z[12]*u1*(z[454]*u1+z[455]*u5-z[456]*u3)-z[11]*(u3*z[668]-
  u1*z[673]-u5*z[679])) + rf*z[21]*z[399]*(u3*z[124]-z[12]*u1*(z[100]*u3-
  z[101]*u1-z[105]*u5)-u5*z[145]-z[11]*(u3*z[160]-u1*z[144]-u5*z[152])) + 
  z[13]*z[16]*z[35]*(u3*z[124]-z[12]*u1*(z[100]*u3-z[101]*u1-z[105]*u5)-u5*
  z[145]-z[11]*(u3*z[160]-u1*z[144]-u5*z[152])) + z[36]*(z[11]*z[14]*(z[74]*
  u1+z[75]*u3+z[79]*u5)*z[457]-z[13]*u1*z[457]-(z[403]*u1+z[405]*u3-z[407]*u5)*
  z[132]-z[30]*(u3*z[650]-u1*z[648]-u5*z[653])) - lfz*z[16]*u3*(z[454]*u1+
  z[455]*u5-z[456]*u3) - z[13]*z[15]*z[25]*u5*z[125] - z[25]*z[35]*(z[13]*u3*
  z[457]-u5*z[651]) - z[35]*(u1+z[14]*u3-z[23]*u5)*z[701] - z[35]*z[614]*(
  z[13]*u3*q2p-u5*z[126]) - lfz*z[15]*(u3*z[668]-u1*z[673]-u5*z[679]) - lfz*
  z[16]*(u3*z[160]-u1*z[144]-u5*z[152]) - rf*z[30]*(z[403]*u1+z[405]*u3-
  z[407]*u5)*z[123] - rf*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))*
  z[643] - z[21]*(z[439]*u5+z[11]*(z[454]*u1+z[455]*u5-z[456]*u3))*z[125] - 
  z[21]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))*z[642] - rf*
  z[399]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))*z[119] - 
  rf*z[398]*(z[13]*u1*q2p+(z[74]*u1+z[75]*u3+z[79]*u5)*z[132]+z[30]*(u1*
  z[134]+u3*z[156]+u5*z[148]));
  z[703] = mf*((z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-
  z[101]*u1-z[105]*u5))*(z[13]*z[15]*z[29]*z[35]*u5+rf*z[29]*z[399]*(u1-z[23]*
  u5)+lfx*z[397]*(z[74]*u1+z[75]*u3+z[79]*u5)+lfx*z[15]*(z[454]*u1+z[455]*u5-
  z[456]*u3)+lfx*z[28]*(z[403]*u1+z[405]*u3-z[407]*u5)+lfx*z[16]*(z[100]*u3-
  z[101]*u1-z[105]*u5)+rf*z[14]*z[399]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5))-lfx*z[13]*z[15]*u1-z[14]*z[35]*(z[439]*u5+z[11]*(z[454]*u1+
  z[455]*u5-z[456]*u3))-rf*z[398]*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))-z[36]*(z[401]*(z[74]*u1+z[75]*u3+
  z[79]*u5)+z[16]*(z[454]*u1+z[455]*u5-z[456]*u3)+z[26]*(z[403]*u1+z[405]*u3-
  z[407]*u5)-z[13]*z[16]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)))-z[30]*(
  z[403]*u1+z[405]*u3-z[407]*u5)*(lfz*z[13]*z[15]*u1+z[27]*z[35]*(u1+z[14]*u3)+
  lfz*z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*u3-lfx*z[14]*u1-lfx*z[30]*(z[74]*
  u1+z[75]*u3+z[79]*u5)-lfz*z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[23]*z[35]*(
  z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)))-(z[13]*z[15]*u1-z[397]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[15]*(z[454]*u1+z[455]*u5-z[456]*u3)-z[28]*(
  z[403]*u1+z[405]*u3-z[407]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))*(lfx*
  z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*z[23]*u1-z[29]*z[35]*(u1-z[23]*u5)-
  lfx*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[14]*z[35]*(z[27]*u5-z[11]*(
  z[100]*u3-z[101]*u1-z[105]*u5))-z[36]*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)))-(u3+z[14]*u1+z[30]*(z[74]*
  u1+z[75]*u3+z[79]*u5))*(z[35]*z[439]*(u1+z[14]*u3)+lfz*z[401]*(z[74]*u1+
  z[75]*u3+z[79]*u5)+lfz*z[16]*(z[454]*u1+z[455]*u5-z[456]*u3)+lfz*z[26]*(
  z[403]*u1+z[405]*u3-z[407]*u5)+z[11]*z[23]*z[35]*(z[454]*u1+z[455]*u5-
  z[456]*u3)+rf*z[23]*z[399]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))-
  lfz*z[13]*z[16]*u1-rf*z[27]*z[399]*(u1+z[14]*u3)-lfx*z[30]*(z[403]*u1+
  z[405]*u3-z[407]*u5)-lfz*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[13]*z[15]*
  z[35]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)))-z[702]);
  z[704] = lfx*z[16]*u3*(z[454]*u1+z[455]*u5-z[456]*u3) + z[13]*z[35]*(z[27]*
  u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))*z[457] + z[13]*z[35]*(z[439]*u5+
  z[11]*(z[454]*u1+z[455]*u5-z[456]*u3))*q2p + lfx*u1*z[651] + rf*z[29]*
  z[399]*u5*z[126] + z[13]*z[15]*z[29]*u5*z[125] + lfx*z[15]*(u3*z[668]-u1*
  z[673]-u5*z[679]) + lfx*z[16]*(u3*z[160]-u1*z[144]-u5*z[152]) + rf*(z[21]*
  u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))*
  z[643] + z[14]*z[35]*(u5*z[674]-z[12]*u1*(z[454]*u1+z[455]*u5-z[456]*u3)-
  z[11]*(u3*z[668]-u1*z[673]-u5*z[679])) + rf*(z[401]*(z[74]*u1+z[75]*u3+
  z[79]*u5)+z[16]*(z[454]*u1+z[455]*u5-z[456]*u3)+z[26]*(z[403]*u1+z[405]*u3-
  z[407]*u5)-z[13]*z[16]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))*z[123] - 
  lfx*z[15]*u3*(z[100]*u3-z[101]*u1-z[105]*u5) - z[12]*z[14]*z[35]*(u1-z[23]*
  u5)*z[457] - rf*z[13]*z[399]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))*
  q2p - z[29]*z[35]*u5*z[651] - z[13]*z[15]*z[35]*u5*z[124] - z[29]*(u1-z[23]*
  u5)*z[642] - rf*z[399]*(u1-z[23]*u5)*z[124] - lfx*(z[74]*u1+z[75]*u3+z[79]*
  u5)*z[640] - lfx*z[397]*(u1*z[134]+u3*z[156]+u5*z[148]) - lfx*(z[403]*u1+
  z[405]*u3-z[407]*u5)*z[121] - lfx*z[28]*(u3*z[650]-u1*z[648]-u5*z[653]) - 
  z[14]*(z[439]*u5+z[11]*(z[454]*u1+z[455]*u5-z[456]*u3))*z[125] - z[14]*(
  z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))*z[642] - rf*z[14]*z[399]*(
  z[12]*u1*(z[100]*u3-z[101]*u1-z[105]*u5)+u5*z[145]+z[11]*(u3*z[160]-u1*
  z[144]-u5*z[152])) - rf*z[398]*(z[15]*u3*(z[100]*u3-z[101]*u1-z[105]*u5)-u1*
  z[119]-(z[74]*u1+z[75]*u3+z[79]*u5)*z[129]-z[26]*(u1*z[134]+u3*z[156]+u5*
  z[148])-z[16]*(u3*z[160]-u1*z[144]-u5*z[152])) - z[36]*(z[16]*(u3*z[668]-u1*
  z[673]-u5*z[679])-z[15]*u3*(z[454]*u1+z[455]*u5-z[456]*u3)-z[16]*u3*(z[100]*
  u3-z[101]*u1-z[105]*u5)-u1*z[657]-(z[74]*u1+z[75]*u3+z[79]*u5)*z[646]-
  z[401]*(u1*z[134]+u3*z[156]+u5*z[148])-(z[403]*u1+z[405]*u3-z[407]*u5)*
  z[129]-z[15]*(u3*z[160]-u1*z[144]-u5*z[152])-z[26]*(u3*z[650]-u1*z[648]-u5*
  z[653]));
  z[705] = mf*((z[401]*(z[74]*u1+z[75]*u3+z[79]*u5)+z[16]*(z[454]*u1+z[455]*
  u5-z[456]*u3)+z[26]*(z[403]*u1+z[405]*u3-z[407]*u5)-z[13]*z[16]*u1-z[15]*(
  z[100]*u3-z[101]*u1-z[105]*u5))*(lfz*z[13]*z[15]*u1+z[27]*z[35]*(u1+z[14]*
  u3)+lfz*z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*u3-lfx*z[14]*u1-lfx*z[30]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-lfz*z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[23]*
  z[35]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)))+(z[13]*z[15]*u1-
  z[397]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[15]*(z[454]*u1+z[455]*u5-z[456]*u3)-
  z[28]*(z[403]*u1+z[405]*u3-z[407]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))*(
  lfz*z[13]*z[16]*u1+z[25]*z[35]*(u1+z[14]*u3-z[23]*u5)+lfz*z[15]*(z[100]*u3-
  z[101]*u1-z[105]*u5)+z[36]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-
  lfz*z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[21]*z[35]*(z[27]*u5-z[29]*u3-z[11]*(
  z[100]*u3-z[101]*u1-z[105]*u5)))+(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*
  u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))*(z[35]*z[439]*(u1+z[14]*u3)+lfz*
  z[401]*(z[74]*u1+z[75]*u3+z[79]*u5)+lfz*z[16]*(z[454]*u1+z[455]*u5-z[456]*
  u3)+lfz*z[26]*(z[403]*u1+z[405]*u3-z[407]*u5)+z[11]*z[23]*z[35]*(z[454]*u1+
  z[455]*u5-z[456]*u3)+rf*z[23]*z[399]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5))-lfz*z[13]*z[16]*u1-rf*z[27]*z[399]*(u1+z[14]*u3)-lfx*z[30]*(
  z[403]*u1+z[405]*u3-z[407]*u5)-lfz*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)-
  z[13]*z[15]*z[35]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)))+(z[28]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))*(
  z[13]*z[15]*z[25]*z[35]*u5+lfz*z[397]*(z[74]*u1+z[75]*u3+z[79]*u5)+lfz*
  z[15]*(z[454]*u1+z[455]*u5-z[456]*u3)+lfz*z[28]*(z[403]*u1+z[405]*u3-z[407]*
  u5)+rf*z[25]*z[399]*(u1+z[14]*u3-z[23]*u5)+lfz*z[16]*(z[100]*u3-z[101]*u1-
  z[105]*u5)+z[21]*z[35]*(z[439]*u5+z[11]*(z[454]*u1+z[455]*u5-z[456]*u3))-
  lfz*z[13]*z[15]*u1-z[35]*z[614]*(u1+z[14]*u3-z[23]*u5)-z[30]*z[36]*(z[403]*
  u1+z[405]*u3-z[407]*u5)-rf*z[398]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+
  z[79]*u5))-rf*z[21]*z[399]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5))-z[13]*z[16]*z[35]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5)))-z[704]);
  z[706] = z[11]*z[79]*z[637] + z[30]*z[79]*z[691] + z[30]*z[407]*z[265] + (
  z[16]*z[105]+z[26]*z[79])*z[638] + z[30]*z[79]*z[656] + z[30]*z[79]*z[681] + 
  z[39]*z[407]*z[240] + (z[105]+z[12]*z[79])*z[683] + (z[15]*z[105]+z[28]*
  z[79])*z[685] + (z[16]*z[105]+z[26]*z[79])*z[686] + (z[455]-z[12]*z[407])*
  z[248] + z[79]*(lrx*z[30]+lrz*z[39])*z[688] + (z[16]*z[105]+z[28]*z[407]-
  z[15]*z[455]-z[79]*z[397])*z[264] + (lfx*z[30]*z[79]-lfz*z[16]*z[105]-lfz*
  z[26]*z[79]-z[11]*z[23]*z[35]*z[105])*z[690] + (lrz*(z[105]+z[12]*z[79])-
  rrt*z[11]*z[13]*(z[105]+z[114])-rr*z[13]*(z[105]+z[114]+z[12]*z[79]))*
  z[700] + (rrt*z[11]*z[14]*(z[444]-z[455])-lrx*(z[455]-z[12]*z[407])-rr*
  z[14]*(z[455]-z[444]-z[12]*z[407]))*z[278] + (lfz*z[26]*z[407]+rf*z[11]*
  z[23]*z[105]*z[399]-lfx*z[30]*z[407]-lfz*z[15]*z[105]-lfz*z[16]*z[455]-lfz*
  z[79]*z[401]-z[11]*z[23]*z[35]*z[455]-z[11]*z[13]*z[15]*z[35]*z[105])*
  z[286] + (lfz*z[15]*z[455]+lfz*z[79]*z[397]+z[23]*z[35]*z[614]+z[30]*z[36]*
  z[407]+z[13]*z[15]*z[25]*z[35]+z[21]*z[35]*(z[439]+z[11]*z[455])-lfz*z[16]*
  z[105]-lfz*z[28]*z[407]-rf*z[23]*z[25]*z[399]-rf*z[30]*z[79]*z[398]-rf*
  z[21]*z[399]*(z[27]+z[11]*z[105])-z[13]*z[16]*z[35]*(z[27]+z[11]*z[105]))*
  z[282] + (lfx*z[16]*z[105]+lfx*z[28]*z[407]+rf*z[23]*z[29]*z[399]+z[14]*
  z[35]*(z[439]+z[11]*z[455])+rf*z[398]*(z[16]*z[105]+z[26]*z[79])-lfx*z[15]*
  z[455]-lfx*z[79]*z[397]-z[13]*z[15]*z[29]*z[35]-rf*z[14]*z[399]*(z[27]+
  z[11]*z[105])-z[36]*(z[26]*z[407]-z[15]*z[105]-z[16]*z[455]-z[79]*z[401]))*
  z[290] - z[11]*z[407]*z[235] - (z[26]*z[407]-z[15]*z[105]-z[16]*z[455]-
  z[79]*z[401])*z[266] - z[30]*z[407]*z[244] - z[30]*z[407]*z[256] - z[39]*
  z[79]*z[692] - ICyy*(z[105]+z[114]+z[12]*z[79])*z[696] - z[407]*(lrx*z[30]+
  lrz*z[39])*z[274] - (z[455]-z[444]-z[12]*z[407])*z[234] - IFyy*(1+z[15]*
  z[105]+z[28]*z[79])*z[684] - (z[16]*z[105]+z[28]*z[407]-z[15]*z[455]-z[79]*
  z[397])*z[260] - (z[26]*z[407]-z[15]*z[105]-z[16]*z[455]-z[79]*z[401])*
  z[252] - (lrx*(z[105]+z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+z[114])+rr*z[14]*(
  z[105]+z[114]+z[12]*z[79]))*z[698] - (z[30]*z[36]*z[79]-lfz*z[15]*z[105]-
  lfz*z[28]*z[79]-z[23]*z[25]*z[35]-z[21]*z[35]*(z[27]+z[11]*z[105]))*z[703] - (
  lfx*z[15]*z[105]+lfx*z[28]*z[79]+z[23]*z[29]*z[35]-z[14]*z[35]*(z[27]+z[11]*
  z[105])-z[36]*(z[16]*z[105]+z[26]*z[79]))*z[705] - (lrz*(z[455]-z[12]*
  z[407])+rrt*z[11]*z[13]*(z[444]-z[455])-rr*z[13]*(z[455]-z[444]-z[12]*
  z[407]))*z[270];
  z[707] = z[30]*z[79]*z[620] + z[39]*z[79]*z[626] + z[39]*z[239]*z[407] + 
  z[628]*(z[16]*z[105]+z[26]*z[79]) + z[273]*z[407]*(lrx*z[30]+lrz*z[39]) + 
  mr*z[79]*z[627]*(lrx*z[30]+lrz*z[39]) + z[259]*(z[16]*z[105]+z[28]*z[407]-
  z[15]*z[455]-z[79]*z[397]) + z[263]*(z[16]*z[105]+z[28]*z[407]-z[15]*z[455]-
  z[79]*z[397]) + mr*z[629]*(lrx*(z[105]+z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+
  z[114])+rr*z[14]*(z[105]+z[114]+z[12]*z[79])) + mf*z[631]*(z[30]*z[36]*
  z[79]-lfz*z[15]*z[105]-lfz*z[28]*z[79]-z[23]*z[25]*z[35]-z[21]*z[35]*(z[27]+
  z[11]*z[105])) + mf*z[630]*(lfx*z[15]*z[105]+lfx*z[28]*z[79]+z[23]*z[29]*
  z[35]-z[14]*z[35]*(z[27]+z[11]*z[105])-z[36]*(z[16]*z[105]+z[26]*z[79])) - 
  z[30]*z[79]*z[618] - z[30]*z[243]*z[407] - z[30]*z[255]*z[407] - z[621]*(
  z[105]+z[12]*z[79]) - z[246]*(z[455]-z[12]*z[407]) - ICyy*z[622]*(z[105]+
  z[114]+z[12]*z[79]) - IEyy*z[623]*(z[15]*z[105]+z[28]*z[79]) - IFyy*z[623]*(
  1+z[15]*z[105]+z[28]*z[79]) - z[232]*(z[455]-z[444]-z[12]*z[407]) - z[251]*(
  z[26]*z[407]-z[15]*z[105]-z[16]*z[455]-z[79]*z[401]) - mf*z[625]*(lfx*z[30]*
  z[79]-lfz*z[16]*z[105]-lfz*z[26]*z[79]-z[11]*z[23]*z[35]*z[105]) - mr*
  z[624]*(lrz*(z[105]+z[12]*z[79])-rrt*z[11]*z[13]*(z[105]+z[114])-rr*z[13]*(
  z[105]+z[114]+z[12]*z[79])) - z[269]*(lrz*(z[455]-z[12]*z[407])+rrt*z[11]*
  z[13]*(z[444]-z[455])-rr*z[13]*(z[455]-z[444]-z[12]*z[407])) - z[277]*(rrt*
  z[11]*z[14]*(z[444]-z[455])-lrx*(z[455]-z[12]*z[407])-rr*z[14]*(z[455]-
  z[444]-z[12]*z[407])) - z[285]*(lfz*z[26]*z[407]+rf*z[11]*z[23]*z[105]*
  z[399]-lfx*z[30]*z[407]-lfz*z[15]*z[105]-lfz*z[16]*z[455]-lfz*z[79]*z[401]-
  z[11]*z[23]*z[35]*z[455]-z[11]*z[13]*z[15]*z[35]*z[105]) - z[281]*(lfz*
  z[15]*z[455]+lfz*z[79]*z[397]+z[23]*z[35]*z[614]+z[30]*z[36]*z[407]+z[13]*
  z[15]*z[25]*z[35]+z[21]*z[35]*(z[439]+z[11]*z[455])-lfz*z[16]*z[105]-lfz*
  z[28]*z[407]-rf*z[23]*z[25]*z[399]-rf*z[30]*z[79]*z[398]-rf*z[21]*z[399]*(
  z[27]+z[11]*z[105])-z[13]*z[16]*z[35]*(z[27]+z[11]*z[105])) - z[288]*(lfx*
  z[16]*z[105]+lfx*z[28]*z[407]+rf*z[23]*z[29]*z[399]+z[14]*z[35]*(z[439]+
  z[11]*z[455])+rf*z[398]*(z[16]*z[105]+z[26]*z[79])-lfx*z[15]*z[455]-lfx*
  z[79]*z[397]-z[13]*z[15]*z[29]*z[35]-rf*z[14]*z[399]*(z[27]+z[11]*z[105])-
  z[36]*(z[26]*z[407]-z[15]*z[105]-z[16]*z[455]-z[79]*z[401]));
  z[708] = z[30]*z[79]*z[602] + z[30]*z[79]*z[604] + z[39]*z[238]*z[407] + 
  z[611]*(z[105]+z[12]*z[79]) + z[606]*(z[16]*z[105]+z[26]*z[79]) + z[272]*
  z[407]*(lrx*z[30]+lrz*z[39]) + z[258]*(z[16]*z[105]+z[28]*z[407]-z[15]*
  z[455]-z[79]*z[397]) + z[262]*(z[16]*z[105]+z[28]*z[407]-z[15]*z[455]-z[79]*
  z[397]) + mr*z[609]*(lrx*(z[105]+z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+
  z[114])+rr*z[14]*(z[105]+z[114]+z[12]*z[79])) + mf*z[615]*(z[30]*z[36]*
  z[79]-lfz*z[15]*z[105]-lfz*z[28]*z[79]-z[23]*z[25]*z[35]-z[21]*z[35]*(z[27]+
  z[11]*z[105])) + mf*z[610]*(lfx*z[15]*z[105]+lfx*z[28]*z[79]+z[23]*z[29]*
  z[35]-z[14]*z[35]*(z[27]+z[11]*z[105])-z[36]*(z[16]*z[105]+z[26]*z[79])) - 
  z[30]*z[242]*z[407] - z[30]*z[254]*z[407] - z[39]*z[79]*z[605] - z[247]*(
  z[455]-z[12]*z[407]) - ICyy*z[612]*(z[105]+z[114]+z[12]*z[79]) - IEyy*
  z[607]*(z[15]*z[105]+z[28]*z[79]) - IFyy*z[607]*(1+z[15]*z[105]+z[28]*z[79]) - 
  mr*z[79]*z[608]*(lrx*z[30]+lrz*z[39]) - z[233]*(z[455]-z[444]-z[12]*z[407]) - 
  z[250]*(z[26]*z[407]-z[15]*z[105]-z[16]*z[455]-z[79]*z[401]) - mf*z[616]*(
  lfx*z[30]*z[79]-lfz*z[16]*z[105]-lfz*z[26]*z[79]-z[11]*z[23]*z[35]*z[105]) - 
  mr*z[613]*(lrz*(z[105]+z[12]*z[79])-rrt*z[11]*z[13]*(z[105]+z[114])-rr*
  z[13]*(z[105]+z[114]+z[12]*z[79])) - z[268]*(lrz*(z[455]-z[12]*z[407])+rrt*
  z[11]*z[13]*(z[444]-z[455])-rr*z[13]*(z[455]-z[444]-z[12]*z[407])) - z[275]*(
  rrt*z[11]*z[14]*(z[444]-z[455])-lrx*(z[455]-z[12]*z[407])-rr*z[14]*(z[455]-
  z[444]-z[12]*z[407])) - z[283]*(lfz*z[26]*z[407]+rf*z[11]*z[23]*z[105]*
  z[399]-lfx*z[30]*z[407]-lfz*z[15]*z[105]-lfz*z[16]*z[455]-lfz*z[79]*z[401]-
  z[11]*z[23]*z[35]*z[455]-z[11]*z[13]*z[15]*z[35]*z[105]) - z[280]*(lfz*
  z[15]*z[455]+lfz*z[79]*z[397]+z[23]*z[35]*z[614]+z[30]*z[36]*z[407]+z[13]*
  z[15]*z[25]*z[35]+z[21]*z[35]*(z[439]+z[11]*z[455])-lfz*z[16]*z[105]-lfz*
  z[28]*z[407]-rf*z[23]*z[25]*z[399]-rf*z[30]*z[79]*z[398]-rf*z[21]*z[399]*(
  z[27]+z[11]*z[105])-z[13]*z[16]*z[35]*(z[27]+z[11]*z[105])) - z[289]*(lfx*
  z[16]*z[105]+lfx*z[28]*z[407]+rf*z[23]*z[29]*z[399]+z[14]*z[35]*(z[439]+
  z[11]*z[455])+rf*z[398]*(z[16]*z[105]+z[26]*z[79])-lfx*z[15]*z[455]-lfx*
  z[79]*z[397]-z[13]*z[15]*z[29]*z[35]-rf*z[14]*z[399]*(z[27]+z[11]*z[105])-
  z[36]*(z[26]*z[407]-z[15]*z[105]-z[16]*z[455]-z[79]*z[401]));
  z[709] = z[311]*z[707] + z[316]*z[634] - z[312]*z[708] - z[315]*z[632];
  z[710] = z[292]*z[397]*(lfx*z[14]-z[27]*z[35]-lfz*z[13]*z[15]-lfz*z[16]*
  z[101]-z[11]*z[23]*z[35]*z[101]) + z[292]*z[28]*(lfz*z[13]*z[16]+rf*z[27]*
  z[399]+rf*z[11]*z[23]*z[101]*z[399]-z[35]*z[439]-lfz*z[15]*z[101]-lfz*z[16]*
  z[454]-z[11]*z[23]*z[35]*z[454]-z[11]*z[13]*z[15]*z[35]*z[101]) + z[292]*
  z[26]*(lfz*z[15]*z[454]+rf*z[25]*z[399]+z[11]*z[21]*z[35]*z[454]-z[35]*
  z[614]-lfz*z[13]*z[15]-lfz*z[16]*z[101]-rf*z[14]*z[398]-rf*z[11]*z[21]*
  z[101]*z[399]-z[11]*z[13]*z[16]*z[35]*z[101]) + z[292]*z[30]*(lfx*z[13]*
  z[15]+lfx*z[16]*z[101]+z[11]*z[14]*z[35]*z[454]+rf*z[398]*(z[21]+z[16]*
  z[101])-lfx*z[15]*z[454]-rf*z[29]*z[399]-rf*z[11]*z[14]*z[101]*z[399]-z[36]*(
  z[13]*z[16]-z[15]*z[101]-z[16]*z[454])) - Trw*z[438] - z[292]*z[401]*(z[14]*
  z[36]+z[25]*z[35]+lfz*z[13]*z[16]-lfz*z[15]*z[101]-z[11]*z[21]*z[35]*z[101]) - 
  z[291]*z[39]*(lrz*z[454]+rrt*z[11]*z[13]*(z[438]-z[454])+rr*z[13]*(z[438]-
  z[454]-z[12]*z[403])) - z[291]*z[30]*(lrx*z[454]-rrt*z[11]*z[14]*(z[438]-
  z[454])-rr*z[14]*(z[438]-z[454]-z[12]*z[403]));
  z[711] = ICyy*z[11]*((z[403]*u1+z[405]*u3-z[407]*u5)*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)+(z[74]*u1+z[75]*u3+z[79]*u5)*(
  z[438]*u1+z[444]*u5+z[456]*u3-z[448]*u3-z[454]*u1-z[455]*u5-2*z[12]*(z[403]*
  u1+z[405]*u3-z[407]*u5)));
  z[712] = z[11]*z[74]*z[637] + z[11]*z[403]*z[235] + (z[14]+z[30]*z[74])*
  z[691] + (z[21]+z[16]*z[101]+z[26]*z[74])*z[638] + z[30]*z[403]*z[244] + 
  z[30]*z[403]*z[256] + (z[14]+z[30]*z[74])*z[656] + (z[14]+z[30]*z[74])*
  z[681] + (z[101]+z[12]*z[74])*z[683] + (z[454]+z[12]*z[403])*z[248] + (
  z[13]-z[39]*z[74])*z[692] + (z[21]+z[16]*z[101]+z[26]*z[74])*z[686] + 
  z[403]*(lrx*z[30]+lrz*z[39])*z[274] + (z[438]-z[454]-z[12]*z[403])*z[234] + 
  IFyy*(z[23]-z[15]*z[101]-z[28]*z[74])*z[684] + (z[13]*z[15]+z[16]*z[101]-
  z[15]*z[454]-z[28]*z[403]-z[74]*z[397])*z[264] + (rrt*z[38]+lrz*(z[101]+
  z[12]*z[74])-rrt*z[11]*z[13]*(z[101]+z[110])-rr*z[13]*(z[101]+z[110]+z[12]*
  z[74]))*z[700] + (lfz*z[15]*z[101]+lfz*z[28]*z[74]+z[11]*z[21]*z[35]*z[101]-
  z[25]*z[35]-lfz*z[13]*z[16]-z[36]*(z[14]+z[30]*z[74]))*z[703] + (lfx*z[14]+
  lfx*z[30]*z[74]-z[27]*z[35]-lfz*z[13]*z[15]-lfz*z[16]*z[101]-lfz*z[26]*
  z[74]-z[11]*z[23]*z[35]*z[101])*z[690] + (lfx*z[30]*z[403]+lfz*z[13]*z[16]+
  rf*z[27]*z[399]+rf*z[11]*z[23]*z[101]*z[399]-z[35]*z[439]-lfz*z[15]*z[101]-
  lfz*z[16]*z[454]-lfz*z[26]*z[403]-lfz*z[74]*z[401]-z[11]*z[23]*z[35]*z[454]-
  z[11]*z[13]*z[15]*z[35]*z[101])*z[286] + (lfz*z[15]*z[454]+lfz*z[28]*z[403]+
  lfz*z[74]*z[397]+rf*z[25]*z[399]+z[11]*z[21]*z[35]*z[454]-z[35]*z[614]-lfz*
  z[13]*z[15]-lfz*z[16]*z[101]-z[30]*z[36]*z[403]-rf*z[11]*z[21]*z[101]*
  z[399]-z[11]*z[13]*z[16]*z[35]*z[101]-rf*z[398]*(z[14]+z[30]*z[74]))*z[282] + (
  lfx*z[13]*z[15]+lfx*z[16]*z[101]+z[11]*z[14]*z[35]*z[454]+rf*z[398]*(z[21]+
  z[16]*z[101]+z[26]*z[74])-lfx*z[15]*z[454]-lfx*z[28]*z[403]-lfx*z[74]*
  z[397]-rf*z[29]*z[399]-rf*z[11]*z[14]*z[101]*z[399]-z[36]*(z[13]*z[16]-
  z[15]*z[101]-z[16]*z[454]-z[26]*z[403]-z[74]*z[401]))*z[290] - z[711] - 
  z[30]*z[403]*z[265] - (z[13]*z[16]-z[15]*z[101]-z[16]*z[454]-z[26]*z[403]-
  z[74]*z[401])*z[266] - z[39]*z[403]*z[240] - ICyy*(z[101]+z[110]+z[12]*
  z[74])*z[696] - (z[23]-z[15]*z[101]-z[28]*z[74])*z[685] - (z[13]*z[15]+
  z[16]*z[101]-z[15]*z[454]-z[28]*z[403]-z[74]*z[397])*z[260] - (z[13]*z[16]-
  z[15]*z[101]-z[16]*z[454]-z[26]*z[403]-z[74]*z[401])*z[252] - (lrz*z[13]-rr-
  lrx*z[14]-rrt*z[11]-lrx*z[30]*z[74]-lrz*z[39]*z[74])*z[688] - (rrt*z[29]+
  lrx*(z[101]+z[12]*z[74])+rrt*z[11]*z[14]*(z[101]+z[110])+rr*z[14]*(z[101]+
  z[110]+z[12]*z[74]))*z[698] - (lrz*(z[454]+z[12]*z[403])+rrt*z[11]*z[13]*(
  z[438]-z[454])+rr*z[13]*(z[438]-z[454]-z[12]*z[403]))*z[270] - (lrx*(z[454]+
  z[12]*z[403])-rrt*z[11]*z[14]*(z[438]-z[454])-rr*z[14]*(z[438]-z[454]-z[12]*
  z[403]))*z[278] - (lfx*z[15]*z[101]+lfx*z[28]*z[74]-lfx*z[23]-z[29]*z[35]-
  z[11]*z[14]*z[35]*z[101]-z[36]*(z[21]+z[16]*z[101]+z[26]*z[74]))*z[705];
  z[713] = z[307]*z[707] + z[316]*z[617] - z[308]*z[708] - z[315]*z[633];
  z[714] = Trw*z[448] + z[292]*z[30]*z[456]*(lfx*z[15]-z[16]*z[36]-z[11]*
  z[14]*z[35]) + z[291]*z[39]*(lrz*z[456]+rrt*z[11]*z[13]*(z[448]-z[456])-rr*
  z[13]*(z[456]-z[448]-z[12]*z[405])) - z[292]*z[30]*z[100]*(lfx*z[16]+z[15]*
  z[36]+rf*z[16]*z[398]-rf*z[11]*z[14]*z[399]) - z[292]*z[401]*(z[36]+lfz*
  z[15]*z[100]+z[14]*z[25]*z[35]+z[21]*z[35]*(z[29]+z[11]*z[100])) - z[292]*
  z[397]*(z[14]*z[27]*z[35]-lfx-lfz*z[16]*z[100]-z[23]*z[35]*(z[29]+z[11]*
  z[100])) - z[291]*z[30]*(rrt*z[11]*z[14]*(z[448]-z[456])-lrx*z[456]-rr*
  z[14]*(z[456]-z[448]-z[12]*z[405])) - z[292]*z[28]*(z[14]*z[35]*z[439]+rf*
  z[23]*z[399]*(z[29]+z[11]*z[100])-lfz*z[15]*z[100]-lfz*z[16]*z[456]-rf*
  z[14]*z[27]*z[399]-z[11]*z[23]*z[35]*z[456]-z[13]*z[15]*z[35]*(z[29]+z[11]*
  z[100])) - z[292]*z[26]*(rf*z[398]+lfz*z[15]*z[456]+z[14]*z[35]*z[614]+
  z[11]*z[21]*z[35]*z[456]-lfz*z[16]*z[100]-rf*z[14]*z[25]*z[399]-rf*z[21]*
  z[399]*(z[29]+z[11]*z[100])-z[13]*z[16]*z[35]*(z[29]+z[11]*z[100]));
  z[715] = z[11]*z[75]*z[637] + z[11]*z[405]*z[235] + (1+z[30]*z[75])*z[691] + 
  z[30]*z[75]*z[656] + z[30]*z[405]*z[244] + z[30]*z[405]*z[256] + (1+z[30]*
  z[75])*z[681] + z[75]*(lrx*z[30]+lrz*z[39])*z[688] + z[405]*(lrx*z[30]+lrz*
  z[39])*z[274] + (z[456]-z[448]-z[12]*z[405])*z[234] + ICyy*(z[100]+z[109]-
  z[12]*z[75])*z[696] + IFyy*(z[15]*z[100]-z[28]*z[75])*z[684] + (z[15]*
  z[456]-z[16]*z[100]-z[28]*z[405]-z[75]*z[397])*z[264] + (lfx*z[15]*z[100]-
  lfx*z[28]*z[75]-z[11]*z[14]*z[35]*z[100]-z[36]*(z[16]*z[100]-z[26]*z[75]))*
  z[705] + (lrx*(z[100]-z[12]*z[75])+rrt*z[11]*z[14]*(z[100]+z[109])+rr*z[14]*(
  z[100]+z[109]-z[12]*z[75]))*z[698] + (lrz*(z[456]-z[12]*z[405])+rrt*z[11]*
  z[13]*(z[448]-z[456])-rr*z[13]*(z[456]-z[448]-z[12]*z[405]))*z[270] + (lfz*
  z[28]*z[75]-lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+z[30]*z[75])-z[21]*
  z[35]*(z[29]+z[11]*z[100]))*z[703] + (lfx*z[15]*z[456]+rf*z[11]*z[14]*
  z[100]*z[399]-lfx*z[16]*z[100]-lfx*z[28]*z[405]-lfx*z[75]*z[397]-z[11]*
  z[14]*z[35]*z[456]-rf*z[398]*(z[16]*z[100]-z[26]*z[75])-z[36]*(z[15]*z[100]+
  z[16]*z[456]-z[26]*z[405]-z[75]*z[401]))*z[290] - z[30]*z[405]*z[265] - (
  z[16]*z[100]-z[26]*z[75])*z[638] - (z[15]*z[100]+z[16]*z[456]-z[26]*z[405]-
  z[75]*z[401])*z[266] - z[39]*z[75]*z[692] - z[39]*z[405]*z[240] - (z[100]-
  z[12]*z[75])*z[683] - (z[456]-z[12]*z[405])*z[248] - (z[15]*z[100]-z[28]*
  z[75])*z[685] - (z[16]*z[100]-z[26]*z[75])*z[686] - (z[15]*z[100]+z[16]*
  z[456]-z[26]*z[405]-z[75]*z[401])*z[252] - (z[15]*z[456]-z[16]*z[100]-z[28]*
  z[405]-z[75]*z[397])*z[260] - (lrz*(z[100]-z[12]*z[75])-rrt*z[11]*z[13]*(
  z[100]+z[109])-rr*z[13]*(z[100]+z[109]-z[12]*z[75]))*z[700] - (lfz*z[26]*
  z[75]+z[14]*z[27]*z[35]-lfx-lfx*z[30]*z[75]-lfz*z[16]*z[100]-z[23]*z[35]*(
  z[29]+z[11]*z[100]))*z[690] - (rrt*z[11]*z[14]*(z[448]-z[456])-lrx*(z[456]-
  z[12]*z[405])-rr*z[14]*(z[456]-z[448]-z[12]*z[405]))*z[278] - (lfz*z[26]*
  z[405]+lfz*z[75]*z[401]+z[14]*z[35]*z[439]+rf*z[23]*z[399]*(z[29]+z[11]*
  z[100])-lfx*z[30]*z[405]-lfz*z[15]*z[100]-lfz*z[16]*z[456]-rf*z[14]*z[27]*
  z[399]-z[11]*z[23]*z[35]*z[456]-z[13]*z[15]*z[35]*(z[29]+z[11]*z[100]))*
  z[286] - (lfz*z[15]*z[456]+z[14]*z[35]*z[614]+z[30]*z[36]*z[405]+z[11]*
  z[21]*z[35]*z[456]+rf*z[398]*(1+z[30]*z[75])-lfz*z[16]*z[100]-lfz*z[28]*
  z[405]-lfz*z[75]*z[397]-rf*z[14]*z[25]*z[399]-rf*z[21]*z[399]*(z[29]+z[11]*
  z[100])-z[13]*z[16]*z[35]*(z[29]+z[11]*z[100]))*z[282];
  z[716] = z[403]*(IDxz*z[39]-IDzz*z[30]);
  z[717] = z[15]*z[101] + z[16]*z[454] + z[26]*z[403] + z[74]*z[401] - z[13]*
  z[16];
  z[718] = -IExz*z[717] - IEzz*z[30]*z[403];
  z[719] = -IExx*z[717] - IExz*z[30]*z[403];
  z[720] = rrt*z[11]*z[14]*(z[438]-z[454]) + rr*z[14]*(z[438]-z[454]-z[12]*
  z[403]) - lrx*(z[454]+z[12]*z[403]);
  z[721] = lfz*z[15]*z[454] + lfz*z[28]*z[403] + lfz*z[74]*z[397] + rf*z[25]*
  z[399] + z[11]*z[21]*z[35]*z[454] - z[35]*z[614] - lfz*z[13]*z[15] - lfz*
  z[16]*z[101] - z[30]*z[36]*z[403] - rf*z[11]*z[21]*z[101]*z[399] - z[11]*
  z[13]*z[16]*z[35]*z[101] - rf*z[398]*(z[14]+z[30]*z[74]);
  z[722] = lfx*z[13]*z[15] + lfx*z[16]*z[101] + z[11]*z[14]*z[35]*z[454] + rf*
  z[398]*(z[21]+z[16]*z[101]+z[26]*z[74]) - lfx*z[15]*z[454] - lfx*z[28]*
  z[403] - lfx*z[74]*z[397] - rf*z[29]*z[399] - rf*z[11]*z[14]*z[101]*z[399] - 
  z[36]*(z[13]*z[16]-z[15]*z[101]-z[16]*z[454]-z[26]*z[403]-z[74]*z[401]);
  z[723] = z[403]*(IDxx*z[39]-IDxz*z[30]);
  z[724] = IDyy*(z[454]+z[12]*z[403]);
  z[725] = z[454] + z[12]*z[403] - z[438];
  z[726] = z[15]*z[454] + z[28]*z[403] + z[74]*z[397] - z[13]*z[15] - z[16]*
  z[101];
  z[727] = z[403]*(lrx*z[30]+lrz*z[39]);
  z[728] = lfx*z[30]*z[403] + lfz*z[13]*z[16] + rf*z[27]*z[399] + rf*z[11]*
  z[23]*z[101]*z[399] - z[35]*z[439] - lfz*z[15]*z[101] - lfz*z[16]*z[454] - 
  lfz*z[26]*z[403] - lfz*z[74]*z[401] - z[11]*z[23]*z[35]*z[454] - z[11]*
  z[13]*z[15]*z[35]*z[101];
  z[729] = lrz*(z[454]+z[12]*z[403]) + rrt*z[11]*z[13]*(z[438]-z[454]) + rr*
  z[13]*(z[438]-z[454]-z[12]*z[403]);
  z[730] = z[30]*z[79]*z[716] + z[30]*z[79]*z[718] + z[39]*z[237]*z[407] + 
  z[719]*(z[16]*z[105]+z[26]*z[79]) + z[271]*z[407]*(lrx*z[30]+lrz*z[39]) + 
  z[257]*(z[16]*z[105]+z[28]*z[407]-z[15]*z[455]-z[79]*z[397]) + z[261]*(
  z[16]*z[105]+z[28]*z[407]-z[15]*z[455]-z[79]*z[397]) + mr*z[720]*(lrx*(
  z[105]+z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+z[114])+rr*z[14]*(z[105]+z[114]+
  z[12]*z[79])) + mf*z[721]*(z[30]*z[36]*z[79]-lfz*z[15]*z[105]-lfz*z[28]*
  z[79]-z[23]*z[25]*z[35]-z[21]*z[35]*(z[27]+z[11]*z[105])) + mf*z[722]*(lfx*
  z[15]*z[105]+lfx*z[28]*z[79]+z[23]*z[29]*z[35]-z[14]*z[35]*(z[27]+z[11]*
  z[105])-z[36]*(z[16]*z[105]+z[26]*z[79])) - z[30]*z[241]*z[407] - z[30]*
  z[253]*z[407] - z[39]*z[79]*z[723] - z[724]*(z[105]+z[12]*z[79]) - z[245]*(
  z[455]-z[12]*z[407]) - ICyy*z[725]*(z[105]+z[114]+z[12]*z[79]) - IEyy*
  z[726]*(z[15]*z[105]+z[28]*z[79]) - IFyy*z[726]*(1+z[15]*z[105]+z[28]*z[79]) - 
  mr*z[79]*z[727]*(lrx*z[30]+lrz*z[39]) - z[231]*(z[455]-z[444]-z[12]*z[407]) - 
  z[249]*(z[26]*z[407]-z[15]*z[105]-z[16]*z[455]-z[79]*z[401]) - mf*z[728]*(
  lfx*z[30]*z[79]-lfz*z[16]*z[105]-lfz*z[26]*z[79]-z[11]*z[23]*z[35]*z[105]) - 
  mr*z[729]*(lrz*(z[105]+z[12]*z[79])-rrt*z[11]*z[13]*(z[105]+z[114])-rr*
  z[13]*(z[105]+z[114]+z[12]*z[79])) - z[267]*(lrz*(z[455]-z[12]*z[407])+rrt*
  z[11]*z[13]*(z[444]-z[455])-rr*z[13]*(z[455]-z[444]-z[12]*z[407])) - z[276]*(
  rrt*z[11]*z[14]*(z[444]-z[455])-lrx*(z[455]-z[12]*z[407])-rr*z[14]*(z[455]-
  z[444]-z[12]*z[407])) - z[284]*(lfz*z[26]*z[407]+rf*z[11]*z[23]*z[105]*
  z[399]-lfx*z[30]*z[407]-lfz*z[15]*z[105]-lfz*z[16]*z[455]-lfz*z[79]*z[401]-
  z[11]*z[23]*z[35]*z[455]-z[11]*z[13]*z[15]*z[35]*z[105]) - z[279]*(lfz*
  z[15]*z[455]+lfz*z[79]*z[397]+z[23]*z[35]*z[614]+z[30]*z[36]*z[407]+z[13]*
  z[15]*z[25]*z[35]+z[21]*z[35]*(z[439]+z[11]*z[455])-lfz*z[16]*z[105]-lfz*
  z[28]*z[407]-rf*z[23]*z[25]*z[399]-rf*z[30]*z[79]*z[398]-rf*z[21]*z[399]*(
  z[27]+z[11]*z[105])-z[13]*z[16]*z[35]*(z[27]+z[11]*z[105])) - z[287]*(lfx*
  z[16]*z[105]+lfx*z[28]*z[407]+rf*z[23]*z[29]*z[399]+z[14]*z[35]*(z[439]+
  z[11]*z[455])+rf*z[398]*(z[16]*z[105]+z[26]*z[79])-lfx*z[15]*z[455]-lfx*
  z[79]*z[397]-z[13]*z[15]*z[29]*z[35]-rf*z[14]*z[399]*(z[27]+z[11]*z[105])-
  z[36]*(z[26]*z[407]-z[15]*z[105]-z[16]*z[455]-z[79]*z[401]));
  z[731] = z[30]*z[241]*z[403] + z[30]*z[253]*z[403] + z[716]*(z[14]+z[30]*
  z[74]) + z[718]*(z[14]+z[30]*z[74]) + z[723]*(z[13]-z[39]*z[74]) + z[719]*(
  z[21]+z[16]*z[101]+z[26]*z[74]) + z[231]*(z[438]-z[454]-z[12]*z[403]) + 
  IEyy*z[726]*(z[23]-z[15]*z[101]-z[28]*z[74]) + IFyy*z[726]*(z[23]-z[15]*
  z[101]-z[28]*z[74]) + z[257]*(z[13]*z[15]+z[16]*z[101]-z[15]*z[454]-z[28]*
  z[403]-z[74]*z[397]) + z[261]*(z[13]*z[15]+z[16]*z[101]-z[15]*z[454]-z[28]*
  z[403]-z[74]*z[397]) + mr*z[727]*(lrz*z[13]-rr-lrx*z[14]-rrt*z[11]-lrx*
  z[30]*z[74]-lrz*z[39]*z[74]) + mr*z[720]*(rrt*z[29]+lrx*(z[101]+z[12]*z[74])+
  rrt*z[11]*z[14]*(z[101]+z[110])+rr*z[14]*(z[101]+z[110]+z[12]*z[74])) + 
  z[276]*(lrx*(z[454]+z[12]*z[403])-rrt*z[11]*z[14]*(z[438]-z[454])-rr*z[14]*(
  z[438]-z[454]-z[12]*z[403])) + mf*z[722]*(lfx*z[15]*z[101]+lfx*z[28]*z[74]-
  lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(z[21]+z[16]*z[101]+
  z[26]*z[74])) - z[39]*z[237]*z[403] - z[245]*(z[454]+z[12]*z[403]) - z[724]*(
  z[101]+z[12]*z[74]) - ICyy*z[725]*(z[101]+z[110]+z[12]*z[74]) - z[271]*
  z[403]*(lrx*z[30]+lrz*z[39]) - z[249]*(z[13]*z[16]-z[15]*z[101]-z[16]*
  z[454]-z[26]*z[403]-z[74]*z[401]) - z[267]*(lrz*(z[454]+z[12]*z[403])+rrt*
  z[11]*z[13]*(z[438]-z[454])+rr*z[13]*(z[438]-z[454]-z[12]*z[403])) - mr*
  z[729]*(rrt*z[38]+lrz*(z[101]+z[12]*z[74])-rrt*z[11]*z[13]*(z[101]+z[110])-
  rr*z[13]*(z[101]+z[110]+z[12]*z[74])) - mf*z[721]*(lfz*z[15]*z[101]+lfz*
  z[28]*z[74]+z[11]*z[21]*z[35]*z[101]-z[25]*z[35]-lfz*z[13]*z[16]-z[36]*(
  z[14]+z[30]*z[74])) - mf*z[728]*(lfx*z[14]+lfx*z[30]*z[74]-z[27]*z[35]-lfz*
  z[13]*z[15]-lfz*z[16]*z[101]-lfz*z[26]*z[74]-z[11]*z[23]*z[35]*z[101]) - 
  z[284]*(lfx*z[30]*z[403]+lfz*z[13]*z[16]+rf*z[27]*z[399]+rf*z[11]*z[23]*
  z[101]*z[399]-z[35]*z[439]-lfz*z[15]*z[101]-lfz*z[16]*z[454]-lfz*z[26]*
  z[403]-lfz*z[74]*z[401]-z[11]*z[23]*z[35]*z[454]-z[11]*z[13]*z[15]*z[35]*
  z[101]) - z[279]*(lfz*z[15]*z[454]+lfz*z[28]*z[403]+lfz*z[74]*z[397]+rf*
  z[25]*z[399]+z[11]*z[21]*z[35]*z[454]-z[35]*z[614]-lfz*z[13]*z[15]-lfz*
  z[16]*z[101]-z[30]*z[36]*z[403]-rf*z[11]*z[21]*z[101]*z[399]-z[11]*z[13]*
  z[16]*z[35]*z[101]-rf*z[398]*(z[14]+z[30]*z[74])) - z[287]*(lfx*z[13]*z[15]+
  lfx*z[16]*z[101]+z[11]*z[14]*z[35]*z[454]+rf*z[398]*(z[21]+z[16]*z[101]+
  z[26]*z[74])-lfx*z[15]*z[454]-lfx*z[28]*z[403]-lfx*z[74]*z[397]-rf*z[29]*
  z[399]-rf*z[11]*z[14]*z[101]*z[399]-z[36]*(z[13]*z[16]-z[15]*z[101]-z[16]*
  z[454]-z[26]*z[403]-z[74]*z[401]));
  z[732] = z[30]*z[75]*z[716] + z[30]*z[241]*z[405] + z[30]*z[253]*z[405] + 
  z[718]*(1+z[30]*z[75]) + z[245]*(z[456]-z[12]*z[405]) + z[724]*(z[100]-
  z[12]*z[75]) + ICyy*z[725]*(z[100]+z[109]-z[12]*z[75]) + IEyy*z[726]*(z[15]*
  z[100]-z[28]*z[75]) + IFyy*z[726]*(z[15]*z[100]-z[28]*z[75]) + z[231]*(
  z[456]-z[448]-z[12]*z[405]) + z[257]*(z[15]*z[456]-z[16]*z[100]-z[28]*
  z[405]-z[75]*z[397]) + z[261]*(z[15]*z[456]-z[16]*z[100]-z[28]*z[405]-z[75]*
  z[397]) + mr*z[729]*(lrz*(z[100]-z[12]*z[75])-rrt*z[11]*z[13]*(z[100]+
  z[109])-rr*z[13]*(z[100]+z[109]-z[12]*z[75])) + z[267]*(lrz*(z[456]-z[12]*
  z[405])+rrt*z[11]*z[13]*(z[448]-z[456])-rr*z[13]*(z[456]-z[448]-z[12]*
  z[405])) + mf*z[728]*(lfz*z[26]*z[75]+z[14]*z[27]*z[35]-lfx-lfx*z[30]*z[75]-
  lfz*z[16]*z[100]-z[23]*z[35]*(z[29]+z[11]*z[100])) + z[276]*(rrt*z[11]*
  z[14]*(z[448]-z[456])-lrx*(z[456]-z[12]*z[405])-rr*z[14]*(z[456]-z[448]-
  z[12]*z[405])) + z[284]*(lfz*z[26]*z[405]+lfz*z[75]*z[401]+z[14]*z[35]*
  z[439]+rf*z[23]*z[399]*(z[29]+z[11]*z[100])-lfx*z[30]*z[405]-lfz*z[15]*
  z[100]-lfz*z[16]*z[456]-rf*z[14]*z[27]*z[399]-z[11]*z[23]*z[35]*z[456]-
  z[13]*z[15]*z[35]*(z[29]+z[11]*z[100])) + z[279]*(lfz*z[15]*z[456]+z[14]*
  z[35]*z[614]+z[30]*z[36]*z[405]+z[11]*z[21]*z[35]*z[456]+rf*z[398]*(1+z[30]*
  z[75])-lfz*z[16]*z[100]-lfz*z[28]*z[405]-lfz*z[75]*z[397]-rf*z[14]*z[25]*
  z[399]-rf*z[21]*z[399]*(z[29]+z[11]*z[100])-z[13]*z[16]*z[35]*(z[29]+z[11]*
  z[100])) - z[39]*z[75]*z[723] - z[39]*z[237]*z[405] - z[271]*z[405]*(lrx*
  z[30]+lrz*z[39]) - z[719]*(z[16]*z[100]-z[26]*z[75]) - mr*z[75]*z[727]*(lrx*
  z[30]+lrz*z[39]) - z[249]*(z[15]*z[100]+z[16]*z[456]-z[26]*z[405]-z[75]*
  z[401]) - mf*z[722]*(lfx*z[15]*z[100]-lfx*z[28]*z[75]-z[11]*z[14]*z[35]*
  z[100]-z[36]*(z[16]*z[100]-z[26]*z[75])) - mr*z[720]*(lrx*(z[100]-z[12]*
  z[75])+rrt*z[11]*z[14]*(z[100]+z[109])+rr*z[14]*(z[100]+z[109]-z[12]*z[75])) - 
  mf*z[721]*(lfz*z[28]*z[75]-lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+
  z[30]*z[75])-z[21]*z[35]*(z[29]+z[11]*z[100])) - z[287]*(lfx*z[15]*z[456]+
  rf*z[11]*z[14]*z[100]*z[399]-lfx*z[16]*z[100]-lfx*z[28]*z[405]-lfx*z[75]*
  z[397]-z[11]*z[14]*z[35]*z[456]-rf*z[398]*(z[16]*z[100]-z[26]*z[75])-z[36]*(
  z[15]*z[100]+z[16]*z[456]-z[26]*z[405]-z[75]*z[401]));
  z[733] = z[306]*z[634] + z[311]*z[731] - z[307]*z[732] - z[310]*z[617];
  z[734] = z[306]*z[632] + z[312]*z[731] - z[308]*z[732] - z[310]*z[633];
  z[735] = z[314]*z[635] + z[316]*z[733] + z[345]*z[707] + z[347]*z[730] - 
  z[315]*z[734] - z[346]*z[708];
  z[736] = (z[735]*(z[347]*z[344]+z[349]*z[342]-z[352]*z[343])+z[348]*(z[713]*
  z[343]+z[347]*(z[636]+z[706])+z[349]*(z[710]+z[712])-z[635]*z[344]-z[709]*
  z[342]-z[352]*(z[714]+z[715])))/pow(z[348],2);
  z[737] = ICyy*(z[100]*u3+z[109]*u3-2*z[101]*u1-2*z[110]*u1-z[105]*u5-z[114]*
  u5-2*z[12]*z[74]*u1-z[12]*(z[75]*u3+z[79]*u5));
  z[738] = IFyy*((z[23]-z[15]*z[101]-z[28]*z[74])*(u3+z[14]*u1+z[30]*(z[74]*
  u1+z[75]*u3+z[79]*u5))-(z[14]+z[30]*z[74])*(u5+z[28]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)));
  z[739] = z[11]*z[13]*z[101] - z[12]*z[14];
  z[740] = z[11]*z[15] + z[11]*z[13]*z[16]*z[101] - z[12]*z[24];
  z[741] = rf*z[28]*z[740]/(pow(z[31],0.5)*pow(z[32],2));
  z[742] = z[740]*(z[32]+pow(z[28],2)/pow(z[31],0.5))/pow(z[32],2);
  z[743] = z[11]*z[741] + z[12]*(rrt+z[35]) - lr*z[13]*z[101] - ls*z[14]*
  z[101] - lf*z[13]*z[15]*z[101] - rf*z[14]*z[16]*z[742] - z[13]*z[16]*z[36]*
  z[101];
  z[744] = -z[12]*z[13] - z[11]*z[14]*z[101];
  z[745] = z[11]*z[16] + z[12]*z[22] - z[11]*z[13]*z[15]*z[101];
  z[746] = lr*z[744] + z[15]*(lf*z[744]-ls*z[745]) + z[16]*(ls*z[740]+z[36]*
  z[744]+rf*z[30]*z[742]);
  z[747] = (z[56]*z[743]-z[65]*z[746])/pow(z[56],2);
  z[748] = z[11]*z[14]*z[741] + z[12]*z[14]*z[35] - rf*z[16]*z[742] - z[11]*
  z[13]*z[35]*z[101];
  z[749] = (z[56]*z[748]-z[66]*z[746])/pow(z[56],2);
  z[750] = -z[11]*z[23]*z[741] - z[12]*z[23]*z[35] - z[11]*z[14]*z[16]*z[35]*
  z[101];
  z[751] = (z[56]*z[750]-z[69]*z[746])/pow(z[56],2);
  z[752] = -z[14]*z[101]*u1 - z[739]*(z[74]*u1+z[75]*u3+z[79]*u5) - z[14]*q2p - 
  z[74]*z[173] - z[39]*(z[747]*u1+z[749]*u3+z[751]*u5+z[134]);
  z[753] = z[13]*z[101]*u1 + z[744]*(z[74]*u1+z[75]*u3+z[79]*u5) + z[13]*q2p + 
  z[74]*z[132] + z[30]*(z[747]*u1+z[749]*u3+z[751]*u5+z[134]);
  z[754] = IDxz*(z[101]+z[12]*z[74])*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*
  u5)) + IDxx*(z[101]+z[12]*z[74])*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*
  u5)) + IDyy*(z[13]-z[39]*z[74])*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*
  u1+z[75]*u3+z[79]*u5)) - IDyy*(z[101]+z[12]*z[74])*(z[13]*u1-z[39]*(z[74]*
  u1+z[75]*u3+z[79]*u5)) - IDxz*(z[14]+z[30]*z[74])*(z[100]*u3-z[101]*u1-
  z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IDxx*(z[13]-z[39]*z[74])*(
  z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IDxz*
  z[752] - IDzz*z[753];
  z[755] = z[11]*z[14]*z[741] + z[12]*z[14]*(rrt+z[35]) - rf*z[16]*z[742] - 
  rr*z[13]*z[101] - z[11]*z[13]*z[101]*(rrt+z[35]);
  z[756] = -rrt*z[12]*z[13] - z[14]*z[101]*(rr+rrt*z[11]);
  z[757] = z[13]*z[101]*(rr+rrt*z[11]) - rrt*z[12]*z[14];
  z[758] = rr*z[14]*z[101] + z[11]*z[13]*z[741] + z[12]*z[13]*(rrt+z[35]) + 
  z[11]*z[14]*z[101]*(rrt+z[35]);
  z[759] = z[55]*z[755] + z[58]*z[756] - z[54]*z[757] - z[59]*z[758];
  z[760] = -lf*z[740] - lr*z[11] - z[36]*z[745] - rf*z[26]*z[742] - rr*z[11]*
  z[14] - rr*z[12]*z[13]*z[101];
  z[761] = z[57]*z[749] + z[75]*z[760];
  z[762] = z[11]*z[13] - z[12]*z[14]*z[101];
  z[763] = z[11]*z[14] + z[12]*z[13]*z[101];
  z[764] = z[13]*z[29]*z[741] + z[14]*z[38]*z[741] + z[14]*z[29]*z[35]*z[101] - 
  rf*z[15]*z[742] - z[13]*z[35]*z[762] - z[14]*z[35]*z[763] - z[13]*z[35]*
  z[38]*z[101];
  z[765] = rr*z[12]*z[14]*z[101] + z[15]*(ls*z[740]+z[36]*z[744]+rf*z[30]*
  z[742]) - rr*z[11]*z[13] - z[16]*(lf*z[744]-ls*z[745]);
  z[766] = z[764] - z[53]*z[749] - z[75]*z[765];
  z[767] = (z[759]*(z[55]*z[97]+z[59]*z[84])-z[81]*(z[55]*z[761]+z[59]*z[766]+
  z[84]*z[757]+z[97]*z[756]))/pow(z[81],2);
  z[768] = z[38]*z[741] + lf*z[13]*z[16]*z[101] - rf*z[14]*z[15]*z[742] - 
  z[13]*z[15]*z[36]*z[101] - z[763]*(rrt+z[35]);
  z[769] = z[768] - z[53]*z[747] - z[74]*z[765];
  z[770] = rf*z[21]*z[742] + lf*z[14]*z[16]*z[101] + z[762]*(rrt+z[35]) - 
  z[29]*z[741] - z[14]*z[15]*z[36]*z[101];
  z[771] = z[770] - z[57]*z[747] - z[74]*z[760];
  z[772] = (z[759]*(z[55]*z[90]-z[59]*z[85])+z[81]*(z[59]*z[769]+z[85]*z[757]-
  z[55]*z[771]-z[90]*z[756]))/pow(z[81],2);
  z[773] = -z[11]*z[24] - z[12]*z[15] - z[12]*z[13]*z[16]*z[101];
  z[774] = z[35]*(z[13]*z[773]+z[23]*z[763]-z[14]*z[27]*z[101]-z[14]*z[16]*
  z[38]*z[101]) - z[741]*(z[13]*z[27]+z[23]*z[38]);
  z[775] = z[774] - z[53]*z[751] - z[79]*z[765];
  z[776] = -z[741]*(z[14]*z[27]-z[23]*z[29]) - z[35]*(z[23]*z[762]-z[14]*
  z[773]-z[13]*z[27]*z[101]-z[14]*z[16]*z[29]*z[101]);
  z[777] = z[776] - z[57]*z[751] - z[79]*z[760];
  z[778] = (z[759]*(z[55]*z[93]-z[59]*z[89])+z[81]*(z[59]*z[775]+z[89]*z[757]-
  z[55]*z[777]-z[93]*z[756]))/pow(z[81],2);
  z[779] = z[15]*z[101]*u3 + z[745]*(z[74]*u1+z[75]*u3+z[79]*u5) + z[119] + 
  z[74]*z[129] + z[26]*(z[747]*u1+z[749]*u3+z[751]*u5+z[134]) + z[16]*(z[767]*
  u3-z[772]*u1-z[778]*u5-z[144]) - z[14]*z[15]*z[101]*u1;
  z[780] = IExz*(z[14]+z[30]*z[74])*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*
  u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) + IExx*(z[21]+z[16]*z[101]+z[26]*
  z[74])*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*
  u1-z[105]*u5)) + IEyy*(z[23]-z[15]*z[101]-z[28]*z[74])*(z[21]*u1+z[26]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IExz*(
  z[23]-z[15]*z[101]-z[28]*z[74])*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*
  u5)) - IEyy*(z[21]+z[16]*z[101]+z[26]*z[74])*(z[28]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IExx*(z[23]-
  z[15]*z[101]-z[28]*z[74])*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IExz*z[779] - IEzz*z[753];
  z[781] = z[767]*u3 + 2*z[11]*z[74]*u1 + z[11]*(z[75]*u3+z[79]*u5) + z[12]*(
  z[747]*u1+z[749]*u3+z[751]*u5+z[134]) - z[772]*u1 - z[778]*u5 - z[144];
  z[782] = IDzz*(z[14]+z[30]*z[74])*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*
  u5)) - IDxx*(z[13]-z[39]*z[74])*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)) - (
  2*IDxz*(z[14]+z[30]*z[74])-IDzz*(z[13]-z[39]*z[74]))*(z[14]*u1+z[30]*(z[74]*
  u1+z[75]*u3+z[79]*u5)) - (IDxx*(z[14]+z[30]*z[74])-2*IDxz*(z[13]-z[39]*
  z[74]))*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IDyy*z[781];
  z[783] = z[14]*z[16]*z[101]*u1 + z[740]*(z[74]*u1+z[75]*u3+z[79]*u5) + 
  z[74]*z[121] + z[28]*(z[747]*u1+z[749]*u3+z[751]*u5+z[134]) + z[15]*(z[767]*
  u3-z[772]*u1-z[778]*u5-z[144]) - z[16]*z[101]*u3 - z[126];
  z[784] = IEzz*(z[14]+z[30]*z[74])*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*
  u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IExx*(z[21]+z[16]*z[101]+z[26]*
  z[74])*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)) - (2*IExz*(z[14]+
  z[30]*z[74])-IEzz*(z[21]+z[16]*z[101]+z[26]*z[74]))*(u3+z[14]*u1+z[30]*(
  z[74]*u1+z[75]*u3+z[79]*u5)) - (IExx*(z[14]+z[30]*z[74])-2*IExz*(z[21]+
  z[16]*z[101]+z[26]*z[74]))*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IEyy*z[783];
  z[785] = IEzz*(z[23]-z[15]*z[101]-z[28]*z[74])*(u3+z[14]*u1+z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5)) + IEyy*(z[14]+z[30]*z[74])*(z[28]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) + IExz*(z[23]-
  z[15]*z[101]-z[28]*z[74])*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IEyy*(z[23]-z[15]*z[101]-z[28]*
  z[74])*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IEzz*(z[14]+z[30]*
  z[74])*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*
  u1-z[105]*u5)) - IExz*(z[21]+z[16]*z[101]+z[26]*z[74])*(z[28]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IExx*
  z[779] - IExz*z[753];
  z[786] = lrx*z[13]*z[101]*u1 + lrz*z[14]*z[101]*u1 + lrx*z[744]*(z[74]*u1+
  z[75]*u3+z[79]*u5) + lrz*z[739]*(z[74]*u1+z[75]*u3+z[79]*u5) + lrx*z[13]*
  q2p + lrz*z[14]*q2p + lrx*z[74]*z[132] + lrz*z[74]*z[173] + lrx*z[30]*(
  z[747]*u1+z[749]*u3+z[751]*u5+z[134]) + lrz*z[39]*(z[747]*u1+z[749]*u3+
  z[751]*u5+z[134]) - 2*rrt*z[12]*u1;
  z[787] = mr*((z[14]+z[30]*z[74])*(lrz*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5))-rrt*z[38]*u1-rrt*z[11]*z[13]*(z[100]*u3+z[109]*
  u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)-rr*z[13]*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))-(
  rrt*z[29]+lrx*(z[101]+z[12]*z[74])+rrt*z[11]*z[14]*(z[101]+z[110])+rr*z[14]*(
  z[101]+z[110]+z[12]*z[74]))*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))-(
  rrt*z[38]+lrz*(z[101]+z[12]*z[74])-rrt*z[11]*z[13]*(z[101]+z[110])-rr*z[13]*(
  z[101]+z[110]+z[12]*z[74]))*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-(
  z[13]-z[39]*z[74])*(rrt*z[29]*u1-lrx*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5))-rrt*z[11]*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-
  z[105]*u5-z[110]*u1-z[114]*u5)-rr*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-
  z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))-z[786]);
  z[788] = lfx*z[13]*z[101]*u1 + lfz*z[13]*z[16]*u3 + lfz*z[14]*z[15]*z[101]*
  u1 + z[27]*z[741]*(u1+z[14]*u3) + lfx*z[744]*(z[74]*u1+z[75]*u3+z[79]*u5) + 
  lfx*z[13]*q2p + lfz*z[14]*z[15]*q2p + z[27]*z[125] + lfx*z[74]*z[132] + 
  z[11]*z[23]*z[101]*z[125] + lfx*z[30]*(z[747]*u1+z[749]*u3+z[751]*u5+z[134]) + 
  z[23]*z[35]*(z[762]*u3+2*z[12]*z[101]*u1-z[12]*(z[100]*u3-z[105]*u5)-z[11]*(
  z[767]*u3-z[772]*u1-z[778]*u5-z[144])) - lfz*z[15]*z[101]*u3 - z[13]*z[27]*
  z[35]*z[101]*u3 - z[35]*z[773]*(u1+z[14]*u3) - lfz*z[745]*(z[74]*u1+z[75]*
  u3+z[79]*u5) - z[23]*z[741]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)) - 
  z[14]*z[16]*z[35]*z[101]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)) - 
  z[35]*z[145] - lfz*z[74]*z[129] - z[11]*z[35]*z[101]*z[126] - lfz*z[26]*(
  z[747]*u1+z[749]*u3+z[751]*u5+z[134]) - lfz*z[16]*(z[767]*u3-z[772]*u1-
  z[778]*u5-z[144]);
  z[789] = mf*((z[25]*z[35]+lfz*z[13]*z[16]-lfz*z[15]*z[101]-lfz*z[28]*z[74]-
  z[11]*z[21]*z[35]*z[101])*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))+(
  z[14]+z[30]*z[74])*(lfz*z[13]*z[16]*u1+z[25]*z[35]*(u1+z[14]*u3-z[23]*u5)+
  lfz*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)+2*z[36]*(u3+z[14]*u1+z[30]*(z[74]*
  u1+z[75]*u3+z[79]*u5))-lfz*z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[21]*z[35]*(
  z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)))-(lfx*z[15]*z[101]+
  lfx*z[28]*z[74]-lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-2*z[36]*(
  z[21]+z[16]*z[101]+z[26]*z[74]))*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*
  u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))-(z[21]+z[16]*z[101]+z[26]*z[74])*(
  lfx*z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*z[23]*u1-z[29]*z[35]*(u1-z[23]*
  u5)-lfx*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[14]*z[35]*(z[27]*u5-z[11]*(
  z[100]*u3-z[101]*u1-z[105]*u5)))-z[788]);
  z[790] = IFyy*((z[21]+z[16]*z[101]+z[26]*z[74])*(u5+z[28]*(z[74]*u1+z[75]*
  u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))-(z[23]-z[15]*
  z[101]-z[28]*z[74])*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(
  z[100]*u3-z[101]*u1-z[105]*u5)));
  z[791] = IDyy*(z[101]+z[12]*z[74])*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*
  u5)) + IDzz*(z[14]+z[30]*z[74])*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*
  u1+z[75]*u3+z[79]*u5)) + IDxz*(z[13]-z[39]*z[74])*(z[100]*u3-z[101]*u1-
  z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IDzz*(z[101]+z[12]*z[74])*(
  z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IDxz*(z[101]+z[12]*z[74])*(
  z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IDyy*(z[14]+z[30]*z[74])*(
  z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IDxx*
  z[752] - IDxz*z[753];
  z[792] = (z[759]*(z[54]*z[97]+z[58]*z[84])-z[81]*(z[54]*z[761]+z[58]*z[766]+
  z[84]*z[755]+z[97]*z[758]))/pow(z[81],2);
  z[793] = (z[759]*(z[54]*z[90]-z[58]*z[85])+z[81]*(z[58]*z[769]+z[85]*z[755]-
  z[54]*z[771]-z[90]*z[758]))/pow(z[81],2);
  z[794] = (z[759]*(z[54]*z[93]-z[58]*z[89])+z[81]*(z[58]*z[775]+z[89]*z[755]-
  z[54]*z[777]-z[93]*z[758]))/pow(z[81],2);
  z[795] = z[767]*u3 + z[792]*u3 + 2*z[11]*z[74]*u1 + z[11]*(z[75]*u3+z[79]*
  u5) + z[12]*(z[747]*u1+z[749]*u3+z[751]*u5+z[134]) - z[772]*u1 - z[778]*u5 - 
  z[793]*u1 - z[794]*u5 - z[144] - z[153];
  z[796] = 2*rrt*z[12]*z[14]*(z[101]+z[110])*u1 + rrt*z[11]*z[13]*z[101]*(
  z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5) + rr*z[13]*
  z[101]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5)) + lrx*(z[772]*u1+z[778]*u5+z[144]-z[767]*u3-2*
  z[11]*z[74]*u1-z[11]*(z[75]*u3+z[79]*u5)-z[12]*(z[747]*u1+z[749]*u3+z[751]*
  u5+z[134])) + rr*z[14]*(z[772]*u1+z[778]*u5+z[793]*u1+z[794]*u5+z[144]+
  z[153]-z[767]*u3-z[792]*u3-2*z[11]*z[74]*u1-z[11]*(z[75]*u3+z[79]*u5)-z[12]*(
  z[747]*u1+z[749]*u3+z[751]*u5+z[134])) - rrt*z[762]*u1 - rrt*z[12]*z[14]*(
  z[100]*u3+z[109]*u3-z[105]*u5-z[114]*u5) - rrt*z[11]*z[13]*(z[101]+z[110])*
  q2p - rr*z[13]*(z[101]+z[110]+z[12]*z[74])*q2p - rrt*z[124] - rrt*z[11]*
  z[14]*(z[767]*u3+z[792]*u3-z[772]*u1-z[778]*u5-z[793]*u1-z[794]*u5-z[144]-
  z[153]);
  z[797] = mr*((lrz*z[13]-rr-lrx*z[14]-rrt*z[11]-lrx*z[30]*z[74]-lrz*z[39]*
  z[74])*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))+(z[13]-z[39]*z[74])*(
  lrz*z[13]*u1-rr*u1-lrx*z[14]*u1-rrt*z[11]*u1-lrx*z[30]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-lrz*z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))+(z[101]+z[12]*z[74])*(rrt*
  z[38]*u1+rrt*z[11]*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5)+rr*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))-(rrt*z[38]+2*lrz*(z[101]+
  z[12]*z[74])-rrt*z[11]*z[13]*(z[101]+z[110])-rr*z[13]*(z[101]+z[110]+z[12]*
  z[74]))*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))-
  z[796]);
  z[798] = rrt*z[763]*u1 + 2*rrt*z[12]*z[13]*(z[101]+z[110])*u1 + rrt*z[11]*
  z[14]*(z[101]+z[110])*q2p + rr*z[14]*(z[101]+z[110]+z[12]*z[74])*q2p + rrt*
  z[137] + rr*z[13]*(z[772]*u1+z[778]*u5+z[793]*u1+z[794]*u5+z[144]+z[153]-
  z[767]*u3-z[792]*u3-2*z[11]*z[74]*u1-z[11]*(z[75]*u3+z[79]*u5)-z[12]*(
  z[747]*u1+z[749]*u3+z[751]*u5+z[134])) - rrt*z[12]*z[13]*(z[100]*u3+z[109]*
  u3-z[105]*u5-z[114]*u5) - rrt*z[11]*z[14]*z[101]*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5) - rr*z[14]*z[101]*(z[100]*u3+
  z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+
  z[79]*u5)) - rrt*z[11]*z[13]*(z[767]*u3+z[792]*u3-z[772]*u1-z[778]*u5-
  z[793]*u1-z[794]*u5-z[144]-z[153]) - lrz*(z[772]*u1+z[778]*u5+z[144]-z[767]*
  u3-2*z[11]*z[74]*u1-z[11]*(z[75]*u3+z[79]*u5)-z[12]*(z[747]*u1+z[749]*u3+
  z[751]*u5+z[134]));
  z[799] = mr*((z[101]+z[12]*z[74])*(rrt*z[29]*u1-rrt*z[11]*z[14]*(z[100]*u3+
  z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)-rr*z[14]*(z[100]*u3+
  z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+
  z[79]*u5)))-(lrz*z[13]-rr-lrx*z[14]-rrt*z[11]-lrx*z[30]*z[74]-lrz*z[39]*
  z[74])*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-(z[14]+z[30]*z[74])*(
  lrz*z[13]*u1-rr*u1-lrx*z[14]*u1-rrt*z[11]*u1-lrx*z[30]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-lrz*z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))-(rrt*z[29]+2*lrx*(z[101]+
  z[12]*z[74])+rrt*z[11]*z[14]*(z[101]+z[110])+rr*z[14]*(z[101]+z[110]+z[12]*
  z[74]))*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))-
  z[798]);
  z[800] = z[11]*z[22] + z[12]*z[13]*z[15]*z[101] - z[12]*z[16];
  z[801] = lfz*z[14]*z[16]*z[101]*u1 + lfz*z[740]*(z[74]*u1+z[75]*u3+z[79]*u5) + 
  z[25]*z[741]*(u1+z[14]*u3-z[23]*u5) + lfz*z[14]*z[16]*q2p + z[25]*z[125] + 
  lfz*z[74]*z[121] + z[11]*z[35]*z[101]*z[119] + lfz*z[28]*(z[747]*u1+z[749]*
  u3+z[751]*u5+z[134]) + lfz*z[15]*(z[767]*u3-z[772]*u1-z[778]*u5-z[144]) - 
  lfz*z[13]*z[15]*u3 - lfz*z[16]*z[101]*u3 - z[25]*z[35]*z[101]*(z[13]*u3+
  z[14]*z[16]*u5) - z[35]*z[800]*(u1+z[14]*u3-z[23]*u5) - rf*z[742]*(u3+z[14]*
  u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)) - z[21]*z[741]*(z[27]*u5-z[29]*u3-
  z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)) - z[14]*z[15]*z[35]*z[101]*(z[27]*u5-
  z[29]*u3-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)) - z[35]*z[218] - z[11]*
  z[21]*z[101]*z[125] - rf*(z[14]+z[30]*z[74])*z[123] - z[36]*(z[13]*z[101]*
  u1+z[744]*(z[74]*u1+z[75]*u3+z[79]*u5)+z[13]*q2p+z[74]*z[132]+z[30]*(z[747]*
  u1+z[749]*u3+z[751]*u5+z[134])) - z[21]*z[35]*(z[762]*u3+2*z[12]*z[101]*u1-
  z[773]*u5-z[12]*(z[100]*u3-z[105]*u5)-z[11]*(z[767]*u3-z[772]*u1-z[778]*u5-
  z[144]));
  z[802] = mf*((lfx*z[14]+lfx*z[30]*z[74]-z[27]*z[35]-lfz*z[13]*z[15]-lfz*
  z[16]*z[101]-lfz*z[26]*z[74]-z[11]*z[23]*z[35]*z[101])*(u3+z[14]*u1+z[30]*(
  z[74]*u1+z[75]*u3+z[79]*u5))+(lfx*z[15]*z[101]+lfx*z[28]*z[74]-lfx*z[23]-
  z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(z[21]+z[16]*z[101]+z[26]*z[74]))*(
  z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-
  z[105]*u5))-(z[14]+z[30]*z[74])*(lfz*z[13]*z[15]*u1+z[27]*z[35]*(u1+z[14]*
  u3)+lfz*z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*u3-lfx*z[14]*u1-lfx*z[30]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-lfz*z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[23]*
  z[35]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)))-(z[23]-z[15]*z[101]-
  z[28]*z[74])*(lfx*z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*z[23]*u1-z[29]*
  z[35]*(u1-z[23]*u5)-lfx*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[14]*z[35]*(
  z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5))-z[36]*(z[21]*u1+z[26]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)))-z[801]);
  z[803] = lfx*z[16]*z[101]*u3 + z[14]*z[16]*z[29]*z[35]*z[101]*u5 + z[35]*
  z[762]*(u1-z[23]*u5) + z[13]*z[35]*z[101]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*
  u1-z[105]*u5)) + rf*z[742]*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) + z[11]*z[13]*z[35]*z[101]*q2p + lfx*
  z[126] + z[35]*z[124] + rf*(z[21]+z[16]*z[101]+z[26]*z[74])*z[123] + z[14]*
  z[35]*(z[773]*u5+z[12]*(z[100]*u3-2*z[101]*u1-z[105]*u5)+z[11]*(z[767]*u3-
  z[772]*u1-z[778]*u5-z[144])) - lfx*z[14]*z[16]*z[101]*u1 - z[29]*z[741]*(u1-
  z[23]*u5) - lfx*z[740]*(z[74]*u1+z[75]*u3+z[79]*u5) - z[14]*z[741]*(z[27]*
  u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)) - z[29]*z[125] - lfx*z[74]*z[121] - 
  z[11]*z[14]*z[101]*z[125] - lfx*z[28]*(z[747]*u1+z[749]*u3+z[751]*u5+z[134]) - 
  lfx*z[15]*(z[767]*u3-z[772]*u1-z[778]*u5-z[144]) - z[36]*(z[14]*z[15]*
  z[101]*u1-z[15]*z[101]*u3-z[745]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[119]-z[74]*
  z[129]-z[26]*(z[747]*u1+z[749]*u3+z[751]*u5+z[134])-z[16]*(z[767]*u3-z[772]*
  u1-z[778]*u5-z[144]));
  z[804] = mf*((lfz*z[15]*z[101]+lfz*z[28]*z[74]+z[11]*z[21]*z[35]*z[101]-
  z[25]*z[35]-lfz*z[13]*z[16]-z[36]*(z[14]+z[30]*z[74]))*(z[28]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))+(z[21]+
  z[16]*z[101]+z[26]*z[74])*(lfz*z[13]*z[15]*u1+z[27]*z[35]*(u1+z[14]*u3)+lfz*
  z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*u3-lfx*z[14]*u1-lfx*z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-lfz*z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[23]*z[35]*(
  z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)))+(z[23]-z[15]*z[101]-z[28]*
  z[74])*(lfz*z[13]*z[16]*u1+z[25]*z[35]*(u1+z[14]*u3-z[23]*u5)+lfz*z[15]*(
  z[100]*u3-z[101]*u1-z[105]*u5)+z[36]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+
  z[79]*u5))-lfz*z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[21]*z[35]*(z[27]*u5-
  z[29]*u3-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)))-(lfx*z[14]+lfx*z[30]*z[74]-
  z[27]*z[35]-lfz*z[13]*z[15]-lfz*z[16]*z[101]-lfz*z[26]*z[74]-z[11]*z[23]*
  z[35]*z[101])*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-
  z[101]*u1-z[105]*u5))-z[803]);
  z[805] = z[11]*z[79]*z[737] + z[30]*z[79]*z[754] + z[30]*z[79]*z[780] + (
  z[105]+z[12]*z[79])*z[782] + (z[15]*z[105]+z[28]*z[79])*z[784] + (z[16]*
  z[105]+z[26]*z[79])*z[785] + z[79]*(lrx*z[30]+lrz*z[39])*z[787] + (lfx*
  z[30]*z[79]-lfz*z[16]*z[105]-lfz*z[26]*z[79]-z[11]*z[23]*z[35]*z[105])*
  z[789] + (lrz*(z[105]+z[12]*z[79])-rrt*z[11]*z[13]*(z[105]+z[114])-rr*z[13]*(
  z[105]+z[114]+z[12]*z[79]))*z[799] - z[30]*z[79]*z[790] - (z[16]*z[105]+
  z[26]*z[79])*z[738] - z[39]*z[79]*z[791] - ICyy*(z[105]+z[114]+z[12]*z[79])*
  z[795] - IFyy*(1+z[15]*z[105]+z[28]*z[79])*z[783] - (lrx*(z[105]+z[12]*
  z[79])+rrt*z[11]*z[14]*(z[105]+z[114])+rr*z[14]*(z[105]+z[114]+z[12]*z[79]))*
  z[797] - (z[30]*z[36]*z[79]-lfz*z[15]*z[105]-lfz*z[28]*z[79]-z[23]*z[25]*
  z[35]-z[21]*z[35]*(z[27]+z[11]*z[105]))*z[802] - (lfx*z[15]*z[105]+lfx*
  z[28]*z[79]+z[23]*z[29]*z[35]-z[14]*z[35]*(z[27]+z[11]*z[105])-z[36]*(z[16]*
  z[105]+z[26]*z[79]))*z[804];
  z[806] = ICyy*z[11]*((z[101]+z[110])*(z[74]*u1+z[75]*u3+z[79]*u5)-z[74]*(
  z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-2*z[12]*(z[74]*
  u1+z[75]*u3+z[79]*u5)));
  z[807] = z[806] + z[11]*z[74]*z[737] + (z[14]+z[30]*z[74])*z[754] + (z[14]+
  z[30]*z[74])*z[780] + (z[101]+z[12]*z[74])*z[782] + (z[13]-z[39]*z[74])*
  z[791] + (z[21]+z[16]*z[101]+z[26]*z[74])*z[785] + IFyy*(z[23]-z[15]*z[101]-
  z[28]*z[74])*z[783] + (rrt*z[38]+lrz*(z[101]+z[12]*z[74])-rrt*z[11]*z[13]*(
  z[101]+z[110])-rr*z[13]*(z[101]+z[110]+z[12]*z[74]))*z[799] + (lfz*z[15]*
  z[101]+lfz*z[28]*z[74]+z[11]*z[21]*z[35]*z[101]-z[25]*z[35]-lfz*z[13]*z[16]-
  z[36]*(z[14]+z[30]*z[74]))*z[802] + (lfx*z[14]+lfx*z[30]*z[74]-z[27]*z[35]-
  lfz*z[13]*z[15]-lfz*z[16]*z[101]-lfz*z[26]*z[74]-z[11]*z[23]*z[35]*z[101])*
  z[789] - (z[14]+z[30]*z[74])*z[790] - (z[21]+z[16]*z[101]+z[26]*z[74])*
  z[738] - ICyy*(z[101]+z[110]+z[12]*z[74])*z[795] - (z[23]-z[15]*z[101]-
  z[28]*z[74])*z[784] - (lrz*z[13]-rr-lrx*z[14]-rrt*z[11]-lrx*z[30]*z[74]-lrz*
  z[39]*z[74])*z[787] - (rrt*z[29]+lrx*(z[101]+z[12]*z[74])+rrt*z[11]*z[14]*(
  z[101]+z[110])+rr*z[14]*(z[101]+z[110]+z[12]*z[74]))*z[797] - (lfx*z[15]*
  z[101]+lfx*z[28]*z[74]-lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(
  z[21]+z[16]*z[101]+z[26]*z[74]))*z[804];
  z[808] = z[11]*z[75]*z[737] + (z[16]*z[100]-z[26]*z[75])*z[738] + z[30]*
  z[75]*z[754] + (1+z[30]*z[75])*z[780] + z[75]*(lrx*z[30]+lrz*z[39])*z[787] + 
  ICyy*(z[100]+z[109]-z[12]*z[75])*z[795] + IFyy*(z[15]*z[100]-z[28]*z[75])*
  z[783] + (lfx*z[15]*z[100]-lfx*z[28]*z[75]-z[11]*z[14]*z[35]*z[100]-z[36]*(
  z[16]*z[100]-z[26]*z[75]))*z[804] + (lrx*(z[100]-z[12]*z[75])+rrt*z[11]*
  z[14]*(z[100]+z[109])+rr*z[14]*(z[100]+z[109]-z[12]*z[75]))*z[797] + (lfz*
  z[28]*z[75]-lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+z[30]*z[75])-z[21]*
  z[35]*(z[29]+z[11]*z[100]))*z[802] - (1+z[30]*z[75])*z[790] - z[39]*z[75]*
  z[791] - (z[100]-z[12]*z[75])*z[782] - (z[15]*z[100]-z[28]*z[75])*z[784] - (
  z[16]*z[100]-z[26]*z[75])*z[785] - (lrz*(z[100]-z[12]*z[75])-rrt*z[11]*
  z[13]*(z[100]+z[109])-rr*z[13]*(z[100]+z[109]-z[12]*z[75]))*z[799] - (lfz*
  z[26]*z[75]+z[14]*z[27]*z[35]-lfx-lfx*z[30]*z[75]-lfz*z[16]*z[100]-z[23]*
  z[35]*(z[29]+z[11]*z[100]))*z[789];
  z[809] = (z[347]*z[805]+z[349]*z[807]-z[352]*z[808])/z[348];
  z[810] = ICyy*(z[100]+z[109]-z[12]*z[75])*u1;
  z[811] = IFyy*((z[15]*z[100]-z[28]*z[75])*(u3+z[14]*u1+z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5))-(1+z[30]*z[75])*(u5+z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)));
  z[812] = z[14]*z[15] - z[13]*z[16]*z[100];
  z[813] = z[11]*z[812] - z[12]*z[16];
  z[814] = rf*z[28]*z[813]/(pow(z[31],0.5)*pow(z[32],2));
  z[815] = z[813]*(z[32]+pow(z[28],2)/pow(z[31],0.5))/pow(z[32],2);
  z[816] = z[11]*z[814] + lf*z[14]*z[16] + lr*z[13]*z[100] + ls*z[14]*z[100] + 
  lf*z[13]*z[15]*z[100] + z[13]*z[16]*z[36]*z[100] - z[14]*z[15]*z[36] - rf*
  z[14]*z[16]*z[815];
  z[817] = -z[14]*z[16] - z[13]*z[15]*z[100];
  z[818] = z[12]*z[15] - z[11]*z[817];
  z[819] = lr*z[11]*z[14]*z[100] + z[15]*(ls*z[28]+z[30]*z[36]) + z[16]*(ls*
  z[813]+rf*z[30]*z[815]+z[11]*z[14]*z[36]*z[100]) - z[16]*(lf*z[30]-ls*z[26]) - 
  z[15]*(ls*z[818]-lf*z[11]*z[14]*z[100]);
  z[820] = (z[56]*z[816]-z[65]*z[819])/pow(z[56],2);
  z[821] = lf*z[16] + z[11]*z[14]*z[814] + z[11]*z[13]*z[35]*z[100] - z[15]*
  z[36] - rf*z[16]*z[815];
  z[822] = (z[56]*z[821]-z[66]*z[819])/pow(z[56],2);
  z[823] = z[13]*z[15] + z[14]*z[16]*z[100];
  z[824] = z[11]*(z[23]*z[814]-z[35]*z[823]);
  z[825] = (z[56]*z[824]+z[69]*z[819])/pow(z[56],2);
  z[826] = z[14]*z[100]*u1 + z[11]*z[13]*z[100]*(z[74]*u1+z[75]*u3+z[79]*u5) + 
  z[39]*(z[825]*u5-z[820]*u1-z[822]*u3-z[156]) - z[75]*z[173];
  z[827] = z[11]*z[14]*z[100]*(z[74]*u1+z[75]*u3+z[79]*u5) + z[75]*z[132] - 
  z[13]*z[100]*u1 - z[30]*(z[825]*u5-z[820]*u1-z[822]*u3-z[156]);
  z[828] = IDyy*(z[100]-z[12]*z[75])*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*
  u5)) + IDxx*z[39]*z[75]*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+
  z[75]*u3+z[79]*u5)) - IDxz*(z[100]-z[12]*z[75])*(z[14]*u1+z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5)) - IDxx*(z[100]-z[12]*z[75])*(z[13]*u1-z[39]*(z[74]*u1+
  z[75]*u3+z[79]*u5)) - IDxz*z[30]*z[75]*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5)) - IDyy*z[39]*z[75]*(z[100]*u3-z[101]*u1-z[105]*
  u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IDxz*z[826] - IDzz*z[827];
  z[829] = z[14]*z[15]*z[100] - z[13]*z[16];
  z[830] = lf*z[16] + rr*z[13]*z[100] + z[11]*z[14]*z[814] + z[11]*z[13]*
  z[100]*(rrt+z[35]) - z[15]*z[36] - rf*z[16]*z[815];
  z[831] = z[14]*z[100]*(rr+rrt*z[11]);
  z[832] = z[13]*z[100]*(rr+rrt*z[11]);
  z[833] = z[11]*z[13]*z[814] - rr*z[14]*z[100] - z[11]*z[14]*z[100]*(rrt+
  z[35]);
  z[834] = z[54]*z[832] + z[55]*z[830] + z[58]*z[831] - z[59]*z[833];
  z[835] = rr*z[12]*z[13]*z[100] - lf*z[813] - z[36]*z[818] - rf*z[26]*z[815];
  z[836] = z[57]*z[822] + z[75]*z[835];
  z[837] = lf*z[15] + z[16]*z[36] + z[13]*z[29]*z[814] + z[14]*z[38]*z[814] + 
  z[13]*z[35]*z[38]*z[100] - rf*z[15]*z[815] - z[14]*z[29]*z[35]*z[100];
  z[838] = z[16]*(ls*z[818]-lf*z[11]*z[14]*z[100]) + z[15]*(ls*z[813]+rf*
  z[30]*z[815]+z[11]*z[14]*z[36]*z[100]) - rr*z[12]*z[14]*z[100] - z[16]*(ls*
  z[28]+z[30]*z[36]) - z[15]*(lf*z[30]-ls*z[26]);
  z[839] = z[837] - z[53]*z[822] - z[75]*z[838];
  z[840] = (z[834]*(z[55]*z[97]+z[59]*z[84])+z[81]*(z[84]*z[832]-z[55]*z[836]-
  z[59]*z[839]-z[97]*z[831]))/pow(z[81],2);
  z[841] = z[38]*z[814] + lf*z[14]*z[15] + z[14]*z[16]*z[36] + z[13]*z[15]*
  z[36]*z[100] + z[12]*z[13]*z[100]*(rrt+z[35]) - lf*z[13]*z[16]*z[100] - rf*
  z[14]*z[15]*z[815];
  z[842] = z[841] - z[53]*z[820] - z[74]*z[838];
  z[843] = z[36]*z[829] + rf*z[21]*z[815] + z[12]*z[14]*z[100]*(rrt+z[35]) - 
  lf*z[823] - z[29]*z[814];
  z[844] = z[843] - z[57]*z[820] - z[74]*z[835];
  z[845] = (z[834]*(z[55]*z[90]-z[59]*z[85])+z[81]*(z[59]*z[842]-z[55]*z[844]-
  z[85]*z[832]-z[90]*z[831]))/pow(z[81],2);
  z[846] = -z[11]*z[16] - z[12]*z[812];
  z[847] = z[35]*(z[13]*z[846]+z[38]*z[823]+z[14]*z[27]*z[100]-z[12]*z[13]*
  z[23]*z[100]) - z[814]*(z[13]*z[27]+z[23]*z[38]);
  z[848] = z[847] + z[53]*z[825] - z[79]*z[838];
  z[849] = z[35]*(z[14]*z[846]-z[29]*z[823]-z[13]*z[27]*z[100]-z[12]*z[14]*
  z[23]*z[100]) - z[814]*(z[14]*z[27]-z[23]*z[29]);
  z[850] = z[849] + z[57]*z[825] - z[79]*z[835];
  z[851] = (z[834]*(z[55]*z[93]-z[59]*z[89])+z[81]*(z[59]*z[848]-z[55]*z[850]-
  z[89]*z[832]-z[93]*z[831]))/pow(z[81],2);
  z[852] = z[829]*u1 + z[15]*(z[101]*u1+z[105]*u5) + z[818]*(z[74]*u1+z[75]*
  u3+z[79]*u5) + z[75]*z[129] - 2*z[15]*z[100]*u3 - z[16]*(z[845]*u1+z[851]*
  u5-z[840]*u3-z[160]) - z[26]*(z[825]*u5-z[820]*u1-z[822]*u3-z[156]);
  z[853] = IExz*(1+z[30]*z[75])*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-
  z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) + IEyy*(z[15]*z[100]-z[28]*z[75])*(
  z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-
  z[105]*u5)) + IEyy*(z[16]*z[100]-z[26]*z[75])*(z[28]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IExz*(z[15]*
  z[100]-z[28]*z[75])*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IExx*(
  z[15]*z[100]-z[28]*z[75])*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IExx*(z[16]*z[100]-z[26]*z[75])*(
  z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-
  z[105]*u5)) - IExz*z[852] - IEzz*z[827];
  z[854] = z[840]*u3 + z[11]*z[75]*u1 + z[160] - z[845]*u1 - z[851]*u5 - 
  z[12]*(z[825]*u5-z[820]*u1-z[822]*u3-z[156]);
  z[855] = IDzz*z[30]*z[75]*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5)) + 
  z[39]*z[75]*(IDxx*(z[14]*u1+2*z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-2*IDxz*(
  z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))) - IDxx*z[13]*z[30]*z[75]*u1 - 
  z[75]*(IDzz*z[39]+2*IDxz*z[30])*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)) - 
  IDyy*z[854];
  z[856] = 2*z[16]*z[100]*u3 + z[813]*(z[74]*u1+z[75]*u3+z[79]*u5) + z[75]*
  z[121] - z[823]*u1 - z[16]*(z[101]*u1+z[105]*u5) - z[15]*(z[845]*u1+z[851]*
  u5-z[840]*u3-z[160]) - z[28]*(z[825]*u5-z[820]*u1-z[822]*u3-z[156]);
  z[857] = IEzz*(1+z[30]*z[75])*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) + (z[16]*z[100]-z[26]*z[75])*(IExx*(
  u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-2*IExz*(z[21]*u1+z[26]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))) - (2*
  IExz*(1+z[30]*z[75])+IEzz*(z[16]*z[100]-z[26]*z[75]))*(u3+z[14]*u1+z[30]*(
  z[74]*u1+z[75]*u3+z[79]*u5)) - IExx*(1+z[30]*z[75])*(z[21]*u1+z[26]*(z[74]*
  u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IEyy*z[856];
  z[858] = IEzz*(z[15]*z[100]-z[28]*z[75])*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*
  u3+z[79]*u5)) + IEyy*(1+z[30]*z[75])*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) + IExz*(z[15]*z[100]-z[28]*
  z[75])*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*
  u1-z[105]*u5)) + IExz*(z[16]*z[100]-z[26]*z[75])*(z[28]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IEyy*(z[15]*
  z[100]-z[28]*z[75])*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IEzz*(
  1+z[30]*z[75])*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*
  u3-z[101]*u1-z[105]*u5)) - IExx*z[852] - IExz*z[827];
  z[859] = lrx*z[11]*z[14]*z[100]*(z[74]*u1+z[75]*u3+z[79]*u5) + lrx*z[75]*
  z[132] + lrz*z[75]*z[173] - lrx*z[13]*z[100]*u1 - lrz*z[14]*z[100]*u1 - lrz*
  z[11]*z[13]*z[100]*(z[74]*u1+z[75]*u3+z[79]*u5) - lrx*z[30]*(z[825]*u5-
  z[820]*u1-z[822]*u3-z[156]) - lrz*z[39]*(z[825]*u5-z[820]*u1-z[822]*u3-
  z[156]);
  z[860] = mr*((lrx*(z[100]-z[12]*z[75])+rrt*z[11]*z[14]*(z[100]+z[109])+rr*
  z[14]*(z[100]+z[109]-z[12]*z[75]))*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*
  u5))+(lrz*(z[100]-z[12]*z[75])-rrt*z[11]*z[13]*(z[100]+z[109])-rr*z[13]*(
  z[100]+z[109]-z[12]*z[75]))*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))+
  z[30]*z[75]*(lrz*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+
  z[79]*u5))-rrt*z[38]*u1-rrt*z[11]*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-
  z[105]*u5-z[110]*u1-z[114]*u5)-rr*z[13]*(z[100]*u3+z[109]*u3-z[101]*u1-
  z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))+z[39]*
  z[75]*(rrt*z[29]*u1-lrx*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+
  z[75]*u3+z[79]*u5))-rrt*z[11]*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*
  u5-z[110]*u1-z[114]*u5)-rr*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-
  z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))-z[859]);
  z[861] = lfz*z[13]*z[16]*u1 + 2*lfz*z[15]*z[100]*u3 + z[13]*z[27]*z[35]*
  z[100]*u3 + z[27]*z[814]*(u1+z[14]*u3) + lfx*z[11]*z[14]*z[100]*(z[74]*u1+
  z[75]*u3+z[79]*u5) + z[35]*z[823]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5)) + lfx*z[75]*z[132] + z[14]*z[27]*z[125] + z[35]*(z[29]+z[11]*
  z[100])*z[126] + lfz*z[16]*(z[845]*u1+z[851]*u5-z[840]*u3-z[160]) + lfz*
  z[26]*(z[825]*u5-z[820]*u1-z[822]*u3-z[156]) - lfx*z[13]*z[100]*u1 - lfz*
  z[14]*z[15]*z[100]*u1 - z[35]*z[846]*(u1+z[14]*u3) - lfz*z[15]*(z[101]*u1+
  z[105]*u5) - lfz*z[818]*(z[74]*u1+z[75]*u3+z[79]*u5) - z[23]*z[814]*(z[29]*
  u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)) - z[13]*z[27]*z[35]*q2p - lfz*
  z[75]*z[129] - z[14]*z[35]*z[145] - z[23]*(z[29]+z[11]*z[100])*z[125] - lfx*
  z[30]*(z[825]*u5-z[820]*u1-z[822]*u3-z[156]) - z[23]*z[35]*(z[12]*z[100]*u1-
  z[12]*z[14]*z[100]*u3-z[124]-z[11]*(z[845]*u1+z[851]*u5-z[840]*u3-z[160]));
  z[862] = mf*((lfx*z[15]*z[100]-lfx*z[28]*z[75]-z[11]*z[14]*z[35]*z[100]-2*
  z[36]*(z[16]*z[100]-z[26]*z[75]))*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*
  u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))+(z[16]*z[100]-z[26]*z[75])*(lfx*
  z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*z[23]*u1-z[29]*z[35]*(u1-z[23]*u5)-
  lfx*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[14]*z[35]*(z[27]*u5-z[11]*(
  z[100]*u3-z[101]*u1-z[105]*u5)))+(1+z[30]*z[75])*(lfz*z[13]*z[16]*u1+z[25]*
  z[35]*(u1+z[14]*u3-z[23]*u5)+lfz*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)+2*
  z[36]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-lfz*z[28]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[21]*z[35]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*
  u1-z[105]*u5)))-(lfz*z[28]*z[75]-lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[21]*
  z[35]*(z[29]+z[11]*z[100]))*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-
  z[861]);
  z[863] = IFyy*((z[15]*z[100]-z[28]*z[75])*(z[21]*u1+z[26]*(z[74]*u1+z[75]*
  u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))+(z[16]*z[100]-z[26]*
  z[75])*(u5+z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-
  z[101]*u1-z[105]*u5)));
  z[864] = IDzz*(z[100]-z[12]*z[75])*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*
  u5)) + IDxz*(z[100]-z[12]*z[75])*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*
  u5)) + IDzz*z[30]*z[75]*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+
  z[75]*u3+z[79]*u5)) - IDyy*(z[100]-z[12]*z[75])*(z[14]*u1+z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5)) - IDxz*z[39]*z[75]*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5)) - IDyy*z[30]*z[75]*(z[100]*u3-z[101]*u1-z[105]*
  u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IDxx*z[826] - IDxz*z[827];
  z[865] = (z[834]*(z[54]*z[97]+z[58]*z[84])-z[81]*(z[54]*z[836]+z[58]*z[839]+
  z[84]*z[830]+z[97]*z[833]))/pow(z[81],2);
  z[866] = (z[834]*(z[54]*z[90]-z[58]*z[85])+z[81]*(z[58]*z[842]+z[85]*z[830]-
  z[54]*z[844]-z[90]*z[833]))/pow(z[81],2);
  z[867] = (z[834]*(z[54]*z[93]-z[58]*z[89])+z[81]*(z[58]*z[848]+z[89]*z[830]-
  z[54]*z[850]-z[93]*z[833]))/pow(z[81],2);
  z[868] = z[840]*u3 + z[865]*u3 + z[11]*z[75]*u1 + z[160] + z[161] - z[845]*
  u1 - z[851]*u5 - z[866]*u1 - z[867]*u5 - z[12]*(z[825]*u5-z[820]*u1-z[822]*
  u3-z[156]);
  z[869] = rrt*z[11]*z[13]*(z[100]+z[109])*q2p + rr*z[13]*(z[100]+z[109]-
  z[12]*z[75])*q2p + rrt*z[11]*z[14]*(z[845]*u1+z[851]*u5+z[866]*u1+z[867]*u5-
  z[840]*u3-z[865]*u3-z[160]-z[161]) - 2*rrt*z[12]*z[14]*z[100]*u1 - rrt*
  z[12]*z[14]*z[109]*u1 - rrt*z[11]*z[13]*z[100]*(z[100]*u3+z[109]*u3-z[101]*
  u1-z[105]*u5-z[110]*u1-z[114]*u5) - rr*z[13]*z[100]*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - 
  lrx*(z[840]*u3+z[11]*z[75]*u1+z[160]-z[845]*u1-z[851]*u5-z[12]*(z[825]*u5-
  z[820]*u1-z[822]*u3-z[156])) - rr*z[14]*(z[840]*u3+z[865]*u3+z[11]*z[75]*u1+
  z[160]+z[161]-z[845]*u1-z[851]*u5-z[866]*u1-z[867]*u5-z[12]*(z[825]*u5-
  z[820]*u1-z[822]*u3-z[156]));
  z[870] = mr*((2*lrz*(z[100]-z[12]*z[75])-rrt*z[11]*z[13]*(z[100]+z[109])-rr*
  z[13]*(z[100]+z[109]-z[12]*z[75]))*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5))-z[75]*(lrx*z[30]+lrz*z[39])*(z[13]*u1-z[39]*(
  z[74]*u1+z[75]*u3+z[79]*u5))-z[39]*z[75]*(lrz*z[13]*u1-rr*u1-lrx*z[14]*u1-
  rrt*z[11]*u1-lrx*z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-lrz*z[39]*(z[74]*u1+
  z[75]*u3+z[79]*u5))-(z[100]-z[12]*z[75])*(rrt*z[38]*u1+rrt*z[11]*z[13]*(
  z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)+rr*z[13]*(
  z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+
  z[75]*u3+z[79]*u5)))-z[869]);
  z[871] = rrt*z[11]*z[14]*z[100]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-
  z[110]*u1-z[114]*u5) + rr*z[14]*z[100]*(z[100]*u3+z[109]*u3-z[101]*u1-
  z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) + rrt*
  z[11]*z[13]*(z[845]*u1+z[851]*u5+z[866]*u1+z[867]*u5-z[840]*u3-z[865]*u3-
  z[160]-z[161]) + lrz*(z[840]*u3+z[11]*z[75]*u1+z[160]-z[845]*u1-z[851]*u5-
  z[12]*(z[825]*u5-z[820]*u1-z[822]*u3-z[156])) - 2*rrt*z[12]*z[13]*z[100]*u1 - 
  rrt*z[12]*z[13]*z[109]*u1 - rrt*z[11]*z[14]*(z[100]+z[109])*q2p - rr*z[14]*(
  z[100]+z[109]-z[12]*z[75])*q2p - rr*z[13]*(z[840]*u3+z[865]*u3+z[11]*z[75]*
  u1+z[160]+z[161]-z[845]*u1-z[851]*u5-z[866]*u1-z[867]*u5-z[12]*(z[825]*u5-
  z[820]*u1-z[822]*u3-z[156]));
  z[872] = mr*(z[75]*(lrx*z[30]+lrz*z[39])*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+
  z[79]*u5))+z[14]*(rrt*z[11]*(z[100]+z[109])+rr*(z[100]+z[109]-z[12]*z[75]))*(
  z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5))-z[30]*
  z[75]*(lrz*z[13]*u1-rr*u1-lrx*z[14]*u1-rrt*z[11]*u1-lrx*z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-lrz*z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))-(z[100]-z[12]*
  z[75])*(rrt*z[29]*u1-2*lrx*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+
  z[75]*u3+z[79]*u5))-rrt*z[11]*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*
  u5-z[110]*u1-z[114]*u5)-rr*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-
  z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))-z[871]);
  z[873] = z[11]*z[15] + z[12]*z[817];
  z[874] = 2*lfz*z[16]*z[100]*u3 + lfz*z[813]*(z[74]*u1+z[75]*u3+z[79]*u5) + 
  z[25]*z[814]*(u1+z[14]*u3-z[23]*u5) + z[35]*z[829]*(z[27]*u5-z[29]*u3-z[11]*(
  z[100]*u3-z[101]*u1-z[105]*u5)) + z[25]*z[35]*(z[823]*u5+z[13]*z[100]*u3-
  z[13]*q2p) + lfz*z[75]*z[121] + z[14]*z[25]*z[125] + z[21]*(z[29]+z[11]*
  z[100])*z[125] + z[21]*z[35]*(z[846]*u5+z[12]*z[100]*u1-z[12]*z[14]*z[100]*
  u3-z[124]-z[11]*(z[845]*u1+z[851]*u5-z[840]*u3-z[160])) - lfz*z[13]*z[15]*
  u1 - lfz*z[14]*z[16]*z[100]*u1 - lfz*z[16]*(z[101]*u1+z[105]*u5) - z[35]*
  z[873]*(u1+z[14]*u3-z[23]*u5) - rf*z[815]*(u3+z[14]*u1+z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5)) - z[21]*z[814]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-
  z[101]*u1-z[105]*u5)) - z[14]*z[35]*z[218] - rf*(1+z[30]*z[75])*z[123] - 
  z[35]*(z[29]+z[11]*z[100])*z[119] - lfz*z[15]*(z[845]*u1+z[851]*u5-z[840]*
  u3-z[160]) - lfz*z[28]*(z[825]*u5-z[820]*u1-z[822]*u3-z[156]) - z[36]*(
  z[11]*z[14]*z[100]*(z[74]*u1+z[75]*u3+z[79]*u5)+z[75]*z[132]-z[13]*z[100]*
  u1-z[30]*(z[825]*u5-z[820]*u1-z[822]*u3-z[156]));
  z[875] = mf*((lfz*z[26]*z[75]+z[14]*z[27]*z[35]-lfx-lfx*z[30]*z[75]-lfz*
  z[16]*z[100]-z[23]*z[35]*(z[29]+z[11]*z[100]))*(u3+z[14]*u1+z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5))+(lfx*z[15]*z[100]-lfx*z[28]*z[75]-z[11]*z[14]*z[35]*
  z[100]-z[36]*(z[16]*z[100]-z[26]*z[75]))*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))+(1+z[30]*z[75])*(lfz*z[13]*
  z[15]*u1+z[27]*z[35]*(u1+z[14]*u3)+lfz*z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  lfx*u3-lfx*z[14]*u1-lfx*z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfz*z[16]*(
  z[100]*u3-z[101]*u1-z[105]*u5)-z[23]*z[35]*(z[29]*u3+z[11]*(z[100]*u3-
  z[101]*u1-z[105]*u5)))+(z[15]*z[100]-z[28]*z[75])*(lfx*z[28]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-lfx*z[23]*u1-z[29]*z[35]*(u1-z[23]*u5)-lfx*z[15]*(z[100]*
  u3-z[101]*u1-z[105]*u5)-z[14]*z[35]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5))-z[36]*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(
  z[100]*u3-z[101]*u1-z[105]*u5)))+z[874]);
  z[876] = lfx*z[823]*u1 + lfx*z[16]*(z[101]*u1+z[105]*u5) + z[12]*z[14]*
  z[35]*z[100]*(2*u1-z[23]*u5) + rf*z[815]*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) + z[11]*z[14]*z[100]*
  z[125] + lfx*z[15]*(z[845]*u1+z[851]*u5-z[840]*u3-z[160]) + lfx*z[28]*(
  z[825]*u5-z[820]*u1-z[822]*u3-z[156]) + z[14]*z[35]*(z[846]*u5-z[11]*(
  z[845]*u1+z[851]*u5-z[840]*u3-z[160])) + z[36]*(z[829]*u1+z[15]*(z[101]*u1+
  z[105]*u5)+z[818]*(z[74]*u1+z[75]*u3+z[79]*u5)+z[75]*z[129]-2*z[15]*z[100]*
  u3-z[16]*(z[845]*u1+z[851]*u5-z[840]*u3-z[160])-z[26]*(z[825]*u5-z[820]*u1-
  z[822]*u3-z[156])) - 2*lfx*z[16]*z[100]*u3 - z[29]*z[35]*z[823]*u5 - z[29]*
  z[814]*(u1-z[23]*u5) - lfx*z[813]*(z[74]*u1+z[75]*u3+z[79]*u5) - z[14]*
  z[814]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)) - z[13]*z[35]*
  z[100]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)) - z[11]*z[13]*z[35]*
  z[100]*q2p - lfx*z[75]*z[121] - rf*(z[16]*z[100]-z[26]*z[75])*z[123];
  z[877] = mf*((lfz*z[26]*z[75]+z[14]*z[27]*z[35]-lfx-lfx*z[30]*z[75]-lfz*
  z[16]*z[100]-z[23]*z[35]*(z[29]+z[11]*z[100]))*(z[21]*u1+z[26]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))+(lfz*z[28]*z[75]-
  lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+z[30]*z[75])-z[21]*z[35]*(z[29]+
  z[11]*z[100]))*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*
  u3-z[101]*u1-z[105]*u5))+(z[15]*z[100]-z[28]*z[75])*(lfz*z[13]*z[16]*u1+
  z[25]*z[35]*(u1+z[14]*u3-z[23]*u5)+lfz*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)+
  z[36]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-lfz*z[28]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[21]*z[35]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*
  u1-z[105]*u5)))-(z[16]*z[100]-z[26]*z[75])*(lfz*z[13]*z[15]*u1+z[27]*z[35]*(
  u1+z[14]*u3)+lfz*z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*u3-lfx*z[14]*u1-lfx*
  z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfz*z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)-
  z[23]*z[35]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)))-z[876]);
  z[878] = z[11]*z[79]*z[810] + z[30]*z[79]*z[863] + z[30]*z[79]*z[828] + 
  z[30]*z[79]*z[853] + (z[105]+z[12]*z[79])*z[855] + (z[15]*z[105]+z[28]*
  z[79])*z[857] + (z[16]*z[105]+z[26]*z[79])*z[858] + z[79]*(lrx*z[30]+lrz*
  z[39])*z[860] + (lfx*z[30]*z[79]-lfz*z[16]*z[105]-lfz*z[26]*z[79]-z[11]*
  z[23]*z[35]*z[105])*z[862] + (lrz*(z[105]+z[12]*z[79])-rrt*z[11]*z[13]*(
  z[105]+z[114])-rr*z[13]*(z[105]+z[114]+z[12]*z[79]))*z[872] + (z[30]*z[36]*
  z[79]-lfz*z[15]*z[105]-lfz*z[28]*z[79]-z[23]*z[25]*z[35]-z[21]*z[35]*(z[27]+
  z[11]*z[105]))*z[875] - (z[16]*z[105]+z[26]*z[79])*z[811] - z[39]*z[79]*
  z[864] - ICyy*(z[105]+z[114]+z[12]*z[79])*z[868] - IFyy*(1+z[15]*z[105]+
  z[28]*z[79])*z[856] - (lrx*(z[105]+z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+
  z[114])+rr*z[14]*(z[105]+z[114]+z[12]*z[79]))*z[870] - (lfx*z[15]*z[105]+
  lfx*z[28]*z[79]+z[23]*z[29]*z[35]-z[14]*z[35]*(z[27]+z[11]*z[105])-z[36]*(
  z[16]*z[105]+z[26]*z[79]))*z[877];
  z[879] = ICyy*z[11]*((z[100]+z[109]-2*z[12]*z[75])*(z[74]*u1+z[75]*u3+z[79]*
  u5)+z[75]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5));
  z[880] = z[11]*z[74]*z[810] + (z[14]+z[30]*z[74])*z[863] + (z[14]+z[30]*
  z[74])*z[828] + (z[14]+z[30]*z[74])*z[853] + (z[101]+z[12]*z[74])*z[855] + (
  z[13]-z[39]*z[74])*z[864] + (z[21]+z[16]*z[101]+z[26]*z[74])*z[858] + IFyy*(
  z[23]-z[15]*z[101]-z[28]*z[74])*z[856] + (rrt*z[38]+lrz*(z[101]+z[12]*z[74])-
  rrt*z[11]*z[13]*(z[101]+z[110])-rr*z[13]*(z[101]+z[110]+z[12]*z[74]))*
  z[872] + (lfx*z[14]+lfx*z[30]*z[74]-z[27]*z[35]-lfz*z[13]*z[15]-lfz*z[16]*
  z[101]-lfz*z[26]*z[74]-z[11]*z[23]*z[35]*z[101])*z[862] - z[879] - (z[21]+
  z[16]*z[101]+z[26]*z[74])*z[811] - ICyy*(z[101]+z[110]+z[12]*z[74])*z[868] - (
  z[23]-z[15]*z[101]-z[28]*z[74])*z[857] - (lrz*z[13]-rr-lrx*z[14]-rrt*z[11]-
  lrx*z[30]*z[74]-lrz*z[39]*z[74])*z[860] - (rrt*z[29]+lrx*(z[101]+z[12]*
  z[74])+rrt*z[11]*z[14]*(z[101]+z[110])+rr*z[14]*(z[101]+z[110]+z[12]*z[74]))*
  z[870] - (lfz*z[15]*z[101]+lfz*z[28]*z[74]+z[11]*z[21]*z[35]*z[101]-z[25]*
  z[35]-lfz*z[13]*z[16]-z[36]*(z[14]+z[30]*z[74]))*z[875] - (lfx*z[15]*z[101]+
  lfx*z[28]*z[74]-lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(z[21]+
  z[16]*z[101]+z[26]*z[74]))*z[877];
  z[881] = z[11]*z[75]*z[810] + (1+z[30]*z[75])*z[863] + (z[16]*z[100]-z[26]*
  z[75])*z[811] + z[30]*z[75]*z[828] + (1+z[30]*z[75])*z[853] + z[75]*(lrx*
  z[30]+lrz*z[39])*z[860] + ICyy*(z[100]+z[109]-z[12]*z[75])*z[868] + IFyy*(
  z[15]*z[100]-z[28]*z[75])*z[856] + (lfx*z[15]*z[100]-lfx*z[28]*z[75]-z[11]*
  z[14]*z[35]*z[100]-z[36]*(z[16]*z[100]-z[26]*z[75]))*z[877] + (lrx*(z[100]-
  z[12]*z[75])+rrt*z[11]*z[14]*(z[100]+z[109])+rr*z[14]*(z[100]+z[109]-z[12]*
  z[75]))*z[870] - z[39]*z[75]*z[864] - (z[100]-z[12]*z[75])*z[855] - (z[15]*
  z[100]-z[28]*z[75])*z[857] - (z[16]*z[100]-z[26]*z[75])*z[858] - (lrz*(
  z[100]-z[12]*z[75])-rrt*z[11]*z[13]*(z[100]+z[109])-rr*z[13]*(z[100]+z[109]-
  z[12]*z[75]))*z[872] - (lfz*z[26]*z[75]+z[14]*z[27]*z[35]-lfx-lfx*z[30]*
  z[75]-lfz*z[16]*z[100]-z[23]*z[35]*(z[29]+z[11]*z[100]))*z[862] - (lfz*
  z[28]*z[75]-lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+z[30]*z[75])-z[21]*
  z[35]*(z[29]+z[11]*z[100]))*z[875];
  z[882] = (z[347]*z[878]+z[349]*z[880]-z[352]*z[881])/z[348];
  z[883] = ICyy*(z[105]+z[114]+z[12]*z[79])*u1;
  z[884] = IFyy*((1+z[15]*z[105]+z[28]*z[79])*(u3+z[14]*u1+z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5))+z[30]*z[79]*(u5+z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)));
  z[885] = z[11]*z[13]*z[16]*z[105];
  z[886] = rf*z[28]*z[885]/(pow(z[31],0.5)*pow(z[32],2));
  z[887] = z[885]*(z[32]+pow(z[28],2)/pow(z[31],0.5))/pow(z[32],2);
  z[888] = z[11]*z[886] - lr*z[13]*z[105] - ls*z[14]*z[105] - lf*z[13]*z[15]*
  z[105] - rf*z[14]*z[16]*z[887] - z[13]*z[16]*z[36]*z[105];
  z[889] = z[11]*z[13]*z[15]*z[105];
  z[890] = z[15]*(ls*z[889]-lf*z[11]*z[14]*z[105]) + z[16]*(ls*z[885]+rf*
  z[30]*z[887]-z[11]*z[14]*z[36]*z[105]) - lr*z[11]*z[14]*z[105];
  z[891] = (z[56]*z[888]-z[65]*z[890])/pow(z[56],2);
  z[892] = z[11]*z[14]*z[886] - rf*z[16]*z[887] - z[11]*z[13]*z[35]*z[105];
  z[893] = (z[56]*z[892]-z[66]*z[890])/pow(z[56],2);
  z[894] = z[11]*(z[23]*z[886]+z[14]*z[16]*z[35]*z[105]);
  z[895] = (z[56]*z[894]+z[69]*z[890])/pow(z[56],2);
  z[896] = z[39]*(z[895]*u5-z[891]*u1-z[893]*u3-z[148]) - z[14]*z[105]*u1 - 
  z[11]*z[13]*z[105]*(z[74]*u1+z[75]*u3+z[79]*u5) - z[79]*z[173];
  z[897] = z[13]*z[105]*u1 + z[79]*z[132] - z[11]*z[14]*z[105]*(z[74]*u1+
  z[75]*u3+z[79]*u5) - z[30]*(z[895]*u5-z[891]*u1-z[893]*u3-z[148]);
  z[898] = IDxz*(z[105]+z[12]*z[79])*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*
  u5)) + IDxx*(z[105]+z[12]*z[79])*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*
  u5)) + IDxx*z[39]*z[79]*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+
  z[75]*u3+z[79]*u5)) - IDyy*(z[105]+z[12]*z[79])*(z[13]*u1-z[39]*(z[74]*u1+
  z[75]*u3+z[79]*u5)) - IDxz*z[30]*z[79]*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5)) - IDyy*z[39]*z[79]*(z[100]*u3-z[101]*u1-z[105]*
  u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IDxz*z[896] - IDzz*z[897];
  z[899] = z[11]*z[14]*z[886] - rf*z[16]*z[887] - rr*z[13]*z[105] - z[11]*
  z[13]*z[105]*(rrt+z[35]);
  z[900] = z[14]*z[105]*(rr+rrt*z[11]);
  z[901] = z[13]*z[105]*(rr+rrt*z[11]);
  z[902] = rr*z[14]*z[105] + z[11]*z[13]*z[886] + z[11]*z[14]*z[105]*(rrt+
  z[35]);
  z[903] = z[55]*z[899] - z[54]*z[901] - z[58]*z[900] - z[59]*z[902];
  z[904] = z[36]*z[889] - lf*z[885] - rf*z[26]*z[887] - rr*z[12]*z[13]*z[105];
  z[905] = z[57]*z[893] + z[75]*z[904];
  z[906] = z[13]*z[29]*z[886] + z[14]*z[38]*z[886] + z[14]*z[29]*z[35]*z[105] - 
  rf*z[15]*z[887] - z[13]*z[35]*z[38]*z[105];
  z[907] = rr*z[12]*z[14]*z[105] + z[15]*(ls*z[885]+rf*z[30]*z[887]-z[11]*
  z[14]*z[36]*z[105]) - z[16]*(ls*z[889]-lf*z[11]*z[14]*z[105]);
  z[908] = z[906] - z[53]*z[893] - z[75]*z[907];
  z[909] = (z[903]*(z[55]*z[97]+z[59]*z[84])-z[81]*(z[55]*z[905]+z[59]*z[908]+
  z[84]*z[901]-z[97]*z[900]))/pow(z[81],2);
  z[910] = z[38]*z[886] + lf*z[13]*z[16]*z[105] - rf*z[14]*z[15]*z[887] - 
  z[13]*z[15]*z[36]*z[105] - z[12]*z[13]*z[105]*(rrt+z[35]);
  z[911] = z[910] - z[53]*z[891] - z[74]*z[907];
  z[912] = rf*z[21]*z[887] + lf*z[14]*z[16]*z[105] - z[29]*z[886] - z[14]*
  z[15]*z[36]*z[105] - z[12]*z[14]*z[105]*(rrt+z[35]);
  z[913] = z[912] - z[57]*z[891] - z[74]*z[904];
  z[914] = (z[903]*(z[55]*z[90]-z[59]*z[85])-z[81]*(z[55]*z[913]-z[59]*z[911]-
  z[85]*z[901]-z[90]*z[900]))/pow(z[81],2);
  z[915] = z[12]*z[13]*z[16]*z[105];
  z[916] = z[35]*(z[12]*z[13]*z[23]*z[105]-z[13]*z[915]-z[14]*z[27]*z[105]-
  z[14]*z[16]*z[38]*z[105]) - z[886]*(z[13]*z[27]+z[23]*z[38]);
  z[917] = z[916] + z[53]*z[895] - z[79]*z[907];
  z[918] = -z[886]*(z[14]*z[27]-z[23]*z[29]) - z[35]*(z[14]*z[915]-z[13]*
  z[27]*z[105]-z[12]*z[14]*z[23]*z[105]-z[14]*z[16]*z[29]*z[105]);
  z[919] = z[918] + z[57]*z[895] - z[79]*z[904];
  z[920] = (z[903]*(z[55]*z[93]-z[59]*z[89])-z[81]*(z[55]*z[919]-z[59]*z[917]-
  z[89]*z[901]-z[93]*z[900]))/pow(z[81],2);
  z[921] = z[15]*z[105]*u3 + z[79]*z[129] + z[16]*(z[909]*u3-z[914]*u1-z[920]*
  u5-z[152]) - z[14]*z[15]*z[105]*u1 - z[889]*(z[74]*u1+z[75]*u3+z[79]*u5) - 
  z[26]*(z[895]*u5-z[891]*u1-z[893]*u3-z[148]);
  z[922] = IExz*(z[15]*z[105]+z[28]*z[79])*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*
  u3+z[79]*u5)) + IExz*z[30]*z[79]*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*
  u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) + IExx*(z[15]*z[105]+z[28]*z[79])*(
  z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-
  z[105]*u5)) + IExx*(z[16]*z[105]+z[26]*z[79])*(z[28]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IEyy*(z[15]*
  z[105]+z[28]*z[79])*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(
  z[100]*u3-z[101]*u1-z[105]*u5)) - IEyy*(z[16]*z[105]+z[26]*z[79])*(z[28]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) - 
  IExz*z[921] - IEzz*z[897];
  z[923] = z[909]*u3 + z[11]*z[79]*u1 - z[914]*u1 - z[920]*u5 - z[152] - 
  z[12]*(z[895]*u5-z[891]*u1-z[893]*u3-z[148]);
  z[924] = IDzz*z[30]*z[79]*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5)) + 
  z[39]*z[79]*(IDxx*(z[14]*u1+2*z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-2*IDxz*(
  z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))) - IDxx*z[13]*z[30]*z[79]*u1 - 
  z[79]*(IDzz*z[39]+2*IDxz*z[30])*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)) - 
  IDyy*z[923];
  z[925] = z[14]*z[16]*z[105]*u1 + z[885]*(z[74]*u1+z[75]*u3+z[79]*u5) + 
  z[79]*z[121] + z[15]*(z[909]*u3-z[914]*u1-z[920]*u5-z[152]) - z[16]*z[105]*
  u3 - z[28]*(z[895]*u5-z[891]*u1-z[893]*u3-z[148]);
  z[926] = IEzz*z[30]*z[79]*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IExx*(z[16]*z[105]+z[26]*z[79])*(
  u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)) - (2*IExz*z[30]*z[79]-IEzz*(
  z[16]*z[105]+z[26]*z[79]))*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)) - (
  IExx*z[30]*z[79]-2*IExz*(z[16]*z[105]+z[26]*z[79]))*(z[21]*u1+z[26]*(z[74]*
  u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IEyy*z[925];
  z[927] = IEyy*(z[15]*z[105]+z[28]*z[79])*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*
  u3+z[79]*u5)) + IEyy*z[30]*z[79]*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*
  u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IEzz*(z[15]*z[105]+z[28]*z[79])*(
  u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IEzz*z[30]*z[79]*(z[28]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)) - 
  IExz*(z[15]*z[105]+z[28]*z[79])*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)) - IExz*(z[16]*z[105]+z[26]*z[79])*(
  z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-z[101]*u1-
  z[105]*u5)) - IExx*z[921] - IExz*z[897];
  z[928] = lrx*z[13]*z[105]*u1 + lrz*z[14]*z[105]*u1 + lrz*z[11]*z[13]*z[105]*(
  z[74]*u1+z[75]*u3+z[79]*u5) + lrx*z[79]*z[132] + lrz*z[79]*z[173] - lrx*
  z[11]*z[14]*z[105]*(z[74]*u1+z[75]*u3+z[79]*u5) - lrx*z[30]*(z[895]*u5-
  z[891]*u1-z[893]*u3-z[148]) - lrz*z[39]*(z[895]*u5-z[891]*u1-z[893]*u3-
  z[148]);
  z[929] = mr*(z[30]*z[79]*(lrz*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*
  u1+z[75]*u3+z[79]*u5))-rrt*z[38]*u1-rrt*z[11]*z[13]*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)-rr*z[13]*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))+
  z[39]*z[79]*(rrt*z[29]*u1-lrx*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*
  u1+z[75]*u3+z[79]*u5))-rrt*z[11]*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-
  z[105]*u5-z[110]*u1-z[114]*u5)-rr*z[14]*(z[100]*u3+z[109]*u3-z[101]*u1-
  z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)))-(lrx*(
  z[105]+z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+z[114])+rr*z[14]*(z[105]+z[114]+
  z[12]*z[79]))*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))-(lrz*(z[105]+
  z[12]*z[79])-rrt*z[11]*z[13]*(z[105]+z[114])-rr*z[13]*(z[105]+z[114]+z[12]*
  z[79]))*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-z[928]);
  z[930] = lfx*z[13]*z[105]*u1 + lfz*z[14]*z[15]*z[105]*u1 + z[27]*z[886]*(u1+
  z[14]*u3) + z[35]*z[915]*(u1+z[14]*u3) + lfz*z[889]*(z[74]*u1+z[75]*u3+
  z[79]*u5) + lfx*z[79]*z[132] + z[11]*z[23]*z[105]*z[125] + lfz*z[26]*(
  z[895]*u5-z[891]*u1-z[893]*u3-z[148]) + z[23]*z[35]*(z[12]*z[105]*u1-z[12]*
  z[14]*z[105]*u3-z[11]*(z[909]*u3-z[914]*u1-z[920]*u5-z[152])) - lfz*z[15]*
  z[105]*u3 - z[13]*z[27]*z[35]*z[105]*u3 - lfx*z[11]*z[14]*z[105]*(z[74]*u1+
  z[75]*u3+z[79]*u5) - z[23]*z[886]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5)) - z[14]*z[16]*z[35]*z[105]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-
  z[105]*u5)) - lfz*z[79]*z[129] - z[11]*z[35]*z[105]*z[126] - lfx*z[30]*(
  z[895]*u5-z[891]*u1-z[893]*u3-z[148]) - lfz*z[16]*(z[909]*u3-z[914]*u1-
  z[920]*u5-z[152]);
  z[931] = mf*((2*z[30]*z[36]*z[79]-lfz*z[15]*z[105]-lfz*z[28]*z[79]-z[23]*
  z[25]*z[35]-z[21]*z[35]*(z[27]+z[11]*z[105]))*(u3+z[14]*u1+z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5))+z[30]*z[79]*(lfz*z[13]*z[16]*u1+z[25]*z[35]*(u1+z[14]*
  u3-z[23]*u5)+lfz*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)-lfz*z[28]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[21]*z[35]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*
  u1-z[105]*u5)))-(lfx*z[15]*z[105]+lfx*z[28]*z[79]+z[23]*z[29]*z[35]-2*z[36]*(
  z[16]*z[105]+z[26]*z[79])-z[14]*z[35]*(z[27]+z[11]*z[105]))*(z[21]*u1+z[26]*(
  z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))-(z[16]*
  z[105]+z[26]*z[79])*(lfx*z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*z[23]*u1-
  z[29]*z[35]*(u1-z[23]*u5)-lfx*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[14]*
  z[35]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)))-z[930]);
  z[932] = IFyy*((1+z[15]*z[105]+z[28]*z[79])*(z[21]*u1+z[26]*(z[74]*u1+z[75]*
  u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))+(z[16]*z[105]+z[26]*
  z[79])*(u5+z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-z[15]*(z[100]*u3-
  z[101]*u1-z[105]*u5)));
  z[933] = IDyy*(z[105]+z[12]*z[79])*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*
  u5)) + IDzz*z[30]*z[79]*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+
  z[75]*u3+z[79]*u5)) - IDzz*(z[105]+z[12]*z[79])*(z[14]*u1+z[30]*(z[74]*u1+
  z[75]*u3+z[79]*u5)) - IDxz*(z[105]+z[12]*z[79])*(z[13]*u1-z[39]*(z[74]*u1+
  z[75]*u3+z[79]*u5)) - IDxz*z[39]*z[79]*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(
  z[74]*u1+z[75]*u3+z[79]*u5)) - IDyy*z[30]*z[79]*(z[100]*u3-z[101]*u1-z[105]*
  u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - IDxx*z[896] - IDxz*z[897];
  z[934] = (z[903]*(z[54]*z[97]+z[58]*z[84])-z[81]*(z[54]*z[905]+z[58]*z[908]+
  z[84]*z[899]+z[97]*z[902]))/pow(z[81],2);
  z[935] = (z[903]*(z[54]*z[90]-z[58]*z[85])+z[81]*(z[58]*z[911]+z[85]*z[899]-
  z[54]*z[913]-z[90]*z[902]))/pow(z[81],2);
  z[936] = (z[903]*(z[54]*z[93]-z[58]*z[89])+z[81]*(z[58]*z[917]+z[89]*z[899]-
  z[54]*z[919]-z[93]*z[902]))/pow(z[81],2);
  z[937] = z[909]*u3 + z[934]*u3 + z[11]*z[79]*u1 - z[914]*u1 - z[920]*u5 - 
  z[935]*u1 - z[936]*u5 - z[152] - z[154] - z[12]*(z[895]*u5-z[891]*u1-z[893]*
  u3-z[148]);
  z[938] = rrt*z[12]*z[14]*z[114]*u1 + 2*rrt*z[12]*z[14]*z[105]*u1 + rrt*
  z[11]*z[13]*z[105]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-
  z[114]*u5) + rr*z[13]*z[105]*(z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-
  z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+z[79]*u5)) - rrt*z[11]*z[13]*(
  z[105]+z[114])*q2p - rr*z[13]*(z[105]+z[114]+z[12]*z[79])*q2p - rrt*z[11]*
  z[14]*(z[909]*u3+z[934]*u3-z[914]*u1-z[920]*u5-z[935]*u1-z[936]*u5-z[152]-
  z[154]) - lrx*(z[909]*u3+z[11]*z[79]*u1-z[914]*u1-z[920]*u5-z[152]-z[12]*(
  z[895]*u5-z[891]*u1-z[893]*u3-z[148])) - rr*z[14]*(z[909]*u3+z[934]*u3+
  z[11]*z[79]*u1-z[914]*u1-z[920]*u5-z[935]*u1-z[936]*u5-z[152]-z[154]-z[12]*(
  z[895]*u5-z[891]*u1-z[893]*u3-z[148]));
  z[939] = mr*((z[105]+z[12]*z[79])*(rrt*z[38]*u1+rrt*z[11]*z[13]*(z[100]*u3+
  z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)+rr*z[13]*(z[100]*u3+
  z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+
  z[79]*u5)))-z[79]*(lrx*z[30]+lrz*z[39])*(z[13]*u1-z[39]*(z[74]*u1+z[75]*u3+
  z[79]*u5))-z[39]*z[79]*(lrz*z[13]*u1-rr*u1-lrx*z[14]*u1-rrt*z[11]*u1-lrx*
  z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-lrz*z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))-(
  2*lrz*(z[105]+z[12]*z[79])-rrt*z[11]*z[13]*(z[105]+z[114])-rr*z[13]*(z[105]+
  z[114]+z[12]*z[79]))*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*
  u3+z[79]*u5))-z[938]);
  z[940] = rrt*z[12]*z[13]*z[114]*u1 + 2*rrt*z[12]*z[13]*z[105]*u1 + rrt*
  z[11]*z[14]*(z[105]+z[114])*q2p + rr*z[14]*(z[105]+z[114]+z[12]*z[79])*q2p + 
  lrz*(z[909]*u3+z[11]*z[79]*u1-z[914]*u1-z[920]*u5-z[152]-z[12]*(z[895]*u5-
  z[891]*u1-z[893]*u3-z[148])) - rrt*z[11]*z[14]*z[105]*(z[100]*u3+z[109]*u3-
  z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5) - rr*z[14]*z[105]*(z[100]*u3+
  z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+
  z[79]*u5)) - rrt*z[11]*z[13]*(z[909]*u3+z[934]*u3-z[914]*u1-z[920]*u5-
  z[935]*u1-z[936]*u5-z[152]-z[154]) - rr*z[13]*(z[909]*u3+z[934]*u3+z[11]*
  z[79]*u1-z[914]*u1-z[920]*u5-z[935]*u1-z[936]*u5-z[152]-z[154]-z[12]*(
  z[895]*u5-z[891]*u1-z[893]*u3-z[148]));
  z[941] = mr*(z[79]*(lrx*z[30]+lrz*z[39])*(z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+
  z[79]*u5))+(z[105]+z[12]*z[79])*(rrt*z[29]*u1-rrt*z[11]*z[14]*(z[100]*u3+
  z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5)-rr*z[14]*(z[100]*u3+
  z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-z[12]*(z[74]*u1+z[75]*u3+
  z[79]*u5)))-z[30]*z[79]*(lrz*z[13]*u1-rr*u1-lrx*z[14]*u1-rrt*z[11]*u1-lrx*
  z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-lrz*z[39]*(z[74]*u1+z[75]*u3+z[79]*u5))-(
  2*lrx*(z[105]+z[12]*z[79])+rrt*z[11]*z[14]*(z[105]+z[114])+rr*z[14]*(z[105]+
  z[114]+z[12]*z[79]))*(z[100]*u3-z[101]*u1-z[105]*u5-z[12]*(z[74]*u1+z[75]*
  u3+z[79]*u5))-z[940]);
  z[942] = z[12]*z[13]*z[15]*z[105];
  z[943] = lfz*z[14]*z[16]*z[105]*u1 + lfz*z[885]*(z[74]*u1+z[75]*u3+z[79]*u5) + 
  z[25]*z[886]*(u1+z[14]*u3-z[23]*u5) + lfz*z[79]*z[121] + z[23]*z[35]*z[218] + 
  z[35]*(z[27]+z[11]*z[105])*z[119] + lfz*z[15]*(z[909]*u3-z[914]*u1-z[920]*
  u5-z[152]) - lfz*z[16]*z[105]*u3 - z[35]*z[942]*(u1+z[14]*u3-z[23]*u5) - rf*
  z[887]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)) - z[21]*z[886]*(
  z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)) - z[14]*z[15]*
  z[35]*z[105]*(z[27]*u5-z[29]*u3-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)) - 
  z[23]*z[25]*z[125] - rf*z[30]*z[79]*z[123] - z[21]*(z[27]+z[11]*z[105])*
  z[125] - z[25]*z[35]*(z[13]*z[105]*u3+z[14]*z[16]*z[105]*u5-z[126]) - lfz*
  z[28]*(z[895]*u5-z[891]*u1-z[893]*u3-z[148]) - z[21]*z[35]*(z[915]*u5+z[12]*
  z[105]*u1-z[12]*z[14]*z[105]*u3-z[145]-z[11]*(z[909]*u3-z[914]*u1-z[920]*u5-
  z[152])) - z[36]*(z[13]*z[105]*u1+z[79]*z[132]-z[11]*z[14]*z[105]*(z[74]*u1+
  z[75]*u3+z[79]*u5)-z[30]*(z[895]*u5-z[891]*u1-z[893]*u3-z[148]));
  z[944] = mf*((lfx*z[30]*z[79]-lfz*z[16]*z[105]-lfz*z[26]*z[79]-z[11]*z[23]*
  z[35]*z[105])*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))+(lfx*z[15]*
  z[105]+lfx*z[28]*z[79]+z[23]*z[29]*z[35]-z[14]*z[35]*(z[27]+z[11]*z[105])-
  z[36]*(z[16]*z[105]+z[26]*z[79]))*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*
  u1-z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))+(z[15]*z[105]+z[28]*z[79])*(lfx*
  z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*z[23]*u1-z[29]*z[35]*(u1-z[23]*u5)-
  lfx*z[15]*(z[100]*u3-z[101]*u1-z[105]*u5)-z[14]*z[35]*(z[27]*u5-z[11]*(
  z[100]*u3-z[101]*u1-z[105]*u5))-z[36]*(z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)))-z[30]*z[79]*(lfz*z[13]*
  z[15]*u1+z[27]*z[35]*(u1+z[14]*u3)+lfz*z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-
  lfx*u3-lfx*z[14]*u1-lfx*z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfz*z[16]*(
  z[100]*u3-z[101]*u1-z[105]*u5)-z[23]*z[35]*(z[29]*u3+z[11]*(z[100]*u3-
  z[101]*u1-z[105]*u5)))-z[943]);
  z[945] = lfx*z[16]*z[105]*u3 + z[14]*z[16]*z[29]*z[35]*z[105]*u5 + z[13]*
  z[35]*z[105]*(z[27]*u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)) + rf*z[887]*(
  z[21]*u1+z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-
  z[105]*u5)) + z[13]*z[35]*(z[27]+z[11]*z[105])*q2p + z[23]*z[29]*z[125] + 
  rf*(z[16]*z[105]+z[26]*z[79])*z[123] + lfx*z[28]*(z[895]*u5-z[891]*u1-
  z[893]*u3-z[148]) + z[36]*(z[15]*z[105]*u3+z[79]*z[129]+z[16]*(z[909]*u3-
  z[914]*u1-z[920]*u5-z[152])-z[14]*z[15]*z[105]*u1-z[889]*(z[74]*u1+z[75]*u3+
  z[79]*u5)-z[26]*(z[895]*u5-z[891]*u1-z[893]*u3-z[148])) - lfx*z[14]*z[16]*
  z[105]*u1 - z[29]*z[886]*(u1-z[23]*u5) - lfx*z[885]*(z[74]*u1+z[75]*u3+
  z[79]*u5) - z[12]*z[14]*z[35]*z[105]*(2*u1-z[23]*u5) - z[14]*z[886]*(z[27]*
  u5-z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)) - lfx*z[79]*z[121] - z[23]*z[35]*
  z[124] - z[29]*z[35]*z[126] - z[14]*(z[27]+z[11]*z[105])*z[125] - lfx*z[15]*(
  z[909]*u3-z[914]*u1-z[920]*u5-z[152]) - z[14]*z[35]*(z[915]*u5-z[145]-z[11]*(
  z[909]*u3-z[914]*u1-z[920]*u5-z[152]));
  z[946] = mf*((z[16]*z[105]+z[26]*z[79])*(lfz*z[13]*z[15]*u1+z[27]*z[35]*(u1+
  z[14]*u3)+lfz*z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfx*u3-lfx*z[14]*u1-lfx*
  z[30]*(z[74]*u1+z[75]*u3+z[79]*u5)-lfz*z[16]*(z[100]*u3-z[101]*u1-z[105]*u5)-
  z[23]*z[35]*(z[29]*u3+z[11]*(z[100]*u3-z[101]*u1-z[105]*u5)))-(lfx*z[30]*
  z[79]-lfz*z[16]*z[105]-lfz*z[26]*z[79]-z[11]*z[23]*z[35]*z[105])*(z[21]*u1+
  z[26]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[16]*(z[100]*u3-z[101]*u1-z[105]*u5))-(
  z[30]*z[36]*z[79]-lfz*z[15]*z[105]-lfz*z[28]*z[79]-z[23]*z[25]*z[35]-z[21]*
  z[35]*(z[27]+z[11]*z[105]))*(z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[23]*u1-
  z[15]*(z[100]*u3-z[101]*u1-z[105]*u5))-(z[15]*z[105]+z[28]*z[79])*(lfz*
  z[13]*z[16]*u1+z[25]*z[35]*(u1+z[14]*u3-z[23]*u5)+lfz*z[15]*(z[100]*u3-
  z[101]*u1-z[105]*u5)+z[36]*(u3+z[14]*u1+z[30]*(z[74]*u1+z[75]*u3+z[79]*u5))-
  lfz*z[28]*(z[74]*u1+z[75]*u3+z[79]*u5)-z[21]*z[35]*(z[27]*u5-z[29]*u3-z[11]*(
  z[100]*u3-z[101]*u1-z[105]*u5)))-z[945]);
  z[947] = (z[16]*z[105]+z[26]*z[79])*z[884] + z[30]*z[79]*z[898] + z[30]*
  z[79]*z[922] + (z[105]+z[12]*z[79])*z[924] + (z[15]*z[105]+z[28]*z[79])*
  z[926] + (z[16]*z[105]+z[26]*z[79])*z[927] + z[79]*(lrx*z[30]+lrz*z[39])*
  z[929] + (lfx*z[30]*z[79]-lfz*z[16]*z[105]-lfz*z[26]*z[79]-z[11]*z[23]*
  z[35]*z[105])*z[931] + (lrz*(z[105]+z[12]*z[79])-rrt*z[11]*z[13]*(z[105]+
  z[114])-rr*z[13]*(z[105]+z[114]+z[12]*z[79]))*z[941] - z[11]*z[79]*z[883] - 
  z[30]*z[79]*z[932] - z[39]*z[79]*z[933] - ICyy*(z[105]+z[114]+z[12]*z[79])*
  z[937] - IFyy*(1+z[15]*z[105]+z[28]*z[79])*z[925] - (lrx*(z[105]+z[12]*
  z[79])+rrt*z[11]*z[14]*(z[105]+z[114])+rr*z[14]*(z[105]+z[114]+z[12]*z[79]))*
  z[939] - (z[30]*z[36]*z[79]-lfz*z[15]*z[105]-lfz*z[28]*z[79]-z[23]*z[25]*
  z[35]-z[21]*z[35]*(z[27]+z[11]*z[105]))*z[944] - (lfx*z[15]*z[105]+lfx*
  z[28]*z[79]+z[23]*z[29]*z[35]-z[14]*z[35]*(z[27]+z[11]*z[105])-z[36]*(z[16]*
  z[105]+z[26]*z[79]))*z[946];
  z[948] = ICyy*z[11]*((z[105]+z[114])*(z[74]*u1+z[75]*u3+z[79]*u5)-z[79]*(
  z[100]*u3+z[109]*u3-z[101]*u1-z[105]*u5-z[110]*u1-z[114]*u5-2*z[12]*(z[74]*
  u1+z[75]*u3+z[79]*u5)));
  z[949] = z[948] + (z[21]+z[16]*z[101]+z[26]*z[74])*z[884] + (z[14]+z[30]*
  z[74])*z[898] + (z[14]+z[30]*z[74])*z[922] + (z[101]+z[12]*z[74])*z[924] + (
  z[13]-z[39]*z[74])*z[933] + (z[21]+z[16]*z[101]+z[26]*z[74])*z[927] + IFyy*(
  z[23]-z[15]*z[101]-z[28]*z[74])*z[925] + (rrt*z[38]+lrz*(z[101]+z[12]*z[74])-
  rrt*z[11]*z[13]*(z[101]+z[110])-rr*z[13]*(z[101]+z[110]+z[12]*z[74]))*
  z[941] + (lfz*z[15]*z[101]+lfz*z[28]*z[74]+z[11]*z[21]*z[35]*z[101]-z[25]*
  z[35]-lfz*z[13]*z[16]-z[36]*(z[14]+z[30]*z[74]))*z[944] + (lfx*z[14]+lfx*
  z[30]*z[74]-z[27]*z[35]-lfz*z[13]*z[15]-lfz*z[16]*z[101]-lfz*z[26]*z[74]-
  z[11]*z[23]*z[35]*z[101])*z[931] - z[11]*z[74]*z[883] - (z[14]+z[30]*z[74])*
  z[932] - ICyy*(z[101]+z[110]+z[12]*z[74])*z[937] - (z[23]-z[15]*z[101]-
  z[28]*z[74])*z[926] - (lrz*z[13]-rr-lrx*z[14]-rrt*z[11]-lrx*z[30]*z[74]-lrz*
  z[39]*z[74])*z[929] - (rrt*z[29]+lrx*(z[101]+z[12]*z[74])+rrt*z[11]*z[14]*(
  z[101]+z[110])+rr*z[14]*(z[101]+z[110]+z[12]*z[74]))*z[939] - (lfx*z[15]*
  z[101]+lfx*z[28]*z[74]-lfx*z[23]-z[29]*z[35]-z[11]*z[14]*z[35]*z[101]-z[36]*(
  z[21]+z[16]*z[101]+z[26]*z[74]))*z[946];
  z[950] = z[30]*z[75]*z[898] + (1+z[30]*z[75])*z[922] + z[75]*(lrx*z[30]+lrz*
  z[39])*z[929] + ICyy*(z[100]+z[109]-z[12]*z[75])*z[937] + IFyy*(z[15]*
  z[100]-z[28]*z[75])*z[925] + (lfx*z[15]*z[100]-lfx*z[28]*z[75]-z[11]*z[14]*
  z[35]*z[100]-z[36]*(z[16]*z[100]-z[26]*z[75]))*z[946] + (lrx*(z[100]-z[12]*
  z[75])+rrt*z[11]*z[14]*(z[100]+z[109])+rr*z[14]*(z[100]+z[109]-z[12]*z[75]))*
  z[939] + (lfz*z[28]*z[75]-lfz*z[15]*z[100]-z[14]*z[25]*z[35]-z[36]*(1+z[30]*
  z[75])-z[21]*z[35]*(z[29]+z[11]*z[100]))*z[944] - z[11]*z[75]*z[883] - (1+
  z[30]*z[75])*z[932] - (z[16]*z[100]-z[26]*z[75])*z[884] - z[39]*z[75]*
  z[933] - (z[100]-z[12]*z[75])*z[924] - (z[15]*z[100]-z[28]*z[75])*z[926] - (
  z[16]*z[100]-z[26]*z[75])*z[927] - (lrz*(z[100]-z[12]*z[75])-rrt*z[11]*
  z[13]*(z[100]+z[109])-rr*z[13]*(z[100]+z[109]-z[12]*z[75]))*z[941] - (lfz*
  z[26]*z[75]+z[14]*z[27]*z[35]-lfx-lfx*z[30]*z[75]-lfz*z[16]*z[100]-z[23]*
  z[35]*(z[29]+z[11]*z[100]))*z[931];
  z[951] = (z[347]*z[947]+z[349]*z[949]-z[352]*z[950])/z[348];
  z[952] = z[310]*z[569] + z[316]*z[597] - z[312]*z[595] - z[314]*z[492];
  z[953] = z[306]*z[569] + z[316]*z[596] - z[308]*z[595] - z[314]*z[493];
  z[954] = (z[600]*(z[346]*z[344]+z[350]*z[342]-z[353]*z[343])+z[348]*(z[953]*
  z[343]+z[346]*(z[498]+z[568])+z[350]*(z[573]+z[575])-z[599]*z[344]-z[952]*
  z[342]-z[353]*(z[577]+z[578])))/pow(z[348],2);
  z[955] = z[310]*z[707] + z[316]*z[732] - z[312]*z[730] - z[314]*z[632];
  z[956] = z[306]*z[707] + z[316]*z[731] - z[308]*z[730] - z[314]*z[633];
  z[957] = (z[735]*(z[346]*z[344]+z[350]*z[342]-z[353]*z[343])+z[348]*(z[956]*
  z[343]+z[346]*(z[636]+z[706])+z[350]*(z[710]+z[712])-z[734]*z[344]-z[955]*
  z[342]-z[353]*(z[714]+z[715])))/pow(z[348],2);
  z[958] = (z[346]*z[805]+z[350]*z[807]-z[353]*z[808])/z[348];
  z[959] = (z[346]*z[878]+z[350]*z[880]-z[353]*z[881])/z[348];
  z[960] = (z[346]*z[947]+z[350]*z[949]-z[353]*z[950])/z[348];
  z[961] = z[310]*z[570] + z[315]*z[597] - z[311]*z[595] - z[314]*z[494];
  z[962] = z[306]*z[570] + z[315]*z[596] - z[307]*z[595] - z[314]*z[475];
  z[963] = (z[600]*(z[345]*z[344]+z[351]*z[342]-z[354]*z[343])+z[348]*(z[962]*
  z[343]+z[345]*(z[498]+z[568])+z[351]*(z[573]+z[575])-z[598]*z[344]-z[961]*
  z[342]-z[354]*(z[577]+z[578])))/pow(z[348],2);
  z[964] = z[310]*z[708] + z[315]*z[732] - z[311]*z[730] - z[314]*z[634];
  z[965] = z[306]*z[708] + z[315]*z[731] - z[307]*z[730] - z[314]*z[617];
  z[966] = (z[735]*(z[345]*z[344]+z[351]*z[342]-z[354]*z[343])+z[348]*(z[965]*
  z[343]+z[345]*(z[636]+z[706])+z[351]*(z[710]+z[712])-z[733]*z[344]-z[964]*
  z[342]-z[354]*(z[714]+z[715])))/pow(z[348],2);
  z[967] = (z[345]*z[805]+z[351]*z[807]-z[354]*z[808])/z[348];
  z[968] = (z[345]*z[878]+z[351]*z[880]-z[354]*z[881])/z[348];
  z[969] = (z[345]*z[947]+z[351]*z[949]-z[354]*z[950])/z[348];
  z[970] = (z[109]*z[352]+z[110]*z[349]+z[114]*z[347])/z[348];
  z[971] = (z[109]*z[353]+z[110]*z[350]+z[114]*z[346])/z[348];
  z[972] = (z[109]*z[354]+z[110]*z[351]+z[114]*z[345])/z[348];
  z[973] = -z[10]*z[13] - z[9]*z[12]*z[14];
  z[974] = z[15]*z[973] - z[9]*z[11]*z[16];
  z[975] = z[9]*z[12]*z[13] - z[10]*z[14];
  z[976] = -z[16]*z[973] - z[9]*z[11]*z[15];
  z[977] = z[10]*(z[12]*z[16]-z[11]*z[14]*z[15]);
  z[978] = z[10]*(z[12]*z[15]+z[11]*z[14]*z[16]);
  z[979] = z[16]*z[373] - z[15]*z[377];
  z[980] = z[9]*z[13] - z[10]*z[12]*z[14];
  z[981] = z[15]*z[980] - z[10]*z[11]*z[16];
  z[982] = z[9]*z[14] + z[10]*z[12]*z[13];
  z[983] = -z[16]*z[980] - z[10]*z[11]*z[15];
  z[984] = z[9]*(z[12]*z[16]-z[11]*z[14]*z[15]);
  z[985] = z[9]*(z[12]*z[15]+z[11]*z[14]*z[16]);
  z[986] = -z[15]*z[378] - z[16]*z[374];

  no_fn[0] = q6 + lf*z[381] + lr*z[377] + ls*z[379] - rr*z[375] - z[36]*
  z[384];
  no_fn[1] = q7 + lf*z[382] + lr*z[378] + ls*z[380] + rr*z[376] - z[36]*
  z[385];
  no_fn[2] = lf*z[26] + ls*z[30] - rrt - z[35] - lr*z[39] - rr*z[11] - z[28]*
  z[36];
  cn_cm[0] = rr*z[48] + rr*z[14]*z[49] + rr*z[22]*z[50] + rr*z[24]*z[52] + 
  rrt*z[11]*z[48] + rrt*z[28]*z[52] + rrt*z[39]*z[49] + (lrz*mr*z[51]+lrz*mr*
  z[30]*z[47]+mf*(lf+lfx)*z[50]+mf*(lfz+ls)*z[51]+(lr*mf+lrx*mr)*z[49]+mf*(lf+
  lfx)*z[15]*z[49]+mf*(lf+lfx)*z[22]*z[48]+mf*(lf+lfx)*z[26]*z[47]+mf*(lfz+ls)*
  z[30]*z[47]+(lr*mf+lrx*mr)*z[14]*z[48]+(lr*mf+lrx*mr)*z[15]*z[50]+(lr*mf+
  lrx*mr)*z[16]*z[52]-lrz*mr*z[13]*z[48]-mf*(lfz+ls)*z[13]*z[48]-(lr*mf+lrx*
  mr)*z[39]*z[47])/(mf+mr) - rrt*z[47] - rr*z[11]*z[47] - rr*z[13]*z[51] - 
  rrt*z[26]*z[50] - rrt*z[30]*z[51];
  cn_cm[1] = rr*z[12]*z[13]*z[49] + rr*z[13]*(z[26]*z[52]+z[28]*z[50]) + rr*
  z[22]*(z[28]*z[51]+z[30]*z[52]) + rr*z[24]*(z[26]*z[51]-z[30]*z[50]) - (lrz*
  mr*z[12]*z[49]+lrz*mr*z[12]*z[14]*z[48]+mf*(lfz+ls)*z[12]*z[49]+lrz*mr*(
  z[26]*z[52]+z[28]*z[50])+mf*(lf+lfx)*z[12]*z[21]*z[48]+mf*(lfz+ls)*z[12]*
  z[14]*z[48]+(lr*mf+lrx*mr)*z[12]*z[13]*z[48]+mf*(lfz+ls)*(z[26]*z[52]+z[28]*
  z[50])-mf*(lf+lfx)*z[16]*z[30]*z[49]-mf*(lf+lfx)*(z[28]*z[51]+z[30]*z[52])-(
  lr*mf+lrx*mr)*z[15]*(z[28]*z[51]+z[30]*z[52])-(lr*mf+lrx*mr)*z[16]*(z[26]*
  z[51]-z[30]*z[50]))/(mf+mr);
  cn_cm[2] = (lrz*mr*z[30]+mf*(lf+lfx)*z[26]+mf*(lfz+ls)*z[30]-(lr*mf+lrx*mr)*
  z[39])/(mf+mr) - rrt - rr*z[11];
  h2_cl[0] = z[33]*(z[12]*z[27]*z[48]+z[25]*z[27]*z[50]+z[27]*z[38]*z[49]-
  z[13]*z[23]*z[49]-z[14]*z[23]*z[51]-z[21]*z[23]*z[50]-z[27]*z[29]*z[51]-
  z[52]*pow(z[23],2)-z[52]*pow(z[27],2));
  h2_cl[1] = z[33]*(z[12]*z[23]*z[48]+z[11]*z[27]*z[30]*z[49]+z[12]*z[14]*
  z[23]*z[49]+z[12]*z[27]*z[29]*z[49]+z[14]*z[23]*(z[26]*z[52]+z[28]*z[50])+
  z[25]*z[27]*(z[28]*z[51]+z[30]*z[52])+z[27]*z[29]*(z[26]*z[52]+z[28]*z[50])-
  z[21]*z[23]*(z[28]*z[51]+z[30]*z[52])-pow(z[23],2)*(z[26]*z[51]-z[30]*z[50])-
  pow(z[27],2)*(z[26]*z[51]-z[30]*z[50]));
  h2_cl[2] = 0;
  constraints[0] = z[38]*(rrt*u1+z[35]*u1+z[14]*z[35]*u3-z[23]*z[35]*u5) + 
  z[15]*(ls*u0*z[28]+ls*u2*z[15]+u0*z[30]*z[36]+z[36]*u3+z[14]*z[36]*u1) + 
  z[16]*(ls*u0*z[26]+ls*u2*z[16]-lf*u0*z[30]-lf*u3-lf*z[14]*u1) - rr*z[13]*(
  u2+u4+u0*z[12]) - z[13]*(u2*z[11]*z[35]+rrt*(u2+u4)*z[11]+z[27]*z[35]*u5-
  z[29]*z[35]*u3);
  constraints[1] = (rr-ls*z[13])*u1 + lr*(u0*z[30]+z[14]*u1) + z[11]*(rrt*u1+
  z[35]*u1+z[14]*z[35]*u3-z[23]*z[35]*u5) + z[16]*(ls*u0*z[28]+u0*z[30]*z[36]+
  z[36]*u3+z[14]*z[36]*u1) - z[15]*(ls*u0*z[26]-lf*u0*z[30]-lf*u3-lf*z[14]*u1);
  constraints[2] = lf*z[23]*u1 - lf*u0*z[28] - lf*u2*z[15] - u0*z[26]*z[36] - 
  u2*z[16]*z[36] - lr*(u2+u0*z[12]) - rr*z[14]*(u2+u4+u0*z[12]) - z[21]*z[36]*
  u1 - z[29]*(rrt*u1+z[35]*u1-z[23]*z[35]*u5) - z[14]*(u2*z[11]*z[35]+rrt*(u2+
  u4)*z[11]+z[27]*z[35]*u5);
  A[0] = 0;
  A[1] = z[396];
  A[2] = z[408];
  A[3] = 0;
  A[4] = 0;
  A[5] = 0;
  A[6] = 0;
  A[7] = z[74];
  A[8] = z[75];
  A[9] = z[79];
  A[10] = 0;
  A[11] = 0;
  A[12] = 0;
  A[13] = 0;
  A[14] = 0;
  A[15] = 0;
  A[16] = 0;
  A[17] = 1;
  A[18] = 0;
  A[19] = 0;
  A[20] = 0;
  A[21] = 0;
  A[22] = 0;
  A[23] = 0;
  A[24] = 0;
  A[25] = 0;
  A[26] = 0;
  A[27] = 0;
  A[28] = 1;
  A[29] = 0;
  A[30] = 0;
  A[31] = z[429];
  A[32] = z[449];
  A[33] = 0;
  A[34] = 0;
  A[35] = 0;
  A[36] = 0;
  A[37] = z[110];
  A[38] = -z[109];
  A[39] = z[114];
  A[40] = 0;
  A[41] = 0;
  A[42] = 0;
  A[43] = 0;
  A[44] = 0;
  A[45] = 0;
  A[46] = 0;
  A[47] = 0;
  A[48] = 0;
  A[49] = 1;
  A[50] = sin(q0)*(rr+rrt*z[11])*(q2p+q4p);
  A[51] = -z[9]*((rr+rrt*z[11])*(z[429]+z[453])-rrt*sin(q1)*(q2p+q4p));
  A[52] = -z[9]*(rr+rrt*z[11])*(z[449]+z[457]);
  A[53] = 0;
  A[54] = 0;
  A[55] = 0;
  A[56] = 0;
  A[57] = -z[9]*(z[101]+z[110])*(rr+rrt*z[11]);
  A[58] = z[9]*(z[100]+z[109])*(rr+rrt*z[11]);
  A[59] = -z[9]*(z[105]+z[114])*(rr+rrt*z[11]);
  A[60] = -cos(q0)*(rr+rrt*z[11])*(q2p+q4p);
  A[61] = -z[10]*((rr+rrt*z[11])*(z[429]+z[453])-rrt*sin(q1)*(q2p+q4p));
  A[62] = -z[10]*(rr+rrt*z[11])*(z[449]+z[457]);
  A[63] = 0;
  A[64] = 0;
  A[65] = 0;
  A[66] = 0;
  A[67] = -z[10]*(z[101]+z[110])*(rr+rrt*z[11]);
  A[68] = z[10]*(z[100]+z[109])*(rr+rrt*z[11]);
  A[69] = -z[10]*(z[105]+z[114])*(rr+rrt*z[11]);
  A[70] = 0;
  A[71] = -z[601];
  A[72] = -z[736];
  A[73] = 0;
  A[74] = 0;
  A[75] = 0;
  A[76] = 0;
  A[77] = -z[809];
  A[78] = -z[882];
  A[79] = -z[951];
  A[80] = 0;
  A[81] = z[954];
  A[82] = z[957];
  A[83] = 0;
  A[84] = 0;
  A[85] = 0;
  A[86] = 0;
  A[87] = z[958];
  A[88] = z[959];
  A[89] = z[960];
  A[90] = 0;
  A[91] = -z[963];
  A[92] = -z[966];
  A[93] = 0;
  A[94] = 0;
  A[95] = 0;
  A[96] = 0;
  A[97] = -z[967];
  A[98] = -z[968];
  A[99] = -z[969];
  B[0] = 0;
  B[1] = 0;
  B[2] = 0;
  B[3] = 0;
  B[4] = 0;
  B[5] = 0;
  B[6] = 0;
  B[7] = 0;
  B[8] = 0;
  B[9] = 0;
  B[10] = 0;
  B[11] = 0;
  B[12] = 0;
  B[13] = 0;
  B[14] = 0;
  B[15] = 0;
  B[16] = 0;
  B[17] = 0;
  B[18] = 0;
  B[19] = 0;
  B[20] = 0;
  B[21] = z[352]/z[348];
  B[22] = -z[970];
  B[23] = -z[347]/z[348];
  B[24] = -z[353]/z[348];
  B[25] = z[971];
  B[26] = z[346]/z[348];
  B[27] = z[354]/z[348];
  B[28] = -z[972];
  B[29] = -z[345]/z[348];
  C[0] = 0;
  C[1] = z[391]*u1 + z[393]*u3 - z[395]*u5;
  C[2] = z[403]*u1 + z[405]*u3 - z[407]*u5;
  C[3] = 0;
  C[4] = 0;
  C[5] = 0;
  C[6] = 0;
  C[7] = z[74];
  C[8] = z[75];
  C[9] = z[79];
  C[10] = 0;
  C[11] = z[452]*u3 - z[450]*u1 - z[451]*u5;
  C[12] = z[454]*u1 + z[455]*u5 - z[456]*u3;
  C[13] = 0;
  C[14] = 0;
  C[15] = 0;
  C[16] = 0;
  C[17] = z[101];
  C[18] = -z[100];
  C[19] = z[105];
  C[20] = 0;
  C[21] = z[428]*u3 - z[418]*u1 - z[424]*u5;
  C[22] = z[448]*u3 - z[438]*u1 - z[444]*u5;
  C[23] = 0;
  C[24] = 0;
  C[25] = 0;
  C[26] = 0;
  C[27] = z[110];
  C[28] = -z[109];
  C[29] = z[114];
  C[30] = lf*z[974] + lr*z[973] + ls*z[975] - z[36]*z[976] - rr*cos(q0)*z[12];
  C[31] = lf*z[977] + ls*cos(q1)*z[10]*z[13] - z[36]*z[978] - rf*z[384]*
  z[387] - rr*cos(q1)*z[10] - lr*cos(q1)*z[10]*z[14];
  C[32] = lf*z[384] - z[36]*z[979] - rf*z[384]*z[398];
  C[33] = 0;
  C[34] = 0;
  C[35] = 1;
  C[36] = 0;
  C[37] = 0;
  C[38] = 0;
  C[39] = 0;
  C[40] = lf*z[981] + lr*z[980] + ls*z[982] - z[36]*z[983] - rr*sin(q0)*z[12];
  C[41] = z[36]*z[985] + rr*cos(q1)*z[9] + lr*cos(q1)*z[9]*z[14] - lf*z[984] - 
  rf*z[385]*z[387] - ls*cos(q1)*z[9]*z[13];
  C[42] = lf*z[385] - z[36]*z[986] - rf*z[385]*z[398];
  C[43] = 0;
  C[44] = 0;
  C[45] = 0;
  C[46] = 1;
  C[47] = 0;
  C[48] = 0;
  C[49] = 0;
} // computeOutputs()
